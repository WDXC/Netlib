// all callback in this file

#ifndef CALLBACK_H_
#define CALLBACK_H_

#include <memory>
#include <functional>
#include "../Timer/TimeStamp.hpp"

class Buffer;
class TcpConnection;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallback = std::function<void (const TcpConnectionPtr &)>;
using CloseCallback = std::function<void(const TcpConnectionPtr &)>;
using WriteCompleteCallback = std::function<void (const TcpConnectionPtr &)>;

using MessageCallback = std::function<void (const TcpConnectionPtr &,
                                      Buffer *,
                                      TimeStamp)>;
using HighWaterMarkCallback = std::function<void(const TcpConnectionPtr &, size_t)>;



#endif