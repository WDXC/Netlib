#ifndef LOGGER_H_
#define LOGGER_H_

#include <string>
#include <mutex>
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <sys/stat.h>
#include <sys/time.h>
#include "BlockQueue.hpp"
#include "../Base/NonCopyable.hpp"
#include "../Base/Singleton.hpp"

class ThreadObject;

class Log {
    public: 
        enum {
            DEBUG,
            INFO,
            WARN,
            ERROR,
        };
        // 生成日志文件
        class SourceFile {
            public:
                // 根据传入的文件名，为其生成对应的日志源文件
                explicit SourceFile(const char* filename) : data_(filename) {
                    const char* slash = strrchr(filename, '/');
                    if (slash) data_ = slash + 1;
                    size_ = static_cast<int> (strlen(data_));
                }
                const char* data_;
                int size_;
        };

        Log();
        virtual ~Log();
        void init(const char* path = "./log/",
                  int split_lines = 5000000,
                  int max_queue_size = 1000);
        // 刷新日志线程
        static void FlushLogThread();
        // 将传入的数据，以对应的格式写入
        void write(int level, SourceFile file, int line, const char* format, ...);
        // 刷新buffer缓冲区
        void flush();
        bool isOpen() const { 
            return m_isOpen; 
        }

    private:
        // 异步写入
        void AsyncWrite();
        // 刷新缓冲区
        void FlushBuff();

    private:
        static const int LOG_NAME_LEN = 256;    // 日志文件名长度
        static const int LOG_BUF_SIZE = 8192;   // 日志缓冲区大小

    private:
        int m_isOpen;           // 是否开启日志
        int m_lineMax;           // 日志最大行数 
        int m_lineCount;        // 已写日志行数
        const char* m_path;     // 日志文件路径 
        int m_today;            // 当前时间的天
        char* m_buf;            // 临时日志缓冲区
        std::string m_logStr;   // 日志缓冲区

        FILE* m_fp;
        std::unique_ptr<BlockQueue<std::string> > m_dequePtr;
        std::unique_ptr<ThreadObject> m_logThreadPtr;
        std::mutex m_mutex;
};

using g_LogMgr = Singleton<Log>;

#define LOG_BASE(level, format, ...) {                                                  \
    Log* log = g_LogMgr::instance();                                                    \
    if (log->isOpen()) {                                                                \
        log->write(level, Log::SourceFile(__FILE__), __LINE__, format, ##__VA_ARGS__);  \
        log->flush();                                                                   \
    }                                                                                   \
} while (false);

#define LOG_DEBUG(format, ...) LOG_BASE(Log::DEBUG, format, ##__VA_ARGS__);
#define LOG_INFO(format, ...) LOG_BASE(Log::INFO, format, ##__VA_ARGS__);
#define LOG_WARN(format, ...) LOG_BASE(Log::WARN, format, ##__VA_ARGS__);
#define LOG_ERROR(format, ...) LOG_BASE(Log::ERROR, format, ##__VA_ARGS__);

#endif
