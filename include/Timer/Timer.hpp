#ifndef TIMER_H_
#define TIMER_H_

// 定时器实现

#include <stdint.h>
#include <sys/time.h>
#include <functional>
#include <atomic>
#include "../Base/NonCopyable.hpp"
#include "../Base/Atomic.hpp"
#include "TimeStamp.hpp"

class Timer : NoCopyable {
    public:
        using TimerCallback = std::function<void()>;
    public:
        Timer(TimerCallback cb, TimeStamp when, double interval);
        void run() const {timerCallbac_();}
        void restart(TimeStamp now);
        TimeStamp expiration() const {return m_expiration;}
        bool repeat() const { return m_repeat;}
        int64_t sequence() const {
            return sequence_;
        }
        static int64_t numCreated() {
            return s_numCreated_.get();
        }

    private:
        const TimerCallback timerCallbac_;
        TimeStamp m_expiration;
        const double m_interval;
        const bool m_repeat;
        const int64_t sequence_;

        static AtomicInt64 s_numCreated_;
};

#endif