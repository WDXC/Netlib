#include "SqlConnPool.hpp"
#include "Log.hpp"

SqlConnPool::SqlConnPool() {

}

SqlConnPool::~SqlConnPool() {
    ClosePool();
}

void SqlConnPool::Init(const std::string& host, int port,
                   const std::string& user, const std::string& pwd,
                   const std::string& db_name, int conn_size) {
    for (int i = 0; i < m_connQue.size(); ++i) {
        MYSQL* sql = nullptr;
        sql = mysql_init(sql);
        if (!sql) {
            LOG_ERROR("Mysql init error");
        }
        sql = mysql_real_connect(sql, host.c_str(), user.c_str(), pwd.c_str(), db_name.c_str(), port, nullptr, 0);
        if (!sql) {
            LOG_ERROR("Mysql connection error");
        }
        m_connQue.push(sql);
    }
    m_max_conn = conn_size;
}

// 连接池在释放时，需要先清空整个队列,再关闭数据库连接
void SqlConnPool::ClosePool() {
    std::unique_lock<std::mutex> locker(m_mutex);
    while (!m_connQue.empty()) {
        auto item = m_connQue.front();
        m_connQue.pop();
        mysql_close(item);
    }
    mysql_library_end();
}

// 获取数据库对象，即从整个数据连接队列中取出一个对象
MYSQL* SqlConnPool::getConnObj() {
    MYSQL* sql = nullptr;
    if (m_connQue.empty()) {
        LOG_WARN("SqlConnPool busy");
        return nullptr;
    }


    {
        std::unique_lock<std::mutex> locker(m_mutex);
        sql = m_connQue.front();
        m_connQue.pop();
    }
    return sql;
}

// 释放，就是将连接送回原先的连接队列中
void SqlConnPool::FreeConnObj(MYSQL* conn) {
    if (!conn) {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_connQue.push(conn);
    }
}
