/*** 
 * @Author: Zty
 * @Date: 2022-02-13 10:04:27
 * @LastEditTime: 2022-02-13 10:10:37
 * @LastEditors: Zty
 * @Description: 时间戳
 * @FilePath: /multhread/Timer/TimeStamp.hpp
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
        std::string to_string();
    private:
        int64_t times_;
};


#endif