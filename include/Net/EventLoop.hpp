#ifndef EVENTLOOP_H_
#define EVENTLOOP_H_

// 事件循环

#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

#include "../Base/Callbacks.hpp"
#include "../Base/NonCopyable.hpp"
#include "../Timer/TimerId.hpp"
#include "../Timer/TimerQueue.hpp"
#include "CurrentThread.hpp"
#include "Channel.hpp"
#include "Poller.hpp"

using Functor = std::function<void()>;
using ChannelList = std::vector<Channel*>;

class EventLoop : NoCopyable {
   public:
    EventLoop();
    ~EventLoop();

    // 开启事件循环
    void loop();
    // 关闭事件循环
    void quit();

    TimeStamp get_poll_returnTime() const {
        return poll_return_time_;
    }

    // 在当前loop中执行cb
    void run_in_loop(Functor cb);
    // 将cb放入队列中，唤醒loop所在线程执行cb(pending_functor)
    void queue_in_loop(Functor cb);

    void wakeup();

    // poller 方法
    void update_channel(Channel* channel);
    void remove_channel(Channel* channel);
    bool has_channel(Channel* channel);

    //Timer 方法
    TimerId runAt(TimeStamp time, TimerCallback cb);
    TimerId runAfter(double delay, TimerCallback cb);
    TimerId runEvery(double delay, TimerCallback cb);
    void cancel(TimerId timerId);

    bool assertInLoopThread() {
        if (!is_in_loopThread()) {
            abortNoInLoopThread();
            return false;
        }
        return true;
    }

    // 判断eventloop对象是否在自己的线程中
    bool is_in_loopThread() const {
        return threadId_ == CurrentThread::tid();
    }

   private:
    void handle_read();
    void do_pending_fucntor();
    void abortNoInLoopThread();

    void printActiveChannel() const ;

   private:
    std::atomic_bool looping_;
    std::atomic_bool quit_;  // 退出循环标志
    std::atomic_bool eventHandling_;

    const pid_t threadId_;        // 记录当前loop所在线程id
    TimeStamp poll_return_time_;  // poller返回的发生事件时间点
    std::unique_ptr<Poller> poller_;
    std::unique_ptr<TimerQueue> timerQueue_;

    int wakeup_fd;                             // 当main loop获取一个新用户的channel,通过轮询，选择一个subloop,通过该成员唤醒subloop,外理channel
    std::unique_ptr<Channel> wakeup_channel_;  // 包装wakefd

    ChannelList active_channels;  // eventloop 所管理的所有channel;
    Channel* currentActiveChannel_;

    std::atomic_bool calling_pending_functors_;  // 标识当前loop是否有需要执行的回调操作
    int64_t iteration_;
    std::mutex functor_mutex_;
    std::vector<Functor> pending_Functors_;       // loop所执行的所有回调操作
};

#endif
