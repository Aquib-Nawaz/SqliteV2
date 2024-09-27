
add_executable(btree_test btree.test.cpp)

target_link_libraries(btree_test PRIVATE
        BTree BNode HashMapDBMemory SqliteV2_compiler_flags)

add_test(NAME BTreeTests COMMAND btree_test)