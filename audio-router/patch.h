#pragma once

#include <Audioclient.h>

// TODO/audiorouterdev: iaudioclockadjustment implementation for adjusting sample rate

enum IAUDIOCLIENT_VFTPTR
{
    IAUDIOCLIENT_VFTPTR_IND_0,
    IAUDIOCLIENT_VFTPTR_IND_1,
    IAUDIOCLIENT_VFTPTR_IND_RELEASE,
    IAUDIOCLIENT_VFTPTR_IND_INITIALIZE,
    IAUDIOCLIENT_VFTPTR_IND_GET_BUFFER_SIZE,
    IAUDIOCLIENT_VFTPTR_IND_GET_STREAM_LATENCY,
    IAUDIOCLIENT_VFTPTR_IND_GET_CURRENT_PADDING,
    IAUDIOCLIENT_VFTPTR_IND_IS_FORMAT_SUPPORTED,
    IAUDIOCLIENT_VFTPTR_IND_GET_MIX_FORMAT,
    IAUDIOCLIENT_VFTPTR_IND_GET_DEVICE_PERIOD,
    IAUDIOCLIENT_VFTPTR_IND_START,
    IAUDIOCLIENT_VFTPTR_IND_STOP,
    IAUDIOCLIENT_VFTPTR_IND_RESET,
    IAUDIOCLIENT_VFTPTR_IND_SET_EVENT_HANDLE,
    IAUDIOCLIENT_VFTPTR_IND_GET_SERVICE,
    IAUDIOCLIENT_VFTPTR_IND_OLD,
    IAUDIOCLIENT_VFTPTR_IND_DUP,
    IAUDIOCLIENT_VFTPTR_IND_SESSION_GUID,
    IAUDIOCLIENT_VFTPTR_IND_18,
    IAUDIOCLIENT_VFTPTR_COUNT
};

enum IAUDIORENDERCLIENT_VFTPTR
{
    IAUDIORENDERCLIENT_VFTPTR_IND_0,
    IAUDIORENDERCLIENT_VFTPTR_IND_1,
    IAUDIORENDERCLIENT_VFTPTR_IND_RELEASE,
    IAUDIORENDERCLIENT_VFTPTR_IND_GET_BUFFER,
    IAUDIORENDERCLIENT_VFTPTR_IND_RELEASE_BUFFER,
    IAUDIORENDERCLIENT_VFTPTR_IND_OLD,
    IAUDIORENDERCLIENT_VFTPTR_IND_DUP,
    IAUDIORENDERCLIENT_VFTPTR_IND_BUFFER_PTR,
    IAUDIORENDERCLIENT_VFTPTR_IND_NUM_FRAMES_REQUESTED,
    IAUDIORENDERCLIENT_VFTPTR_IND_FRAME_SIZE,
    IAUDIORENDERCLIENT_VFTPTR_COUNT
};

enum IAUDIOSTREAMVOLUME_VFTPTR
{
    IAUDIOSTREAMVOLUME_VFTPTR_IND_0,
    IAUDIOSTREAMVOLUME_VFTPTR_IND_1,
    IAUDIOSTREAMVOLUME_VFTPTR_IND_RELEASE,
    IAUDIOSTREAMVOLUME_VFTPTR_IND_GET_CHANNEL_COUNT,
    IAUDIOSTREAMVOLUME_VFTPTR_IND_SET_CHANNEL_VOLUME,
    IAUDIOSTREAMVOLUME_VFTPTR_IND_GET_CHANNEL_VOLUME,
    IAUDIOSTREAMVOLUME_VFTPTR_IND_SET_ALL_VOLUMES,
    IAUDIOSTREAMVOLUME_VFTPTR_IND_GET_ALL_VOLUMES,
    IAUDIOSTREAMVOLUME_VFTPTR_IND_OLD,
    IAUDIOSTREAMVOLUME_VFTPTR_IND_DUP,
    IAUDIOSTREAMVOLUME_VFTPTR_COUNT
};

template <typename T> struct duplicate
{
    T *proxy;
    duplicate *next;

    explicit duplicate(T *proxy) : proxy(proxy), next(NULL) {}

    ~duplicate()
    {
        if (this->proxy) {
            this->proxy->Release();
        }

        delete this->next;
    }

    void add(T *proxy)
    {
        duplicate **item = &this->next;

        while (*item != NULL) {
            item = &(*item)->next;
        }
        *item = new duplicate(proxy);
    }
};

typedef duplicate<IAudioClient> iaudioclient_duplicate;
typedef duplicate<IAudioRenderClient> iaudiorenderclient_duplicate;
typedef duplicate<IAudioStreamVolume> iaudiostreamvolume_duplicate;

void patch_iaudioclient(IAudioClient *host, LPGUID session_guid);
iaudioclient_duplicate* get_duplicate(IAudioClient *host);

void patch_iaudiorenderclient(IAudioRenderClient *host, WORD block_align);
iaudiorenderclient_duplicate* get_duplicate(IAudioRenderClient *host);

void patch_iaudiostreamvolume(IAudioStreamVolume *host);
iaudiostreamvolume_duplicate* get_duplicate(IAudioStreamVolume *host);
