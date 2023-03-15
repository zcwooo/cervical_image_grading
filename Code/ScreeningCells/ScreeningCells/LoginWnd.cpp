#include "stdafx.h"
#include "LoginWnd.h"
#include "LoginSkipWnd.h"
#include "Global.h"
#include "ControlEx.h"
#include "SerialNumber.h"


struct LoginParam gLoginParam;
extern bool gLoginSkipWndContinue;

DUI_BEGIN_MESSAGE_MAP(CLoginWnd, WindowImplBase)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_END_MESSAGE_MAP()

CLoginWnd::CLoginWnd()
{
}

CLoginWnd::~CLoginWnd()
{
}

void CLoginWnd::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

DuiLib::CDuiString CLoginWnd::GetSkinFile()
{
	return _T("xmls/login.xml");
}

LPCTSTR CLoginWnd::GetWindowClassName(void) const
{
	return _T("LoginWnd");
}

void CLoginWnd::InitWindow()
{
	CEditUI* pEditName = static_cast<CEditUI*>(m_pm.FindControl(_T("edit1")));
	pEditName->SetFocus();
}

DuiLib::CControlUI* CLoginWnd::CreateControl(LPCTSTR pstrClass)
{
	if (lstrcmpi(pstrClass, _T("CEditUIEx")) == 0) {
		CEditUIEx * pEditEx = new CEditUIEx();
		pEditEx->SetParentEx(this);
		return pEditEx;
	}
	return NULL;
}

void CLoginWnd::Notify(TNotifyUI& msg)
{
	return WindowImplBase::Notify(msg);
}

void CLoginWnd::OnClick(TNotifyUI &msg)
{
	CDuiString sName = msg.pSender->GetName();
	sName.MakeLower();
	if (0 == sName.Compare(_T("close")))
	{
		gLoginSkipWndContinue = false;
		Close(MSGID_OK);
	}
	else if (0 == sName.Compare(_T("login")))
	{
		Login();
	}
	else if (0 == sName.Compare(_T("cancel")))
	{
		gLoginSkipWndContinue = false;
		Close(MSGID_OK);
	}
}


LRESULT CLoginWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (uMsg == WM_TIMER)
	{
		if (wParam == 1000)
		{
			KillTimer(m_hWnd, 1000);
			Close(MSGID_OK);
			int  i = -1;
		}
	}
	if (uMsg == WM_KEYDOWN && wParam == VK_RETURN)
	{
		bHandled = true;
		Login();
		return 0;
	}
	else if (uMsg == WM_LOGIN)
	{
		bHandled = true;
		Login();
		return 0;
	}
	bHandled = FALSE;
	return 0;
}


