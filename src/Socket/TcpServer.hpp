/*** 
 * @Author: Zty
 * @Date: 2022-02-15 10:10:18
 * @LastEditTime: 2022-02-19 16:58:38
 * @LastEditors: Zty
 * @Description: 
 * @FilePath: /multhread/src/Socket/TcpServer.hpp
 */

#ifndef TCPSERVER_H_
#define TCPSERVER_H_

#include <memory>
#include <string>
#include <atomic>
#include <functional>
#include <unordered_map>

#include "../Log/Log.hpp"
#include "../Net/EventLoop.hpp"
#include "../Base/Callbacks.hpp"
#include "../Base/NonCopyable.hpp"
#include "../Thread/ThreadPool.hpp"
#include "../Net/EventLoopThreadPool.hpp"


#include "Buffer.hpp"
#include "Acceptor.hpp"
#include "InetAddress.hpp"
#include "TcpConnection.hpp"


class TcpServer : NoCopyable {
    public:
        using ThreadInitCallback = std::function<void(EventLoop*)>;
        using ConnectionMap = std::unordered_map<std::string, TcpConnectionPtr>;
        enum Option {
            k_noReuse_port,
            k_reuse_port,
        };
    public:
        TcpServer(EventLoop* loop, const InetAddress& listenAddr,
                  const std::string& name,
                  Option option = k_noReuse_port);
        ~TcpServer();

        //  设置回调
        void setThreadInitCallback(const ThreadInitCallback& cb) {
            threadCallback_ = cb;
        }
        void setConnectionCallback(const ConnectionCallback& cb) {
            connectionCallback_ = cb;
        }
        void setMessageCallback(const MessageCallback& cb) {
            messageCallback_ = cb;
        }
        void setWriteCallback(const WriteCompleteCallback& cb) {
            writeCompleteCallback_ = cb;
        }

        void set_thread_num(int thread_num);


        void start();

        EventLoop* getLoop() const {return loop_;}

        const std::string& getName() {return name_;}

        const std::string& getIpPort() {return ip_port;}
    
    private:
        void new_connection(int sockfd, const InetAddress& peerAddr);
        void remove_connection(const TcpConnectionPtr& conn);
        void remove_connection_inLoop(const TcpConnectionPtr& conn);

    private:
        static const int BUF_SIZE = 64;

        EventLoop* loop_;
        const std::string ip_port;
        const std::string name_;

        std::unique_ptr<Acceptor> acceptor_;
        std::shared_ptr<EventLoopThreadPool> thread_pool_;

        ConnectionCallback connectionCallback_;
        MessageCallback messageCallback_;
        WriteCompleteCallback writeCompleteCallback_;

        ThreadInitCallback threadCallback_;

        std::atomic_int started_;
        int next_conn_id_;
        ConnectionMap connections_;
};


#endif