cmake_minimum_required(VERSION 3.8)

project(actions_ext)

if(MSVC AND NOT CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "Arch must be Win32")
endif()

include("cmake/sourcemod.cmake")

include_directories("source")
include_directories("source/other")
include_directories("source/public")
include_directories("source/sdk")
include_directories("source/actions")
include_directories("source/actions/natives")
include_directories("source/actions/public")

add_compile_definitions(SE_LEFT4DEAD2=9 SE_LEFT4DEAD=8 SOURCEMOD_BUILD)

if (DEBUG_OUTPUT)
	add_compile_definitions(_DEBUG)
endif()

file(GLOB_RECURSE CPP_FILES "source/*.cpp")
file(GLOB_RECURSE H_FILES "source/*.h")
list(APPEND HPP_FILES ${H_FILES})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CPP_FILES} ${HPP_FILES})

add_extension(actions.ext.2.l4d2 9 ${CPP_FILES} ${HPP_FILES})
add_extension(actions.ext.2.l4d 8 ${CPP_FILES} ${HPP_FILES})