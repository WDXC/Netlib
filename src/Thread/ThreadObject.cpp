#include "ThreadObject.hpp"
#include "CurrentThread.hpp"
#include "TimeStamp.hpp"
#include <sys/syscall.h>
#include <unistd.h>

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

namespace produce{
    pid_t gettid() {
        return static_cast<pid_t>(::syscall(SYS_gettid));
    }
}  // namespace detail


void CurrentThread::cacheTid() {
    if (t_cachedTid == 0) {
        t_cachedTid = produce::gettid();
        t_tidStringLength = snprintf(t_tidString, sizeof(t_tidString), "%5d", t_cachedTid);
    }
}

bool CurrentThread::isMainThread() {
    return tid() == ::getpid();
}

void CurrentThread::sleepUsec(int64_t usec) {
    struct timespec ts = {0, 0};
    ts.tv_sec = static_cast<time_t>(usec / TimeStamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>(usec % TimeStamp::kMicroSecondsPerSecond * 1000);
    ::nanosleep(&ts, NULL);
}