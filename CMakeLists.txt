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
include_directories("source/actions/sdk")

add_compile_definitions(SE_EPISODEONE=1 SE_DARKMESSIAH=2 SE_ORANGEBOX=3 SE_BLOODYGOODTIME=4 SE_EYE=5 SE_ORANGEBOXVALVE=6 SE_LEFT4DEAD=7 SE_LEFT4DEAD2=8 SE_ALIENSWARM=9 SE_PORTAL2=10 SE_CSGO=11)

file(GLOB_RECURSE CPP_FILES "source/*.cpp")
file(GLOB_RECURSE H_FILES "source/*.h")
list(APPEND HPP_FILES ${H_FILES})

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${CPP_FILES} ${HPP_FILES})

add_extension(actions.ext.2.l4d2 8 ${CPP_FILES} ${HPP_FILES})