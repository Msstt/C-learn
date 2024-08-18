# cmake 最低版本需求
cmake_minimum_required(VERSION 3.13)

# 工程名称
project(cmake_study)

# 设置
set(CMAKE_CXX_STANDARD 11)

add_subdirectory(math)

add_subdirectory(magic)

include_directories(deps/gtest/include)
link_directories(deps/gtest/lib)

# 编译源码生成目标
add_executable(Demo src/main.cpp)

target_link_libraries(Demo magic gtest)

target_link_libraries(Demo gtest)
