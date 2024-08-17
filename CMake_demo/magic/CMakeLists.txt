aux_source_directory(src SRCS)

aux_source_directory(includes INCLUDES)

add_library(magic SHARED ${SRCS})

target_link_libraries(magic PUBLIC mmath)

target_include_directories(magic PUBLIC includes)
