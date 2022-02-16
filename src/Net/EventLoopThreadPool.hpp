/*** 
 * @Author: Zty
 * @Date: 2022-02-15 18:11:32
 * @LastEditTime: 2022-02-15 19:47:15
 * @LastEditors: Zty
 * @Description: 事件线程池
 * @FilePath: /multhread/Net/EventLoopThreadPool.hpp
 */

#ifndef EVENTLOOPTHREADPOOL_H_
#define EVENTLOOPTHREADPOOL_H_


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

        void set_threadNum(int thread_num) { 
            thread_nums_ = thread_num;
        }

        void start(const ThreadInitCallback& cb = ThreadInitCallback());

        EventLoop* appendThread();

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