LRESULT CLoginWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if (uMsg == WM_KEYDOWN)
	{
		switch (wParam)
		{
		case VK_RETURN:
		{
			Login();
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

LRESULT CLoginWnd::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0L;
}

BOOL CLoginWnd::OpenDB()
{
	BOOL bOK = FALSE;
	m_pConnection = _ConnectionPtr(__uuidof(Connection));
	try
	{
		//https://bbs.csdn.net/topics/330263772
		CString strDBName;
		strDBName = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=";
		strDBName += GetExePath();
		strDBName += "patient.mdb;";
		strDBName += "User ID='admin';Password=;Jet OLEDB:Database Password='yzy_db_2019'";

	//	m_pConnection->ConnectionString = (_bstr_t)strDBName;
		//m_pConnection->ConnectionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=patient.mdb;";
		m_pConnection->Open(strDBName.GetString(), "", "", adConnectUnspecified);
		bOK = TRUE;
	}
	catch (...)
	{
		//::MessageBox(NULL, _T("用户名或者密码错误"), _T("警告"), MB_OKCANCEL);
		return FALSE;
	}
// 	catch (_com_error *e)
// 	{
// 		AfxMessageBox(e->ErrorMessage());
// 		bOK = FALSE;
// 	}

	/* OK
	//CString csFilter;
	//csFilter = ("ID=2");
	//CString csFilter;
	//csFilter = ("patient_name='张三'");

	//CString csSQLstr;
	//csSQLstr = "SELECT * FROM patient_info where ";
	//csSQLstr += csFilter;
	*/
	CString csSQLstr;
// 	csSQLstr.Format(_T("SELECT * FROM patient_login where username=%s AND userpwd=%s"), 
// 		gLoginParam.sName.GetData(), gLoginParam.sPassword.GetData());
	csSQLstr = "SELECT * FROM patient_login";
	csSQLstr += " order by ID desc";

	m_pRecordset.CreateInstance(__uuidof(Recordset));
	try
	{
		m_pRecordset->Open((_bstr_t)csSQLstr, m_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
		bOK = TRUE;
	}
	catch (_com_error *e)
	{
		SOUND_WARNING
		AfxMessageBox(e->ErrorMessage());
		bOK = FALSE;
	}
	return bOK;
}

void CLoginWnd::LoadData()
{
	_variant_t var;
	CString strName, strPwd, strV;;

	try
	{
		if (!m_pRecordset->BOF)
			m_pRecordset->MoveFirst();
		else
		{
			SOUND_WARNING
			::MessageBox(m_hWnd, _T("用户不存在"), _T("警告"), MB_OK);
			return;
		}

		int row = 0;
		int nCol = 0;
		while (!m_pRecordset->adoEOF)
		{
			nCol = 0;
			var = m_pRecordset->GetCollect("ID");
			if (var.vt != VT_NULL)
			{
				int nID = (int)(var);
				strV.Format(_T("%d"), nID);
			}
			nCol++;

			var = m_pRecordset->GetCollect("username");
			if (var.vt != VT_NULL)
			{
				strName = (LPCSTR)_bstr_t(var);
			}
			nCol++;
			var = m_pRecordset->GetCollect( "userpwd" );
			if (var.vt != VT_NULL)
			{
				strPwd = (LPCSTR)_bstr_t(var);
			}
			nCol++;

			if (strName.Compare(gLoginParam.sName) == 0 && 
				strPwd.Compare(gLoginParam.sPassword) == 0)
			{
				gLoginParam.bLogin = true;
				break;
			}

			m_pRecordset->MoveNext();
			row++;
		}

	}
	catch (_com_error *e)
	{
		SOUND_WARNING
		AfxMessageBox(e->ErrorMessage());
	}
}

void CLoginWnd::CloseDB()
{
	if (m_pRecordset != NULL)
	{
		try
		{
			if (m_pRecordset->State)
			{
				m_pRecordset->Close();//关闭记录集
			}
			m_pRecordset = NULL;
		}
		catch (_com_error e)
		{
			SOUND_WARNING
			AfxMessageBox(e.ErrorMessage());
		}

	}

	if (m_pConnection != NULL)
	{
		try
		{
			if (m_pConnection->State)
			{
				m_pConnection->Close();
			}
			m_pConnection = NULL;
		}
		catch (_com_error e)
		{
			SOUND_WARNING
			AfxMessageBox(e.ErrorMessage());
		}
	}
}

CString CLoginWnd::GetExePath()
{
	TCHAR lpPath[MAX_PATH];
	GetModuleFileName(NULL, lpPath, sizeof(lpPath)-1);
	PathRemoveFileSpec(lpPath);
	PathRemoveBackslash(lpPath);
	PathAddBackslash(lpPath);
	return lpPath;
}

void CLoginWnd::Login()
{
	SerialNumber sn;
	bool r = sn.CheckPcID();
	if (!r)
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("系统运行环境错误."), _T("警告"), MB_OK);
		return;
	}

	// 调整窗口样式
// 	LONG styleValue = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
// 	styleValue &= ~WS_EX_TOPMOST;
// 	styleValue |= WS_EX_TOOLWINDOW;
// 	::SetWindowLong(m_hWnd, GWL_EXSTYLE, styleValue);

// 	::SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
// 	return;

	//ShowWindow(false);
	CEditUI* pEditName = static_cast<CEditUI*>(m_pm.FindControl(_T("edit1")));
	gLoginParam.sName = pEditName->GetText();
	CEditUI* pEditPsw = static_cast<CEditUI*>(m_pm.FindControl(_T("edit2")));
	gLoginParam.sPassword = pEditPsw->GetText();

	// 		gLoginParam.sName = _T("test");
	// 		gLoginParam.sPassword = _T("123456");

	CString str = gLoginParam.sName;
	for (int i = 0; i < str.GetLength(); ++i)
	{
		TCHAR c = str.GetAt(i);
		if (c >= 48 && c <= 57 || c >= 65 && c <= 90 || c >= 97 && c <= 122)
		{
		}
		else
		{
			SOUND_WARNING
			::MessageBox(m_hWnd, _T("用户名只能为英文字母和数字"), _T("警告"), MB_OK);
			return;
		}
	}


	if (gLoginParam.sName.IsEmpty())
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("用户名不能为空"), _T("警告"), MB_OK);
		return;
	}
	if (gLoginParam.sName.GetLength() > 60)
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("用户名大于60个字符"), _T("警告"), MB_OK);
		return;
	}
	if (gLoginParam.sPassword.IsEmpty())
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("用户密码不能为空"), _T("警告"), MB_OK);
		return;
	}
	if (gLoginParam.sPassword.GetLength() > 30)
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("用户密码不能大于30个字符"), _T("警告"), MB_OK);
		return;
	}

	if (OpenDB())
	{
		LoadData();
		CloseDB();
	}
	if (gLoginParam.bLogin)
	{
		gLoginSkipWndContinue = true;
		Close(MSGID_OK);
	}
	else
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("用户或者密码不正确"), _T("警告"), MB_OK);
		return;
	}
}
