#include "stdafx.h"
#include "PopMenuSettingWnd.h"

DUI_BEGIN_MESSAGE_MAP(CPopMenuSettingWnd, WindowImplBase)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_END_MESSAGE_MAP()

double m_valueDoubleSetting=0;
int    m_valueIntSetting=0;

CPopMenuSettingWnd* CPopMenuSettingWnd::pWnd = NULL;


void CPopMenuSettingWnd::SetValueDouble(double dou)
{
	m_valueDoubleSetting = dou;
}

void CPopMenuSettingWnd::SetValueInt(int i)
{
	m_valueIntSetting = i;
}

CPopMenuSettingWnd::CPopMenuSettingWnd()
{
	m_iType = 0;
	m_pLabel1 = NULL;
	m_pEdit1 = NULL;
}


CPopMenuSettingWnd::~CPopMenuSettingWnd()
{
}

void CPopMenuSettingWnd::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
	pWnd = NULL;
}

DuiLib::CDuiString CPopMenuSettingWnd::GetSkinFile()
{
	return _T("xmls/popmenusetting.xml");
}

LPCTSTR CPopMenuSettingWnd::GetWindowClassName(void) const
{
	return _T("PopMenuSettingWnd");
}

void CPopMenuSettingWnd::InitWindow()
{
	m_pLabel1 = static_cast<CLabelUI*>(m_pm.FindControl(_T("label1")));
	m_pLabel1->SetText(m_strDescribe);
	m_pEdit1 = static_cast<CEditUI*>(m_pm.FindControl(_T("edit1")));
	CDuiString strEdit;
	if (m_iType == 0)
		strEdit.Format(_T("%d"), m_valueIntSetting);
	else
		strEdit.Format(_T("%.3f"), m_valueDoubleSetting);
	m_pEdit1->SetText(strEdit);
	CButtonUI* pBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("ok")));
	pBtn->SetFocus();
}

void CPopMenuSettingWnd::OnClick(TNotifyUI &msg)
{
	CDuiString sName = msg.pSender->GetName();
	sName.MakeLower();
	if (0 == sName.Compare(_T("close")))
	{
		Close(MSGID_CANCEL);
	}
	else if (0 == sName.Compare(_T("cancel")))
	{
		Close(MSGID_CANCEL);
	}
	else if (0 == sName.Compare(_T("ok")))
	{
		GetTextData();
		Close(MSGID_OK);
	}
}

LRESULT CPopMenuSettingWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LRESULT CPopMenuSettingWnd::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0L;
}

void CPopMenuSettingWnd::GetTextData()
{
	CDuiString sEdit = m_pEdit1->GetText();
	if (! sEdit.IsEmpty())
	{
		if (m_iType == 0)
			m_valueIntSetting = _wtoi(sEdit);
		else
			m_valueDoubleSetting = atof(CStringA(sEdit).Trim());
	}
}