cmake_minimum_required (VERSION 3.4)
project(stingray_plugin_js)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "../cmake")
include(CMakeSettings)

# Stingray SDK
set(STINGRAY_SDK_DIR C:/git/stingray/runtime/sdk)

#Vendor
set(VENDOR_DIR ../vendor)

# Chakra
set(CHAKRA_CORE_DIR ${VENDOR_DIR}/chakracore)
set(CHAKRA_CORE_LIBS ChakraCore.lib)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

find_source_files(ALL_SOURCE_FILES)

include_directories (".")
include_directories(${CHAKRA_CORE_DIR})
include_directories(${STINGRAY_SDK_DIR})
link_directories(${CHAKRA_CORE_DIR})

add_library(stingray_plugin_js SHARED ${ALL_SOURCE_FILES})
target_link_libraries(stingray_plugin_js ${CHAKRA_CORE_LIBS})

ADD_CUSTOM_COMMAND (TARGET stingray_plugin_js POST_BUILD
# DEBUG
COMMAND ${CMAKE_COMMAND} -E copy ${CHAKRA_CORE_DIR}/ChakraCore.dll ${CMAKE_BINARY_DIR}/Debug
# Output Message
COMMENT "Copying Chakra Core binaries" VERBATIM)