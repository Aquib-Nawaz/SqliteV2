add_library(DB STATIC db.cpp)
target_include_directories(DB INTERFACE .)

target_link_libraries(DB PUBLIC
        Table
        DBApi
)

target_link_libraries(DB PRIVATE
        SqliteV2_compiler_flags
)