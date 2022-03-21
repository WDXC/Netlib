#include "Channel.hpp"

#include <assert.h>
#include <sys/epoll.h>

#include <sstream>

#include "EventLoop.hpp"
#include "Log.hpp"
#include "SocketOps.hpp"

const int Channel::k_none_event_ = 0;
const int Channel::k_read_event_ = EPOLLIN | EPOLLPRI;
const int Channel::k_write_event_ = EPOLLOUT;

Channel::Channel(EventLoop* loop, int fd) : loop_(loop),
                                            fd_(fd),
                                            events_(0),
                                            real_events_(0),
                                            index_(-1),
                                            eventHandling_(false),
                                            addedToLoop_(false),
                                            logHup_(true),
                                            tied_(false) {
}

Channel::~Channel() {
    assert(!eventHandling_);
    assert(!addedToLoop_);
    if (loop_->is_in_loopThread()) {
        assert(!loop_->has_channel(this));
    }
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

void Channel::update() {
    addedToLoop_ = true;
    loop_->update_channel(this);
}

void Channel::remove() {
    assert(is_none_event());
    addedToLoop_ = false;
    loop_->remove_channel(this);
}

std::string Channel::reventToString() const {
    return eventToString(fd_, real_events_);
}

std::string Channel::eventsToString() const {
    return eventToString(fd_, events_);
}

std::string Channel::eventToString(int fd, int ev) {
    std::ostringstream oss;
    oss << fd << ": ";
    if (ev & EPOLLIN)
        oss << "IN ";
    if (ev & EPOLLPRI)
        oss << "PRI ";
    if (ev & EPOLLOUT)
        oss << "OUT ";
    if (ev & EPOLLHUP)
        oss << "HUP ";
    if (ev & EPOLLRDHUP)
        oss << "RDHUP ";
    if (ev & EPOLLERR)
        oss << "ERR ";
    return oss.str();
}

// 处理回调函数
void Channel::handle_event_withGuard(TimeStamp receive_time) {
    eventHandling_ = true;
    LOG_DEBUG("%s", reventToString().c_str());

    // 连接被挂起
    if ((real_events_ & EPOLLHUP) && !(real_events_ & EPOLLIN)) {
        if (logHup_) {
            LOG_WARN("fd = %d Channel::handle_event() EPOLLHUP", fd_);
        }
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
    eventHandling_ = false;
}
