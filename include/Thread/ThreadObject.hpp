#ifndef THREADOBJECT_H_
#define THREADOBJECT_H_

// 基于pthread封装的函数线程池

#include <functional>
#include <pthread.h>
#include "../Log/Log.hpp"

class ThreadObject {
    public:
        using ThreadFunc = std::function<void()>;

        explicit ThreadObject (ThreadFunc);
        ~ThreadObject();

        void start();
        void join();
        void cancel();
        bool started() const {return m_isStarted; }
    private:
        pthread_t m_pthreadId;
        bool m_isStarted;
        bool m_isJoined;
        ThreadFunc m_func;
        static void* run(void* obj);
};


#endif