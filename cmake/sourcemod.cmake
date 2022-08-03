cmake_minimum_required(VERSION 3.2)

set(SDK_PATH $ENV{SDK})
set(MM_PATH $ENV{METAMOD})
set(SM_PATH $ENV{SOURCEMOD})

if ("${SDK_PATH}" STREQUAL "")
	message("Failed to get SDK, use L4D1SDK instead...")
	set(SDK_PATH $ENV{L4D1SDK})
endif()

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
include_directories("${SM_PATH}/public/jit")
include_directories("${SM_PATH}/public/jit/x86")
include_directories("${SM_PATH}/sourcepawn/include/")
include_directories("${MM_PATH}/core")
include_directories("${MM_PATH}/core/sourcehook")

function(add_extension ext_name engine)
    if(ARGC LESS 3)
        message(FATAL_ERROR "Missing arguments for add_extension")
    endif()
	
	add_library(${ext_name} SHARED ${ARGN} "${CMAKE_CURRENT_SOURCE_DIR}/actions-ext/source/public/extension.cpp")
	target_compile_definitions(${ext_name} PUBLIC SOURCE_ENGINE=${engine})

	target_sources(${ext_name} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/actions-ext/source/public/extension.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/actions-ext/source/public/offset_manager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/actions-ext/source/actions/public/actions_propagate.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/actions-ext/source/actions/public/actions_manager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/actions-ext/source/actions/public/actions_helpers.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/actions/sdk/public/actions_cquery.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/actions/sdk/public/actions_processor.cpp
	${SM_PATH}/public/smsdk_ext.cpp)

	target_include_directories(${ext_name} PUBLIC 
	${SDK_PATH}/common
	${SDK_PATH}/public
	${SDK_PATH}/public/tier0
	${SDK_PATH}/public/tier1
	${SDK_PATH}/game/shared
	${SDK_PATH}/game/server)

    if (UNIX)
		set_target_properties(${ext_name} PROPERTIES COMPILE_OPTIONS "-m32" LINK_FLAGS "-m32")

		target_compile_options(${ext_name} PUBLIC
		-Wall
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
		-msse)

		add_compile_definitions(
			_LINUX
			stricmp=strcmp
			_vsnprintf=vsnprintf
			_GLIBCXX_USE_CXX11_ABI=0)
		
		target_link_libraries(${ext_name} PUBLIC 
		${SDK_PATH}/lib/linux/mathlib_i486.a
		${SDK_PATH}/lib/linux/tier1_i486.a
		${SDK_PATH}/lib/linux/tier2_i486.a
		${SDK_PATH}/lib/linux/tier3_i486.a
		${SDK_PATH}/lib/linux/libtier0_srv.so
		${SDK_PATH}/lib/linux/libvstdlib_srv.so)

		#target_compile_options(${ext_name} PUBLIC -static-libstdc++ -stdlib=libstdc++)
        target_link_options(${ext_name} PUBLIC -static-libstdc++ -static-libgcc)
	
	else()
		add_compile_definitions(
			_ITERATOR_DEBUG_LEVEL=0
			_CRT_SECURE_NO_DEPRECATE
			_CRT_SECURE_NO_WARNINGS
			_CRT_NONSTDC_NO_DEPRECATE
			WIN32)

		target_compile_options(${ext_name} PUBLIC
			"/W3"
			"/EHsc"
			"/TP"
			"/MT")

		target_link_libraries(${ext_name} PUBLIC 
		${SDK_PATH}/lib/public/tier0.lib
		${SDK_PATH}/lib/public/tier1.lib
		${SDK_PATH}/lib/public/vstdlib.lib)
    endif()

	set_target_properties(${ext_name} PROPERTIES POSITION_INDEPENDENT_CODE True)

	set_target_properties(${ext_name} PROPERTIES CXX_STANDARD 17)
    set_target_properties(${ext_name} PROPERTIES CXX_STANDARD_REQUIRED ON)

    set_target_properties(${ext_name} PROPERTIES PREFIX "")
endfunction()
