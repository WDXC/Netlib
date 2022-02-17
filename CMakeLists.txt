cmake_minimum_required(VERSION 3.16)
project(Netlib)

set(CMAKE_CXX_STANARD 14)

# 添加对gdb的支持
SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

add_subdirectory(src)

add_executable(Netlib example.cpp)

target_link_libraries(Netlib lib_base)
