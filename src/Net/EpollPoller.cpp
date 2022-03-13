#include "EpollPoller.hpp"
#include <errno.h>
#include <assert.h>
#include <cstring>
#include <unistd.h>
#include "Log.hpp"
#include "Channel.hpp"

const int k_new = -1;
const int k_added = 1;
const int k_deleted = 2;

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
    LOG_INFO("func = %s fd = %d events = %d index = %d \n", 
             __FUNCTION__, channel->get_fd(), 
             channel->get_events(), index);
    if (index == k_new || index == k_deleted) {
        int sockfd = channel->get_fd();
        if (index == k_new) {
            // 添加
            channel_[sockfd] = channel;
        }
        else {
            assert(channel_.find(sockfd) != channel_.end());
            assert(channel_[sockfd] == channel);
        }
        channel->set_index(k_added);
        update(EPOLL_CTL_ADD, channel);
    } else { // 注册过
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

    channel_.erase(sockfd);
    LOG_INFO("func = %s fd = %d events = %d index = %d \n",__FUNCTION__,
             channel->get_fd(), channel->get_events(), index);
    assert(index == k_added || index == k_deleted);
    if (index == k_added) {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(k_new);
}

TimeStamp EpollPoller::poll(int timeout, ChannelList* active_channels) {
    int events_num = epoll_wait(epollfd_, &*events_.begin(), 
                                static_cast<int>(events_.size()),timeout);
    int save_errno = errno;

    TimeStamp now(TimeStamp::now());
    if (events_num > 0) {
        // LOG_INFO("%d events happened \n", events_num);
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
        channel->set_revent(events_[i].events);
        active_channels->push_back(channel);
    }
}

void EpollPoller::update(int operation, Channel* channel) {
    int sockfd = channel->get_fd();

    epoll_event event;
    memset(&event, 0, sizeof(event));
    event.events = channel->get_events();
    event.data.fd = sockfd;
    event.data.ptr = channel;

    if (epoll_ctl(epollfd_, operation, sockfd, &event) < 0) {
        if (operation == EPOLL_CTL_DEL) {
            LOG_ERROR("epoll_ctl del error: %d \n", errno);
        } else {
            LOG_ERROR("epoll_ctl add/mod error:%d\n", errno);
        }
    }
}
