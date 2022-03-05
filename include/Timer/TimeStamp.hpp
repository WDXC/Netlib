#ifndef TIMESTAMP_H_
#define TIMESTAMP_H_

// 时间戳

#include <string>
#include <iostream>

class TimeStamp {
    public:
        TimeStamp();
        explicit TimeStamp(int64_t times);
        static TimeStamp now();
        void swap(TimeStamp& that);
        static TimeStamp invaild() {return TimeStamp();}
        int64_t MicroSecondsSinceEpoch() const {return m_microSecondsSinceEpoch;}
        std::string toFormattedString(bool showMicroseconds = true) const;
        std::string to_string();
        bool valid() const {return m_microSecondsSinceEpoch > 0;}

    public:
        static const int kMicroSecondsPerSecond = 1000 * 1000; // 微秒
        static const int BUFFER_SIZE = 128;

    private:
        int64_t m_microSecondsSinceEpoch;

};

inline bool operator<(TimeStamp lhs, TimeStamp rhs) {
    return lhs.MicroSecondsSinceEpoch() < rhs.MicroSecondsSinceEpoch();
}

inline bool operator==(TimeStamp lhs, TimeStamp rhs) {
    return lhs.MicroSecondsSinceEpoch() == rhs.MicroSecondsSinceEpoch();
}

inline TimeStamp AddTime(TimeStamp timestamp, double seconds) {
    int64_t delta = static_cast<int64_t>(seconds * TimeStamp::kMicroSecondsPerSecond);
    return TimeStamp(timestamp.MicroSecondsSinceEpoch() + delta);
}

#endif