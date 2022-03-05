#ifndef SOCKETOPS_H_
#define SOCKETOPS_H_

// socket 操作

#include <arpa/inet.h>
#include <unistd.h>


namespace sockets {
    void bind(int sockfd, const struct sockaddr* addr);
    void listen(int sockfd);
    int accept(int sockdf, struct sockaddr_in* addr);
    void close(int sockdf);
    int createNooBlockingSocket();
}

#endif