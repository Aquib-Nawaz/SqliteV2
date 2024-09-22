cmake_minimum_required(VERSION 3.26)
project(SqliteV2)

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES main.cpp)
list(APPEND SOURCE_FILES BTree/btree.cpp)
list(APPEND SOURCE_FILES DataConversion/convertor.cpp)
list(APPEND SOURCE_FILES BNode/bnode.cpp)
#add_executable(SqliteV2 ${SOURCE_FILES})

enable_testing()

add_subdirectory(test)
