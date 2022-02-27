#ifndef CURRENTTHREAD_H_
#define CURRENTTHREAD_H_


#include <unistd.h>
#include <sys/syscall.h>

namespace CurrentThread
{
    extern __thread int t_cachedTid;

    void cache_tid() ;

    inline int tid() {
        if (__builtin_expect(t_cachedTid == 0, 0)) {
            cache_tid();
        }
        return t_cachedTid;
    }
} // namespace CurrentThread



#endif
