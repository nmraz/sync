cmake_minimum_required(VERSION 3.18)
project(sync CXX)

add_library(sync sync/mutex.cpp)
target_include_directories(sync INTERFACE sync)

add_executable(main main/main.cpp)
target_link_libraries(main sync)
