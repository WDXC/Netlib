#ifndef CONNECTOR_H_
#define CONNECTOR_H_

#include <atomic>
#include <functional>
#include <memory>

#include "../Base/NonCopyable.hpp"
#include "InetAddress.hpp"

class Channel;
class EventLoop;


using NewConnectionCallback = std::function<void(int sockfd)>;

class Connector : NoCopyable,
                  public std::enable_shared_from_this<Connector> {
   public:
    Connector(EventLoop* loop, const InetAddress& serverAddr);
    ~Connector();

    void setNewConnectionCallback(const NewConnectionCallback& cb);
    void start();
    void restart();
    void stop();

    const InetAddress& serverAddress() const;

   private:
    enum States {
        kDisconnected,
        kConnecting,
        kConnected
    };
    static const int kMaxRetryDelayMs = 30 * 1000;
    static const int kInitRetryDelayMs = 500;

    void setState(States s);
    void startInLoop();
    void stopInLoop();
    void connect();
    void connecting(int sockfd);
    void handleWrite();
    void handleError();
    void retry(int sockfd);
    int removeAndResetChannel();
    void resetChannel();

   private:
    EventLoop* loop_;
    InetAddress serverAddr_;
    std::atomic_bool connect_;
    States state_;
    std::unique_ptr<Channel> channel_;
    NewConnectionCallback newConnectionCallback_;
    int retryDelayMs_;
};

#endif