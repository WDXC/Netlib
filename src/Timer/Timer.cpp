#include "Timer.hpp"

Timer::Timer(TimerCallback cb, TimeStamp when, double interval) : 
    timerCallbac_(std::move(cb)),
    m_expiration(when),
    m_interval(interval),
    m_repeat(interval > 0) {

}

void Timer::restart(TimeStamp now) {
    if (m_repeat) {
        m_expiration = AddTime(now, m_interval);
    } else {
        m_expiration = TimeStamp::invaild();
    }
}