/*** 
 * @Author: Zty
 * @Date: 2022-02-14 14:08:34
 * @LastEditTime: 2022-02-16 19:52:02
 * @LastEditors: Zty
 * @Description:  网络地址封装
 * @FilePath: /multhread/src/Socket/InetAddress.hpp
 */

#ifndef INETADDRESS_H_
#define INETADDRESS_H_

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