# 选中文件
aux_source_directory(test TEST)
aux_source_directory(src SRC)

# 单元测试
add_executable(mmath_test ${TEST} ${SRC})
target_include_directories(mmath_test PUBLIC include)
# 自动运行
add_custom_target(mmath_test_done COMMAND ./mmath_test DEPENDS mmath_test)

# 生成库
add_library(mmath SHARED ${SRC})
target_include_directories(mmath PUBLIC include)
# 通过单元测试
add_dependencies(mmath mmath_test_done)
