cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_CXX_STANDARD 14)

include_directories(${PROJECT_SOURCE_DIR}/include/Http
										${PROJECT_SOURCE_DIR}/include/Socket
										${PROJECT_SOURCE_DIR}/include/Log
										${PROJECT_SOURCE_DIR}/include/Net
										${PROJECT_SOURCE_DIR}/include/Timer
										${PROJECT_SOURCE_DIR}/include/Thread
										${PROJECT_SOURCE_DIR}/include/Base
										${PROJECT_SOURCE_DIR}/include/Mysql
)

# 添加对gdb的支持
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

file(GLOB_RECURSE native_srcs ./*.cpp)

file(GLOB_RECURSE include_file ${PROJECT_SOURCE_DIR}/include/*.hpp)


add_library(Net STATIC ${native_srcs} ${include_file})

find_package(Threads)

target_link_libraries(Net ${CMAKE_THREAD_LIBS_INIT} -lrt libmysqlclient.so)
