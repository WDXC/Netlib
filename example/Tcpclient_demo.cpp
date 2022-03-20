#include "Log/Log.hpp"
#include "Net/EventLoop.hpp"
#include "Socket/TcpClient.hpp"

TcpClient* g_client;

void timeout()
{
  LOG_INFO("timeout");
  g_client->stop();
}

int main(int argc, char* argv[])
{
  g_LogMgr::instance()->init("./log", 5000000, 1000);
  EventLoop loop;
  InetAddress serverAddr("127.0.0.1", 2); // no such server
  TcpClient client(&loop, serverAddr, "TcpClient");
  g_client = &client;
  loop.runAfter(0.0, timeout);
  loop.runAfter(1.0, std::bind(&EventLoop::quit, &loop));
  client.connect();
  CurrentThread::sleepUsec(100 * 1000);
  loop.loop();
}
