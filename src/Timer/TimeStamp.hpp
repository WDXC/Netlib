/*** 
 * @Author: Zty
 * @Date: 2022-02-13 10:04:27
 * @LastEditTime: 2022-02-19 20:37:45
 * @LastEditors: Zty
 * @Description: 时间戳
 * @FilePath: /multhread/src/Timer/TimeStamp.hpp
 */

#ifndef TIMESTAMP_H_
#define TIMESTAMP_H_


#include <string>
#include <iostream>

class TimeStamp {
    public:
        TimeStamp();
        explicit TimeStamp(int64_t times);
        static TimeStamp now();
        void swap(TimeStamp& that);
        std::string toFormattedString(bool showMicroseconds = true) const;
        std::string to_string();
    private:
        int64_t times_;
};


#endif