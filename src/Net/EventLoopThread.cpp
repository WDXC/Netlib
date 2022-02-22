/*** 
 * @Author: Zty
 * @Date: 2022-02-16 09:18:08
 * @LastEditTime: 2022-02-18 10:33:45
 * @LastEditors: Zty
 * @Description: 
 * @FilePath: /multhread/src/Net/EventLoopThread.cpp
 */
#include "EventLoopThread.hpp"
#include "EventLoop.hpp"
#include  "../Log/Log.hpp"

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