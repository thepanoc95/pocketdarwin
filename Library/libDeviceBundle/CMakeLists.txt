cmake_minimum_required(VERSION 3.10)
project(libDeviceBundle CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Main library sources
set(SOURCES
    DeviceInfo.cpp
    DeviceBundle.cpp
    DeviceTree.cpp
    libpbxparser/plist_decoder.cc
    libpbxparser/value.cc
)

set(HEADERS
    DeviceInfo.hpp
    DeviceBundle.hpp
    DeviceTree.hpp
    libpbxparser/plist_decoder.hpp
    libpbxparser/value.hpp
)

# Create shared library
add_library(DeviceBundle SHARED ${SOURCES})
target_include_directories(DeviceBundle PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/libpbxparser
)

# Create static library
add_library(DeviceBundle_static STATIC ${SOURCES})
set_target_properties(DeviceBundle_static PROPERTIES OUTPUT_NAME DeviceBundle)
target_include_directories(DeviceBundle_static PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/libpbxparser
)

# Create tool executable
add_executable(device-bundle-tool device-bundle-tool.cpp ${SOURCES})
target_include_directories(device-bundle-tool PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/libpbxparser
)

# Create test executable
add_executable(device-bundle-tests tests.cpp ${SOURCES})
target_include_directories(device-bundle-tests PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/libpbxparser
)

# Installation
install(TARGETS DeviceBundle DeviceBundle_static device-bundle-tool
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)
install(FILES ${HEADERS} DESTINATION include/libDeviceBundle)
install(FILES create-device-bundle.py DESTINATION bin PERMISSIONS OWNER_EXECUTE GROUP_EXECUTE WORLD_EXECUTE)
