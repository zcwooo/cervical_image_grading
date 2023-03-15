#pragma once
class CModifyPwdWnd : public WindowImplBase
{
public:
	static int MessageBox(HWND hParent = NULL)
	{
		CModifyPwdWnd* pWnd = new CModifyPwdWnd();
		pWnd->Create(hParent, _T("modifypwdwnd"), WS_POPUP | WS_CLIPCHILDREN, WS_EX_TOOLWINDOW);
		pWnd->CenterWindow();
		return pWnd->ShowModal();
	}
public:
	CModifyPwdWnd();
	~CModifyPwdWnd();

	CEditUI* m_pEdit1;
	CEditUI* m_pEdit2;
	CEditUI* m_pEdit3;
	CEditUI* m_pEdit4;

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
