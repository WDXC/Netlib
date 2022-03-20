cmake_minimum_required(VERSION 3.16)
project(Netlib)

set(CMAKE_CXX_STANARD 14)

# 添加对gdb的支持
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

add_subdirectory(src)

include_directories(./include)

set(EXECUTABLE_OUTPUT_PATH ./bin)

add_executable(EchoServer ./example/EchoServer_test.cpp)
add_executable(Tcpclient ./example/Tcpclient_demo.cpp)

target_link_libraries(EchoServer Net)
target_link_libraries(Tcpclient Net)
