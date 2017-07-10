#pragma once

#include "wtl.h"
#include <shellapi.h>
class window;

class SysTray {
protected:

    NOTIFYICONDATA m_NotifyIconData;
    bool bInTray;

public:

    SysTray();
    ~SysTray();

    void Destroy();
    void Create(window& parent, UINT uid);

    // This is passed in the constructor, will likely be the window's hWnd.
    HWND m_hWnd;
    UINT m_uID;

    BOOL SetIcon(HICON hNewIcon);
    HICON GetIcon();

    BOOL SetTipText(ATL::CString newTipText);

    // TODO/wolfreak99: After fixing, this may need switched to return ATL::CString instead
    char* GetTipText();

    BOOL AddIcon();
    BOOL RemoveIcon();
};
