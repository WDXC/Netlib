#include "SocketOps.hpp"
#include "Log.hpp"

void sockets::bind(int sockfd, const struct sockaddr* addr) {
    int ret = ::bind(sockfd, addr, sizeof(sockaddr_in));
    if (ret < 0) {
        LOG_ERROR("SocketOps: bind address error!");
        return;
    }
}

void sockets::listen(int sockfd) {
    int ret = ::listen(sockfd, SOMAXCONN);
    if (ret < 0) {
        LOG_ERROR("SocketOps: listen error !");
        return;
    }
}

void sockets::close(int sockfd) {
    int ret = ::close(sockfd);
    if (ret < 0) {
        LOG_ERROR("SocketOps: close error errno: %d!", errno);
        return ;
    }
}

int sockets::accept(int sockfd, struct sockaddr_in* addr) {
    socklen_t len = static_cast<socklen_t>(sizeof(*addr));
    int connfd = ::accept4(sockfd, (struct sockaddr*) addr, &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd < 0) {
        // accept 错误信号接收处理
        int savedErrno = errno;
        LOG_ERROR("SocketOps::accept errno=%d, connfd=%d", savedErrno, connfd);
        switch (savedErrno)
        {
            case EAGAIN:
            case ECONNABORTED:
            case EINTR:
            case EPROTO: // ???
            case EPERM:
            case EMFILE: // per-process lmit of open file desctiptor ???
                // expected errors
                errno = savedErrno;
                break;
            case EBADF:
            case EFAULT:
            case EINVAL:
            case ENFILE:
            case ENOBUFS:
            case ENOMEM:
            case ENOTSOCK:
            case EOPNOTSUPP:
                // unexpected errors
                LOG_ERROR("unexpected error of ::accept  %d ", savedErrno) ;
                break;
            default:
                LOG_ERROR("unknown error of ::accept %d", savedErrno);
                break;
        }
    }
    return connfd;
}

int sockets::connect(int sockfd, const struct sockaddr* addr) {
    return ::connect(sockfd, addr, static_cast<socklen_t>(sizeof(struct sockaddr_in6)));
}

ssize_t sockets::read(int sockfd, void* buf, size_t count) {
    return ::read(sockfd, buf, count);
}

ssize_t sockets::readv(int sockfd, const struct iovec *iov, int iovcnt) {
    return ::readv(sockfd, iov, iovcnt);
}

ssize_t sockets::write(int sockfd, const void* buf, size_t count) {
    return ::write(sockfd, buf, count);
}

void sockets::shutdownwrite(int sockfd) {
    if (::shutdown(sockfd, SHUT_WR) < 0) {
        LOG_ERROR("shut wirte error error: %d", errno);
    }
}

void sockets::shutAll(int sockfd) {
    if (::shutdown(sockfd, SHUT_RDWR) < 0) {
        LOG_ERROR("shut write & read error: %d " , errno);
    }
}

int sockets::getSocketError(int sockfd) {
    int optval;
    socklen_t optlen = static_cast<socklen_t>(sizeof optval);
    if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
        return errno;
    } else {
        return optval;
    }
}

const struct sockaddr* sockets::sockaddr_cast(const struct sockaddr_in6* addr) {
    return static_cast<const struct sockaddr*>(static_cast<const void*> (addr));
}

struct sockaddr* sockets::sockaddr_cast(struct sockaddr_in6* addr) {
    return static_cast<struct sockaddr*>(static_cast<void*> (addr));
}

const struct sockaddr* sockets::sockaddr_cast(const struct sockaddr_in* addr) {
    return static_cast<const struct sockaddr*>(static_cast<const void*>(addr));
}

const struct sockaddr_in* sockets::sockaddr_in_cast(const struct sockaddr* addr) {
    return static_cast<const struct sockaddr_in*>(static_cast<const void*>(addr));
}

const struct sockaddr_in6* sockets::sockaddr_in6_cast(const struct sockaddr* addr) {
    return static_cast<const struct sockaddr_in6*> (static_cast<const void*>(addr));
}

struct sockaddr_in6 sockets::getLocalAddr(int sockfd) {
    struct sockaddr_in6 localaddr;
    bzero(&localaddr, sizeof(localaddr));
    socklen_t addrlen = static_cast<socklen_t> (sizeof localaddr);
    if (::getsockname(sockfd, sockaddr_cast(&localaddr), &addrlen) < 0) {
            LOG_ERROR("sockets::getLocalAddr");
    }
    return localaddr;
}

struct sockaddr_in6 sockets::getPeerAddr(int sockfd) {
    struct sockaddr_in6 peeraddr;
    bzero(&peeraddr, sizeof(peeraddr));
    socklen_t addrlen = static_cast<socklen_t> (sizeof peeraddr);
    if (::getpeername(sockfd, sockaddr_cast(&peeraddr), &addrlen) < 0) {
        LOG_ERROR("sockets::getPeerAddr");
    }
    return peeraddr;
}

int sockets::createNooBlockingSocket() {
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);

    if (sockfd < 0) {
        LOG_ERROR("SocketOps: Create noBlocking socket error!");
    }
    return sockfd;
}


bool sockets::isSelfConnect(int sockfd) {
    struct sockaddr_in6 localaddr = getLocalAddr(sockfd);
    struct sockaddr_in6 peeraddr = getPeerAddr(sockfd);
    if (localaddr.sin6_family == AF_INET) {
        const struct sockaddr_in* laddr4 = reinterpret_cast<struct sockaddr_in*>(&localaddr);
        const struct sockaddr_in* raddr4 = reinterpret_cast<struct sockaddr_in*>(&peeraddr);
        return laddr4->sin_port == raddr4->sin_port
                && laddr4->sin_addr.s_addr == raddr4->sin_addr.s_addr;
    } else if (localaddr.sin6_family == AF_INET6) {
        return localaddr.sin6_port = peeraddr.sin6_port
                && memcmp(&localaddr.sin6_addr, &peeraddr.sin6_addr, sizeof(localaddr.sin6_addr)) == 0;
    } else {
        return false;
    }
}