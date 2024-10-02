
add_library(PageDBMemory STATIC pagedb.cpp)

target_include_directories(PageDBMemory
        INTERFACE .
)

target_link_libraries(PageDBMemory PRIVATE
        SqliteV2_compiler_flags
        DataConversion
)

target_link_libraries(PageDBMemory PUBLIC
        BTree MMap)