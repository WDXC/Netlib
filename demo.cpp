#include <iostream>
#include "src/Socket/TcpServer.hpp"

using namespace std;

int main() {
	g_LogMgr::instance()->init("./log", 5000000, 1000);
	LOG_INFO("failed");
	return 0;
}
