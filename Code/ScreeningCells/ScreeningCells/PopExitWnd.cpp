#include "stdafx.h"
#include "PopExitWnd.h"

DUI_BEGIN_MESSAGE_MAP(CPopExitWnd, WindowImplBase)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_END_MESSAGE_MAP()

CPopExitWnd::CPopExitWnd()
{
}

CPopExitWnd::~CPopExitWnd()
{
}

void CPopExitWnd::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

DuiLib::CDuiString CPopExitWnd::GetSkinFile()
{
	return _T("xmls/popexit.xml");
}

LPCTSTR CPopExitWnd::GetWindowClassName(void) const
{
	return _T("PopExitWnd");
}

void CPopExitWnd::InitWindow()
{
	CButtonUI* pBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("confirm")));
	pBtn->SetFocus();
}

void CPopExitWnd::OnClick(TNotifyUI &msg)
{
	CDuiString sName = msg.pSender->GetName();
	sName.MakeLower();
	if (0 == sName.Compare(_T("close")))
	{
		Close(MSGID_CANCEL);
	}
	else if (0 == sName.Compare(_T("confirm")))
	{
		Close(MSGID_OK);
	}
	else if (0 == sName.Compare(_T("cancel")))
	{
		Close(MSGID_CANCEL);
	}
}

LRESULT CPopExitWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CPopExitWnd::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0L;
}