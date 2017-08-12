// DialogMessageHook.h: interface for the CDialogMessageHook class.
//
// ////////////////////////////////////////////////////////////////////

#pragma once

#include "wtl.h"
#include <set>

typedef std::set<HWND> THWNDCollection;

// CDialogMessageHook makes it easy to properly
// process tab and accelerator keys in
// ATL modeless dialogs
class CDialogMessageHook {
public:

    // set a dialog message hook for the specified modeless dialog
    static HRESULT InstallHook(HWND hWnd);
    static HRESULT UninstallHook(HWND hWnd);

private:

    // the hook function
    static LRESULT CALLBACK GetMessageProc(int nCode, WPARAM wParam, LPARAM lParam);

    // the hook handle
    static HHOOK m_hHook;

    // the set of HWNDs we are hooking
    static THWNDCollection m_aWindows;
};
