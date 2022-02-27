#ifndef TCPCONNECTION_H_
#define TCPCONNECTION_H_

#include <string>
#include <atomic>
#include <memory>
#include "../Base/NonCopyable.hpp"
#include "../Timer/TimeStamp.hpp"
#include "../Base/Callbacks.hpp"
#include "InetAddress.hpp"
#include "Buffer.hpp"
#include <boost/any.hpp>

class Channel;
class EventLoop;
class Socket;

class TcpConnection : NoCopyable, 
                      public std::enable_shared_from_this<TcpConnection> {
    private:
        enum StateE {
            k_disconnected,
            k_connecting,
            k_connected,
            k_disconnecting,
        };

    public:
        TcpConnection(EventLoop* loop, const std::string name, int sockfd,
                      const InetAddress& localAddr, const InetAddress& peerAddr);
        ~TcpConnection();

        EventLoop* get_loop() const {
            return loop_;
        }

        const std::string &get_name() const {
            return name_;
        }

        const InetAddress& get_localAddr() const {
            return localaddr_;
        }

        const InetAddress& get_peerAddr() const {
            return peeraddr_;
        }

        bool connected() {
            return state_ = k_connected;
        }

        void set_state(StateE state) {
            state_ = state;
        }

        // 发送数据
        void send(const std::string& buf);
        void send(Buffer* buf);

        // 断开连接
        void shutdown();

        // 建立连接
        void establish_connect();

        // 销毁连接
        void destory_connect();

        // 设置回调
        void setCloseCallback(const CloseCallback& cb) {
            closeCallback_ = cb;
        }
        void setHighWaterMarkCallback(const HighWaterMarkCallback& cb) {
            highWaterCallback_ = cb;
        }
        
        void setConnectionCallback(const ConnectionCallback& cb) {
            connectionCallback_ = cb;
        }
        void setMessageCallback(const MessageCallback& cb) {
            messageCallback_ = cb;
        }
        void setWriteCompleteCallback(const WriteCompleteCallback& cb) {
            writeCallback_ = cb;
        }

        void setContext(const boost::any& context) {
            context_ = context;
        }

        const boost::any& getContext() const {
            return context_;
        }

				boost::any* getMutableContext() {
            return &context_;
        }

    private:
        void handle_read(TimeStamp receive_time);
        void handle_write();
        void handle_close();
        void handle_error();

        void send_inLoop(const std::string& buf);
        void send_inLoop(const std::string& buf, size_t len);
        void shutdown_inLoop();

    private:
        EventLoop* loop_;
        const std::string name_;

        std::atomic_int state_;
        bool reading_;

        std::unique_ptr<Socket> socket_;
        std::unique_ptr<Channel> channel_;

        const InetAddress localaddr_;
        const InetAddress peeraddr_;

        ConnectionCallback connectionCallback_;
        MessageCallback messageCallback_;
        WriteCompleteCallback writeCallback_;
        CloseCallback closeCallback_;

        HighWaterMarkCallback highWaterCallback_;
        size_t highWaterMark_;

        Buffer input_buffer_;
        Buffer output_buffer_;
				boost::any context_;
};

#endif
