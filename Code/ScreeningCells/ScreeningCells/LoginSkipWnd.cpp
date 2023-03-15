#include "stdafx.h"
#include "LoginSkipWnd.h"
#include "MsgWnd.h"

DUI_BEGIN_MESSAGE_MAP(CLoginSkipWnd, WindowImplBase)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_END_MESSAGE_MAP()

CLoginSkipWnd* CLoginSkipWnd:: pWnd = NULL;

bool gLoginSkipWndContinue = false;

bool bDetectStatus = false;
int iDetectOk = -1;

CLoginSkipWnd::CLoginSkipWnd()
{
}

CLoginSkipWnd::~CLoginSkipWnd()
{
}

void CLoginSkipWnd::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

DuiLib::CDuiString CLoginSkipWnd::GetSkinFile()
{
	return _T("xmls/login_skip.xml");
}

LPCTSTR CLoginSkipWnd::GetWindowClassName(void) const
{
	return _T("LoginSkipWnd");
}

void CLoginSkipWnd::InitWindow()
{
	//SetTimer(m_hWnd, 1000, 2000, NULL);
}

void CLoginSkipWnd::OnClick(TNotifyUI &msg)
{
	CDuiString sName = msg.pSender->GetName();
	sName.MakeLower();
	if (0 == sName.Compare(_T("close")))
	{
		//Close(MSGID_OK);
		if (MSGID_OK == CMsgWnd::MessageBox(m_hWnd, _T("退出"), _T("确定退出？")))
		{
			Close(MSGID_OK);
		}
		else
		{
			Close(MSGID_CANCEL);
		}
	}
}


 LRESULT CLoginSkipWnd::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
 {
 	//::PostQuitMessage(0L);
 
 	bHandled = FALSE;
 	return 0;
 }

LRESULT CLoginSkipWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (uMsg == WM_TIMER)
	{
		if (wParam == 1000)
		{
			KillTimer(m_hWnd, 1000);
			if (bDetectStatus)
			{
				gLoginSkipWndContinue = true;
				Close(MSGID_OK);
			}
			else
			{
// 				if (IDOK == ::MessageBox(m_hWnd, _T("系统自检成功"), _T("医之云"), IDOK))
// 				{
// 				}
				if (iDetectOk==IDOK)
					gLoginSkipWndContinue = true;
				else
					gLoginSkipWndContinue = false;
				
				Close(MSGID_OK);
			}
		}
	}
	if (uMsg == WM_CLOSE)
	{
		Close(MSGID_OK);
	}
	bHandled = FALSE;
	return 0;
}


bool CLoginSkipWnd::DetectLogin()
{
	DetectPrior();
	SetTimer(m_hWnd, 1000, 1, NULL);
	return true;
}

LRESULT CLoginSkipWnd::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0L;
}

bool CLoginSkipWnd::DetectPrior()
{
	TCHAR lpPath[MAX_PATH], lpINI[MAX_PATH];

	::GetModuleFileName(NULL, (lpPath), MAX_PATH - 1);
	*(_tcsrchr(lpPath, '\\')) = '\0';
	_tcscat(lpPath, _T("\\"));

	_stprintf(lpINI, _T("%s%s"), lpPath, _T("pst.ini"));

	TCHAR lpInfo[128];
	int nComPortID = GetPrivateProfileInt(_T("StagePara"), _T("COM_portID"), 1, lpINI);

	bool bRet = false;
	try
	{
//		if (InitPrior(nComPortID) == true)
		{
			bRet = true;
		}
	}
	catch (...)
	{
	}

	if (!bRet)
	{
		SOUND_WARNING;
		HWND hwd = pWnd->GetHWND();
		iDetectOk = ::MessageBox(pWnd->GetHWND(), _T("显微平台初始化错误！"), _T("警告"), MB_OKCANCEL);
	}

	bDetectStatus = bRet;

	return bRet;
}


BOOL CLoginSkipWnd::InitPrior(int nPort)
{
	AfxOleInit();

	COleException e;
	CLSID clsid;
	LPUNKNOWN lpUnk;
	LPDISPATCH lpDispatch;
	long Port = nPort;

	//(1)
	if (CLSIDFromProgID(OLESTR("prior.scan"), &clsid) != NOERROR)
		return FALSE;

	//(1.1)
	if (GetActiveObject(clsid, NULL, &lpUnk) == NOERROR)
	{
		HRESULT hr = lpUnk->QueryInterface(IID_IDispatch, (LPVOID*)&lpDispatch);
		lpUnk->Release();
		if (hr == NOERROR)
			m_scan.AttachDispatch(lpDispatch, TRUE);
	}
	else
	{
		//return FALSE;
	}

	//(1.2)
	// if not dispatch ptr attached yet, need to create one
	if (m_scan.m_lpDispatch == NULL && !m_scan.CreateDispatch(clsid, &e))
	{
		return FALSE;
	}

	//(2)
	if (CLSIDFromProgID(OLESTR("prior.stage"), &clsid) != NOERROR)
		return FALSE;

	//(2.1)
	if (GetActiveObject(clsid, NULL, &lpUnk) == NOERROR)
	{
		HRESULT hr = lpUnk->QueryInterface(IID_IDispatch, (LPVOID*)&lpDispatch);
		lpUnk->Release();
		if (hr == NOERROR)
			m_stage.AttachDispatch(lpDispatch, TRUE);
	}
	else
	{
		//return FALSE;
	}

	//(2.2)
	// if not dispatch ptr attached yet, need to create one
	if (m_stage.m_lpDispatch == NULL && !m_stage.CreateDispatch(clsid, &e))
	{
		return FALSE;
	}

	//(3)
	if (CLSIDFromProgID(OLESTR("prior.z"), &clsid) != NOERROR)
		return FALSE;

	//(3.1)
	if (GetActiveObject(clsid, NULL, &lpUnk) == NOERROR)
	{
		HRESULT hr = lpUnk->QueryInterface(IID_IDispatch, (LPVOID*)&lpDispatch);
		lpUnk->Release();
		if (hr == NOERROR)
			m_cz.AttachDispatch(lpDispatch, TRUE);
	}
	else
	{
		//return FALSE;
	}

	//(3.2)
	// if not dispatch ptr attached yet, need to create one
	if (m_cz.m_lpDispatch == NULL && !m_cz.CreateDispatch(clsid, &e))
	{
		return FALSE;
	}

	//(4)
	m_scan.Connect(&Port);
	if (Port < 0)
	{
		return FALSE;
	}

	return TRUE;
}