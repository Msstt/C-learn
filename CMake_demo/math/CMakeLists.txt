aux_source_directory(src SRCS)

add_library(mmath SHARED ${SRCS})

target_include_directories(mmath PUBLIC include)
