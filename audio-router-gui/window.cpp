#include "window.h"

#ifndef DISABLE_TELEMETRY
telemetry *telemetry_m = NULL;
#endif

// TODO/wolfreak99: Check audiorouterdevs "remove saved routing functionality", To determine if bootstrap (and
// telemetry) are related to feature.
#ifdef ENABLE_BOOTSTRAP
window::window(bootstrapper *bootstrap) : dlg_main_b(true), bootstrap(bootstrap)
#else
window::window() : dlg_main_b(true)
#endif
{
    this->dlg_main = new dialog_main(*this);
    this->form_view = new formview(*this);

    // our windows hWnd is still null, so we can only initialize sysTray partially.
    // the rest is handled in OnCreate. (what a bunch of shit)
    this->m_SysTray = new SysTray();
}

window::~window()
{
    if (this->dlg_main_b) {
        delete this->dlg_main;
    }

    delete this->form_view;
    delete this->m_SysTray;

#ifndef DISABLE_TELEMETRY
    delete telemetry_m;
    telemetry_m = NULL;
#endif
}

int window::OnCreate(LPCREATESTRUCT lpcs)
{
#ifndef DISABLE_TELEMETRY
    telemetry_m = new telemetry;
#endif

    this->m_hWndClient = this->dlg_main->Create(this->m_hWnd);
    this->dlg_main->ShowWindow(SW_SHOW);

    // Set up handles for the system tray.
    m_SysTray->Create(*this, 1);

    ATL::CString sWindowText;
    GetWindowText(sWindowText);
    m_SysTray->SetTipText(sWindowText);
    m_SysTray->AddIcon();

    return 0;
} // OnCreate

LRESULT window::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    m_SysTray->RemoveIcon();
    return 0;
}

LRESULT window::OnQuit(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    this->DestroyWindow();

    // TODO/wolfreak99: This may not even be needed.
    PostQuitMessage(0);
    return 0;
}

LRESULT window::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    return HTCLOSE;
}

LRESULT window::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    switch (wParam) {
    case SC_CLOSE:

        // Closing the window shows the icon while hiding it from the minimized windows bar.
        this->ShowWindow(SW_HIDE);

        // We do not need any further handling done, so set bHandled to TRUE
        bHandled = TRUE;
        break;
    case SC_RESTORE:

        // TODO/wolfreak99: This may not be needed, it's commented out eventually, but I'm keeping it here just incase
        for (dialog_main::dialog_arrays_t::iterator it = this->dlg_main->dialog_arrays.begin();
            it != this->dlg_main->dialog_arrays.end();
            it++)
        {
            for (dialog_array::dialog_controls_t::iterator jt = (*it)->dialog_controls.begin();
                jt != (*it)->dialog_controls.end();
                jt++)
            {
                (*jt)->set_display_name(false, false);
            }
        }

        ShowWindow(SW_SHOW);
        BringWindowToTop();

        // Have windows still handle the rest
        bHandled = FALSE;
        break;
    default:
        bHandled = false;
        break;
    } // switch

    return 0;
} // OnSysCommand

LRESULT window::OnSystemTrayIcon(UINT, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    ATLASSERT(wParam == 1);

    switch (lParam) {
    case WM_LBUTTONUP:
        ShowOrHideWindow();
        break;
    case WM_RBUTTONUP:

        // SetForegroundWindow(m_hWnd);

        CPoint pos;
        ATLVERIFY(GetCursorPos(&pos));

        CMenu menu;
        menu.LoadMenuW(IDR_TRAYMENU);
        CMenuHandle popupMenu = menu.GetSubMenu(0);

        popupMenu.TrackPopupMenu(TPM_RIGHTALIGN | TPM_BOTTOMALIGN | TPM_LEFTBUTTON,
            pos.x,
            pos.y,
            this->m_hWnd);
        break;
    } // switch
    return 0;
} // OnSystemTrayIcon

LRESULT window::OnFileRefreshlist(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    if (!this->dlg_main_b) {
        this->form_view->refresh_list();
    }

    return 0;
}

LRESULT window::OnAbout(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    // TODO/wolfreak99: Update this about to be more accurate, CLAIM OUR STOLEN FUCKING TERRITORY, FEARLESS OF LAWSUITS.
    this->MessageBoxW(
        L"Audio Router version 0.10.2.\n"\
        L"\nIf you come across any bugs(especially relating to routing or duplicating), "\
        L"or just have an idea for a new feature, "\
        L"please send a PM to the developer on reddit: reddit.com/user/audiorouterdev/",
        L"About", MB_ICONINFORMATION);

    return 0;
}

LRESULT window::OnFileSwitchview(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    RECT rc;

    this->GetClientRect(&rc);

    if (this->dlg_main_b) {
        this->dlg_main->DestroyWindow();
        delete this->dlg_main;

        this->m_hWndClient = this->form_view->Create(*this);

        // this->form_view->ShowWindow(SW_SHOW);
        this->form_view->SetWindowPos(NULL, &rc, SWP_NOZORDER | SWP_SHOWWINDOW);
    }
    else {
        // TODO/wolfreak99: Should form_view be deleted as well?
        this->form_view->DestroyWindow();

        this->dlg_main = new dialog_main(*this);
        this->m_hWndClient = this->dlg_main->Create(*this);

        // this->dlg_main->ShowWindow(SW_SHOW);
        this->dlg_main->SetWindowPos(NULL, &rc, SWP_NOZORDER | SWP_SHOWWINDOW);
    }

    this->dlg_main_b = !this->dlg_main_b;

    return 0;
} // OnFileSwitchview

LRESULT window::OnFileExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    return Quit();
}

LRESULT window::OnTrayMenuExit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    return Quit();
}

LRESULT window::OnTrayMenuShowHide(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
{
    ShowOrHideWindow();
    return 0;
}

void window::ShowOrHideWindow()
{
    if (IsWindowOpen()) {
        SendMessage(WM_SYSCOMMAND, SC_CLOSE);
    }
    else {
        SendMessage(WM_SYSCOMMAND, SC_RESTORE);
    }
}

bool window::IsWindowOpen()
{
    return this->IsWindowVisible() && !this->IsIconic();
}

// Quits the program. Use this instead of calling SendMessage(WM_QUIT) everywhere.
LRESULT window::Quit()
{
    return SendMessage(WM_QUIT);
}
