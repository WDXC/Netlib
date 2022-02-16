/*** 
 * @Author: Zty
 * @Date: 2022-02-13 12:54:07
 * @LastEditTime: 2022-02-13 13:10:30
 * @LastEditors: Zty
 * @Description: 判断是否为当前线程
 * @FilePath: /multhread/Event/CurrentThread.h
 */

#ifndef CURRENTTHREAD_H_
#define CURRENTTHREAD_H_


#include <unistd.h>
#include <sys/syscall.h>

namespace CurrentThread
{
    extern __thread int t_cachedTid;

    __thread int t_cachedTid = 0;

    void cache_tid() {
        if (t_cachedTid == 0) {
            t_cachedTid = static_cast<pid_t>(syscall(SYS_getpid));
        }
    }

    inline int tid() {
        if (__builtin_expect(t_cachedTid == 0, 0)) {
            cache_tid();
        }
        return t_cachedTid;
    }
} // namespace CurrentThread



#endif