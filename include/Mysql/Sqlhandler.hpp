#ifndef SQLHANDLER_H
#define SQLHANDLER_H

#include "SqlConnPool.hpp"

class SqlHandler {
public:
    SqlHandler(SqlConnPool *connpool);
    SqlHandler(MYSQL **sql, SqlConnPool *connpool);
    ~SqlHandler();

private:
    MYSQL *m_sql;
    SqlConnPool *m_connpool;
};

#endif //NETLIB_SQLHANDLER_H
