
add_library(DataConversion STATIC convertor.cpp)

target_include_directories(DataConversion
        INTERFACE .
)
