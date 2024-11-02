
add_library(BNode STATIC bnode.cpp
        bnodefactory.cpp)

target_include_directories(BNode
        INTERFACE .
)

target_link_libraries(
        BNode
        PUBLIC
        DBApi
)


target_link_libraries(BNode PRIVATE
        DataConversion
        SqliteV2_compiler_flags
)