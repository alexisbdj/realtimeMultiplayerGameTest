cmake_minimum_required(VERSION 3.13.0)

project(server)

add_executable(server)

target_compile_features(server PUBLIC cxx_std_20)

include_directories(server include)

add_subdirectory(src)