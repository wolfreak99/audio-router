#include "AboutDialog.h"
#include "../window.h"
#include "../../audio-router/common.h"
#include <assert.h>
#include "../Version.h"
#include <string.h>


AboutDialog::AboutDialog(window& parent) : parent(parent)
{

}

AboutDialog::~AboutDialog()
{

}

LRESULT AboutDialog::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = false;
    //CStatic _cInfoText;
    //CTabCtrl _cTab1Ctrl;
    //CLinkCtrl _cGithubLinkCtrl;
    //CButton _cCloseBtn;

    // Set info text
    // TODO/wolfreak99: Finish text work
    // TODO/wolfreak99: Make tab box show 3 tabs: general, changelog, and contributors

    auto pWnd = GetDlgItem(IDC_ABOUT_INFOTEXT);
    if (pWnd) {
        if (_infoText[0] == '\0') {
            mbstowcs(_infoText, gVersionInfoFull, strlen(gVersionInfoFull));
        }
        LPWSTR ptr = _infoText;
        SetDlgItemText(IDC_ABOUT_INFOTEXT, ptr);
    }
    
    //this->_cTab1Ctrl = (CTabCtrl*)GetDlgItem(IDC_ABOUT_TAB1);
    //CButton* pButton = (CButton*)GetDlgItem(IDC_);
    
    this->isVisible = true;
    return TRUE;

}

LRESULT AboutDialog::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
    bHandled = true;
    this->EndDialog(0);
    this->isVisible = false;
    
    return 0;
}

LRESULT AboutDialog::OnGithubLinkClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled)
{
    bHandled = true;
    ShellExecute(0, 0, L"http://www.github.com/wolfreak99/audio-router/", 0, 0, SW_SHOW);
    return 0;
}

LRESULT AboutDialog::OnCloseBtnClicked(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL &bHandled)
{
    bHandled = true;
    SendMessage(WM_CLOSE);
    return 0;
}
