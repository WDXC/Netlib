#ifndef TIMER_H_
#define TIMER_H_

// 定时器实现

#include <stdint.h>
#include <sys/time.h>
#include <functional>
#include "../Base/NonCopyable.hpp"
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

    private:
        const TimerCallback timerCallbac_;
        TimeStamp m_expiration;
        const double m_interval;
        const bool m_repeat;
};

#endif