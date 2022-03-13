#include "Socket.hpp"
#include "InetAddress.hpp"
#include "SocketOps.hpp"
#include "Log.hpp"
#include <netinet/tcp.h>

Socket::Socket(int fd) : m_sockfd(fd) {

}

void Socket::bindAddress(const InetAddress& addr) const {
    sockets::bind(m_sockfd, addr.getSockAddr());
}

void Socket::listenAddress() const {
    sockets::listen(m_sockfd);
}

int Socket::acceptAddress(InetAddress* peerAddr) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    int connfd = sockets::accept(m_sockfd, &addr);
    if (connfd >= 0) {
        peerAddr->setSockAddr(addr);
    }
    return connfd;
}

void Socket::shutdown_write() {
    sockets::shutdownwrite(m_sockfd);
}

void Socket::set_tcp_noDelay(bool on) const  {
    int optval = on ? 1 : 0;
    ::setsockopt(m_sockfd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}

void Socket::set_reuseAddr(bool on) const {
    int optval = on ? 1 : 0;
    ::setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
}

void Socket::set_reusePort(bool on) const {
    int optval = on ? 1 : 0;
    ::setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));
}

void Socket::set_keepAlive(bool on) const {
    int optval = on ? 1 : 0;
    ::setsockopt(m_sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
}
