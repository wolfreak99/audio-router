#include "SysTray.h"
#include "window.h"
// TODO/wolfreak99: Create a common.h for stuff such as this
#include <assert.h>

SysTray::SysTray()
{
    bInTray = false;
}

SysTray::~SysTray()
{
}

void SysTray::Create(window& parent, UINT uid)
{
    m_NotifyIconData.cbSize = NOTIFYICONDATAA_V1_SIZE;
    m_NotifyIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_NotifyIconData.uCallbackMessage = WM_SYSTEMTRAYICON;

    m_NotifyIconData.hWnd = parent.m_hWnd;
    m_NotifyIconData.uID = uid;
    m_NotifyIconData.hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR,
        GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON));
}

void SysTray::Destroy()
{

}

BOOL SysTray::SetIcon(HICON hNewIcon)
{
    try {
        m_NotifyIconData.hIcon = hNewIcon;
        // Update the icon if it is visible.
        if (bInTray) {
            BOOL iRetVal;
            iRetVal = Shell_NotifyIcon(NIM_MODIFY, &m_NotifyIconData);
            if (iRetVal) {
                bInTray = true;
            }
            return iRetVal;
        }
        return 1;
    }
    catch (std::wstring err) {
        throw err;
        assert(false);
        return 0;
    }
}

HICON SysTray::GetIcon()
{
    return m_NotifyIconData.hIcon;
}

BOOL SysTray::SetTipText(ATL::CString newTipText)
{
    try {
        _tcscpy_s(m_NotifyIconData.szTip, newTipText);
        // Update the icon if it is visible.
        if (bInTray) {
            BOOL iRetVal;
            iRetVal = Shell_NotifyIcon(NIM_MODIFY, &m_NotifyIconData);
            if (iRetVal) {
                bInTray = true;
            }
            return iRetVal;
        }
        return 1;
    }
    catch (std::wstring err) {
        throw err;
        assert(false);
        return 0;
    }
}

char *SysTray::GetTipText()
{
    // TODO/wolfreak99: Find out how to make this text show up, and then try to get commented code to show.
    return "test"; // NotifyIconData.szTip;
}

BOOL SysTray::AddIcon()
{
    assert(m_NotifyIconData.cbSize);
    assert(!bInTray);
    
    if (!bInTray) {
        BOOL iRetVal = Shell_NotifyIcon(NIM_ADD, &m_NotifyIconData);
        assert(iRetVal);
        if (iRetVal) {
            bInTray = true;
        }
        return iRetVal;
    }

    return 0;
}

BOOL SysTray::RemoveIcon()
{
    assert(m_NotifyIconData.cbSize);
    assert(bInTray);

    if (bInTray) {
        BOOL iRetVal = Shell_NotifyIcon(NIM_DELETE, &m_NotifyIconData);
        assert(iRetVal);
        if (iRetVal) {
            bInTray = false;
        }
        return iRetVal;
    }

    return 0;
}
