/*** 
 * @Author: Zty
 * @Date: 2022-02-13 11:14:02
 * @LastEditTime: 2022-02-17 08:14:09
 * @LastEditors: Zty
 * @Description: 线程对象
 * @FilePath: /multhread/src/Thread/ThreadObject.hpp
 */

#ifndef THREADOBJECT_H_
#define THREADOBJECT_H_

#include <functional>
#include <pthread.h>
#include "../Log/Logger.hpp"

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