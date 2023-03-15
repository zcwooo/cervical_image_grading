#pragma once
class CRegisterWnd : public WindowImplBase
{
public:
	static int MessageBox(HWND hParent = NULL)
	{
		CRegisterWnd* pWnd = new CRegisterWnd();
		pWnd->Create(hParent, _T("registerinfownd"), WS_POPUP | WS_CLIPCHILDREN, WS_EX_TOOLWINDOW);
		pWnd->CenterWindow();
		return pWnd->ShowModal();
	}
public:
	CRegisterWnd();
	~CRegisterWnd();

	CEditUI* m_pEdit1;
	CEditUI* m_pEdit2;
	CEditUI* m_pEdit3;


public:
	virtual void OnFinalMessage(HWND);
	virtual CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName(void) const;
	virtual void InitWindow();

	DUI_DECLARE_MESSAGE_MAP()
		virtual void OnClick(TNotifyUI& msg);

	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);


};

