#include "stdafx.h"
#include "HelpWnd.h"
#include "MainDlgWnd.h"
extern CMainDlgWnd* gMainDlgWnd;

DUI_BEGIN_MESSAGE_MAP(HelpWnd, WindowImplBase)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_END_MESSAGE_MAP()

HelpWnd::HelpWnd()
{
}

HelpWnd::~HelpWnd()
{
}

void HelpWnd::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

DuiLib::CDuiString HelpWnd::GetSkinFile()
{
	return _T("xmls/helpwnd.xml");
}

LPCTSTR HelpWnd::GetWindowClassName(void) const
{
	return _T("HelpWnd");
}

void HelpWnd::InitWindow()
{

}

void HelpWnd::OnClick(TNotifyUI &msg)
{
	CDuiString sName = msg.pSender->GetName();
	sName.MakeLower();
	if (0 == sName.Compare(_T("close")))
	{
		Close(MSGID_OK);
	}
	else if (0 == sName.Compare(_T("btnhtml")))
	{
		gMainDlgWnd->On32818();
	}
}

LRESULT HelpWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (uMsg == WM_TIMER)
	{
		if (wParam == 1000)
		{
		}
	}
	bHandled = FALSE;
	return 0;
}

LRESULT HelpWnd::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0L;
}

LRESULT HelpWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if (uMsg == WM_KEYDOWN)
	{
		switch (wParam)
		{
		case VK_RETURN:
		{
			return 1L;
		}
		default:
			break;
		}
	}
	WindowImplBase::MessageHandler(uMsg, wParam, lParam, bHandled);
	bHandled = false;
	return 0L;
}
