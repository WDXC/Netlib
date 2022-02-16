/*** 
 * @Author: Zty
 * @Date: 2022-02-13 10:00:33
 * @LastEditTime: 2022-02-13 10:06:43
 * @LastEditors: Zty
 * @Description: 不可拷贝基类
 * @FilePath: /multhread/Base/NonCopyable.hpp
 */

#ifndef NONCOPYABLE_H_
#define NONCOPYABLE_H_

class NoCopyable {
    protected:
        NoCopyable() = default;
        ~NoCopyable() = default;
        NoCopyable(const NoCopyable& rhs) = delete;
        NoCopyable& operator=(const NoCopyable& rhs) = delete;
};

#endif