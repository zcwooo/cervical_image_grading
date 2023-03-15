#include "stdafx.h"
#include "DiagramWnd.h"
#include "ControlEx.h"

DUI_BEGIN_MESSAGE_MAP(CDiagramWnd, WindowImplBase)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_END_MESSAGE_MAP()

CDiagramWnd::CDiagramWnd()
{
}

CDiagramWnd::~CDiagramWnd()
{
}

void CDiagramWnd::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

CDuiString CDiagramWnd::GetSkinFile()
{
	return _T("xmls/diagram.xml");
}

LPCTSTR CDiagramWnd::GetWindowClassName(void) const
{
	return _T("DiagramWnd");
}

void CDiagramWnd::InitWindow()
{

}

CControlUI* CDiagramWnd::CreateControl(LPCTSTR pstrClass)
{
	if (lstrcmpi(pstrClass, _T("diagram")) == 0)
	{
		return new CDiagramUI();
	}
	return NULL;
}

void CDiagramWnd::OnClick(TNotifyUI &msg)
{
	CDuiString sName = msg.pSender->GetName();
	sName.MakeLower();
	if (0 == sName.Compare(_T("close")))
	{
		Close(MSGID_OK);
	}
}

LRESULT CDiagramWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CDiagramWnd::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0L;
}