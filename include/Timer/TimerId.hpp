#ifndef TIMERID_H_
#define TIMERID_H_

#include <stdint.h>

class Timer;

class TimerId {
   public:
    TimerId() : timer_(nullptr),
                sequence_(0) {
    }
    TimerId(Timer* timer, int64_t seq) : timer_(timer),
                                         sequence_(seq) {
    }

    friend class TimerQueue;

   private:
    Timer* timer_;
    int64_t sequence_;
};

#endif