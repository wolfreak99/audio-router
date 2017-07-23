#include "patch.h"
#include <comdef.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <cassert>

#pragma comment(lib, "comsuppw.lib")

#define AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM 0x80000000
#define AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY 0x08000000

void tell_error(HRESULT hr)
{
    std::wstringstream sts;

    sts << L"HRESULT 0x" << std::uppercase << std::setfill(L'0') << std::setw(8) << std::hex << hr
    << L": " << _com_error(hr).ErrorMessage()
    << L"\n\nRouting functionality may not be available "\
       L"until target process restart.";
    MessageBoxW(NULL, sts.str().c_str(), L"Routing Error", MB_ICONERROR);
}

DWORD_PTR* swap_vtable(IAudioClient *this_)
{
    DWORD_PTR *old_vftptr = ((DWORD_PTR **)this_)[0];

    ((DWORD_PTR **)this_)[0] = ((DWORD_PTR ***)this_)[0][IAUDIOCLIENT_VFTPTR_IND_OLD];
    return old_vftptr;
}

HRESULT __stdcall release_patch(IAudioClient *this_)
{
    iaudioclient_duplicate *dup = get_duplicate(this_);
    IAudioClient *proxy = dup->proxy;
    GUID *arg = ((GUID ***)this_)[0][IAUDIOCLIENT_VFTPTR_IND_SESSION_GUID];
    WORD *arg2 = ((WORD ***)this_)[0][IAUDIOCLIENT_VFTPTR_IND_18];
    DWORD_PTR *old_vftptr = swap_vtable(this_);
    ULONG result = this_->Release();

    if (result == 0) {
        dup->proxy = NULL;
        delete[] old_vftptr;
        delete dup;
        delete arg;
        delete arg2;
    }
    else {
        ((DWORD_PTR **)this_)[0] = old_vftptr;
    }

    return result;
} // release_patch

iaudioclient_duplicate* get_duplicate(IAudioClient *this_)
{
    return ((iaudioclient_duplicate ***)this_)[0][IAUDIOCLIENT_VFTPTR_IND_DUP];
}

HRESULT __stdcall initialize_patch(IAudioClient *this_, AUDCLNT_SHAREMODE ShareMode, DWORD StreamFlags,
    REFERENCE_TIME hnsBufferDuration, REFERENCE_TIME hnsPeriodicity, const WAVEFORMATEX *pFormat,
    LPCGUID AudioSessionGuid)
{
    // synchronize initializing so it doesn't happen while streams are being flushed
    HANDLE audio_router_mutex = OpenMutexW(SYNCHRONIZE, FALSE, L"Local\\audio-router-mutex");

    assert(audio_router_mutex != NULL);

    if (audio_router_mutex) {
        DWORD res = WaitForSingleObject(audio_router_mutex, INFINITE);
        assert(res == WAIT_OBJECT_0);
    }

    IAudioClient *proxy = get_duplicate(this_)->proxy;
    LPCGUID guid = ((GUID ***)this_)[0][IAUDIOCLIENT_VFTPTR_IND_SESSION_GUID];
    DWORD_PTR *old_vftptr = swap_vtable(this_);
    HRESULT hr = proxy->Initialize(
        ShareMode,
        StreamFlags |
        AUDCLNT_SESSIONFLAGS_EXPIREWHENUNOWNED |
        AUDCLNT_SESSIONFLAGS_DISPLAY_HIDEWHENEXPIRED,
        hnsBufferDuration,
        hnsPeriodicity,
        pFormat,
        guid);
    ((DWORD_PTR **)this_)[0] = old_vftptr;

    if (hr != S_OK) {
        tell_error(hr);
    }
    else {
        *((WORD ***)this_)[0][IAUDIOCLIENT_VFTPTR_IND_18] = pFormat->nBlockAlign;
    }

    if (hr == S_OK) {
        for (iaudioclient_duplicate *next = get_duplicate(this_)->next;
            next != NULL; next = next->next)
        {
            HRESULT hr2 = next->proxy->Initialize(
                ShareMode,
                StreamFlags |
                AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM |
                AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY |
                AUDCLNT_SESSIONFLAGS_EXPIREWHENUNOWNED |
                AUDCLNT_SESSIONFLAGS_DISPLAY_HIDEWHENEXPIRED,
                hnsBufferDuration,
                hnsPeriodicity,
                pFormat,
                guid);

            if (hr2 != S_OK) {
                tell_error(hr2);
            }
        }
    }

    ReleaseMutex(audio_router_mutex);
    CloseHandle(audio_router_mutex);
    return hr;
} // initialize_patch

