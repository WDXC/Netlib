#include "../Thread/ThreadObject.hpp"
#include "Logger.hpp"

Log::Log () :
    m_isOpen(false),
    m_lineCount(0),
    m_today(0),
    m_dequePtr(nullptr),
    m_logThreadPtr(nullptr) {

}

Log::~Log() {
    if (m_logThreadPtr) {
        m_dequePtr->PushBack(m_logStr); // 将未写入的缓冲区加入到队列中
        while (!m_dequePtr->isEmpty()) m_dequePtr->Flush();
        m_dequePtr->Close();
        m_logThreadPtr->join(); // 析构时显示调用
    }
    if (m_fp) {
        std::lock_guard<std::mutex> locker(m_mutex);
        fclose(m_fp);
    }
}

void Log::init(const char* path, int split_line, int max_queue_size) {
    if (m_isOpen) return;

    m_dequePtr = std::make_unique<BlockQueue<std::string>>(max_queue_size);
    m_logThreadPtr = std::make_unique<ThreadObject>(FlushLogThread);
    m_logThreadPtr->start();
    m_isOpen = true;
    m_lineMax = split_line;
    m_path = path;
    m_buf = new char[LOG_BUF_SIZE];
    memset(&m_buf, '\0', LOG_BUF_SIZE);

    // 获取当前时间 
    time_t  timer = time(nullptr);
    struct tm *systime  = localtime (&timer);
    struct tm time = *systime;

    char filename[LOG_NAME_LEN] = {0};
    snprintf(filename, LOG_NAME_LEN - 1, "%s/%04d%02d%02d%s",
             m_path, time.tm_year+1900, time.tm_mon+1, time.tm_mday, ".log");
    m_today = time.tm_mday;

    {
        std::lock_guard<std::mutex> locker(m_mutex);
        m_fp = fopen(filename, "a");
        if (m_fp == nullptr) {
            mkdir(m_path, 0777);
            m_fp = fopen(filename, "a");
        }
    }
}

void Log::write(int level, SourceFile file, int line ,const char* format, ...) {
    if (!m_isOpen) return;
    struct timeval now = {0, 0};
    gettimeofday(&now, nullptr);
    time_t timer = now.tv_sec;
    struct tm* sysTime = localtime(&timer);
    struct tm time = *sysTime;
    va_list valist;

    std::string logType;

    switch (level) {
        case DEBUG: 
            logType = "[DEBUG]:";
            break;
        case INFO:
            logType = "[INFO]:";
            break;
        case WARN: 
            logType = "[WARN]:";
            break;
        case ERROR:
            logType = "[ERROR]:";
            break;
    }

    if (m_today != time.tm_mday || (m_lineCount && (m_lineCount % m_lineMax == 0))) {
        char newFile[LOG_NAME_LEN] = {0};
        char tail[16] = {0};
        snprintf(tail, 16, "%04d%02d%02d", time.tm_year + 1900,
                 time.tm_mon+1, time.tm_mday);
        
        if (m_today != time.tm_mday) { //超过24点
            snprintf(newFile, LOG_NAME_LEN-1, "%s/%s%s", m_path, tail, ".log");
            m_today = time.tm_mday;
            m_lineCount = 0;
        } else {
            FlushBuff();
            snprintf(newFile, LOG_NAME_LEN-1, "%s/%s_%d%s", m_path, tail, (m_lineCount / m_lineMax), ".log");
        }
        std::lock_guard<std::mutex> locker(m_mutex);
        fflush(m_fp);
        fclose(m_fp);
        m_fp = fopen(newFile, "a");
    }   // 文件名处理结束，开始写入日志 

    {
        std::lock_guard<std::mutex> locker(m_mutex);
        ++m_lineCount;
        va_start(valist, format);
        int n = snprintf(m_buf, 256, "%s %04d-%02d-%02d %02d:%02d:%02d.%06ld [%s:%d] ",
                logType.c_str(), time.tm_year + 1900, time.tm_mon + 1, time.tm_mday,
                time.tm_hour, time.tm_min, time.tm_sec, now.tv_usec, file.data_, line);
        int m = vsnprintf(m_buf + n, LOG_BUF_SIZE - 1, format, valist);
        m_buf[n+m] = '\n';
        m_buf[n+m+1] = '\0';
        m_logStr += m_buf;
        va_end(valist);

        if (m_logStr.size() >= LOG_BUF_SIZE) { // 超过日志缓冲区大小，添加到队列中
            m_dequePtr->PushBack(m_logStr);
            m_logStr = "";
        }
    }
}

void Log::AsyncWrite () {
    std::string str;
    while (m_dequePtr->PopFront(str)) {
        std::lock_guard<std::mutex> locker(m_mutex);
        fputs(str.c_str(), m_fp);
    }
}

void Log::FlushBuff () {
    std::lock_guard<std::mutex> locker(m_mutex);
    fputs(m_logStr.c_str(), m_fp);
    m_logStr = "";
}

void Log::flush() {
    std::lock_guard<std::mutex> locker(m_mutex);
    m_dequePtr->Flush();
    fflush(m_fp);
}

void Log::FlushLogThread() {
    g_LogMgr::instance()->AsyncWrite();
}