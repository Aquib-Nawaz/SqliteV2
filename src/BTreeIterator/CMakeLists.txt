add_library(BTreeIterator STATIC iterator.cpp)
target_include_directories(BTreeIterator INTERFACE .)

target_link_libraries(BTreeIterator PUBLIC BTree)
target_link_libraries(BTreeIterator PRIVATE
        SqliteV2_compiler_flags)