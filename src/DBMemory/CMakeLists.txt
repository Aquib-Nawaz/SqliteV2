
add_library(DBMemory STATIC dbmemory.cpp)

target_include_directories(DBMemory
        INTERFACE .
)

target_link_libraries(DBMemory PRIVATE SqliteV2_compiler_flags)