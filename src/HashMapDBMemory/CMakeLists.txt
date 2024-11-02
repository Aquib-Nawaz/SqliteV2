
add_library(HashMapDBMemory STATIC hashmapkv.cpp btreememory/inmemorybtreeio.cpp)

target_include_directories(HashMapDBMemory
        INTERFACE . btreememory
)

target_link_libraries(HashMapDBMemory PRIVATE
        SqliteV2_compiler_flags
)

target_link_libraries(HashMapDBMemory PUBLIC
        BTree
        DB
)