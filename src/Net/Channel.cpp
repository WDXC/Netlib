#include "Channel.hpp"
#include "Log.hpp"
#include <assert.h>
#include "EventLoop.hpp"
#include <sys/epoll.h>

const int Channel::k_none_event_ = 0;
const int Channel::k_read_event_ = EPOLLIN | EPOLLPRI;
const int Channel::k_write_event_ = EPOLLOUT;

Channel::Channel(EventLoop* loop, int fd) : 
    loop_(loop),
    fd_(fd),
    events_(0),
    real_events_(0),
    index_(-1),
    tied_(false) {

}

Channel::~Channel() {
}

void Channel::handle_event(TimeStamp receive_time) {
    if (tied_) {
        std::shared_ptr<void> guard = tie_.lock();
        if (guard) {
            handle_event_withGuard(receive_time);
        }
    } else {
        handle_event_withGuard(receive_time);
    }
}

void Channel::tie(const std::shared_ptr<void>& obj) {
    tie_ = obj;
    tied_ = true;
}

void Channel::remove() {
    loop_->update_channel(this);
}

void Channel::update() {
    loop_->update_channel(this);
}

// 处理回调函数
void Channel::handle_event_withGuard(TimeStamp receive_time) {
    LOG_INFO("channel handleEvent revents: %d\n", real_events_);

    // 断开连接
    if ( (real_events_ & EPOLLHUP) && !(real_events_ & EPOLLIN)) {
        if (closeCallback_) {
            closeCallback_();
        }
    }

    // 发生错误
    if (real_events_ & EPOLLERR) {
        if (errorCallback_) {
            errorCallback_();
        }
    }

    // 读事件
    if (real_events_ & (EPOLLIN | EPOLLPRI)) {
        if (readCallback_) {
            readCallback_(receive_time);
        }
    }

    // 写事件
    if (real_events_ & EPOLLOUT) {
        if (writeCallback_) {
            writeCallback_();
        }
    }
}
