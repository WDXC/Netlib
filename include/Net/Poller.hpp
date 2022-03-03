#ifndef POLLER_H_
#define POLLER_H_

// 对于 POLL 与 EPOLL的封装

#include "../Base/NonCopyable.hpp"
#include "Channel.hpp"
#include <vector>
#include <unordered_map>

class EventLoop;

using ChannelList = std::vector<Channel*>;
using ChannelMap = std::unordered_map<int, Channel*>;

class Poller : NoCopyable {
    public:
        Poller(EventLoop* loop);
        virtual ~Poller() = default;

        // 所有IO征用保留统一接口
        virtual TimeStamp poll(int timeout, ChannelList* active_channels) = 0;


        // 判断channel是否在当前poller中
        bool has_channel(Channel* channel) const ;
        
        // 获取这个事件循环的poller;
        static Poller* new_deafultPoller(EventLoop* loop);
        virtual void update_channel(Channel* channel) = 0;
        virtual void remove_channel(Channel* channel) = 0;
    protected:
        ChannelMap channel_;
    private:
        EventLoop* owner_loop_;
};

#endif