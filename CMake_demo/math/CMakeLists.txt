aux_source_directory(src SRCS)

aux_source_directory(includes INCLUDES)

add_library(mmath SHARED ${SRCS})

target_include_directories(mmath PUBLIC includes)

# target_link_libraries(mmath PUBLIC magic)
