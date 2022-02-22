include_directories(
	PRIVATE ./Base
)
# 添加对gdb的支持
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set (base_SRCS
	 Net/EventLoopThreadPool.cpp
	 Net/EventLoopThread.hpp
	 Net/EpollPoller.cpp
	 Net/EventLoopThread.cpp
	 Net/EpollPoller.hpp
	 Net/CurrentThread.hpp
	 Net/CurrentThread.cpp
	 Net/EventLoopThreadPool.hpp
	 Net/Channel.cpp
	 Net/Channel.hpp
	 Net/EventLoop.cpp
	 Net/Poller.hpp
	 Net/EventLoop.hpp
	 Net/Poller.cpp
	 Net/DefaultPoller.cpp
	 Base/NonCopyable.hpp
	 Base/Callbacks.hpp
	 Base/Singleton.hpp
	 Base/Any.hpp
	 Thread/ThreadObject.hpp
	 Thread/FuncThreadPool.hpp
	 Thread/ThreadObject.cpp
	 Thread/ThreadPool.hpp
	 Thread/FuncThreadPool.cpp
	 Socket/Socket.cpp
	 Socket/TcpServer.hpp
	 Socket/Buffer.cpp
	 Socket/Buffer.hpp
	 Socket/Socket.hpp
	 Socket/TcpServer.cpp
	 Socket/SocketOps.hpp
	 Socket/TcpConnection.cpp
	 Socket/InetAddress.hpp
	 Socket/TcpConnection.hpp
	 Socket/Acceptor.hpp
	 Socket/InetAddress.cpp
	 Socket/Accetpor.cpp
	 Socket/SocketOps.cpp
	 Log/BlockQueue.hpp
	 Log/Log.hpp
	 Log/Log.cpp
	 Timer/TimeStamp.cpp
	 Timer/TimeStamp.hpp
	 Mysql/SqlConnPool.hpp
	 Mysql/SqlConnPool.cpp
	 Mysql/SqlHandler.hpp
	 Mysql/SqlHandler.cpp
	 Http/HttpRequest.hpp
	 Http/HttpContext.hpp
	 Http/HttpContext.cpp
	 Http/HttpResponse.hpp
	 Http/HttpResponse.cpp
	 Http/HttpServer.hpp
	 Http/HttpServer.cpp
)

set(LIBRARY_OUTPUT_LIB ./lib)

add_library(Net STATIC ${base_SRCS})

find_package(Threads)

target_link_libraries(Net ${CMAKE_THREAD_LIBS_INIT} -lrt libmysqlclient.so)

