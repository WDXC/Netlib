#include "TcpServer.hpp"
#include <assert.h>
#include <string.h>

namespace check {
    EventLoop* CheckLoopNotNull(EventLoop* loop) {
        if (loop == nullptr) {
            LOG_ERROR("%s:%s:%d mainloop is null \n", __FILE__, __FUNCTION__, __LINE__);
        }
        return loop;
    }
}


TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenaddr,
                     const std::string& name, Option option) : 
    loop_(check::CheckLoopNotNull(loop)),
    ip_port(listenaddr.get_ip_port()),
    name_(name),
    acceptor_(new Acceptor(loop, listenaddr, option=k_reuse_port)),
    thread_pool_(new EventLoopThreadPool(loop, name_)),
    connectionCallback_(),
    messageCallback_(),
    next_conn_id_(1),
    started_(0) {
    acceptor_->setNewConnectionCallback(std::bind(&TcpServer::new_connection, this, std::placeholders::_1, std::placeholders::_2));
}


TcpServer::~TcpServer() {
    loop_->assertInLoopThread();
    for (auto& it : connections_) {
        TcpConnectionPtr conn(it.second);
        it.second.reset();
        conn->get_loop()->run_in_loop(std::bind(&TcpConnection::destory_connect, conn));
    }
}

void TcpServer::set_thread_num(int thread_num) {
    assert(0 <= thread_num);
    thread_pool_->set_threadNum(thread_num);
}

void TcpServer::start() {
    if (started_++ == 0) {
        thread_pool_->start(threadCallback_);
        assert(!acceptor_->listening());
        loop_->run_in_loop(std::bind(&Acceptor::listen, acceptor_.get()));
    }
}

void TcpServer::new_connection (int sockfd, const InetAddress& peerAddr) {
    loop_->assertInLoopThread();
    EventLoop* ioloop = thread_pool_->appendThread();

    char buffer[BUF_SIZE] = {0};
    snprintf(buffer, sizeof(buffer), "-%s#%d", ip_port.c_str(), next_conn_id_);
    ++next_conn_id_;
    std::string conn_name = name_ + buffer;

    LOG_INFO("tcp server:: new connection[%s] - new connection[%s] from %s\n", name_.c_str(), conn_name.c_str(), peerAddr.get_ip_port().c_str());

    sockaddr_in local;
    bzero(&local, sizeof(local));
    socklen_t addrlen = sizeof(local);
    if (::getsockname(sockfd, (sockaddr*)& local, &addrlen) < 0) {
        LOG_ERROR("new connection get localaddr error\n");
    }
    InetAddress localaddr(local);

    TcpConnectionPtr conn(new TcpConnection(ioloop, conn_name, sockfd, localaddr, peerAddr));
    connections_[conn_name] = conn;

    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);

    // 设置如何关闭连接的回调
    conn->setCloseCallback(std::bind(&TcpServer::remove_connection, this, std::placeholders::_1));
    ioloop->run_in_loop(std::bind(&TcpConnection::establish_connect, conn));
}

void TcpServer::remove_connection (const TcpConnectionPtr& conn) {
    loop_->run_in_loop(std::bind(&TcpServer::remove_connection_inLoop, this, conn));
}

void TcpServer::remove_connection_inLoop(const TcpConnectionPtr& conn) {
    loop_->assertInLoopThread();
    LOG_INFO("tcp server::remove connection in loop[%s]-connection[%s]\n", name_.c_str(), conn->get_name().c_str());
    
    connections_.erase(conn->get_name());
    EventLoop* ioloop = conn->get_loop();
    ioloop->queue_in_loop(std::bind(&TcpConnection::destory_connect, conn));
}
