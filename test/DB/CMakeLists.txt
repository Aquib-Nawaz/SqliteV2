
add_executable(db_test db.test.cpp)

target_link_libraries(db_test PRIVATE DB )

add_test(NAME DBTests COMMAND db_test)