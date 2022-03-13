#include "TimerQueue.hpp"
#include "Log.hpp"
#include <sys/timerfd.h>

using Entry = std::pair<TimeStamp, std::shared_ptr<Timer>>;

namespace handle {
    int createTimerfd() {
        int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
        if (timerfd < 0) {
            LOG_ERROR("failed in timerfd_create");
        }
        return timerfd;
    }

    struct timespec howMuchTimeFromNow(TimeStamp when) {
        int64_t microseconds = when.MicroSecondsSinceEpoch() -
                                TimeStamp::now().MicroSecondsSinceEpoch();
        if (microseconds < 100) {
            microseconds = 100;
        }
        struct timespec ts;
        ts.tv_sec = static_cast<time_t>(
            microseconds / TimeStamp::kMicroSecondsPerSecond
        );
        ts.tv_nsec = static_cast<long> (
            (microseconds % TimeStamp::kMicroSecondsPerSecond) * 1000
        );
        return ts;
    }

    void readTimerfd(int timerfd, TimeStamp now) {
        uint64_t howmany;
        ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));
        LOG_INFO("TimerQueue::handleRead() %d at %s", howmany, now.to_string().c_str());
        if (n != sizeof(howmany)) {
            LOG_ERROR("TimerQueue::handleRead() reads %d bytes instead of 8", n);
        }
    }

    void restartTimerfd(int timerfd, TimeStamp expiration) {
        struct itimerspec newValue;
        struct itimerspec oldValue;
        memset(&newValue, 0, sizeof(newValue));
        memset(&oldValue, 0, sizeof(oldValue));

        newValue.it_value = howMuchTimeFromNow(expiration);
        int ret = timerfd_settime(timerfd, 0, &newValue, &oldValue);
        if (ret) {
            LOG_ERROR("timerfd_settime failed");
        }
    }
} // namespace handle 

TimerQueue::TimerQueue(EventLoop* loop) : 
    m_loop(loop),
    m_timerfd(handle::createTimerfd()),
    m_timerfdChannel(loop, m_timerfd),
    m_timers() {
    m_timerfdChannel.setReadCallback(std::bind(&TimerQueue::handleRead, this));
    m_timerfdChannel.enable_reading();
}

TimerQueue::~TimerQueue() {
    m_timerfdChannel.dis_enable_all();
    m_timerfdChannel.remove();
    ::close(m_timerfd);
}

void TimerQueue::handleRead () {
    m_loop->assertInLoopThread();
    TimeStamp now(TimeStamp::now());
    handle::readTimerfd(m_timerfd, now);

    std::vector<Entry> expired = getExpired(now);
    for (const Entry &i : expired) {
        i.second->run();
    }

    reset(expired, now);
}

void TimerQueue::addTimer(TimerCallback cb, TimeStamp when, double interval) {
    Timer* timer = new Timer(std::move(cb), when, interval);
    // m_loop->runInLoop();
}

void TimerQueue::addTimerInLoop(Timer *timer) {
    m_loop->assertInLoopThread();

    if (insert(timer)) {
        handle::restartTimerfd(m_timerfd, timer->expiration());
    }
}

std::vector<Entry> TimerQueue::getExpired(TimeStamp now) {
    Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
    TimerList::iterator end = m_timers.lower_bound(sentry);

    std::vector<Entry> expired(m_timers.begin(), end);
    m_timers.erase(m_timers.begin(), end);

    return expired;
}

void TimerQueue::reset (const std::vector<Entry>& expired, TimeStamp now) {
    TimeStamp nextExpire;

    for (const Entry &i : expired) {
        if (i.second->repeat()) {
            i.second->restart(now);
            insert(i.second.get());
        }
    }

    if (!m_timers.empty()) {
        nextExpire = m_timers.begin()->second->expiration();
    }
    if (nextExpire.valid()) {
        handle::restartTimerfd(m_timerfd, nextExpire);
    }
}
