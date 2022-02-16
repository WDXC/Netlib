/*** 
 * @Author: Zty
 * @Date: 2022-02-14 12:29:58
 * @LastEditTime: 2022-02-15 16:39:07
 * @LastEditors: Zty
 * @Description: 线程池
 * @FilePath: /multhread/Thread/ThreadPool.hpp
 */

#ifndef THREADPOOL_H_
#define THREADPOOL_H_


#include <vector>
#include <queue>
#include <pthread.h>
#include <mutex>
#include <condition_variable>
#include "../Log/Logger.hpp"

template <class T>
class ThreadPool {
    public:
        explicit ThreadPool (int threadNumber = 5, int maxRequest = 10000);
        ~ThreadPool();

        bool append(T* request); // 任务添加接口

    private:
        static void* entryFunc(void* arg);
        void run();
    
    private:
        int m_threadNubmer;                             // 线程数
        int m_maxRequests;                              // 最大任务数
        std::queue<T*> m_workQueue;                     // 任务队列
        std::mutex m_mutex;                             // 互斥量
        std::condition_variable noLoopThreadConVar_;    // 条件变量
        bool m_stop;                                    // 线程池是否执行
};

template<class T>
ThreadPool<T>::ThreadPool(int threadNumber, int maxRequest) : 
    m_threadNubmer(threadNumber),
    m_maxRequests(maxRequest),
    m_stop(false) {
    if (m_threadNubmer <= 0 || m_threadNubmer > m_maxRequests) {
        throw std::exception();
    }
    for (int i = 0; i < maxRequest; ++i) {
        pthread_t pid;
        if (pthread_create(&pid, nullptr, entryFunc, static_cast<void*>(this)) == 0) {
            LOG_DEBUG("ThreadPool: create %d thread", i+ 1);
            pthread_detach(pid);
        }
    }
}

template<class T>
ThreadPool<T>::~ThreadPool() {
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_stop = true;
    }
    noLoopThreadConVar_.notify_all();
}

template <class T>
bool ThreadPool<T>::append(T* request) {
    if (m_workQueue.size() > m_maxRequests) {
        LOG_INFO("ThreadPool: Work queue is full");
        return false;
    }
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_workQueue.emplace(request);
    }
    noLoopThreadConVar_.notify_one();
    return true;
}


template<class T>
void* ThreadPool<T>::entryFunc (void* arg) {
    ThreadPool* ptr = static_cast<ThreadPool*> (arg);
    ptr->run();
    return nullptr;
}

template<class T>
// process 是传入类型所提供的函数
void ThreadPool<T>::run() {
    std::unique_lock<std::mutex> locker(m_mutex);
    while (!m_stop) {
        noLoopThreadConVar_.wait(locker);
        if (!m_workQueue.empty()) {
            T* request = m_workQueue.front();
            m_workQueue.pop();
            if (request) request->process();
        }
    }
}


#endif