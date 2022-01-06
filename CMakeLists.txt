cmake_minimum_required(VERSION 3.18)
project(sync CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

add_library(sync sync/mutex.cpp)
target_include_directories(sync PUBLIC sync/include)

add_executable(main main/main.cpp)
target_link_libraries(main sync)
