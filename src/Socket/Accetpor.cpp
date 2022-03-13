#include "Acceptor.hpp"
#include "InetAddress.hpp"
#include "SocketOps.hpp"
#include "Log.hpp"
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

Acceptor::Acceptor(EventLoop* loop, const InetAddress& addr, bool reuseport) : 
    m_loop(loop),
    m_acceptSocket(sockets::createNooBlockingSocket()),
    m_acceptChannel(m_loop, m_acceptSocket.fd()),
		listening_(false),
		idleFd_(::open("/dev/null", O_RDONLY | O_CLOEXEC)) {
    m_acceptSocket.set_reuseAddr(true);
    m_acceptSocket.set_reusePort(true);
    m_acceptSocket.bindAddress(addr);
    m_acceptChannel.setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor() {
    m_acceptChannel.dis_enable_all();
    m_acceptChannel.remove();
    ::close(idleFd_);
}

void Acceptor::listen() {
    m_loop->assertInLoopThread();
		listening_ = true;
    m_acceptSocket.listenAddress();
    m_acceptChannel.enable_reading();
}

void Acceptor::handleRead () {
    m_loop->assertInLoopThread();
    InetAddress peerAddress;
    int connfd = m_acceptSocket.acceptAddress(&peerAddress);
    if (connfd >= 0) {
        if (newConnectionCallback_) {
            newConnectionCallback_(connfd, peerAddress);
        } else {
            ::close(connfd);
        }
    } else {
        if (errno == EMFILE) {
            ::close(idleFd_);
            idleFd_ = ::accept(m_acceptSocket.fd(), NULL, NULL);
            ::close(idleFd_);
            idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
        }
    }
}
