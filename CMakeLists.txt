cmake_minimum_required(VERSION "2.8.9")

project(Citizens)

file(GLOB server_sources server/src/*.cpp common/src/*.cpp)
file(GLOB client_sources client/src/*.cpp common/src/*.cpp)
include_directories(server/include client/include common/include)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")

add_executable(bin/server ${server_sources})
add_executable(bin/client ${client_sources})
