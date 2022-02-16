/*** 
 * @Author: Zty
 * @Date: 2022-02-15 09:07:08
 * @LastEditTime: 2022-02-15 19:07:58
 * @LastEditors: Zty
 * @Description: 连接接收类
 * @FilePath: /multhread/Socket/Acceptor.hpp
 */

#ifndef ACCEPTOR_H_
#define ACCEPTOR_H_

#include "../Base/NonCopyable.hpp"
#include "../Net/EventLoop.hpp"
#include "../Net/Channel.hpp"
#include "Socket.hpp"

class Acceptor : NoCopyable {
    public: 
        using NewConnectionCallback = std::function<void(int sockfd, const InetAddress&)>;

        Acceptor(EventLoop* loop, const InetAddress& addr, bool reuse);
        ~Acceptor();
        
        void setNewConnectionCallback(const NewConnectionCallback& cb) { 
            newConnectionCallback_ = cb;
        }

        void listen();
    
    private:
        void handleRead();

    private:
        EventLoop* m_loop;
        Socket m_acceptSocket;
        Channel m_acceptChannel;
        NewConnectionCallback newConnectionCallback_;
};



#endif