#include "TimeStamp.hpp"
#include <ctime>


TimeStamp::TimeStamp() : m_microSecondsSinceEpoch(0) {

}

TimeStamp::TimeStamp(int64_t times) : m_microSecondsSinceEpoch(times) {

}

TimeStamp TimeStamp::now() {
    return TimeStamp(time(nullptr));
}

void TimeStamp::swap(TimeStamp& that) {
    std::swap(m_microSecondsSinceEpoch, that.m_microSecondsSinceEpoch);
}

std::string TimeStamp::to_string() {
    char buffer[BUFFER_SIZE] = {0};
    tm* times = localtime(&m_microSecondsSinceEpoch);
    snprintf(buffer, BUFFER_SIZE, "%4d-%02d-%02d %02d:%02d:%02d",
             times->tm_year+1900,
             times->tm_mon+1,
             times->tm_mday,
             times->tm_hour,
             times->tm_min,
             times->tm_sec);
    return buffer;
}

std::string TimeStamp::toFormattedString(bool showMicroseconds) const {
      char buf[64] = {0};
  time_t seconds = static_cast<time_t>(m_microSecondsSinceEpoch / kMicroSecondsPerSecond);
  struct tm tm_time;
  gmtime_r(&seconds, &tm_time);

  if (showMicroseconds)
  {
    int microseconds = static_cast<int>( m_microSecondsSinceEpoch % kMicroSecondsPerSecond);
    snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
             tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
             tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
             microseconds);
  }
  else
  {
    snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
             tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
             tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
  }
  return buf;
}