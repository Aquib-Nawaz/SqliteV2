
add_executable(diskkv_test diskkv.test.cpp)

target_link_libraries(diskkv_test PRIVATE
        PageKVMemory)

add_test(NAME DiskKVTests COMMAND diskkv_test)