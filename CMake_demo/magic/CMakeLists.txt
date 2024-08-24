aux_source_directory(test TEST)
aux_source_directory(src SRC)

add_executable(magic_test ${TEST} ${SRC})
target_link_libraries(magic_test gtest mmath)
target_include_directories(magic_test PUBLIC include)
add_custom_target(magic_test_done COMMAND ./magic_test DEPENDS magic_test)

add_library(magic SHARED ${SRC})
add_dependencies(magic magic_test_done)
target_include_directories(magic PUBLIC include)
target_link_libraries(magic PUBLIC mmath)
