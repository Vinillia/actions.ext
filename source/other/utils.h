#pragma once

#include <commonmacros.h>
#include <memalloc.h>

#define CONFIG_FILE_NAME "actions.games"

#ifndef __linux__
	void* operator new(size_t size);
	void  operator delete(void* const block);
#endif

// #define NOLOGS
// #define NO_RUNTIME_VALIDATION

#ifndef __linux__
	MEM_INTERFACE IMemAlloc* g_pMemAlloc;
#endif

#ifdef __has_cpp_attribute
#   if __has_cpp_attribute(nodiscard)
#       define NODISCARD [[nodiscard]]
#   else
#       define NODISCARD
#   endif
#	if __has_cpp_attribute(maybe_unused)
#       define MAYBE_UNSED [[maybe_unused]]
#   else
#       define MAYBE_UNSED
#   endif
#else
#   define NODISCARD
#   define MAYBE_UNSED
#endif

#ifndef NOLOGS
	#define LOG(fmt, ...) g_pSM->LogMessage(myself, fmt, ##__VA_ARGS__)
	#define LOGERROR(fmt, ...) g_pSM->LogError(myself, fmt, ##__VA_ARGS__)
	#ifdef _DEBUG
		#define LOGDEBUG(fmt, ...) g_pSM->LogMessage(myself, fmt, ##__VA_ARGS__)
	#else
		#define LOGDEBUG(fmt, ...) ((void)0)
	#endif
#else
	#define LOG(fmt, ...) ((void)0)
	#define LOGERROR(fmt, ...) ((void)0)
#endif