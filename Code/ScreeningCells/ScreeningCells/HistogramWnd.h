#pragma once
class CHistogramWnd : public WindowImplBase
{
public:
	static int MessageBox(HWND hParent = NULL)
	{
		CHistogramWnd* pWnd = new CHistogramWnd();
		pWnd->Create(hParent, _T("loginwnd"), WS_POPUP | WS_CLIPCHILDREN, WS_EX_TOOLWINDOW);
		pWnd->CenterWindow();
		return pWnd->ShowModal();
	}

public:
	CHistogramWnd();
	~CHistogramWnd();

public:
	virtual void OnFinalMessage(HWND);
	virtual CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName(void) const;
	virtual void InitWindow();

	CControlUI* CreateControl(LPCTSTR pstrClass);

	DUI_DECLARE_MESSAGE_MAP()
		virtual void OnClick(TNotifyUI& msg);

	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
};

