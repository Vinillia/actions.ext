#pragma once

#include <commonmacros.h>
#include <memalloc.h>

#ifndef __linux__
	void* operator new(size_t size);
	void  operator delete(void* const block);
#endif

// #define NOLOGS
// #define NO_RUNTIME_VALIDATE_CHECK

#define STRIP(x) x
#define STRIPEX(x) STRIP x

#ifndef __linux__
	MEM_INTERFACE IMemAlloc* g_pMemAlloc;
#endif

#ifdef __has_cpp_attribute
#   if __has_cpp_attribute(nodiscard)
#       define NODISCARD [[nodiscard]]
#   else
#       define NODISCARD
#   endif
#else
#   define NODISCARD
#endif

#ifndef NOLOGS
	#define LOG(fmt, ...) g_pSM->LogMessage(myself, fmt, ##__VA_ARGS__)
	#define LOGERROR(fmt, ...) g_pSM->LogError(myself, fmt, ##__VA_ARGS__)
	#ifdef _DEBUG
		#define DEBUG(fmt, ...) g_pSM->LogMessage(myself, fmt, ##__VA_ARGS__)
	#else
		#define DEBUG(fmt, ...) ((void)0)
	#endif
#else
	#define LOG(fmt, ...) ((void)0)
	#define LOGERROR(fmt, ...) ((void)0)
#endif

#if SOURCE_ENGINE == SE_LEFT4DEAD2
	#ifndef __linux__
		#define INTENTION_RESET_OFFSET 40
	#else
		#define INTENTION_RESET_OFFSET 41
	#endif
#else
	#ifndef __linux__
		#define INTENTION_RESET_OFFSET 35
	#else
		#define INTENTION_RESET_OFFSET 36
	#endif
#endif