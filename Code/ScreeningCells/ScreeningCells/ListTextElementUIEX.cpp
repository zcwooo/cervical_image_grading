#include "stdafx.h"
#include "ListTextElementUIEX.h"
#include "MainDlgWnd.h"

extern CMainDlgWnd* gMainDlgWnd;

IMPLEMENT_DUICONTROL(CListTextElementUIEX)

CListTextElementUIEX::CListTextElementUIEX()
{
}


CListTextElementUIEX::~CListTextElementUIEX()
{
}
DWORD GetColorDword(LPCTSTR clr)
{
	if (clr == NULL) return 0;
	LPCTSTR colVal = clr;
	if (*colVal == _T('#')) colVal = ::CharNext(colVal);
	LPTSTR pstr = NULL;
	DWORD clrColor = _tcstoul(colVal, &pstr, 16);
	return clrColor;
}

void CListTextElementUIEX::DoEvent(DuiLib::TEventUI& event)
{
	CListTextElementUI::DoEvent(event);
	if (event.Type == DuiLib::UIEVENT_DBLCLICK)
	{
		m_pManager->SendNotify(this, _T("ldbclick"), event.wParam, event.lParam);
		m_pManager->ReleaseCapture();
	}
	else if (event.Type == DuiLib::UIEVENT_RBUTTONDOWN)
	{
		m_pManager->SendNotify(this, _T("lrightclick"), event.wParam, event.lParam);
		m_pManager->ReleaseCapture();
	}
	else if (event.Type == DuiLib::UIEVENT_BUTTONDOWN)
	{
		m_pManager->SendNotify(this, _T("lleftclick"), event.wParam, event.lParam);
		m_pManager->ReleaseCapture();
	}

	return;
}

bool CListTextElementUIEX::DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl)
{
	

	CListLabelElementUI::DoPaint(hDC, rcPaint, pStopControl);

// 	if (gMainDlgWnd->_iBingLiSel>0)
// 	{
// 		CRenderEngine::DrawColor(hDC, m_rcItem, GetAdjustColor(GetColorDword(_T("603B8CFB"))));
// 	}
	

	return true;
}

void CListTextElementUIEX::PaintBkColor(HDC hDC)
{
	__super::PaintBkColor(hDC);
}
