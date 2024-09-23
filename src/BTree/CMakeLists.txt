
add_library(BTree STATIC btree.cpp)

target_include_directories(BTree
        INTERFACE .
)

target_link_libraries(BTree PRIVATE
        BNode)