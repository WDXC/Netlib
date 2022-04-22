#include "EpollPoller.hpp"
#include <errno.h>
#include <assert.h>
#include <cstring>
#include <unistd.h>
#include "Log.hpp"
#include <poll.h>
#include "Channel.hpp"

const int k_new = -1;
const int k_added = 1;
const int k_deleted = 2;

static_assert(EPOLLIN == POLLIN,        "epoll uses same flag values as poll");
static_assert(EPOLLPRI == POLLPRI,      "epoll uses same flag values as poll");
static_assert(EPOLLOUT == POLLOUT,      "epoll uses same flag values as poll");
static_assert(EPOLLRDHUP == POLLRDHUP,  "epoll uses same flag values as poll");
static_assert(EPOLLERR == POLLERR,      "epoll uses same flag values as poll");
static_assert(EPOLLHUP == POLLHUP,      "epoll uses same flag values as poll");

EpollPoller::EpollPoller(EventLoop* loop) : 
    Poller(loop),
    epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
    events_(k_init_eventlist_size) {
    if (epollfd_ < 0) {
        LOG_WARN("epoll_create1 error: %d\n", errno);
    }
}

EpollPoller::~EpollPoller() {
    ::close(epollfd_);
}

void EpollPoller::update_channel(Channel* channel) {
    // 增加或删除
    const int index = channel->index();
    LOG_INFO("func = %s fd = %d events = %d index = %d", 
             __FUNCTION__, channel->get_fd(), 
             channel->get_events(), index);
    if (index == k_new || index == k_deleted) {
        int sockfd = channel->get_fd();
        if (index == k_new) { // new fd add, use by EPOLL_CTL_ADD
            // 添加
            assert(channel_.find(sockfd) == channel_.end());
            channel_[sockfd] = channel;
        }
        else { // index = k_delete
            assert(channel_.find(sockfd) != channel_.end());
            assert(channel_[sockfd] == channel);
        }
        channel->set_index(k_added);
        update(EPOLL_CTL_ADD, channel);
    } else { // 已经注册过，使用EPOLL_CTL_MOD/DEL
        int sockfd = channel->get_fd();
        (void)sockfd;
        assert(channel_.find(sockfd) != channel_.end());
        assert(channel_[sockfd] == channel);
        assert(index == k_added);
        if (channel->is_none_event()) {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(k_deleted);
        } else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EpollPoller::remove_channel(Channel* channel) {
    int sockfd = channel->get_fd();

    assert(channel_.find(sockfd) != channel_.end());
    assert(channel_[sockfd] == channel);
    assert(channel->is_none_event());

    int index = channel->index();
    assert(index == k_added || index == k_deleted);

    size_t n = channel_.erase(sockfd);
    (void) n;
    assert(n == 1);

    if (index == k_added) {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(k_new);
}

TimeStamp EpollPoller::poll(int timeout, ChannelList* active_channels) {
    LOG_INFO("fd total count %d", channel_.size());
    int events_num = epoll_wait(epollfd_, 
                                &*events_.begin(), 
                                static_cast<int>(events_.size()),
                                timeout);
    int save_errno = errno;
    TimeStamp now(TimeStamp::now());
    if (events_num > 0) {
        LOG_INFO("%d events happended", events_num);
        fill_active_channel(events_num, active_channels);
        if (events_num == events_.size()) {
            events_.resize(events_.size() * 2);
        }
    } else if (events_num == 0) {
        LOG_DEBUG("no events happened");
    } else {
        if (save_errno != EINTR) {
            errno = save_errno;
            LOG_ERROR("EpollPoller::poll error!");
        }
    }
    return now;
}

void EpollPoller::fill_active_channel (int events_num, ChannelList* active_channels) const {
    for (int i = 0; i < events_num; ++i) {
        Channel* channel = static_cast<Channel*> (events_[i].data.ptr);
        int fd = channel->get_fd();
        ChannelMap::const_iterator it = channel_.find(fd);
        assert(it != channel_.end());
        assert(it->second == channel);

        channel->set_revent(events_[i].events);
        active_channels->push_back(channel);
    }
}

void EpollPoller::update(int operation, Channel* channel) {

    struct epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = channel->get_events();
    event.data.ptr = channel;
    int sockfd = channel->get_fd();

    LOG_DEBUG("epoll_ctl_op = %s, fd = %d, event = { %s }", 
               operationToString(operation),
               sockfd, channel->eventsToString().c_str());

    if (epoll_ctl(epollfd_, operation, sockfd, &event) < 0) {
        if (operation == EPOLL_CTL_DEL) {
            LOG_ERROR("epoll_ctl op = %s, fd = %d", 
                      operationToString(operation), sockfd);
        } else {
            LOG_ERROR("epoll_ctl op = %s, fd = %d",
                      operationToString(operation), sockfd);
        }
    }
}

const char* EpollPoller::operationToString (int op) {
    switch (op)
    {
    case EPOLL_CTL_ADD:
        return "ADD";
    case EPOLL_CTL_DEL:
        return "DEL";
    case EPOLL_CTL_MOD:
        return "MOD";
    default:
        assert(false && "ERROR OP");
        return "Unknown Operation";
    }
}
