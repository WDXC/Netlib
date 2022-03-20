#include "Log.hpp"
#include "Timer.hpp"
#include "TimerId.hpp"
#include "EventLoop.hpp"
#include "TimerQueue.hpp"

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
        microseconds / TimeStamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>(
        (microseconds % TimeStamp::kMicroSecondsPerSecond) * 1000);
    return ts;
}

void readTimerfd(int timerfd, TimeStamp now) {
    uint64_t howmany;
    ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));
    LOG_INFO("TimerQueue::handleRead() is aaaa=%d, %d at %s",n, howmany, now.to_string().c_str());
    if (n != sizeof(howmany)) {
        LOG_ERROR("TimerQueue::handleRead() reads %d bytes instead of 8 at fd=%d", n, timerfd);
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
}  // namespace handle

TimerQueue::TimerQueue(EventLoop* loop) : m_loop(loop),
                                          m_timerfd(handle::createTimerfd()),
                                          m_timerfdChannel(loop, m_timerfd),
                                          m_timers(),
                                          callingExpiredTimers_(false) {
    m_timerfdChannel.setReadCallback(std::bind(&TimerQueue::handleRead, this));
    m_timerfdChannel.enable_reading();
}

TimerQueue::~TimerQueue() {
    m_timerfdChannel.dis_enable_all();
    m_timerfdChannel.remove();
    ::close(m_timerfd);

    for (const Entry& timer : m_timers) {
        delete timer.second;
    }
}

void TimerQueue::handleRead() {
    m_loop->assertInLoopThread();
    TimeStamp now(TimeStamp::now());
    handle::readTimerfd(m_timerfd, now);

    std::vector<Entry> expired = getExpired(now);
    callingExpiredTimers_ = true;
    cancelingTimers_.clear();
    for (const Entry& i : expired) {
        i.second->run();
    }
    callingExpiredTimers_ = false;

    reset(expired, now);
}

TimerId TimerQueue::addTimer(TimerCallback cb, TimeStamp when, double interval) {
    Timer* timer = new Timer(std::move(cb), when, interval);
    m_loop->run_in_loop(
        std::bind(&TimerQueue::addTimerInLoop, this, timer));
    return TimerId(timer, timer->sequence());
}

void TimerQueue::addTimerInLoop(Timer* timer) {
    m_loop->assertInLoopThread();

    if (insert(timer)) {
        handle::restartTimerfd(m_timerfd, timer->expiration());
    }
}

void TimerQueue::cancel(TimerId timerId) {
    m_loop->run_in_loop(
        std::bind(&TimerQueue::cancelInLoop, this, timerId));
}

void TimerQueue::cancelInLoop (TimerId timerId) {
    m_loop->assertInLoopThread();
    assert(m_timers.size() == activeTimers_.size());
    ActiveTimer timer(timerId.timer_, timerId.sequence_);
    ActiveTimerSet::iterator it = activeTimers_.find(timer);
    if (it != activeTimers_.end()) {
        size_t n = m_timers.erase(Entry(it->first->expiration(), it->first));
        assert(n == 1); (void)n;
        delete it->first;
        activeTimers_.erase(it);
    } else if (callingExpiredTimers_) {
        cancelingTimers_.insert(timer);
    }
    assert(m_timers.size() == activeTimers_.size());
}



std::vector<TimerQueue::Entry> TimerQueue::getExpired(TimeStamp now) {
    assert(m_timers.size() == activeTimers_.size());
    std::vector<Entry> expired;
    Entry sentry(now, reinterpret_cast<Timer*> (UINTPTR_MAX));
    TimerList::iterator end = m_timers.lower_bound(sentry);
    assert(end == m_timers.end() || now < end->first);
    std::copy(m_timers.begin(), end, std::back_inserter(expired));
    m_timers.erase(m_timers.begin(), end);

    for (const Entry& it : expired) {
        ActiveTimer timer(it.second, it.second->sequence());
        size_t n = activeTimers_.erase(timer);
        assert(n == 1);
        (void) n;
    }

    assert(m_timers.size() == activeTimers_.size());
    return expired;
}

void TimerQueue::reset(const std::vector<Entry>& expired, TimeStamp now) {
    TimeStamp nextExpire;

    for (const Entry& it : expired) {
        ActiveTimer timer(it.second, it.second->sequence());
        if (it.second->repeat() && 
            cancelingTimers_.find(timer) == cancelingTimers_.end()) {
            it.second->restart(now);
            insert(it.second);
        } else {
            delete it.second;
        }
    }

    if (!m_timers.empty()) {
        nextExpire = m_timers.begin()->second->expiration();
    }
    if (nextExpire.valid()) {
        handle::restartTimerfd(m_timerfd, nextExpire);
    }
}

bool TimerQueue::insert (Timer* timer) {
    m_loop->assertInLoopThread();
    assert(m_timers.size() == activeTimers_.size());
    bool earliestChanged = false;
    TimeStamp when = timer->expiration();
    TimerList::iterator it = m_timers.begin();
    if (it == m_timers.end() || when < it->first) {
        earliestChanged = true;
    }
    {
        std::pair<TimerList::iterator, bool> res = 
        m_timers.insert(Entry(when, timer));
        assert(res.second);
        (void)res;
    }
    {
        std::pair<ActiveTimerSet::iterator, bool> res = 
            activeTimers_.insert(ActiveTimer(timer, timer->sequence()));
        assert(res.second); (void)res;
    }

    assert(m_timers.size() == activeTimers_.size());
    return earliestChanged;
}