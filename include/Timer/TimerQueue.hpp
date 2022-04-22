#ifndef TIMEQUEUE_H_
#define TIMEQUEUE_H_

// 通过回调，完成定时器容器

#include <set>
#include <utility>  // make_pair
#include <vector>
#include <assert.h>

#include "TimeStamp.hpp"
#include "../Base/NonCopyable.hpp"
#include "../Net/Channel.hpp"


class EventLoop;
class Timer;
class TimerId;

class TimerQueue : NoCopyable {
   public:
    using TimerCallback = std::function<void()>;

   public:
    explicit TimerQueue(EventLoop* loop);
    ~TimerQueue();

    TimerId addTimer(TimerCallback cb, TimeStamp when, double interval);
    void cancel(TimerId timerId);

   private:
    using Entry = std::pair<TimeStamp, Timer*>;
    using TimerList = std::set<Entry>;
    using ActiveTimer = std::pair<Timer*, int64_t>;
    using ActiveTimerSet = std::set<ActiveTimer>;

   private:
    void addTimerInLoop(Timer* timer);
    void cancelInLoop(TimerId timerid);
    void handleRead();

    std::vector<Entry> getExpired(TimeStamp now);
    void reset(const std::vector<Entry>& expired, TimeStamp now);
    bool insert(Timer* timer);

   private:
    EventLoop* m_loop;
    const int m_timerfd;
    Channel m_timerfdChannel;
    TimerList m_timers;

    // for cancel()
    ActiveTimerSet activeTimers_;
    bool callingExpiredTimers_;
    ActiveTimerSet cancelingTimers_;
};

#endif