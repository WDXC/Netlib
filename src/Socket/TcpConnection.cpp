
#include "TcpConnection.hpp"

#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <functional>

#include "Channel.hpp"
#include "EventLoop.hpp"
#include "Log.hpp"
#include "Socket.hpp"
#include "SocketOps.hpp"
#include "WeakCallback.hpp"

static EventLoop* CheckLoopNotNull(EventLoop* loop) {
    if (!loop) {
        LOG_ERROR("%s:%s:%d tcpconnection is null", __FILE__, __FUNCTION__, __LINE__);
    }
    return loop;
}

TcpConnection::TcpConnection(EventLoop* loop,
                             const std::string name,
                             int sockfd,
                             const InetAddress& localaddr,
                             const InetAddress& peeraddr)
    : loop_(CheckLoopNotNull(loop)),
      name_(name),
      state_(k_connecting),
      reading_(true),
      socket_(new Socket(sockfd)),
      channel_(new Channel(loop, sockfd)),
      localaddr_(localaddr),
      peeraddr_(peeraddr) {
    channel_->setReadCallback(std::bind(
        &TcpConnection::handle_read, this, std::placeholders::_1));
    channel_->setWriteCallback(std::bind(
        &TcpConnection::handle_write, this));
    channel_->setErrorCallback(std::bind(
        &TcpConnection::handle_error, this));
    channel_->setCloseCallback(std::bind(
        &TcpConnection::handle_close, this));
    LOG_DEBUG("Tcp connect::ctor[%s] at fd = %d\n", name.c_str(), sockfd);

    socket_->set_keepAlive(true);
}

TcpConnection::~TcpConnection() {
    LOG_DEBUG("tcp connection::dtor[%s] at fd = %d state %d \n", name_.c_str(),
              channel_->get_fd(),
              (int)state_);
}

void TcpConnection::send(const std::string& buf) {
    // 必须是连接状态
    if (state_ == k_connected) {
        if (loop_->is_in_loopThread()) {
            send_inLoop(buf);
        } else {
            void (TcpConnection::*fp)(const std::string& message) = &TcpConnection::send_inLoop;
            loop_->run_in_loop(std::bind(fp,
                                         this,
                                         buf));
        }
    }
}

void TcpConnection::send(Buffer* buf) {
    // 必须是连接状态
    if (state_ == k_connected) {
        if (loop_->is_in_loopThread()) {
            send_inLoop(buf->peek(), buf->readable_bytes());
            buf->retrieve_all();
        } else {
            void (TcpConnection::*fp)(const std::string& message) = &TcpConnection::send_inLoop;
            loop_->run_in_loop(std::bind(fp,
                                         this,
                                         buf->retrieve_asString()));
        }
    }
}

void TcpConnection::send_inLoop(const std::string& buf) {
    send_inLoop(buf, buf.size());
}

void TcpConnection::send_inLoop(const std::string& buf, size_t len) {
    ssize_t nwrote = 0;
    size_t remaining = len;
    bool error = false;

    if (state_ == k_disconnected) {
        LOG_ERROR("disconnected, give up writing!\n");
        return;
    }

    if (!channel_->is_writing() && output_buffer_.readable_bytes() == 0) {
        nwrote = sockets::write(channel_->get_fd(), buf.c_str(), buf.size());
        if (nwrote >= 0) {
            remaining = len - nwrote;
            if (remaining == 0 && writeCallback_) {
                loop_->queue_in_loop(std::bind(
                    writeCallback_, shared_from_this()));
            }
        } else {
            nwrote = 0;
            // 错误 资源不可用
            if (errno != EWOULDBLOCK) {
                LOG_ERROR("Tcp connection::send in loop\n");
                if (errno == EPIPE || errno == ECONNRESET) {
                    error = true;
                }
            }
        }
    }
    assert(remaining <= len);

    // 当前write没有把数据全部发送出去，剩余数据需要保存在缓冲区中
    // 然后给channel注册epollout,poller发现缓冲区中仍有数据，会调用handle_write
    if (!error && remaining > 0) {
        // 剩余长度
        size_t leaveLen = output_buffer_.readable_bytes();
        if (leaveLen + remaining >= highWaterMark_ && leaveLen < highWaterMark_) {
            loop_->queue_in_loop(std::bind(
                highWaterCallback_, shared_from_this(), leaveLen + remaining));
        }
        output_buffer_.append(buf.c_str() + nwrote, remaining);
        if (!channel_->is_writing()) {
            channel_->enable_writing();  // 注册channel 写事件,否则poller不会通知
        }
    }
}

