aux_source_directory(test TEST)
aux_source_directory(src SRC)

add_executable(mmath_test ${TEST} ${SRC})
target_include_directories(mmath_test PUBLIC include)
add_custom_target(mmath_test_done COMMAND ./mmath_test DEPENDS mmath_test)

add_library(mmath SHARED ${SRC})
target_include_directories(mmath PUBLIC include)
add_dependencies(mmath mmath_test_done)
