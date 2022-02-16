#include "SocketOps.hpp"
#include "../Log/Logger.hpp"

void sockets::bind(int sockfd, const struct sockaddr* addr) {
    int ret = ::bind(sockfd, addr, sizeof(addr));
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
        LOG_ERROR("SocketOps: close error!");
        return ;
    }
}

int sockets::accept(int sockfd, struct sockaddr_in* addr) {
    socklen_t len = sizeof(*addr);
    int connfd = ::accept4(sockfd, (struct sockaddr*) addr, &len, SOCK_NONBLOCK | SOCK_CLOEXEC);
    if (connfd < 0) {
        // accept 错误信号接收处理
        int savedErrno = errno;
        LOG_ERROR("SocketOps::accept error");
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



int sockets::createNooBlockingSocket() {
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);

    if (sockfd < 0) {
        LOG_ERROR("SocketOps: Create noBlocking socket error!");
    }
    return sockfd;
}