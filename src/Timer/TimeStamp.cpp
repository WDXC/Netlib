#include "TimeStamp.hpp"
#include <ctime>

static const int BUFFER_SIZE = 128;

TimeStamp::TimeStamp() : times_(0) {

}

TimeStamp::TimeStamp(int64_t times) : times_(times) {

}

TimeStamp TimeStamp::now() {
    return TimeStamp(time(nullptr));
}

std::string TimeStamp::to_string() {
    char buffer[BUFFER_SIZE] = {0};
    tm* times = localtime(&times_);
    snprintf(buffer, BUFFER_SIZE, "%4d-%02d-%02d %02d:%02d:%02d",
             times->tm_year+1900,
             times->tm_mon+1,
             times->tm_mday,
             times->tm_hour,
             times->tm_min,
             times->tm_sec);
    return buffer;
}