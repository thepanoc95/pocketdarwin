cmake_minimum_required(VERSION 3.15)
project(NextStd LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

option(BUILD_SHARED "Build shared library" ON)

add_library(NextStd
    Sources/NSObject.m
    Sources/NSString.m
    Sources/NSArray.m
    Sources/NSDictionary.m
    Sources/NSData.m
    Sources/NSEnumerator.m
)

target_include_directories(NextStd
    PUBLIC Headers/NextStd
)

target_compile_options(NextStd PRIVATE
    -fobjc-arc
    -fobjc-weak
    -fmodules
)

if(BUILD_SHARED)
    set_target_properties(NextStd PROPERTIES
        FRAMEWORK TRUE
        MACOSX_FRAMEWORK_IDENTIFIER com.nextstd.NextStd
    )
endif()