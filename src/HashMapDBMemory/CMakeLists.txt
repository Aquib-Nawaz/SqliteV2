
add_library(HashMapDBMemory STATIC hashmapdb.cpp)

target_include_directories(HashMapDBMemory
        INTERFACE .
)

target_link_libraries(HashMapDBMemory PRIVATE
        SqliteV2_compiler_flags
)

target_link_libraries(HashMapDBMemory PUBLIC
        DBMemory)