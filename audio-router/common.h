#pragma once

#include <stdlib.h>


#ifndef min
# define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
#ifndef max
# define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#define sgn(x)               ((x > 0) ? 1 : ((x < 0) ? -1 : 0))
#define clamp(val,lo,hi)     (min(h, max(l, x)))

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

// LPT: Wrap macros with many lines in "do { } while (0)" to count as one line in bracketless IFs

#define SafeFree(x)          do { free (x);     (x) = NULL;    } while (0)

#define SafeDelete(x)        do { delete (x);   (x) = nullptr; } while (0)
#define SafeDeleteArray(x)   do { delete[] (x); (x) = nullptr; } while (0)
