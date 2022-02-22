#include "HttpServer.hpp"
#include "HttpContext.hpp"
#include "HttpResponse.hpp"

void defaultHttpCallback(const HttpRequest&, HttpResponse* resp) {
    resp->setStatusCode(HttpResponse::k404NotFound);
    resp->setStatusMessage("Not Found");
    resp->setCloseConnection(true);
}

HttpServer::HttpServer(EventLoop* loop,
                       const InetAddress& listAddr,
                       const std::string& name,
                       TcpServer::Option option) :
    server_(loop, listAddr, name, option),
    httpCallback_(defaultHttpCallback) {
    server_.setConnectionCallback(std::bind(
        &HttpServer::onConnection, this, std::placeholders::_1));
    server_.setMessageCallback(std::bind(
        &HttpServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
}

void HttpServer::start() {
    LOG_INFO("HttpServer[%s] starts listening on %s", server_.getName().c_str(), server_.getIpPort().c_str());
    server_.start();
}

void HttpServer::onConnection(const TcpConnectionPtr& conn) {
    if (conn->connected()) {
        conn->setContext((HttpContext) HttpContext());
    }
}

void HttpServer::onMessage (const TcpConnectionPtr& conn,
                            Buffer* buf,
                            TimeStamp receTime) {
		HttpContext* context = boost::any_cast<HttpContext>(conn->getMutableContext());

    if (!context->parseRequest(buf, receTime)) {
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }

    if (context->gotAll()) {
        onRequest(conn, context->getRequest());
        context->reset();
    }
}

void HttpServer::onRequest (const TcpConnectionPtr& conn, const HttpRequest& req) {
    const std::string& connection = req.getHeader("Connection");
    bool close = connection == "close" ||
                (req.getVersion() == HttpRequest::kHttp10 && connection != "Keep-Alive");
    HttpResponse response(close);
    httpCallback_(req, &response);
    Buffer buf;
    response.appendToBuffer(&buf);
    conn->send(&buf);
    if (response.getcloseConnection()) {
        conn->shutdown();
    }
}
