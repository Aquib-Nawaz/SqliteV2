
add_executable(bnode_test bnode.test.cpp)

target_link_libraries(bnode_test PRIVATE BNode)

add_test(NAME BNodeTests COMMAND bnode_test)