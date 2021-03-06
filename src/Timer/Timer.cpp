#include "Timer.hpp"

AtomicInt64 Timer::s_numCreated_;

Timer::Timer(TimerCallback cb, TimeStamp when, double interval) : 
    timerCallbac_(std::move(cb)),
    m_expiration(when),
    m_interval(interval),
    m_repeat(interval > 0),
    sequence_(s_numCreated_.incrementAndGet()) {
}

void Timer::restart(TimeStamp now) {
    if (m_repeat) {
        m_expiration = AddTime(now, m_interval);
    } else {
        m_expiration = TimeStamp::invaild();
    }
}