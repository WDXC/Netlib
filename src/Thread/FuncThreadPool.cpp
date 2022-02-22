#include "FuncThreadPool.hpp"
#include "../Log/Log.hpp"

FuncThreadPool::FuncThreadPool(int threadNumber, int maxRequests) : 
    m_threadNumber(threadNumber),
    m_maxRequests(maxRequests),
    m_stop(false) {
    if (m_threadNumber <= 0 || m_threadNumber > m_maxRequests) {
        throw std::exception();
    }

    for (int i = 0; i < m_threadNumber; ++i) {
        LOG_DEBUG("FuncThreadPool: create %d thread", i+1);
        std::thread([this] {
            FuncThreadPool::entryFunc(this);
        }).detach();
    }
}

FuncThreadPool::~FuncThreadPool() {
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_stop = true;
    }
    m_Convar.notify_all();
}

bool FuncThreadPool::append(FuncThreadPool::Task task) {
    if (m_workQueue.size() > m_maxRequests) {
        LOG_INFO("FuncThreadPool: work queue is full");
        return false;
    }
    {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_workQueue.push(task);
    }
    m_Convar.notify_one();
    return true;
}

void* FuncThreadPool::entryFunc (void* arg) {
    FuncThreadPool* ptr = static_cast<FuncThreadPool*> (arg);
    ptr->run();
    return nullptr;
}

void FuncThreadPool::run () {
    std::unique_lock<std::mutex> locker(m_mutex);
    while (!m_stop) {
        m_Convar.wait(locker);
        if (!m_workQueue.empty()) {
            Task task = m_workQueue.front();
            m_workQueue.pop();
            task();
        }
    }
}