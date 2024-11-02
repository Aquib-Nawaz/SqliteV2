
add_executable(btree_test btree.test.cpp)

target_link_libraries(btree_test PRIVATE
        HashMapDBMemory)

add_test(NAME BTreeTests COMMAND btree_test)