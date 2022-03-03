#ifndef FUNCTHREADPOOL_H_
#define FUNCTHREADPOOL_H_

// 函数线程池

#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

class FuncThreadPool {
    public:
        using Task = std::function<void(void)>;

        explicit FuncThreadPool(int threadNumber = 5, int maxRequests = 10000);
        ~FuncThreadPool();
        // 添加线程
        bool append(Task task);
    private:
        static void* entryFunc(void* arg);
        void run();
    private:
        int m_threadNumber;                 // 线程数
        int m_maxRequests;                  // 最大任务数
        std::queue<Task> m_workQueue;       // 任务队列
        std::mutex m_mutex;                 // 互斥量
        std::condition_variable m_Convar;   // 条件变量
        bool m_stop;                        // 线程池是否执行
};


#endif