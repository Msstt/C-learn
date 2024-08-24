# cmake 最低版本需求
cmake_minimum_required(VERSION 3.13)

# 工程名称
project(cmake_study)

# 设置
set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/deps/gtest/include)
link_directories(${CMAKE_SOURCE_DIR}/deps/gtest/lib)

add_subdirectory(math)

add_subdirectory(magic)

# 编译源码生成目标
add_executable(Demo src/main.cpp)

target_link_libraries(Demo magic gtest)

# add_custom_command(TARGET Demo POST_BUILD COMMAND ./Demo)