#ifndef EPOLLERPOLLER_H_
#define EPOLLERPOLLER_H_

// Epoll IO复用

#include <sys/epoll.h>
#include <vector>
#include "Poller.hpp"

using EventList = std::vector<epoll_event>;

class EpollPoller : public Poller {
    public:
        EpollPoller(EventLoop* loop);
        ~EpollPoller() override;
        TimeStamp poll(int timeout, ChannelList* active_channels) override;

        void update_channel(Channel* channel) final;
        void remove_channel(Channel* channel) final;
    private:
        // 填充活跃事件
        void fill_active_channel(int events_num,ChannelList* active_channels) const;
        // 更新channel, 调用 epoll_ctl
        void update(int operation, Channel* channel);
        static const char* operationToString(int op);
    private:
        static const int  k_init_eventlist_size = 16;
    private:
        int epollfd_;
        EventList events_;
};

#endif