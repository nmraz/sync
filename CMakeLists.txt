cmake_minimum_required(VERSION 3.18)
project(sync CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
add_compile_options(-Wall -Wextra)

add_library(sync src/sync/mutex.cpp src/sync/cond_var.cpp src/sync/util.cpp)
target_include_directories(sync PUBLIC src/sync/include)

function(add_bin NAME)
    add_executable(${NAME} src/bin/${NAME}.cpp)
    target_link_libraries(${NAME} sync)
endfunction()

add_bin(my_mutex_bench)
add_bin(std_mutex_bench)
add_bin(sleep_bench)
