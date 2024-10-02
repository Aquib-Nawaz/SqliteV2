
add_executable(mmap_test mmap.test.cpp)

target_link_libraries(mmap_test PRIVATE MMap
        DataConversion)

add_test(NAME MMapChunkTests COMMAND mmap_test)