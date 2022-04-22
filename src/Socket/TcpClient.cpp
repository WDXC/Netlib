#include "TcpClient.hpp"

#include "Connector.hpp"
#include "EventLoop.hpp"
#include "Log.hpp"
#include "SocketOps.hpp"

namespace detail {
void removeConnection(EventLoop* loop, const TcpConnectionPtr& conn) {
    loop->queue_in_loop(std::bind(
        &TcpConnection::destory_connect, conn));
}

void removeConnector(const ConnectorPtr& Connector) {
}
}  // namespace detail

TcpClient::TcpClient(EventLoop* loop,
                     const InetAddress& serverAddr,
                     const std::string& nameArg)
    : loop_(loop),
      connector_(new Connector(loop, serverAddr)),
      name_(nameArg),
      retry_(false),
      connect_(true),
      nextConnId_(1) {
    connector_->setNewConnectionCallback(std::bind(
        &TcpClient::newConnection, this, std::placeholders::_1));
    LOG_INFO("TcpClient::TcpClient[%s] - connector %p", name_.c_str(), &connector_)
}

TcpClient::~TcpClient() {
    LOG_INFO("TcpClient::~TcpClient[%s] - connector %p", name_.c_str(), &connector_);
    TcpConnectionPtr conn;
    bool unique = false;
    {
        std::lock_guard<std::mutex> locker(m_mutex);
        unique = connection_.unique();
        conn = connection_;
    }
    if (conn) {
        assert(loop_ == conn->get_loop());
        CloseCallback cb = std::bind(&detail::removeConnection, loop_, std::placeholders::_1);
        loop_->run_in_loop(
            std::bind(&TcpConnection::setCloseCallback, conn, cb));
        if (unique) {
            conn->forceClose();
        }
    } else {
        connector_->stop();
        loop_->runAfter(1, std::bind(&detail::removeConnector, connector_));
    }
}

void TcpClient::connect() {
    LOG_INFO("Tcpclient::connect[%s] - connection to %s",
             name_.c_str(), connector_->serverAddress().get_ip_port().c_str());
    connect_ = true;
    connector_->start();
}

void TcpClient::disconnect() {
    connect_ = false;
    {
        std::lock_guard<std::mutex> locker(m_mutex);
        if (connection_) {
            connection_->shutdown();
        }
    }
}

void TcpClient::stop() {
    connect_ = false;
    connector_->stop();
}

void TcpClient::newConnection(int sockfd) {
    loop_->assertInLoopThread();
    InetAddress peerAddr(sockets::getPeerAddr(sockfd));
    char buf[32];
    snprintf(buf, sizeof(buf), ":%s#%d", peerAddr.get_ip_port().c_str(), nextConnId_);
    ++nextConnId_;
    std::string connName = name_ + buf;

    InetAddress localAddr(sockets::getLocalAddr(sockfd));
    TcpConnectionPtr conn(new TcpConnection(loop_,
                                            connName,
                                            sockfd,
                                            localAddr,
                                            peerAddr));
    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCallback_);
    conn->setCloseCallback(
        std::bind(&TcpClient::removeConnection, this, std::placeholders::_1));
    {
        std::lock_guard<std::mutex> locker(m_mutex);
        connection_ = conn;
    }
    conn->establish_connect();
}

void TcpClient::removeConnection(const TcpConnectionPtr& conn) {
    loop_->assertInLoopThread();
    assert(loop_ == conn->get_loop());

    {
        std::lock_guard<std::mutex> locker(m_mutex);
        assert(connection_ == conn);
        connection_.reset();
    }

    loop_->queue_in_loop(
        std::bind(&TcpConnection::destory_connect, conn));

    if (retry_ && connect_) {
        LOG_INFO("TcpClient::connect[%s] - Reconnecting to %s",
                 name_.c_str(), connector_->serverAddress().get_ip_port().c_str());
        connector_->restart();
    }
}
