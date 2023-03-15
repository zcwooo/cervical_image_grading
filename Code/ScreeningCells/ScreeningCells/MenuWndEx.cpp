#include "stdafx.h"
#include "MenuWndEx.h"
#include "CasesMgrWnd.h"

void CMenuWndRight::Notify(TNotifyUI& msg)
{
	if (msg.sType == _T("itemselect")) {

		//Close();
	}
	else if (msg.sType == _T("itemclick")) {
		int val = 0;
		if (msg.pSender->GetName() == _T("menu_select_sample")) {
			val = 0;
		}
		else if (msg.pSender->GetName() == _T("menu_open_report")) {
			val = 1;
		}
		else if (msg.pSender->GetName() == _T("menu_open_path"))
		{
			val = 2;
		}
		else if (msg.pSender->GetName() == _T("menu_delete_report"))
		{
			val = 3;
		}
		//打开路径
		else if (msg.pSender->GetName() == _T("menu_main_open_path"))
		{
			val = 10;
		}
		//复制路径
		else if (msg.pSender->GetName() == _T("menu_main_copy_path"))
		{
			val = 11;
		}
		//设置路径
		else if (msg.pSender->GetName() == _T("menu_main_set_path"))
		{
			val = 12;
		}
		::PostMessage(m_hWndNotify, WM_USER+2000, val, NULL);
		Close();
	}
}

void CMenuWndRight:: Init(CMainDlgWnd* pOwner, POINT pt, HWND wnd) {
	//if( pOwner == NULL ) return;
	m_pOwner = NULL;
	m_ptPos = pt;
	m_hWndNotify = wnd;
	Create(pOwner->GetManager()->GetPaintWindow(), NULL, WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_TOOLWINDOW);
	HWND hWndParent = m_hWnd;
	while (::GetParent(hWndParent) != NULL) hWndParent = ::GetParent(hWndParent);
	::ShowWindow(m_hWnd, SW_SHOW);
	::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);
}