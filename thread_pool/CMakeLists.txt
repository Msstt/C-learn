cmake_minimum_required(VERSION 3.13)
project(cmake_study)
set(CMAKE_CXX_STANDARD 20)

include_directories(/usr/local/include)
link_libraries(gtest pthread)

add_executable(Test main.cpp thread_pool.cpp)
add_custom_command(TARGET Test POST_BUILD COMMAND ./Test)
