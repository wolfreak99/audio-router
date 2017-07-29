#include <string>
#include "Version.h"

// TODO/wolfreak99: Create "third party used:" or whatever people use to list code borrowed from other open source projects, and include link to OpenRCT2's license
// Because right now I'm just trying to get this damned thing to work.

#ifdef AUDIOROUTER_BUILD_INFO_HEADER
#include AUDIOROUTER_BUILD_INFO_HEADER
#endif

const char gVersionInfoFull[] =
AUDIOROUTER_NAME ", v" AUDIOROUTER_VERSION
#ifdef AUDIOROUTER_BRANCH
"-" AUDIOROUTER_BRANCH
#endif
#ifdef AUDIOROUTER_COMMIT_SHA1_SHORT
" build " AUDIOROUTER_COMMIT_SHA1_SHORT
#endif
#ifdef AUDIOROUTER_BUILD_SERVER
" provided by " AUDIOROUTER_BUILD_SERVER
#endif
#ifndef NDEBUG
" (Debug)"
#endif
;
