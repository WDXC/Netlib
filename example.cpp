#include "src/Socket/TcpServer.hpp"
#include <string>
#include <functional>

class EchoServer {
	public:
		EchoServer(EventLoop* loop, InetAddress& addr,const std::string& name) : 
			server_(loop, addr, name), loop_(loop) {
				server_.setConnectionCallback(std::bind(&EchoServer::on_connection, this, std::placeholders::_1));
				server_.setMessageCallback(std::bind(&EchoServer::on_message, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
				server_.set_thread_num(3);
		}

		void start() {
			server_.start();
		}

	private:
			void on_connection(const TcpConnectionPtr& conn) {
				if (conn->connected()) {
					LOG_INFO("conn up: %s", conn->get_peerAddr().get_ip_port().c_str());
				} else {
					LOG_INFO("conn down : %s", conn->get_peerAddr().get_ip_port().c_str());
				}
			}

			void on_message(const TcpConnectionPtr& conn, Buffer* buffer, TimeStamp time) {
				std::string msg = buffer->retrieve_asString();
				conn->send(msg);
				conn->shutdown();
			}
	private:
			TcpServer server_;
			EventLoop* loop_;
};

int main() {
	LOG_INFO("TEST OUTPUT");
	EventLoop loop;
	InetAddress addr(8000, true, false);
	EchoServer server(&loop, addr, "echo o1");
	server.start();
	loop.loop();
	return 0;
}
