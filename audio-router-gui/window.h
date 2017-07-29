#pragma once

#include "dialog_main.h"
#include "formview.h"
#include "bootstrapper.h"
#include "SysTray.h"
#include <memory>
#include <shellapi.h>
#include "Dialogs/AboutDialog.h"

#define WIN_WIDTH 970 // 400
#define WIN_HEIGHT 670 // 360
#define GET(type, item) reinterpret_cast<type&>(this->GetDlgItem(item))

enum {
    WM_FIRST = WM_APP,
    WM_SYSTEMTRAYICON,
};

class window : public CFrameWindowImpl<window>
{
private:

    bool dlg_main_b;

    SysTray *m_SysTray;

public:

    dialog_main *dlg_main;
    AboutDialog *dlg_about;

    formview *form_view;
#ifdef ENABLE_BOOTSTRAP
    bootstrapper *bootstrap;
#endif

#ifdef ENABLE_BOOTSTRAP
    explicit window(bootstrapper *);
#else
    explicit window();
#endif
    ~window();

    DECLARE_FRAME_WND_CLASS(L"Audio Router", IDR_MAINFRAME);

    BEGIN_MSG_MAP(window)
    MESSAGE_HANDLER(WM_SYSCOMMAND, OnSysCommand)
    MESSAGE_HANDLER(WM_SYSTEMTRAYICON, OnSystemTrayIcon)
    MSG_WM_CREATE(OnCreate)
    MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
    MESSAGE_HANDLER(WM_QUIT, OnQuit)

    /*MESSAGE_HANDLER(WM_NCHITTEST, OnNcHitTest)*/
    CHAIN_MSG_MAP(CFrameWindowImpl<window>)
    COMMAND_ID_HANDLER(ID_FILE_REFRESHLIST, OnFileRefreshlist)
    COMMAND_ID_HANDLER(ID_ABOUT, OnAbout)
    COMMAND_ID_HANDLER(ID_FILE_SWITCHVIEW, OnFileSwitchview)
    COMMAND_ID_HANDLER(ID_FILE_EXIT, OnFileExit)
    COMMAND_ID_HANDLER(ID_TRAYMENU_SHOWHIDE, OnTrayMenuShowHide)
    COMMAND_ID_HANDLER(ID_TRAYMENU_EXIT, OnTrayMenuExit)

    /*MSG_WM_NCHITTEST(OnNcHitTest)*/
    END_MSG_MAP()

    int OnCreate(LPCREATESTRUCT);

    LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnQuit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    // TODO/wolfreak99: Find out what NcHit even means, and if it's needed. Do we even need to actually HIT it? can't a
    // negotiation be enough?
    LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

    LRESULT OnFileSwitchview(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnFileRefreshlist(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnFileExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnAbout(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

    // REFACTOR/wolfreak99: Eventually these should be moved to the SysTray class
    LRESULT OnSystemTrayIcon(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnTrayMenuShowHide(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
    LRESULT OnTrayMenuExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

    void ShowOrHideWindow();
    bool IsWindowOpen();
    LRESULT Quit();
};
