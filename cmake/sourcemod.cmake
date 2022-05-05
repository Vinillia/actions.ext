cmake_minimum_required(VERSION 3.2)

# CMake configuration #
# SourceMod SDK

if (GAME STREQUAL "L4D2")
	set(SDK_PATH $ENV{HL2SDKL4D2})
else()
	set(SDK_PATH $ENV{HL2SDKL4D})
endif()

set(MM_PATH $ENV{SOURCEMM})
set(SM_PATH $ENV{SOURCEMOD})

add_compile_definitions(SE_LEFT4DEAD2=9 SE_LEFT4DEAD=8)

include_directories("${SDK_PATH}/public")
include_directories("${SDK_PATH}/public/engine")
include_directories("${SDK_PATH}/public/game/server")
include_directories("${SDK_PATH}/public/tier0")
include_directories("${SDK_PATH}/public/tier1")
include_directories("${SDK_PATH}/mathlib/")
include_directories("${SDK_PATH}/public/mathlib/")
include_directories("${SM_PATH}/core")
include_directories("${SM_PATH}/public")
include_directories("${SM_PATH}/public/amtl")
include_directories("${SM_PATH}/public/amtl/amtl")
include_directories("${SM_PATH}/public/asm")
include_directories("${SM_PATH}/public/CDetour")
include_directories("${SM_PATH}/public/jit")
include_directories("${SM_PATH}/public/jit/x86")
include_directories("${SM_PATH}/sourcepawn/include/")
include_directories("${MM_PATH}/core")
include_directories("${MM_PATH}/core/sourcehook")

function(add_extension ext_name)
    if(ARGC LESS 3)
        message(FATAL_ERROR "Missing arguments for add_extension")
    endif()
	
	add_library(${ext_name} SHARED ${ARGN} "${CMAKE_CURRENT_SOURCE_DIR}/source/public/extension.cpp")
	set_target_properties(${ext_name} PROPERTIES COMPILE_OPTIONS "-m32" LINK_FLAGS "-m32")

	target_sources(${ext_name} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/source/public/extension.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/actions/public/actions_propagate.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/actions/public/actions_manager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/actions/public/actions_processor.cpp
	${SM_PATH}/public/smsdk_ext.cpp)

	target_include_directories(${ext_name} PUBLIC 
	${SDK_PATH}/common
	${SDK_PATH}/public
	${SDK_PATH}/public/tier0
	${SDK_PATH}/public/tier1
	${SDK_PATH}/game/shared
	${SDK_PATH}/game/server)

    if (UNIX)
		target_compile_options(${ext_name} PUBLIC
		-Wall
		-Wno-class-memaccess
		-Wno-delete-non-virtual-dtor
		-Wno-invalid-offsetof
		-Wno-overloaded-virtual
		-Wno-reorder
		-Wno-sign-compare
		-Wno-unknown-pragmas
		-Wno-unused
		-Wregister
		-fno-strict-aliasing
		-fpermissive
		-Wregister
		-fvisibility-inlines-hidden
		-fvisibility=hidden
		-m32
		-march=pentium3
		-mmmx
		-msse
		-std=c++11)
		
		add_compile_definitions(
			_LINUX
			stricmp=strcmp
			_vsnprintf=vsnprintf)
		
		target_compile_definitions(${ext_name} PUBLIC SOURCEMOD_BUILD)
		target_link_libraries(${ext_name} PUBLIC 
		${SDK_PATH}/lib/linux/mathlib_i486.a
		${SDK_PATH}/lib/linux/tier1_i486.a
		${SDK_PATH}/lib/linux/tier2_i486.a
		${SDK_PATH}/lib/linux/tier3_i486.a)

		if (GAME STREQUAL "L4D2")
			target_link_libraries(${ext_name} PUBLIC 
			${SDK_PATH}/lib/linux/libtier0_srv.so
			${SDK_PATH}/lib/linux/libvstdlib_srv.so)
		else()
			target_link_libraries(${ext_name} PUBLIC 
			${SDK_PATH}/lib/linux/libtier0.so
			${SDK_PATH}/lib/linux/libvstdlib.so)
		endif()
        
		if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
            # Lesser than 3.13
            if(NOT COMMAND target_link_options)
                function(target_link_options)
                    target_link_libraries(${ARGV})
                endfunction()
            endif()

            # Use libstdc++ instead of libc++
			if ("${CMAKE_C_COMPILER_ID}" MATCHES "Clang")
				target_compile_options(${ext_name} PUBLIC -stdlib=libstdc++)
			endif()
			
            target_link_options(${ext_name} PUBLIC -static-libstdc++ -static-libgcc)
        endif()

		set_target_properties(${ext_name} PROPERTIES CXX_STANDARD 14)
	else()
		target_link_libraries(${ext_name} PUBLIC 
		${SDK_PATH}/lib/public/tier0.lib
		${SDK_PATH}/lib/public/tier1.lib
		${SDK_PATH}/lib/public/vstdlib.lib)

		add_compile_definitions(WIN32)
		set_target_properties(${ext_name} PROPERTIES CXX_STANDARD 17)
    endif()

	set_target_properties(${ext_name} PROPERTIES POSITION_INDEPENDENT_CODE True)

    
    set_target_properties(${ext_name} PROPERTIES CXX_STANDARD_REQUIRED ON)

    set_target_properties(${ext_name} PROPERTIES PREFIX "")
endfunction()
