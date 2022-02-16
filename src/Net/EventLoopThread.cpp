#include "EventLoopThread.hpp"
#include "EventLoop.hpp"
#include  "../Log/Logger.hpp"

EventLoopThread::EventLoopThread(ThreadInitCallback cb, const std::string& name) : 
    loop_(nullptr),
    thread_(std::bind(&EventLoopThread::threadFunc, this)),
    threadCallback_(std::move(cb)) {
}

EventLoopThread::~EventLoopThread() {
    if (loop_ != nullptr) {
        loop_->quit();
        thread_.join();
    }
}

EventLoop* EventLoopThread::startLoop() {
    if (!thread_.started()) {
        thread_.start();
        {
            std::unique_lock<std::mutex> locker(m_mutex_);
            while (loop_ == nullptr) {
                noLoopConVar_.wait(locker);
            }
        }
    }
    return loop_;
}

void EventLoopThread::threadFunc() {
    EventLoop loop;
    if (threadCallback_) {
        threadCallback_(&loop);
    }
    {
        std::unique_lock<std::mutex> locker(m_mutex_);
        loop_ = &loop;
        noLoopConVar_.notify_all();
    }
    loop_->loop();
    std::unique_lock<std::mutex> locker(m_mutex_);
    loop_ = nullptr;
}