/*** 
 * @Author: Zty
 * @Date: 2022-02-13 10:03:40
 * @LastEditTime: 2022-02-15 10:21:05
 * @LastEditors: Zty
 * @Description: 所有回调
 * @FilePath: /multhread/Base/Callbacks.hpp
 */

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