
add_library(BTree STATIC btree.cpp)

target_include_directories(BTree
        INTERFACE .
)

target_link_libraries(BTree PRIVATE
        SqliteV2_compiler_flags)

target_link_libraries(BTree PUBLIC
        BNode)