#ifndef SQLCONNPOOL_H_
#define SQLCONNPOOL_H_

#include <string>
#include <queue>
#include <mutex>
#include <mysql/mysql.h>
#include "../Base/Singleton.hpp"

class SqlConnPool {
    public:
        SqlConnPool();
        ~SqlConnPool();

        // 初始化数据库连接的相关数据 
        void Init(const std::string& host, int port,
                  const std::string& user, const std::string& pwd,
                  const std::string& db_name, int conn_name);

        // 关闭连接池 
        void ClosePool();

        // 获取连接池中的对象
        MYSQL* getConnObj();
        // 释放连接对象
        void FreeConnObj(MYSQL* conn);

    private:
        std::queue<MYSQL*> m_connQue;
        int m_max_conn;
        std::mutex m_mutex;
};

using g_sqlMgr = Singleton<SqlConnPool>;

#endif