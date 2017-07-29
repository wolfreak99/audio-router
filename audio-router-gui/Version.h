#pragma once

#include "..\audio-router\common.h"

#define AUDIOROUTER_NAME               "Audio-Router"
#define AUDIOROUTER_VERSION            "0.0.1"

#if defined(__amd64__) || defined(_M_AMD64)
#define AUDIOROUTER_ARCHITECTURE       "x86-64"
#elif defined(__i386__) || defined(_M_IX86)
#define AUDIOROUTER_ARCHITECTURE       "x86"
#elif defined(PLATFORM_64BIT)
#define AUDIOROUTER_ARCHITECTURE       "64-bit"
#elif defined(PLATFORM_32BIT)
#define AUDIOROUTER_ARCHITECTURE       "32-bit"
#endif

#ifndef AUDIOROUTER_ARCHITECTURE
#error "AUDIOROUTER_ARCHITECTURE is undefined. Please add identification."
#endif

// Platform
#ifdef _WIN32
#define AUDIOROUTER_PLATFORM       "Windows"
#endif
#ifndef AUDIOROUTER_PLATFORM
#error Unknown platform!
#endif

#define AUDIOROUTER_TIMESTAMP          __DATE__ " " __TIME__

#ifdef __cplusplus
extern "C"
{
#endif
    extern const char gVersionInfoFull[];
#ifdef __cplusplus
}
#endif
