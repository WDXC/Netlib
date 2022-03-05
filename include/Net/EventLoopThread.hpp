#ifndef EVENTLOOPTHREAD_H_
#define EVENTLOOPTHREAD_H_

// 事件循环线程

#include <functional>
#include <mutex>
#include <condition_variable>
#include "../Base/NonCopyable.hpp"
#include "../Thread/ThreadObject.hpp"

class EventLoop;

class EventLoopThread : NoCopyable {
    public:
        using ThreadInitCallback = std::function<void(EventLoop*)>;

        explicit EventLoopThread(ThreadInitCallback cb = ThreadInitCallback(),
                                 const std::string& name = std::string());
        ~EventLoopThread();
        // 启动线程
        EventLoop* startLoop();
    private:
        void threadFunc();  // 执行回调

    private:
        EventLoop* loop_;
        ThreadObject thread_;
        ThreadInitCallback threadCallback_;
        std::mutex m_mutex_;
        std::condition_variable noLoopConVar_;

};



#endif