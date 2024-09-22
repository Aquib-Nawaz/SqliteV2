
add_executable(convertor_test convertor.test.cpp)

target_link_libraries(convertor_test PRIVATE DataConversion)

add_test(NAME DataConversionTests COMMAND convertor_test)