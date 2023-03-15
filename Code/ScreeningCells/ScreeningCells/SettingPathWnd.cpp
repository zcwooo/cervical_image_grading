#include "stdafx.h"
#include "SettingPathWnd.h"

DUI_BEGIN_MESSAGE_MAP(SettingPathWnd, WindowImplBase)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_END_MESSAGE_MAP()

SettingPathWnd::SettingPathWnd()
{
}

SettingPathWnd::~SettingPathWnd()
{
}

DuiLib::CControlUI* SettingPathWnd::CreateControl(LPCTSTR pstrClass)
{
	if (lstrcmpi(pstrClass, _T("COptionUIEx")) == 0)
	{
// 		COptionUIEx* pOption = new COptionUIEx();
// 		pOption->m_SettingPathWnd = this;
		return new COptionUIEx();
	}
	return NULL;
}

void SettingPathWnd::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

DuiLib::CDuiString SettingPathWnd::GetSkinFile()
{
	return _T("xmls/settingpath.xml");
}

LPCTSTR SettingPathWnd::GetWindowClassName(void) const
{
	return _T("SettingPathWnd");
}

void SettingPathWnd::InitWindow()
{
	m_pOption = static_cast<COptionUIEx*>(m_pm.FindControl(_T("option2")));

	TCHAR lpPath[MAX_PATH];
	GetModuleFileName(NULL, lpPath, sizeof(lpPath)-1);
	PathRemoveFileSpec(lpPath);
	PathRemoveBackslash(lpPath);
	PathAddBackslash(lpPath);
	TCHAR lpini[MAX_PATH];
	wsprintf(lpini, _T("%s%s"), lpPath, _T("number.ini"));
	TCHAR buf[128] = { 0 };
	GetPrivateProfileString(_T("select_path_out"), _T("select_box"), _T("1"), buf, 128, lpini);
	int iSel = _ttoi(buf);
	m_pOption->Selected(iSel);

	TCHAR buf1[256] = { 0 };
	GetPrivateProfileString(_T("select_path_save"), _T("select_path"), _T(""), buf1, 256, lpini);
	if (PathFileExists(buf1))
	{
		m_sSettingPath = buf1;
	}
	else
	{
		CString path = GetOutFilePath();
		WritePrivateProfileString(_T("select_path_save"), _T("select_path"), path.GetString(), lpini);
		m_sSettingPath = path;
	}
	m_pLabel = static_cast<CLabelUI*>(m_pm.FindControl(_T("outpath")));
	m_pLabel->SetText(m_sSettingPath.GetString());
	int a = 1;
}

static TCHAR gszFolder[MAX_PATH * 2];
INT CALLBACK BrowseCallbackProc1(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData)
{
	LPITEMIDLIST tmp = (LPITEMIDLIST)lp;
	switch (uMsg)
	{
	case   BFFM_INITIALIZED:
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)gszFolder);
		break;

	case   BFFM_SELCHANGED:
		TCHAR szc[MAX_PATH] = { 0 };
		if (SHGetPathFromIDList((LPITEMIDLIST)tmp, szc))
		{
			SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)szc);
		}
		break;
	}
	return   0;
}

