
add_library(MMap STATIC mmap.cpp)

target_include_directories(MMap INTERFACE .)

target_link_libraries(MMap PRIVATE
        SqliteV2_compiler_flags
        DataConversion
)

