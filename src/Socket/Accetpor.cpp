#include "Acceptor.hpp"
#include "InetAddress.hpp"
#include "SocketOps.hpp"
#include "../Log/Logger.hpp"

Acceptor::Acceptor(EventLoop* loop, const InetAddress& addr, bool reuseport) : 
    m_loop(loop),
    m_acceptSocket(sockets::createNooBlockingSocket()),
    m_acceptChannel(m_loop, m_acceptSocket.fd()) {
    m_acceptSocket.set_reuseAddr(true);
    m_acceptSocket.set_reusePort(true);
    m_acceptSocket.bindAddress(addr);
    m_acceptChannel.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor() {
    m_acceptChannel.dis_enable_all();
}

void Acceptor::listen() {
    m_loop->is_in_loopThread();
    m_acceptSocket.listenAddress();
    m_acceptChannel.enable_reading();
}

void Acceptor::handleRead () {
    m_loop->is_in_loopThread();
    InetAddress peerAddress;
    int connfd = m_acceptSocket.acceptAddress(&peerAddress);
    if (connfd >= 0) {
        if (newConnectionCallback_) {
            newConnectionCallback_(connfd, peerAddress);
        } else {
            sockets::close(connfd);
        }
    } 
}