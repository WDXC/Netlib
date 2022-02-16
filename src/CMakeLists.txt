include_directories(
	PRIVATE ./Base
)

set (base_SRCS
	 Net/EventLoopThreadPool.cpp
	 Net/EventLoopThread.hpp
	 Net/EpollPoller.cpp
	 Net/EventLoopThread.cpp
	 Net/EpollPoller.hpp
	 Net/CurrentThread.h
	 Net/EventLoopThreadPool.hpp
	 Net/Channel.cpp
	 Net/Channel.hpp
	 Net/EventLoop.cpp
	 Net/Poller.hpp
	 Net/EventLoop.hpp
	 Net/Poller.cpp
	 Base/NonCopyable.hpp
	 Base/Callbacks.hpp
	 Base/Singleton.hpp
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
	 Log/Logger.cpp
	 Log/BlockQueue.h
	 Log/Logger.hpp
	 Timer/TimeStamp.cpp
	 Timer/TimeStamp.hpp
	 Mysql/SqlConnPool.hpp
	 Mysql/SqlConnPool.cpp
)

add_library(lib_base ${base_SRCS})

find_package(Threads)

target_link_libraries(lib_base ${CMAKE_THREAD_LIBS_INIT} -lrt libmysqlclient.so)

