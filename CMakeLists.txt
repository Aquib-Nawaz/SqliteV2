cmake_minimum_required(VERSION 3.26)
project(SqliteV2)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src/DataConversion)
add_subdirectory(src/BNode)
enable_testing()

add_subdirectory(test/DataConversion)
add_subdirectory(test/BNode)
