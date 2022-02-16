cmake_minimum_required(VERSION 3.16)
project(Netlib)

set(CMAKE_CXX_STANARD 14)

add_subdirectory(src)

add_executable(Netlib main.cpp)

target_link_libraries(Netlib lib_base)
