#include <unistd.h>
#include <fcntl.h>
#include <functional>
#include <sys/eventfd.h>
#include "EventLoop.hpp"
#include <assert.h>
#include "SocketOps.hpp"
#include "Log.hpp"

namespace {
	__thread EventLoop *t_loop_in_thisThread = nullptr;

	const int k_poll_timeout = 10000;
	int CreateEventfd() {
		int event_fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
			if (event_fd < 0) {
				  LOG_WARN("eventfd error: %d \n", errno);
			}
    return event_fd;
	}
}




EventLoop::EventLoop() : 
    looping_(false),
    quit_(false),
    calling_pending_functors_(false),
    threadId_(syscall(SYS_gettid)),
    poller_(Poller::new_deafultPoller(this)),
    wakeup_fd(CreateEventfd()),
    wakeup_channel_(new Channel(this, wakeup_fd)) {

    LOG_DEBUG("EventLoop created %p in thread %d \n", this, threadId_);
    if (t_loop_in_thisThread) {
        LOG_ERROR("Another EventLoop %p exists in this thread %d \n", this, threadId_);
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
    close(wakeup_fd);
    t_loop_in_thisThread = nullptr;
}

void EventLoop::loop() {
    looping_ = true;
    quit_ = false;

    LOG_INFO("EventLoop %p start looping \n", this);

    while (!quit_) {
        active_channels.clear();
        poll_return_time_ = poller_->poll(k_poll_timeout, &active_channels);

        for (Channel* channel : active_channels) {
            channel->handle_event(poll_return_time_);
        }

        do_pending_fucntor();
    }
    LOG_INFO("EventLoop %p stop looping ", this);
    looping_ = false;
}

void EventLoop::quit() {
    quit_ = true;

    if (!assertInLoopThread() || calling_pending_functors_) {
        wakeup();
    }
}

void EventLoop::run_in_loop(Functor cb) {
    // 在当前loop线程中执行回调
    if (assertInLoopThread()) {
        cb();
    }
    // 在其它线程执行cb，唤醒loop所在线程执行cb
    else {
        queue_in_loop(cb);
    }
}

void EventLoop::queue_in_loop(Functor cb) {
    {
        std::unique_lock<std::mutex> locker(functor_mutex_);
        pending_Functors_.emplace_back(cb);
    }
    if (!assertInLoopThread() || calling_pending_functors_) {
        wakeup(); // 唤醒loop所在线程
    }
}

// 向wakeupfd中写入一个数据，wakeupchannel发生读事件，就会被唤醒
void EventLoop::wakeup() {
    uint64_t one = 1;
    ssize_t n = sockets::write(wakeup_fd, &one, sizeof(one));
    if (n != sizeof(one)) {
        LOG_ERROR("EventLoop::wakeup() writes %lu bytes instead of 8 \n", n);
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
    poller_->remove_channel(channel);
}

bool EventLoop::has_channel(Channel* channel) {
    return poller_->has_channel(channel);
}


void EventLoop::handle_read () {
    wakeup();
}

void EventLoop::do_pending_fucntor () {
    std::vector<Functor> functors;
    calling_pending_functors_ = true;

    {
        std::lock_guard<std::mutex> locker(functor_mutex_);
        functors.swap(pending_Functors_);
    }

    for (const Functor & func : functors) {
        func();
    }
    calling_pending_functors_ = false;
}
