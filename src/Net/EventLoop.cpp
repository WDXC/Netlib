#include "EventLoop.hpp"

#include <assert.h>
#include <fcntl.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <functional>
#include <algorithm>

#include "Log.hpp"
#include "SocketOps.hpp"

namespace {
__thread EventLoop* t_loop_in_thisThread = nullptr;

const int k_poll_timeout = 10000;
int CreateEventfd() {
    int event_fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (event_fd < 0) {
        LOG_WARN("eventfd error: %d \n", errno);
        abort();
    }
    return event_fd;
}
}  // namespace

EventLoop::EventLoop() : looping_(false),
                         quit_(false),
                         calling_pending_functors_(false),
                         threadId_(syscall(SYS_gettid)),
                         poller_(Poller::new_deafultPoller(this)),
                         timerQueue_(new TimerQueue(this)),
                         wakeup_fd(CreateEventfd()),
                         wakeup_channel_(new Channel(this, wakeup_fd)),
                         currentActiveChannel_(NULL),
                         iteration_(0) {
    LOG_DEBUG("EventLoop created %p in thread %d ", this, threadId_);
    if (t_loop_in_thisThread) {
        LOG_ERROR("Another EventLoop %p exists in this thread %d ", this, threadId_);
    } else {
        t_loop_in_thisThread = this;
    }
    //  设置wakeupfd的事件类型以及事件发生后的回调操作
    wakeup_channel_->setReadCallback(std::bind(&EventLoop::handle_read, this));
    wakeup_channel_->enable_reading();
}

EventLoop::~EventLoop() {
    wakeup_channel_->dis_enable_all();
    wakeup_channel_->remove();
    sockets::close(wakeup_fd);
    t_loop_in_thisThread = NULL;
}

void EventLoop::loop() {
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;
    quit_ = false;

    LOG_INFO("EventLoop %p start looping ", this);

    while (!quit_) {
        active_channels.clear();
        poll_return_time_ = poller_->poll(k_poll_timeout, &active_channels);
        ++iteration_;
        printActiveChannel();
        eventHandling_ = true;
        for (Channel* channel : active_channels) {
            currentActiveChannel_ = channel;
            channel->handle_event(poll_return_time_);
        }
        currentActiveChannel_ = NULL;
        eventHandling_ = false;
        do_pending_fucntor();
    }
    LOG_INFO("EventLoop %p stop looping ", this);
    looping_ = false;
}

void EventLoop::quit() {
    quit_ = true;

    if (!is_in_loopThread()) {
        wakeup();
    }
}

void EventLoop::run_in_loop(Functor cb) {
    // 在当前loop线程中执行回调
    if (is_in_loopThread()) {
        cb();
    }
    // 在其它线程执行cb，唤醒loop所在线程执行cb
    else {
        queue_in_loop(std::move(cb));
    }
}

void EventLoop::queue_in_loop(Functor cb) {
    {
        std::unique_lock<std::mutex> locker(functor_mutex_);
        pending_Functors_.emplace_back(cb);
    }
    if (!is_in_loopThread() || calling_pending_functors_) {
        wakeup();  // 唤醒loop所在线程
    }
}



void EventLoop::abortNoInLoopThread() {
    LOG_INFO("EventLoop abortNoInLoopThread");
}

// poller 方法
void EventLoop::update_channel(Channel* channel) {
    assert(channel->ower_loop() == this);
    assertInLoopThread();
    poller_->update_channel(channel);
}

void EventLoop::remove_channel(Channel* channel) {
    assert(channel->ower_loop() == this);
    assertInLoopThread();
    if (eventHandling_) {
        assert(currentActiveChannel_ == channel || 
               std::find(active_channels.begin(), active_channels.end(), channel) == active_channels.end());
    }
    poller_->remove_channel(channel);
}

bool EventLoop::has_channel(Channel* channel) {
    assert(channel->ower_loop() == this);
    assertInLoopThread();
    return poller_->has_channel(channel);
}

TimerId EventLoop::runAt(TimeStamp time, TimerCallback cb) {
    return timerQueue_->addTimer(std::move(cb), time, 0.0);
}

TimerId EventLoop::runAfter(double delay, TimerCallback cb) {
    TimeStamp time(AddTime(TimeStamp::now(), delay));
    return runAt(time, std::move(cb));
}

TimerId EventLoop::runEvery(double interval, TimerCallback cb) {
    TimeStamp time(AddTime(TimeStamp::now(), interval));
    return timerQueue_->addTimer(std::move(cb), time, interval);
}

void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = sockets::write(wakeup_fd, &one, sizeof(one));
    if (n != sizeof(one)) {
        LOG_ERROR("EventLoop::wakeup() writes %lu bytes instead of 8 ", n);
    }
}

void EventLoop::handle_read() {
    uint64_t one = 1;
    ssize_t n = sockets::read(wakeup_fd, &one, sizeof(one));
    if (n != sizeof(one)) {
        LOG_ERROR("EventLoop::handleRead() reads %d bytes instead of 8");
    }
}

void EventLoop::do_pending_fucntor() {
    std::vector<Functor> functors;
    calling_pending_functors_ = true;

    {
        std::lock_guard<std::mutex> locker(functor_mutex_);
        functors.swap(pending_Functors_);
    }

    for (const Functor& func : functors) {
        func();
    }
    calling_pending_functors_ = false;
}

void EventLoop::printActiveChannel () const {
    for (const Channel* channel : active_channels) {
        LOG_DEBUG("{ %s }", channel->reventToString().c_str());
    }
}