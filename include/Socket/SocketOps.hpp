#ifndef SOCKETOPS_H_
#define SOCKETOPS_H_

// socket 操作

#include <arpa/inet.h>
#include <sys/uio.h>
#include <unistd.h>


namespace sockets {
    void bind(int sockfd, const struct sockaddr* addr);
    void listen(int sockfd);
    int accept(int sockdf, struct sockaddr_in* addr);
    void close(int sockdf);
    int connect(int sockfd, const struct sockaddr* addr);
    ssize_t read(int sockfd, void* buf, size_t count);
    ssize_t readv(int sockfd, const struct iovec* iov, int iovcnt);
    ssize_t write(int sockfd, const void* buf, size_t count);
    void shutdownwrite(int sockfd);
    void shutAll(int sockfd);
    int getSocketError(int sockfd);

    const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr);
    const struct sockaddr* sockaddr_cast(const struct sockaddr_in6* addr);
    struct sockaddr* sockaddr_cast(struct sockaddr_in6* addr);
    const struct sockaddr_in* sockaddr_in_cast(const struct sockaddr* addr);
    const struct sockaddr_in6* sockaddr_in6_cast(const struct sockaddr* addr);
    struct sockaddr_in6 getLocalAddr(int sockfd);
    struct sockaddr_in6 getPeerAddr(int sockfd);
    
    int createNooBlockingSocket();
    bool isSelfConnect(int sockfd);
}

#endif