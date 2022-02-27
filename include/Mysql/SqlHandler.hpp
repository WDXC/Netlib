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