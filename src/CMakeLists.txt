cmake_minimum_required (VERSION 3.4)
project(stingray_plugin_js)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "../cmake")
include(CMakeSettings)

# Set directories
set(STINGRAY_ENGINE_DIR C:/git/stingray/build/binaries/engine/win64/debug)
set(STINGRAY_PLUGIN_DIR ${STINGRAY_ENGINE_DIR}/plugins)
set(STINGRAY_SDK_DIR C:/git/stingray/runtime/sdk)

# Vendor
set(VENDOR_DIR ../vendor)

# Chakra
set(CHAKRA_CORE_DIR ${VENDOR_DIR}/chakracore)
#set(CHAKRA_CORE_LIBS ChakraCore.lib rpcrt4.lib msvcrtd.lib vcruntimed.lib ucrtd.lib)
set(CHAKRA_CORE_LIBS ChakraCore.lib)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

find_source_files(ALL_SOURCE_FILES)

include_directories (".")
include_directories(${STINGRAY_SDK_DIR})
include_directories(${CHAKRA_CORE_DIR})
link_directories(${CHAKRA_CORE_DIR})

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${STINGRAY_PLUGIN_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${STINGRAY_PLUGIN_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${STINGRAY_PLUGIN_DIR})

foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${STINGRAY_PLUGIN_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${STINGRAY_PLUGIN_DIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${STINGRAY_PLUGIN_DIR})
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

# Dynamic library
add_library(stingray_plugin_js SHARED ${ALL_SOURCE_FILES})
target_link_libraries(stingray_plugin_js ${CHAKRA_CORE_LIBS})

# Custom post build steps
ADD_CUSTOM_COMMAND (TARGET stingray_plugin_js POST_BUILD
# DEBUG
COMMAND ${CMAKE_COMMAND} -E copy ${CHAKRA_CORE_DIR}/ChakraCore.dll ${STINGRAY_ENGINE_DIR}
COMMAND ${CMAKE_COMMAND} -E copy ${CHAKRA_CORE_DIR}/ChakraCore.pdb ${STINGRAY_ENGINE_DIR}
# Output Message
COMMENT "Copying Chakra Core binaries" VERBATIM)