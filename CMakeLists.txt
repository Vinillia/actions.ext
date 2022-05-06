cmake_minimum_required(VERSION 3.8)

project(actions_ext)

# CMake configuration #

option(DEBUG_OUTPUT "Used to disable/enable logs" OFF)

set(GAME "" CACHE STRING "Description")

if (GAME STREQUAL "")
    set(GAME "L4D2")
    message(NOTICE "GAME ignored use default build: ${GAME}")
endif()

if (GAME STREQUAL "L4D2")
    set(OUTPUT_NAME "actions.ext.2.l4d2")
    add_compile_definitions(SOURCE_ENGINE=9)
elseif (GAME STREQUAL "L4D")
    set(OUTPUT_NAME "actions.ext.2.l4d")
    add_compile_definitions(SOURCE_ENGINE=8)
else()
    message(FATAL_ERROR "Invalid build scope value: ${GAME}")
endif()

include("cmake/sourcemod.cmake")

if(MSVC AND NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "Arch must be Win32")
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8 /W3")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32")
endif()

include_directories("source")
include_directories("source/other")
include_directories("source/public")
include_directories("source/sdk")
include_directories("source/actions")
include_directories("source/actions/natives")
include_directories("source/actions/public")

file(GLOB_RECURSE CPP_FILES "source/*.cpp")
file(GLOB_RECURSE H_FILES "source/*.h")
list(APPEND HPP_FILES ${H_FILES})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CPP_FILES} ${HPP_FILES})

# Add a executable file #
add_extension(${OUTPUT_NAME} ${CPP_FILES} ${HPP_FILES})