cmake_minimum_required(VERSION 3.13)
project(cmake_study)
set(CMAKE_CXX_STANDARD 20)

include_directories(${CMAKE_SOURCE_DIR}/deps/gtest/include)
link_directories(${CMAKE_SOURCE_DIR}/deps/gtest/lib)

add_executable(Test main.cpp thread_pool.cpp)
target_link_libraries(Test gtest)
add_custom_command(TARGET Test POST_BUILD COMMAND ./Test)