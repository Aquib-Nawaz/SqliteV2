add_library(Scanner STATIC
    Scanner.cpp)

target_link_libraries(Scanner
    PUBLIC
    Table BTreeIterator
)

target_include_directories(Scanner
    INTERFACE
    .
)
