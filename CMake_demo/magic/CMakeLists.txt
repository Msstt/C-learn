# 选中文件
aux_source_directory(test TEST)
aux_source_directory(src SRC)

# 单元测试（自动运行）
add_executable(magic_test ${TEST} ${SRC})
target_include_directories(magic_test PUBLIC include)
# 依赖其他模块
target_link_libraries(magic_test mmath)
# 自动运行
add_custom_target(magic_test_done COMMAND ./magic_test DEPENDS magic_test)

# 生成库（通过单元测试后）
add_library(magic SHARED ${SRC})
target_include_directories(magic PUBLIC include)
# 依赖其他模块
target_link_libraries(magic PUBLIC mmath)
# 通过单元测试
add_dependencies(magic magic_test_done)
