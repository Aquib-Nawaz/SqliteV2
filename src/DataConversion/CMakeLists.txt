
add_library(DataConversion STATIC convertor.cpp)

target_include_directories(DataConversion
        INTERFACE .
)

target_link_libraries(DataConversion PRIVATE SqliteV2_compiler_flags)