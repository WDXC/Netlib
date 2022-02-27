#ifndef SOCKET_H_
#define SOCKET_H_ 


#include "../Base/NonCopyable.hpp"

class InetAddress;

class Socket : NoCopyable {
    public:
        explicit Socket(int fd);
        int fd() const {return m_sockfd;}

        void bindAddress(const InetAddress& addr) const;
        void listenAddress() const;
        int acceptAddress(InetAddress* peerAddress);

        void shutdown_write();


        // 禁用nagle 算法
        void set_tcp_noDelay(bool on) const ;
        // 地址复用
        void set_reuseAddr(bool on) const ;
        // 端口复用
        void set_reusePort(bool on) const ;
        // 保持长连接
        void set_keepAlive(bool on) const ;
    private:
        int m_sockfd;
};


#endif