HRESULT __stdcall start_patch(IAudioClient *this_)
{
    IAudioClient *proxy = get_duplicate(this_)->proxy;
    DWORD_PTR *old_vftptr = swap_vtable(this_);
    HRESULT hr = proxy->Start(), hr2;

    ((DWORD_PTR **)this_)[0] = old_vftptr;

    if (hr == S_OK) {
        for (iaudioclient_duplicate *next = get_duplicate(this_)->next;
            next != NULL;
            next = next->next) {
            hr2 = next->proxy->Start();
        }
    }

    return hr;
}

HRESULT __stdcall stop_patch(IAudioClient *this_)
{
    IAudioClient *proxy = get_duplicate(this_)->proxy;
    DWORD_PTR *old_vftptr = swap_vtable(this_);
    HRESULT hr = proxy->Stop();

    ((DWORD_PTR **)this_)[0] = old_vftptr;

    if (hr == S_OK) {
        for (iaudioclient_duplicate *next = get_duplicate(this_)->next;
            next != NULL;
            next = next->next) {
            next->proxy->Stop();
        }
    }

    return hr;
}

HRESULT __stdcall getservice_patch(IAudioClient *this_, REFIID riid, void **ppv)
{
    IAudioClient *proxy = get_duplicate(this_)->proxy;
    DWORD_PTR *old_vftptr = swap_vtable(this_);
    HRESULT hr = proxy->GetService(riid, ppv);

    ((DWORD_PTR **)this_)[0] = old_vftptr;

    // renderclient list has 1:1 mapping to audioclient
    if (hr == S_OK) {
        if (riid == __uuidof(IAudioRenderClient)) {
            IAudioRenderClient *host = *((IAudioRenderClient **)ppv);
            patch_iaudiorenderclient(host, *((WORD ***)this_)[0][18]);

            for (iaudioclient_duplicate *next = get_duplicate(this_)->next;
                next != NULL; next = next->next)
            {
                IAudioRenderClient *renderclient = NULL;
                next->proxy->GetService(riid, (void **)&renderclient);
                get_duplicate(host)->add(renderclient);
            }
        }
        else if (riid == __uuidof(IAudioStreamVolume)) {
            IAudioStreamVolume *host = *((IAudioStreamVolume **)ppv);
            patch_iaudiostreamvolume(host);

            for (iaudioclient_duplicate *next = get_duplicate(this_)->next;
                next != NULL; next = next->next)
            {
                IAudioStreamVolume *streamvolume = NULL;
                next->proxy->GetService(riid, (void **)&streamvolume);

                if (streamvolume != NULL) {
                    get_duplicate(host)->add(streamvolume);
                }
            }
        }
    }

    return hr;
} // getservice_patch

HRESULT __stdcall getbuffersize_patch(IAudioClient *this_, UINT32 *pNumBufferFrames)
{
    IAudioClient *proxy = get_duplicate(this_)->proxy;
    DWORD_PTR *old_vftptr = swap_vtable(this_);
    HRESULT hr = proxy->GetBufferSize(pNumBufferFrames);

    ((DWORD_PTR **)this_)[0] = old_vftptr;

    for (iaudioclient_duplicate *next = get_duplicate(this_)->next; next != NULL; next =
            next->next) {
        UINT32 buf;
        HRESULT hr = next->proxy->GetBufferSize(&buf);
        assert(buf >= *pNumBufferFrames);
    }

    return hr;
}

