#include "stdafx.h"
#include "HistogramWnd.h"
#include "ControlEx.h"

DUI_BEGIN_MESSAGE_MAP(CHistogramWnd, WindowImplBase)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_END_MESSAGE_MAP()

CHistogramWnd::CHistogramWnd()
{
}


CHistogramWnd::~CHistogramWnd()
{
}

void CHistogramWnd::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

DuiLib::CDuiString CHistogramWnd::GetSkinFile()
{
	return _T("xmls/histogram.xml");
}

LPCTSTR CHistogramWnd::GetWindowClassName(void) const
{
	return _T("HistogramWnd");
}

void CHistogramWnd::InitWindow()
{
	//SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_APPWINDOW);
}

CControlUI* CHistogramWnd::CreateControl(LPCTSTR pstrClass)
{
	if (lstrcmpi(pstrClass, _T("histogram")) == 0)
	{
		return new CHistogramUI();
	}
	return NULL;
}

void CHistogramWnd::OnClick(TNotifyUI &msg)
{
	CDuiString sName = msg.pSender->GetName();
	sName.MakeLower();
	if (0 == sName.Compare(_T("close")))
	{
		Close(MSGID_OK);
	}
}

LRESULT CHistogramWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CHistogramWnd::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0L;
}