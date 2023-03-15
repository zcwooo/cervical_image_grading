#pragma once
class CLoginWnd : public WindowImplBase
{
public:
	static int MessageBox(HWND hParent = NULL)
	{
 		CLoginWnd* pWnd = new CLoginWnd();
		pWnd->Create(hParent, _T("loginwnd"), WS_POPUP | WS_CLIPCHILDREN,  WS_EX_TOPMOST);
		pWnd->CenterWindow();
		::SetWindowPos(NULL, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		return pWnd->ShowModal();
	}
public:
	CLoginWnd();
	~CLoginWnd(); 

public:
	virtual void OnFinalMessage(HWND);
	virtual CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName(void) const;
	virtual void InitWindow();
	CControlUI* CreateControl(LPCTSTR pstrClass);

	DUI_DECLARE_MESSAGE_MAP()
	void Notify(TNotifyUI& msg);
	virtual void OnClick(TNotifyUI& msg);

	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

	_ConnectionPtr m_pConnection;
	_RecordsetPtr m_pRecordset;
	BOOL OpenDB();
	void LoadData();
	void CloseDB();

	

	CString GetExePath();

	void Login();
};

