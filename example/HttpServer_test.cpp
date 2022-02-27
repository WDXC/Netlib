#include <string.h>
#include <iostream>
#include <map>
#include "Http/HttpServer.hpp"
#include "Http/HttpContext.hpp"
#include "Http/HttpRequest.hpp"
#include "Http/HttpResponse.hpp"
#include "Timer/TimeStamp.hpp"
#include "Socket/Json_data.hpp"

using namespace std;

bool benchmark = false;

void onRequest(const HttpRequest& req, HttpResponse* resp) {
  std::cout << "Headers " << req.methodString() << " " << req.GetPath()
            << std::endl;

	if (!benchmark)
  {
      const std::map<string, string>& headers = req.Header();
      for (const auto& header : headers)
      {
          std::cout << header.first << ": " << header.second << std::endl;
      }
  }


 if (req.GetPath() == "/api")
  {
    resp->setContentType("*/*");
    resp->addHeader("Server", "Muduo");
    resp->addHeader("Access-Control-Allow-Origin", "*");
    resp->addHeader("Cache-Control", "no-cache");

    std::string strData;
    Json::CJsonData val = req.getRequestBody();
    val.EncodeJson(strData);

    resp->setBody(strData);
  }
	
  if (req.GetPath() == "/register") {
        resp->setContentType("*/*");
        resp->addHeader("Server", "Zty");
        resp->addHeader("Access-Control-Allow-Origin", "*");
        resp->addHeader("Cache-Control", "no-cache");

        Json::CJsonData root;
        root.PushValue("type", "register");
        root.PushValue("body", "hello world");
        std::string strData;
        root.EncodeJson(strData);
        resp->setBody(strData);
  }
}

int main(int argc, char* argv[]) {
  g_LogMgr::instance()->init("./log", 5000000, 1000);
  int numThreads = 0;
  EventLoop loop;
  HttpServer server(&loop, InetAddress(8000), "dummy");
  server.setHttpCallback(onRequest);
  server.setThreadNum(12); // 与CPU核心数相同 
  server.start();
  loop.loop();
}

