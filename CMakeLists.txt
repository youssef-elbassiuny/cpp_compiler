cmake_minimum_required(VERSION 3.28)
project(compiler)
add_executable(main scr/main.cpp)
target_include_directories(main PUBLIC include)