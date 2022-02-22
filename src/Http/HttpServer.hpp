/*** 
 * @Author: Zty
 * @Date: 2022-02-19 16:11:01
 * @LastEditTime: 2022-02-19 16:51:29
 * @LastEditors: Zty
 * @Description: 
 * @FilePath: /multhread/src/Http/HttpServer.hpp
 */

#ifndef HTTPSERVER_H_
#define HTTPSERVER_H_

#include "../Socket/TcpServer.hpp"

class HttpRequest;
class HttpResponse;

class HttpServer : NoCopyable {
    public:
        using HttpCallback = std::function<void (const HttpRequest&, HttpResponse*)>;

    public:
        HttpServer(EventLoop* loop,
                   const InetAddress& listAddr,
                   const std::string& name,
                   TcpServer::Option option = TcpServer::k_noReuse_port);

        void setHttpCallback(const HttpCallback& cb) {
            httpCallback_ = cb;
        }

        void setThreadNum(int num) {
            server_.set_thread_num(num);
        }

        void start();
        
    private:
        void onConnection(const TcpConnectionPtr& conn);
        void onMessage(const TcpConnectionPtr& conn, Buffer* buf, TimeStamp receive);
        void onRequest(const TcpConnectionPtr& conn, const HttpRequest&);

    private:
        TcpServer server_;
        HttpCallback httpCallback_;
};

#endif