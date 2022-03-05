#ifndef EVENTLOOPTHREADPOOL_H_
#define EVENTLOOPTHREADPOOL_H_

// 事件循环线程池 (sub Reactor)

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "../Base/NonCopyable.hpp"

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : NoCopyable {
    public:
        using ThreadInitCallback = std::function<void(EventLoop*)>;

    public:
        EventLoopThreadPool(EventLoop* baseloop, const std::string& name);
        ~EventLoopThreadPool();

        // 设置线程总数
        void set_threadNum(int thread_num) { 
            thread_nums_ = thread_num;
        }

        // 启动线程
        void start(const ThreadInitCallback& cb = ThreadInitCallback());

        // 添加线程
        EventLoop* appendThread();

        // 获取所有循环事件 
        std::vector<EventLoop*> get_allLoops();

        bool get_started() const {return started_;}
        std::string get_name() const {return name_;}

    private:
        EventLoop* baseloop_;
        std::string name_;

        bool started_;
        int thread_nums_;
        int next_;
        std::vector<std::unique_ptr<EventLoopThread>> thread_;
        std::vector<EventLoop*> loops_;
};

#endif