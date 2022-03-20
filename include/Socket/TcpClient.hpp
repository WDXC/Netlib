#ifndef TCPCLIENT_H_
#define TCPCLIENT_H_

#include <atomic>
#include <mutex>

#include "TcpConnection.hpp"
#include "../Base/NonCopyable.hpp"

class Connector;

using ConnectorPtr = std::shared_ptr<Connector>;

class TcpClient : NoCopyable {
    public:
        TcpClient(EventLoop* loop,
                  const InetAddress& serverAddr,
                  const std::string& nameArg);
        ~TcpClient();

        void connect();
        void disconnect();
        void stop();

        TcpConnectionPtr connection() const {
            std::lock_guard<std::mutex> locker(m_mutex);
            return connection_;
        }

        EventLoop* getLoop() const {
            return loop_;
        }
        bool retry() const {
            return retry_;
        }
        void enableRetry() {
            retry_ = true;
        }
        void setConnectionCallback(ConnectionCallback cb) {
            connectionCallback_ = std::move(cb);
        }
        void setMessageCallback(MessageCallback cb) {
            messageCallback_ = std::move(cb);
        }
        void setWriteCallback(WriteCompleteCallback cb) {
            writeCallback_ = std::move(cb);
        }

    private:
        void newConnection(int sockfd);
        void removeConnection(const TcpConnectionPtr& conn);
    
    private:
        EventLoop* loop_;
        ConnectorPtr connector_;
        const std::string name_;
        ConnectionCallback connectionCallback_;
        MessageCallback messageCallback_;
        WriteCompleteCallback writeCallback_;
        std::atomic_bool retry_;
        std::atomic_bool connect_;

        int nextConnId_;
        mutable std::mutex m_mutex;
        TcpConnectionPtr connection_;
};

#endif