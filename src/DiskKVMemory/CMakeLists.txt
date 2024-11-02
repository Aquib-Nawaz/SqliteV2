
add_library(PageKVMemory STATIC pagekv.cpp
        diskbtreememory/diskbtreeio.cpp)

target_include_directories(PageKVMemory
        INTERFACE .
)

target_link_libraries(PageKVMemory PRIVATE
        SqliteV2_compiler_flags
        DataConversion
)

target_link_libraries(PageKVMemory PUBLIC
        BTree MMap DB)