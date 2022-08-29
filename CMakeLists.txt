cmake_minimum_required(VERSION 3.13.0)

project(learningNetworking)

set(CMAKE_CXX_FLAGS_DEBUG_INIT "-g")

add_subdirectory(client)
add_subdirectory(server)

