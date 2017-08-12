#pragma once

#include <stdlib.h>


#ifndef min
# define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
# define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#define sgn(x)               ((x > 0) ? 1 : ((x < 0) ? -1 : 0))
#define clamp(val,lo,hi)     (min(hi, max(lo, val)))

#if defined(_MSC_VER)
#  define countof(arr)       _countof(arr)
#else
#  define countof(arr)       (sizeof(arr) / sizeof((arr)[0]))
#endif

#if defined(_WIN64)
#  define PLATFORM_64BIT
#else
#  define PLATFORM_32BIT
#endif

#ifndef PLATFORM_64BIT
#  define AUDIO_ROUTER_DLL_NAME L"audio-router\\audio-router.dll"
#  define BOOTSTRAPPER_DLL_NAME L"bootstrapper\\bootstrapper.dll"
#  define DO_EXE_NAME           L"do\\do.exe"
#else
#  define AUDIO_ROUTER_DLL_NAME L"audio-router\\audio-router64.dll"
#  define BOOTSTRAPPER_DLL_NAME L"bootstrapper\\bootstrapper64.dll"
#  define DO_EXE_NAME           L"do\\do64.exe"
#endif

#define AUDIO_ROUTER_EXE_NAME L"Audio Router.exe"
#define LOCAL_PARAMS_FILE       L"saved_routings.dat"

// LPT: Wrap macros with many lines in "do { } while (0)" to count as one line in bracketless IFs

#define SafeFree(x)          do { free (x);     (x) = NULL;    } while (0)

#define SafeDelete(x)        do { delete (x);   (x) = nullptr; } while (0)
#define SafeDeleteArray(x)   do { delete[] (x); (x) = nullptr; } while (0)

// #ifndef ENABLE_BOOTSTRAP
// #define ENABLE_BOOTSTRAP
// #endif
