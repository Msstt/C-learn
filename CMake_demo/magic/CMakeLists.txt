aux_source_directory(src SRC)

add_library(magic SHARED ${SRC})

target_link_libraries(magic PUBLIC mmath)

target_include_directories(magic PUBLIC include)
