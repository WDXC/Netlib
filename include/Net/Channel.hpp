#ifndef CHANNEL_H_
#define CHANNEL_H_

// IO事件分发核心

#include <functional>
#include <memory>
#include "../Base/NonCopyable.hpp"
#include "../Timer/TimeStamp.hpp"

class EventLoop;

using EventCallback = std::function<void()>;
using ReadEventCallback = std::function<void(TimeStamp)>;

class Channel : NoCopyable {
    public:
        Channel(EventLoop* loop, int fd);
        ~Channel();
        
        // fd 等到epoller通知后，根据具体发生的事件，调用相应的回调
        void handle_event(TimeStamp receive_time);
        // 防止 Channel在执行回调时，被删除
        // 一个tcpconnection新连接创建时，调用tie 
        void tie(const std::shared_ptr<void> &);

        // 得到socket套接字
        int get_fd() const {return fd_;}

        //  得到感兴趣的事件
        int get_events() const {
            return events_;
        }

        // 设置活跃事件,poller监听到事件然后设置read_event
        int set_revent(int event) {
            real_events_ = events_;
            return 0;
        }

        // 判断fd是否设置过感兴趣事件
        bool is_noneEvent(int event) {
            return events_ == k_none_event_;
        }
        
        // 返回所属eventloop
        EventLoop* ower_loop() {return loop_;}

        /// 在channel所属的eventloop中删除自己
        void remove();

        int index() { return index_; }
        void set_index(int index) { index_ = index;}

    public:
        // 设置fd感兴趣事件
        void enable_reading() {
            events_ |= k_read_event_;
            update();
        }
        void dis_enable_reading() {
            events_ &= ~k_read_event_;
            update();
        }
        void enable_writing() {
            events_ |= k_write_event_;
            update();
        }
        void dis_enable_writing() {
            events_ &= ~k_write_event_;
            update();
        }
        void dis_enable_all() {
            events_ = k_none_event_;
            update();
        }

    public:
        // 返回fd当前感兴趣事件状态
        bool is_none_event() const { return events_ == k_none_event_; }
        bool is_writing() const { return events_ & k_write_event_; }
        bool is_reading() const { return events_ & k_read_event_; }
    
    public:
        // 设置发生不同事件的回调操作
        void setReadCallback(ReadEventCallback cb) {
            readCallback_ = move(cb);
        }
        void setWriteCallback(EventCallback cb) {
            writeCallback_ = move(cb);
        }
        void setCloseCallback(EventCallback cb) {
            closeCallback_ = move(cb);
        }
        void setErrorCallback(EventCallback cb) {
            errorCallback_ = move(cb);
        }
    private: 
        void update();
        void handle_event_withGuard(TimeStamp receive_time);
    private:
        static const int k_none_event_;
        static const int k_read_event_;
        static const int k_write_event_;
    
    private:
        EventLoop* loop_;
        const int fd_;
        int events_;
        int real_events_;
        int index_;

        std::weak_ptr<void> tie_;
        bool tied_;

        ReadEventCallback readCallback_;
        EventCallback writeCallback_;
        EventCallback closeCallback_;
        EventCallback errorCallback_;
};

#endif
