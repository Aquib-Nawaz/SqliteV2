
add_executable(pagedb_test pagedb.test.cpp)

target_link_libraries(pagedb_test PRIVATE PageDBMemory
        DataConversion)

add_test(NAME PageDBMemoryTests COMMAND pagedb_test)