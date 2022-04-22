#include "EventLoopThread.hpp"
#include "EventLoopThreadPool.hpp"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseloop,
                                         const std::string& name) : 
    baseloop_(baseloop),
    name_(name),
    started_(false),
    thread_nums_(0),
    next_(0) {
}

EventLoopThreadPool::~EventLoopThreadPool() {

}

void EventLoopThreadPool::start(const ThreadInitCallback& cb) {
    started_ = true;
    if (thread_nums_ == 0) {
        cb(baseloop_);
    } else {
        for (int i = 0; i < thread_nums_; ++i) {
            char buffer[name_.size() + 32];
            buffer[name_.size() + 32] = {0};
            EventLoopThread* thr = new EventLoopThread(cb);
            thread_.push_back(std::unique_ptr<EventLoopThread>(thr));
            loops_.push_back(thr->startLoop());
        }
    }
}

EventLoop* EventLoopThreadPool::appendThread() {
    EventLoop* loop = baseloop_;
    if (!loops_.empty()) {
        loop = loops_[next_++];

        if (next_ >= loops_.size()) {
            next_ = 0;
        }
    }
    return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::get_allLoops() {
    if (loops_.empty()) {
        return std::vector<EventLoop*> (1, baseloop_);
    } else {
        return loops_;
    }
}