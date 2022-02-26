#include <string.h>
#include <iostream>
#include <map>
#include "Http/HttpServer.hpp"
#include "Http/HttpContext.hpp"
#include "Http/HttpRequest.hpp"
#include "Http/HttpResponse.hpp"
#include "Timer/TimeStamp.hpp"

using namespace std;

void onRequest(const HttpRequest& req, HttpResponse* resp) {
  std::cout << "Headers " << req.methodString() << " " << req.GetPath()
            << std::endl;
  if (req.GetPath() == "/") {
    resp->setStatusCode(HttpResponse::k200Ok);
    resp->setStatusMessage("OK");
    resp->setContentType("text/html");
    resp->addHeader("Server", "Muduo");

    resp->setBody(
        "<html><head><title>This is title</title></head>"
        "<body><h1>Hello</h1>Now is </body></html>");
  } 
}

int main(int argc, char* argv[]) {
  g_LogMgr::instance()->init("./log", 5000000, 1000);
  int numThreads = 0;
  EventLoop loop;
  HttpServer server(&loop, InetAddress(8000), "dummy");
  server.setHttpCallback(onRequest);
  server.setThreadNum(1);
  server.start();
  loop.loop();
}

