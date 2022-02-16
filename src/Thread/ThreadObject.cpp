#include "ThreadObject.hpp"

ThreadObject::ThreadObject(ThreadFunc func) : 
    m_pthreadId(0),
    m_isStarted(false),
    m_isJoined(false),
    m_func(func) {

}

ThreadObject::~ThreadObject() {
    if (m_isStarted && !m_isJoined) {
        ::pthread_detach(m_pthreadId);
    }
}

void ThreadObject::start() {
    m_isStarted = true;
    if (::pthread_create(&m_pthreadId, nullptr, run, this)) {
        m_isStarted = false;
        LOG_ERROR("ThreadObject: create a new Thread failed!");
    }
}

void ThreadObject::join() {
    if (m_isStarted && !m_isJoined) {
        m_isJoined = true;
        ::pthread_join(m_pthreadId, nullptr);
    }
}

void ThreadObject::cancel() {
    if (!pthread_cancel(m_pthreadId)) {
        m_isStarted = false;
    }
}

void* ThreadObject::run (void* obj) {
    ThreadObject* ptr = static_cast<ThreadObject*> (obj);
    ptr->m_func();
    delete ptr;
    return nullptr;
}