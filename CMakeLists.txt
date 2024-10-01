cmake_minimum_required(VERSION 3.26)
project(SqliteV2)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(src/DataConversion)
add_subdirectory(src/BNode)
add_subdirectory(src/BTree)
add_subdirectory(src/HashMapDBMemory)
add_subdirectory(src/PageDBMemory)
add_subdirectory(src/FreeList)
add_subdirectory(src)
enable_testing()
add_library(SqliteV2_compiler_flags INTERFACE)
target_compile_features(SqliteV2_compiler_flags INTERFACE cxx_std_11)

# add compiler warning flags just when building this project via
# the BUILD_INTERFACE genex
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
target_compile_options(SqliteV2_compiler_flags INTERFACE
        "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>>"
        "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>"
)
add_subdirectory(test/DataConversion)
add_subdirectory(test/BNode)
add_subdirectory(test/BTree)
add_subdirectory(test/PageDBMemory)

