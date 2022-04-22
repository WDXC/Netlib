#include "Connector.hpp"

#include <assert.h>
#include <errno.h>

#include "Channel.hpp"
#include "EventLoop.hpp"
#include "Log.hpp"
#include "SocketOps.hpp"

const int Connector::kMaxRetryDelayMs;

Connector::Connector(EventLoop *loop, const InetAddress &serverAddr)
    : loop_(loop),
      serverAddr_(serverAddr),
      connect_(false),
      state_(kDisconnected),
      retryDelayMs_(kInitRetryDelayMs) {
    LOG_DEBUG("ctor[%p]", this);
}

Connector::~Connector() {
    LOG_ERROR("dtor[%p]", this);
    assert(!channel_);
}

void Connector::setNewConnectionCallback(const NewConnectionCallback &cb) {
    newConnectionCallback_ = cb;
}

void Connector::start() {
    connect_ = true;
    loop_->run_in_loop(std::bind(&Connector::startInLoop, this));
}

void Connector::startInLoop() {
    loop_->assertInLoopThread();
    assert(state_ == kDisconnected);
    if (connect_) {
        connect();
    } else {
        LOG_DEBUG("do not connect");
    }
}

void Connector::stop() {
    connect_ = false;
    loop_->queue_in_loop(std::bind(&Connector::stopInLoop, this));
}

void Connector::stopInLoop() {
    loop_->assertInLoopThread();
    if (state_ == kConnecting) {
        setState(kDisconnected);
        int sockfd = removeAndResetChannel();
        retry(sockfd);
    }
}

const InetAddress &Connector::serverAddress() const {
    return serverAddr_;
}

void Connector::setState(States s) {
    state_ = s;
}

void Connector::connect() {
    int sockfd = sockets::createNooBlockingSocket();
    int ret = sockets::connect(sockfd, serverAddr_.getSockAddr());
    int saveErrno = (ret == 0) ? 0 : errno;
    switch (saveErrno) {
        case 0:
        case EINPROGRESS:
        case EINTR:
        case EISCONN:
            connecting(sockfd);
            break;

        case EAGAIN:
        case EADDRINUSE:
        case EADDRNOTAVAIL:
        case ECONNREFUSED:
        case ENETUNREACH:
            retry(sockfd);
            break;

        case EACCES:
        case EPERM:
        case EAFNOSUPPORT:
        case EALREADY:
        case EBADF:
        case EFAULT:
        case ENOTSOCK:
            LOG_ERROR("connector error in Connector::startInLoop %d ", saveErrno);
            sockets::close(sockfd);
            break;

        default:
            LOG_ERROR("Unexpected error in Connector::startInLoop %d", saveErrno);
            sockets::close(sockfd);
            break;
    }
}

void Connector::restart() {
    loop_->assertInLoopThread();
    setState(kDisconnected);
    retryDelayMs_ = kInitRetryDelayMs;
    connect_ = true;
    startInLoop();
}

void Connector::connecting(int sockfd) {
    setState(kConnecting);
    assert(!channel_);
    channel_.reset(new Channel(loop_, sockfd));
    channel_->setWriteCallback(std::bind(&Connector::handleWrite, this));
    channel_->setErrorCallback(std::bind(&Connector::handleError, this));

    channel_->enable_writing();
}

int Connector::removeAndResetChannel() {
    channel_->dis_enable_all();
    LOG_ERROR("Failed in here");
    channel_->remove();
    int sockfd = channel_->get_fd();
    loop_->queue_in_loop(std::bind(&Connector::resetChannel, this));
    return sockfd;
}

void Connector::resetChannel() {
    channel_.reset();
}

void Connector::handleWrite() {
    LOG_DEBUG("Connector::handleWrite %d", (int)state_);

    if (state_ == kConnecting) {
        int sockfd = removeAndResetChannel();
        int err = sockets::getSocketError(sockfd);
        if (err) {
            LOG_WARN("Connector::handleWrite - SO_ERROR = %d", err);
            retry(sockfd);
        } else if (sockets::isSelfConnect(sockfd)) {
            LOG_WARN("Connector::handleWrite - Self Connect");
            retry(sockfd);
        } else {
            setState(kConnected);
            if (connect_) {
                newConnectionCallback_(sockfd);
            } else {
                sockets::close(sockfd);
            }
        }
    } else {
        assert(state_ == kDisconnected);
    }
}

void Connector::handleError() {
    LOG_DEBUG("Connector::handleError state = %d", (int)state_);

    if (state_ == kConnecting) {
        int sockfd = removeAndResetChannel();
        int err = sockets::getSocketError(sockfd);
        LOG_DEBUG("SO_ERROR = %d", err);
        retry(sockfd);
    }
}

void Connector::retry(int sockfd) {
    sockets::close(sockfd);
    setState(kDisconnected);
    if (connect_) {
        LOG_INFO("Connector::retry - Retry connecting to %s in %d milliseconds",
                 serverAddr_.get_ip_port().c_str(), retryDelayMs_);
        loop_->runAfter(retryDelayMs_ / 1000.0,
                        std::bind(&Connector::startInLoop, shared_from_this()));
        retryDelayMs_ = std::min(retryDelayMs_ * 2, kMaxRetryDelayMs);
    } else {
        LOG_DEBUG("do not connect");
    }
}