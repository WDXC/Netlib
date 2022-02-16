/*** 
 * @Author: Zty
 * @Date: 2022-02-14 13:47:16
 * @LastEditTime: 2022-02-15 09:01:43
 * @LastEditors: Zty
 * @Description: socket操作
 * @FilePath: /multhread/Socket/SocketOps.hpp
 */

#ifndef SOCKETOPS_H_
#define SOCKETOPS_H_

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