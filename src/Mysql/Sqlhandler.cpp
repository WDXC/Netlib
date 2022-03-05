#include "Sqlhandler.hpp"

SqlHandler::SqlHandler(SqlConnPool *connpool) {
    if (connpool) {
        m_sql = connpool->getConnObj();
        m_connpool = connpool;
    }
}

SqlHandler::SqlHandler(MYSQL **sql, SqlConnPool *connpool) {
    if (connpool) {
        *sql = connpool->getConnObj();
        m_sql = *sql;
        m_connpool = connpool;
    }
}

SqlHandler::~SqlHandler() {
    if (m_sql) {
        m_connpool->FreeConnObj(m_sql);
    }
}
