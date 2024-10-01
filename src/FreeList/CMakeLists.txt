
add_library(FreeList STATIC flist.cpp)

target_link_libraries(FreeList PRIVATE
        SqliteV2_compiler_flags
        DataConversion
)

