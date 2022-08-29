cmake_minimum_required(VERSION 3.13.0)

project(client)

add_executable(client)

target_compile_features(client PUBLIC cxx_std_20)

include_directories(client include)

add_subdirectory(src)

target_link_libraries(client sfml-graphics sfml-window sfml-system)