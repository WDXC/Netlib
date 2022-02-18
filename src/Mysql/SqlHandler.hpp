/*** 
 * @Author: Zty
 * @Date: 2022-02-18 09:14:24
 * @LastEditTime: 2022-02-18 09:22:59
 * @LastEditors: Zty
 * @Description: 
 * @FilePath: /multhread/src/Mysql/SqlHandler.hpp
 */

#ifndef SQLHANDLER_H_
#define SQLHANDLER_H_

#include "SqlConnPool.hpp"

class SqlHandler {
    public:
        explicit SqlHandler(SqlConnPool* pool);
        SqlHandler(MYSQL** sql, SqlConnPool* conn);
        ~SqlHandler();
    private:
        MYSQL* m_sql;
        SqlConnPool* m_connpool;
};

#endif