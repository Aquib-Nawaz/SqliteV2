
add_library(HashMapDBMemory STATIC hashmapdb.cpp btreememory/inmemorybtreeio.cpp)

target_include_directories(HashMapDBMemory
        INTERFACE .
)

target_link_libraries(HashMapDBMemory PRIVATE
        SqliteV2_compiler_flags
        DB
)

target_link_libraries(HashMapDBMemory PUBLIC
        BTree)