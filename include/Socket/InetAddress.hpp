#ifndef INETADDRESS_H_
#define INETADDRESS_H_

// 地址信息处理，可以接收IPv4与IPv6数据

#include <netinet/in.h>
#include <string>
#include <string.h>

class InetAddress  {
    public:
        explicit InetAddress(uint16_t port = 0, bool ifLoopback = true, bool ipv6 = false);
        InetAddress(const std::string& ip, uint16_t port, bool ipv6 = false);
        
        explicit InetAddress(const struct sockaddr_in& addr);
        explicit InetAddress(const struct sockaddr_in6& addr6);

        const struct sockaddr* getSockAddr() const;
        void setSockAddr(struct sockaddr_in& addr);

        std::string getIpStr() const;
        uint16_t getPort() const ;

        // 获取ip Port数据，用于输出日志
        std::string get_ip_port() const;

        sa_family_t family() const {
            return m_addr.sin_family;
        }
    private:
        union
        {
            struct sockaddr_in m_addr;
            struct sockaddr_in6 m_addr6;
        };
        
};



#endif
