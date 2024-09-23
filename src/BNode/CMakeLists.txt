
add_library(BNode STATIC bnode.cpp)

target_include_directories(BNode
        INTERFACE .
)

target_link_libraries(BNode PRIVATE
        DataConversion
        SqliteV2_compiler_flags)