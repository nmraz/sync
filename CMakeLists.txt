cmake_minimum_required(VERSION 3.18)
project(sync CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
add_compile_options(-Wall -Wextra)

add_library(sync sync/mutex.cpp sync/cond_var.cpp sync/util.cpp)
target_include_directories(sync PUBLIC sync/include)

add_executable(main main/main.cpp)
target_link_libraries(main sync)