HRESULT __stdcall getcurrentpadding_patch(IAudioClient *this_, UINT32 *pNumPaddingFrames)
{
    IAudioClient *proxy = get_duplicate(this_)->proxy;
    DWORD_PTR *old_vftptr = swap_vtable(this_);
    HRESULT hr = proxy->GetCurrentPadding(pNumPaddingFrames);

    ((DWORD_PTR **)this_)[0] = old_vftptr;

    for (iaudioclient_duplicate *next = get_duplicate(this_)->next; next != NULL; next =
            next->next) {
        UINT32 pad;
        next->proxy->GetCurrentPadding(&pad);

        // assert(pad == *pNumPaddingFrames);
    }

    return hr;
}

HRESULT __stdcall seteventhandle_patch(IAudioClient *this_, HANDLE eventHandle)
{
    IAudioClient *proxy = get_duplicate(this_)->proxy;

    for (iaudioclient_duplicate *next = get_duplicate(this_)->next; next != NULL; next =
            next->next) {
        next->proxy->SetEventHandle(eventHandle);
    }

    DWORD_PTR *old_vftptr = swap_vtable(this_);
    HRESULT hr = proxy->SetEventHandle(eventHandle);
    ((DWORD_PTR **)this_)[0] = old_vftptr;

    return hr;
}

HRESULT __stdcall getstreamlatency_patch(IAudioClient *this_, REFERENCE_TIME *phnsLatency)
{
    IAudioClient *proxy = get_duplicate(this_)->proxy;
    DWORD_PTR *old_vftptr = swap_vtable(this_);
    HRESULT hr = proxy->GetStreamLatency(phnsLatency);

    ((DWORD_PTR **)this_)[0] = old_vftptr;

    for (iaudioclient_duplicate *next = get_duplicate(this_)->next; next != NULL; next =
            next->next) {
        REFERENCE_TIME t;
        next->proxy->GetStreamLatency(&t);
        assert(*phnsLatency == t);
    }

    return hr;
}

HRESULT __stdcall getmixformat_patch(IAudioClient *this_, WAVEFORMATEX **ppDeviceFormat)
{
    // STATIC FUNCTION
    IAudioClient *proxy = get_duplicate(this_)->proxy;
    DWORD_PTR *old_vftptr = swap_vtable(this_);
    HRESULT hr = proxy->GetMixFormat(ppDeviceFormat);

    ((DWORD_PTR **)this_)[0] = old_vftptr;

    return hr;
}

HRESULT __stdcall getdeviceperiod_patch(IAudioClient *this_,
    REFERENCE_TIME *phnsDefaultDevicePeriod,
    REFERENCE_TIME *phnsMinimumDevicePeriod)
{
    // STATIC FUNCTION
    IAudioClient *proxy = get_duplicate(this_)->proxy;
    DWORD_PTR *old_vftptr = swap_vtable(this_);
    HRESULT hr = proxy->GetDevicePeriod(phnsDefaultDevicePeriod, phnsMinimumDevicePeriod);

    ((DWORD_PTR **)this_)[0] = old_vftptr;

    for (iaudioclient_duplicate *next = get_duplicate(this_)->next; next != NULL; next =
            next->next) {
        REFERENCE_TIME def, min;
        next->proxy->GetDevicePeriod(&def, &min);
        assert(def == *phnsDefaultDevicePeriod && min == *phnsMinimumDevicePeriod);
    }

    return hr;
}

HRESULT __stdcall reset_patch(IAudioClient *this_)
{
    IAudioClient *proxy = get_duplicate(this_)->proxy;
    DWORD_PTR *old_vftptr = swap_vtable(this_);
    HRESULT hr = proxy->Reset();

    ((DWORD_PTR **)this_)[0] = old_vftptr;

    for (iaudioclient_duplicate *next = get_duplicate(this_)->next; next != NULL; next = next->next) {
        next->proxy->Reset();
    }

    return hr;
}

