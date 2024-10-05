
add_library(Table STATIC table.cpp)

target_include_directories(Table
        INTERFACE .
)

target_link_libraries(Table PRIVATE
        SqliteV2_compiler_flags
        DataConversion
)
