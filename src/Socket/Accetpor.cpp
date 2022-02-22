/*** 
 * @Author: Zty
 * @Date: 2022-02-16 09:18:08
 * @LastEditTime: 2022-02-18 10:36:42
 * @LastEditors: Zty
 * @Description: 
 * @FilePath: /multhread/src/Socket/Accetpor.cpp
 */
#include "Acceptor.hpp"
#include "InetAddress.hpp"
#include "SocketOps.hpp"
#include "../Log/Log.hpp"

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