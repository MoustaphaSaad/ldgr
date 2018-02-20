#pragma once

#include <cpprelude/defines.h>

#ifdef OS_WINDOWS
#ifdef LDGR_DLL
    #define API_LDGR __declspec(dllexport)
#else
	#define API_LDGR __declspec(dllimport)
#endif
#endif

#ifdef OS_LINUX
    #define API_LDGR
#endif