# cmake 最低版本需求
cmake_minimum_required(VERSION 3.13)

# 工程名称
project(cmake_study)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)

# 添加外部依赖
include_directories(/usr/local/include)
link_libraries(gtest pthread)

# 添加子目录
add_subdirectory(math)
add_subdirectory(magic)

# 编译源码生成目标
add_executable(Demo src/main.cpp)
# 目标依赖
target_link_libraries(Demo magic gtest pthread)

# 编译完自动运行
# add_custom_command(TARGET Demo POST_BUILD COMMAND ./Demo)
