#include "stdafx.h"
#include "PopMenuInfoWnd.h"

DUI_BEGIN_MESSAGE_MAP(CPopMenuInfoWnd, WindowImplBase)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_END_MESSAGE_MAP()

CPopMenuInfoWnd* CPopMenuInfoWnd::pWnd = NULL;

void CPopMenuInfoWnd::SetValueInt(int i)
{
	m_valueIntInfo = i;
}

void CPopMenuInfoWnd::SetValueDescribe(CString str)
{
	m_strDescribe = str;
}

void CPopMenuInfoWnd::SetTitle(CString str)
{
	m_strTitle = str;
}

void CPopMenuInfoWnd::SetLabel2Invisible()
{

}

CPopMenuInfoWnd::CPopMenuInfoWnd()
{
	m_pLabel1 = NULL;
	m_pLabel2 = NULL;
	m_valueIntInfo = 0;
	m_bLabel2Invisible = true;
}

CPopMenuInfoWnd::~CPopMenuInfoWnd()
{
}

void CPopMenuInfoWnd::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
	pWnd = NULL;
}

DuiLib::CDuiString CPopMenuInfoWnd::GetSkinFile()
{
	return _T("xmls/popmenuinfo.xml");
}

LPCTSTR CPopMenuInfoWnd::GetWindowClassName(void) const
{
	return _T("PopMenuInfoWnd");
}

void CPopMenuInfoWnd::InitWindow()
{
	m_pLabel1 = static_cast<CLabelUI*>(m_pm.FindControl(_T("label1")));
	m_pLabel1->SetText(m_strDescribe);
	m_pLabel2 = static_cast<CEditUI*>(m_pm.FindControl(_T("label2")));
	CDuiString str2;
	str2.Format(_T("%d"), m_valueIntInfo);
	m_pLabel2->SetText(str2);

	m_pLabelTitle = static_cast<CEditUI*>(m_pm.FindControl(_T("labeltitle")));
	m_pLabelTitle->SetText(m_strTitle);

	if (m_bLabel2Invisible == false)
	{
		m_pLabel2->SetVisible(false);
		m_pLabel2->SetFixedWidth(200);
	}
	else
		m_pLabel2->SetVisible(true);

	CButtonUI* pBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("close")));
	pBtn->SetFocus();
}

void CPopMenuInfoWnd::OnClick(TNotifyUI &msg)
{
	CDuiString sName = msg.pSender->GetName();
	sName.MakeLower();
	if (0 == sName.Compare(_T("close")))
	{
		Close(MSGID_CANCEL);
	}
}

LRESULT CPopMenuInfoWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CPopMenuInfoWnd::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0L;
}
