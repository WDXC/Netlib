#include "SqlConnPool.hpp"
#include "../Log/Logger.hpp"

SqlPool::SqlPool() {

}

SqlPool::~SqlPool() {
    ClosePool();
}

void SqlPool::Init(const std::string& host, int port,
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

void SqlPool::ClosePool() {
    std::unique_lock<std::mutex> locker(m_mutex);
    while (!m_connQue.empty()) {
        auto item = m_connQue.front();
        m_connQue.pop();
        mysql_close(item);
    }
    mysql_library_end();
}

MYSQL* SqlPool::getConnObj() {
    MYSQL* sql = nullptr;
    if (m_connQue.empty()) {
        LOG_WARN("SqlPool busy");
        return nullptr;
    }


    {
        std::unique_lock<std::mutex> locker(m_mutex);
        sql = m_connQue.front();
        m_connQue.pop();
    }
    return sql;
}

void SqlPool::FreeConnObj(MYSQL* conn) {
    if (!conn) {
        std::unique_lock<std::mutex> locker(m_mutex);
        m_connQue.push(conn);
    }
}