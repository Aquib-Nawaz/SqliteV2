
add_executable(table_test table.test.cpp)

target_link_libraries(table_test PRIVATE Table
        DataConversion)

add_test(NAME TableTests COMMAND table_test)