#include "SqlHandler.hpp"

SqlHandler::SqlHandler(SqlConnPool* conn) : m_connpool(conn), m_sql(nullptr) {
    if (conn) {
        m_sql = conn->getConnObj();
        m_connpool = conn;
    }
}

SqlHandler::SqlHandler(MYSQL** sql, SqlConnPool* conn) {
    if (conn) {
        *sql = m_connpool->getConnObj();
        m_sql = *sql;
        m_connpool = conn;
    }
}

SqlHandler::~SqlHandler() {
    if (m_sql) {
        m_connpool->FreeConnObj(m_sql);
    }
}