// 断开连接
void TcpConnection::shutdown() {
    if (state_ == k_connected) {
        set_state(k_disconnecting);
        loop_->run_in_loop(std::bind(
            &TcpConnection::shutdown_inLoop, this));
    }
}

void TcpConnection::shutdown_inLoop() {
    // 当前outputbuffer中数据，全部发送完成
    loop_->assertInLoopThread();
    if (!channel_->is_writing()) {
        socket_->shutdown_write();  // 关闭写端
    }
}

void TcpConnection::establish_connect() {
    loop_->assertInLoopThread();
    assert(state_ == k_connecting);
    set_state(k_connected);
    channel_->tie(shared_from_this());
    channel_->enable_reading();

    connectionCallback_(shared_from_this());
}

void TcpConnection::destory_connect() {
    loop_->assertInLoopThread();
    if (state_ == k_connected) {
        set_state(k_disconnected);
        channel_->dis_enable_all();
        connectionCallback_(shared_from_this());
    }
    channel_->remove();
}

void TcpConnection::forceClose() {
    if (state_ == k_connected || state_ == k_disconnecting) {
        set_state(k_disconnecting);
        loop_->queue_in_loop(std::bind(
            &TcpConnection::forceClose,
            shared_from_this()));
    }
}

void TcpConnection::forceCloseWithDelay(double seconds) {
    if (state_ == k_connected || state_ == k_disconnecting) {
        set_state(k_disconnecting);
        loop_->runAfter(
            seconds,
            makeWeakCallback(shared_from_this(),
                             &TcpConnection::forceClose));
    }
}

void TcpConnection::forceCloseInLoop() {
    loop_->assertInLoopThread();
    if (state_ == k_connected || state_ == k_disconnecting) {
        handle_close();
    }
}

void TcpConnection::setTcpNoDelay(bool on) {
    socket_->set_tcp_noDelay(on);
}

void TcpConnection::startRead() {
    loop_->run_in_loop(std::bind(
        &TcpConnection::startReadInLoop, this));
}

void TcpConnection::startReadInLoop() {
    loop_->assertInLoopThread();
    if (!reading_ || !channel_->is_reading()) {
        channel_->enable_reading();
        reading_ = true;
    }
}

void TcpConnection::stopRead() {
    loop_->run_in_loop(std::bind(
        &TcpConnection::stopReadInLoop, this));
}

void TcpConnection::stopReadInLoop() {
    loop_->assertInLoopThread();
    if (reading_ || channel_->is_reading()) {
        channel_->dis_enable_reading();
        reading_ = false;
    }
}

void TcpConnection::handle_read(TimeStamp receive_time) {
    loop_->assertInLoopThread();
    int save_errno = 0;
    ssize_t n = input_buffer_.readfd(channel_->get_fd(), &save_errno);
    // 在此处判断是否接收完全
    if (n > 0) {
        messageCallback_(shared_from_this(), &input_buffer_, receive_time);
    } else if (n == 0) {
        handle_close();
    } else {
        errno = save_errno;
        LOG_ERROR("tcp connection::handle read\n");
        handle_error();
    }
}

void TcpConnection::handle_write() {
    loop_->assertInLoopThread();
    if (channel_->is_writing()) {
        int save_errno = 0;
        ssize_t n = sockets::write(channel_->get_fd(),
                                   output_buffer_.peek(),
                                   output_buffer_.readable_bytes());
        if (n > 0) {
            // 发送成功
            output_buffer_.retrieve(n);
            if (output_buffer_.readable_bytes() == 0) {
                channel_->dis_enable_writing();
                // 写完成
                if (writeCallback_) {
                    loop_->queue_in_loop(std::bind(
                        writeCallback_, shared_from_this()));
                }
                // 正在关闭
                if (state_ == k_disconnecting) {
                    shutdown_inLoop();
                }
            }
        } else {
            LOG_ERROR("tcp connection::handle write\n");
        }
    } else {  // 不可写
        LOG_ERROR("tcp connection fd = %d is down, no more send\n", channel_->get_fd());
    }
}

void TcpConnection::handle_close() {
    loop_->assertInLoopThread();
    LOG_INFO(" handle_close fd=%d state=%d", channel_->get_fd(), (int)state_);
    assert(state_ == k_connected || state_ == k_disconnecting);

    set_state(k_disconnected);
    channel_->dis_enable_all();

    TcpConnectionPtr connect_ptr(shared_from_this());

    connectionCallback_(connect_ptr);
    closeCallback_(connect_ptr);
}

void TcpConnection::handle_error() {
    int err = sockets::getSocketError(channel_->get_fd());
    LOG_ERROR("tcp connection handle error name: %s S0_ERROR:%d\n", name_.c_str(), err);
}