HRESULT __stdcall isformatsupported_patch(IAudioClient *this_, AUDCLNT_SHAREMODE ShareMode,
    const WAVEFORMATEX *pFormat, WAVEFORMATEX **ppClosestMatch)
{
    // STATIC FUNCTION
    IAudioClient *proxy = get_duplicate(this_)->proxy;
    DWORD_PTR *old_vftptr = swap_vtable(this_);
    HRESULT hr = proxy->IsFormatSupported(ShareMode, pFormat, ppClosestMatch);

    ((DWORD_PTR **)this_)[0] = old_vftptr;

    return hr;
}

// HRESULT __stdcall queryinterface_patch(IAudioClient* this_, REFIID riid, void** ppvObject)
// {
//    IAudioClient* proxy = get_duplicate(this_)->proxy;
//    HRESULT hr = proxy->QueryInterface(riid, ppvObject);
//
//    return hr;
// }



void patch_iaudioclient(IAudioClient *this_, LPGUID session_guid)
{
    // create new virtual table and save old and populate new with default
    DWORD_PTR *old_vftptr = ((DWORD_PTR **)this_)[0]; // save old virtual table

    // create new virtual table (slot 15 for old table ptr and 16 for duplicate)
    ((DWORD_PTR **)this_)[0] = new DWORD_PTR[IAUDIOCLIENT_VFTPTR_COUNT];
    memcpy(((DWORD_PTR **)this_)[0], old_vftptr, 15 * sizeof(DWORD_PTR));

    // created duplicate object
    iaudioclient_duplicate *dup = new iaudioclient_duplicate(this_);

    // patch routines
    DWORD_PTR *vftptr = ((DWORD_PTR **)this_)[0];
    // vftptr[0] = (DWORD_PTR)queryinterface_patch; // NEW
    // NOTE/wolfreak99: some reason index 1 is missing.
    vftptr[IAUDIOCLIENT_VFTPTR_IND_RELEASE]             = (DWORD_PTR)release_patch;
    vftptr[IAUDIOCLIENT_VFTPTR_IND_INITIALIZE]          = (DWORD_PTR)initialize_patch;
    vftptr[IAUDIOCLIENT_VFTPTR_IND_GET_BUFFER_SIZE]     = (DWORD_PTR)getbuffersize_patch;
    vftptr[IAUDIOCLIENT_VFTPTR_IND_GET_STREAM_LATENCY]  = (DWORD_PTR)getstreamlatency_patch;
    vftptr[IAUDIOCLIENT_VFTPTR_IND_GET_CURRENT_PADDING] = (DWORD_PTR)getcurrentpadding_patch;
    vftptr[IAUDIOCLIENT_VFTPTR_IND_IS_FORMAT_SUPPORTED] = (DWORD_PTR)isformatsupported_patch; // static
    vftptr[IAUDIOCLIENT_VFTPTR_IND_GET_MIX_FORMAT]      = (DWORD_PTR)getmixformat_patch; // static
    vftptr[IAUDIOCLIENT_VFTPTR_IND_GET_DEVICE_PERIOD]   = (DWORD_PTR)getdeviceperiod_patch; // static
    vftptr[IAUDIOCLIENT_VFTPTR_IND_START]               = (DWORD_PTR)start_patch;
    vftptr[IAUDIOCLIENT_VFTPTR_IND_STOP]                = (DWORD_PTR)stop_patch;
    vftptr[IAUDIOCLIENT_VFTPTR_IND_RESET]               = (DWORD_PTR)reset_patch;
    vftptr[IAUDIOCLIENT_VFTPTR_IND_SET_EVENT_HANDLE]    = (DWORD_PTR)seteventhandle_patch;
    vftptr[IAUDIOCLIENT_VFTPTR_IND_GET_SERVICE]         = (DWORD_PTR)getservice_patch;
    vftptr[IAUDIOCLIENT_VFTPTR_IND_OLD]                 = (DWORD_PTR)old_vftptr;
    vftptr[IAUDIOCLIENT_VFTPTR_IND_DUP]                 = (DWORD_PTR)dup;
    vftptr[IAUDIOCLIENT_VFTPTR_IND_SESSION_GUID]        = (DWORD_PTR)session_guid;
    vftptr[IAUDIOCLIENT_VFTPTR_IND_18]                  = (DWORD_PTR) new WORD; // block align
} // patch_iaudioclient
