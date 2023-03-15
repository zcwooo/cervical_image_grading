#pragma once
class CRegister
{
public:
	CRegister();
	~CRegister();

	_ConnectionPtr m_pConnection;
	_RecordsetPtr m_pRecordset;
	BOOL OpenDB();
	void CloseDB();
	void SaveRegisterInfo(CString name, CString pwd, CString newPwd, bool bNewRecord = true);

	CString GetExePath();

	void SetHwnd(HWND wnd){ m_hWnd = wnd; }

	HWND m_hWnd;
};

