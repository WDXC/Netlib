/*** 
 * @Author: Zty
 * @Date: 2022-02-16 09:18:08
 * @LastEditTime: 2022-02-18 09:17:23
 * @LastEditors: Zty
 * @Description: 
 * @FilePath: /multhread/src/Mysql/SqlConnPool.hpp
 */
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

        void Init(const std::string& host, int port,
                  const std::string& user, const std::string& pwd,
                  const std::string& db_name, int conn_name);

        void ClosePool();

        MYSQL* getConnObj();
        void FreeConnObj(MYSQL* conn);

    private:
        std::queue<MYSQL*> m_connQue;
        int m_max_conn;
        std::mutex m_mutex;
};

using g_sqlMgr = Singleton<SqlConnPool>;

#endif