#include "stdafx.h"
#include "RegisterWnd.h"
#include "MainDlgWnd.h"
#include "Register.h"

extern CMainDlgWnd* gMainDlgWnd;

DUI_BEGIN_MESSAGE_MAP(CRegisterWnd, WindowImplBase)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_END_MESSAGE_MAP()

CRegisterWnd::CRegisterWnd()
{
}

CRegisterWnd::~CRegisterWnd()
{
}

void CRegisterWnd::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

DuiLib::CDuiString CRegisterWnd::GetSkinFile()
{
	return _T("xmls/register.xml");
}

LPCTSTR CRegisterWnd::GetWindowClassName(void) const
{
	return _T("RegisterWnd");
}

void CRegisterWnd::InitWindow()
{
	m_pEdit1 = static_cast<CEditUI*>(m_pm.FindControl(_T("edit1")));
	m_pEdit2 = static_cast<CEditUI*>(m_pm.FindControl(_T("edit2")));
	m_pEdit3 = static_cast<CEditUI*>(m_pm.FindControl(_T("edit3")));

	CButtonUI* pBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("confirm")));
	pBtn->SetFocus();
}

void CRegisterWnd::OnClick(TNotifyUI &msg)
{
	CDuiString sName = msg.pSender->GetName();
	sName.MakeLower();
	if (0 == sName.Compare(_T("close")))
	{
		Close(MSGID_OK);
	}
	else if (0 == sName.Compare(_T("cancel")))
	{
		Close(MSGID_OK);
	}
	else if (0 == sName.Compare(_T("confirm")))
	{
		CString str = m_pEdit1->GetText();
		for (int i = 0; i < str.GetLength(); ++i)
		{
			TCHAR c = str.GetAt(i);
			if (c >= 48 && c <= 57 || c >= 65 && c <= 90 || c >= 97 && c <= 122)
			{
			}
			else
			{
				SOUND_WARNING
				::MessageBox(m_hWnd, _T("�û���ֻ��ΪӢ����ĸ������"), _T("����"), MB_OK);
				return;
			}
		}
		if (str.IsEmpty())
		{
			SOUND_WARNING
			::MessageBox(m_hWnd, _T("�û�������Ϊ��"), _T("����"), MB_OK);
			return;
		}
		if (str.GetLength() > 60)
		{
			SOUND_WARNING
			::MessageBox(m_hWnd, _T("�û�������60���ַ�"), _T("����"), MB_OK);
			return;
		}

		CString pwd1 = m_pEdit2->GetText(), pwd2 = m_pEdit3->GetText();
		if (pwd1.IsEmpty())
		{
			SOUND_WARNING
			::MessageBox(m_hWnd, _T("�û������벻��Ϊ��"), _T("����"), MB_OK);
			return;
		}
		if (pwd1.Compare(pwd2)!=0)
		{
			SOUND_WARNING
			::MessageBox(m_hWnd, _T("�û����벻һ��"), _T("����"), MB_OK);
			return;
		}
		if (pwd1.GetLength() > 30)
		{
			SOUND_WARNING
			::MessageBox(m_hWnd, _T("�û����벻�ܴ���30���ַ�"), _T("����"), MB_OK);
			return;
		}

		CRegister reg;
		reg.SetHwnd(m_hWnd);
		reg.SaveRegisterInfo(str, pwd1, _T(""), true);
	}
}

LRESULT CRegisterWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT CRegisterWnd::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0L;
}