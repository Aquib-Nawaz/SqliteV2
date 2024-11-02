
add_executable(iterator_test iterator.test.cpp)

target_link_libraries(iterator_test PRIVATE
        BTreeIterator DataConversion PageKVMemory)

add_test(NAME BTreeIteratorTests COMMAND iterator_test)