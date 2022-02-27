#ifndef TIMEQUEUE_H_
#define TIMEQUEUE_H_

#include <set>
#include <vector>
#include <utility> // make_pair
#include "../Base/NonCopyable.hpp"
#include "../Net/EventLoop.hpp"
#include "../Net/Channel.hpp"
#include "Timer.hpp"

class TimerQueue : NoCopyable {
    public:
        using TimerCallback = std::function<void()>;
    
    public:
        explicit TimerQueue(EventLoop* loop);
        ~TimerQueue();

        void addTimer(TimerCallback cb, TimeStamp when, double interval);

    private:
        using Entry = std::pair<TimeStamp, std::shared_ptr<Timer>>;
        using TimerList = std::set<Entry>;

    private:
        void addTimerInLoop(Timer* timer);
        bool insert(Timer* timer);

        void handleRead();
        std::vector<Entry> getExpired(TimeStamp now);
        void reset(const std::vector<Entry>& expired, TimeStamp now);

    private:
        EventLoop* m_loop;
        const int m_timerfd;
        Channel m_timerfdChannel;
        TimerList m_timers;
};



#endif