void SettingPathWnd::OnClick(TNotifyUI &msg)
{
	CDuiString sName = msg.pSender->GetName();
	sName.MakeLower();
	if (0 == sName.Compare(_T("close")))
	{
		Close(MSGID_OK);
	}
	else if (0 == sName.Compare(_T("openpath")))
	{
		if (m_sSettingPath.GetLength() > 0 && !PathFileExists(m_sSettingPath.GetString()))
		{
			SOUND_WARNING;
			::MessageBox(m_hWnd, _T("文件路径错误."), _T("警告"), MB_OKCANCEL);
		}
		ShellExecute(NULL, _T("open"), NULL, NULL, m_sSettingPath.GetString(), SW_SHOWNORMAL);
	}
	else if (0 == sName.Compare(_T("modifypath")))
	{
		BROWSEINFO bi;
		ZeroMemory(&bi, sizeof(BROWSEINFO));
		bi.pidlRoot = NULL;
		bi.hwndOwner = m_hWnd;
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
		bi.lpfn = BrowseCallbackProc1;
		bi.lParam = (LPARAM)&gszFolder;
		LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
		BOOL bRet = FALSE;
		if (pidl)
		{
			if (SHGetPathFromIDList(pidl, gszFolder))bRet = TRUE;
			IMalloc *pMalloc = NULL;
			if (SUCCEEDED(SHGetMalloc(&pMalloc)) && pMalloc)
			{
				pMalloc->Free(pidl);
				pMalloc->Release();
			}
		}
		CString str = gszFolder;

		if (!str.IsEmpty() && PathFileExists(str.GetString()))
		{
			m_sSettingPath = str;
		}

		m_pLabel->SetText(m_sSettingPath.GetString());

		TCHAR lpPath[MAX_PATH];
		GetModuleFileName(NULL, lpPath, sizeof(lpPath)-1);
		PathRemoveFileSpec(lpPath);
		PathRemoveBackslash(lpPath);
		PathAddBackslash(lpPath);
		TCHAR lpini[MAX_PATH];
		wsprintf(lpini, _T("%s%s"), lpPath, _T("number.ini"));
		WritePrivateProfileString(_T("select_path_save"), _T("select_path"), m_sSettingPath.GetString(), lpini);
	}
}

LRESULT SettingPathWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

LRESULT SettingPathWnd::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0L;
}

LRESULT SettingPathWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if (uMsg == WM_KEYDOWN)
	{
		switch (wParam)
		{
		case VK_RETURN:
		{
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

CString SettingPathWnd::GetOutFilePath()
{
	TCHAR lpPath[MAX_PATH];
	GetModuleFileName(NULL, lpPath, sizeof(lpPath)-1);
	PathRemoveFileSpec(lpPath);
	PathRemoveBackslash(lpPath);
	PathAddBackslash(lpPath);

	CString path1, path2;
	int path1Index = -1;
	bool bIsDPan = true, bIsSysPan = false;

	vector<CString> vecStr;
	TCHAR szBuf[100];
	memset(szBuf, 0, 100);
	DWORD len = GetLogicalDriveStrings(sizeof(szBuf) / sizeof(TCHAR), szBuf);
	TCHAR c[20] = _T("abc");
	int ll = _tcslen(c);
	for (TCHAR * s = szBuf; *s; s += _tcslen(s) + 1){
		LPCTSTR sDrivePath = s;
		cout << sDrivePath << endl;
		vecStr.push_back(sDrivePath);
	}

	TCHAR str[MAX_PATH];
	GetSystemDirectory(str, MAX_PATH);
	for (int i = 0; i < vecStr.size(); ++i)
	{
		CString sTemp = vecStr[i];
		sTemp.MakeUpper();
		CString sTempSys(str);
		sTempSys.MakeUpper();
		int pos = sTempSys.Find(sTemp);
		if (pos >= 0)
		{
			path1 = vecStr[i];
			path1Index = i;
		}
		int pos1 = sTemp.Find(_T("D:"));
		if (pos1>=0)
		{
			bIsDPan = true;
			if (path1Index == i)
				bIsSysPan = true;
		}
	}

	path2 = _T("D:\\"); 

	CString path3;
	if (bIsDPan)
	{
		if (bIsSysPan)
		{
			for (int i = 0; i < vecStr.size(); ++i)
			{
				if (path1Index!=i)
				{
					path3 = vecStr[i];
					break;
				}
			}
		}
		else
		{
			path3 = path2;
		}
	}
	else
	{
		for (int i = 0; i < vecStr.size(); ++i)
		{
			if (path1Index != i)
			{
				path3 = vecStr[i];
				break;
			}
		}
	}
	if (!path3.IsEmpty())
	{
		path3 += _T("医之云样本目录\\");

		bool bOk = GetFileAttributes(path3.GetString()) & FILE_ATTRIBUTE_DIRECTORY;
		bool bExists = PathFileExists(path3.GetString());
		if (!bExists) {
			bool flag = CreateDirectory(path3.GetString(), NULL);
			if (flag)
			{
				return path3;
			}
		}
		else {
			cout << "Directory already exists." << endl;
			return path3;
		}

	}

	return lpPath;
}

