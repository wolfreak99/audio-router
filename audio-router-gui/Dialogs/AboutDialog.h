#pragma once

#include "../wtl.h"


class window;

class AboutDialog : public CDialogImpl<AboutDialog>
{
private:
    CTabCtrl *_cTab1Ctrl;
    CLinkCtrl *_cGithubLinkCtrl;
    CButton *_cCloseBtn;

    wchar_t _infoText[256];

public:
    window& parent;
    bool isVisible;

    enum {
        IDD = IDD_ABOUTDLG,
    };
    
    AboutDialog(window& parent);
    ~AboutDialog();

    BEGIN_MSG_MAP(AboutDialog)
        MESSAGE_HANDLER(WM_INITDIALOG, OnCreate)
        MESSAGE_HANDLER(WM_CLOSE, OnClose)

        COMMAND_HANDLER(IDC_ABOUT_GITHUBLINK, BN_CLICKED, OnGithubLinkClicked)
        COMMAND_HANDLER(IDC_ABOUT_CLOSEBTN, BN_CLICKED, OnCloseBtnClicked)
    END_MSG_MAP();

    LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
    LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);

    LRESULT OnGithubLinkClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled);
    LRESULT OnCloseBtnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled);

};
