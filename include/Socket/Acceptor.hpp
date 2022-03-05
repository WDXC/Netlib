#ifndef ACCEPTOR_H_
#define ACCEPTOR_H_

// 主Reactor 连接 子Reactor的连接器

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
				bool listening() const { return listening_; }
    
    private:
        void handleRead();

    private:
        EventLoop* m_loop;
				bool listening_;
        Socket m_acceptSocket;
        Channel m_acceptChannel;
        NewConnectionCallback newConnectionCallback_;
				int idleFd_;
};



#endif
