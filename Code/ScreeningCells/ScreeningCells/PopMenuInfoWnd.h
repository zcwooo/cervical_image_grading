#pragma once
class CPopMenuInfoWnd : public WindowImplBase
{
public:
	int    m_valueIntInfo = 0;
	CString m_strDescribe;
	CString m_strTitle;
	bool m_bLabel2Invisible;

	CLabelUI* m_pLabel1;
	CLabelUI* m_pLabel2;
	CLabelUI* m_pLabelTitle;

public:
	static CPopMenuInfoWnd* pWnd;
	static int MessageBox(HWND hParent = NULL)
	{
		if (pWnd == NULL)
		{
			pWnd = new CPopMenuInfoWnd();
		}
		pWnd->Create(hParent, _T("popmenusettingwnd"), WS_POPUP | WS_CLIPCHILDREN, WS_EX_TOOLWINDOW);
		pWnd->CenterWindow();
		return pWnd->ShowModal();
	}
	static CPopMenuInfoWnd* GetThis()
	{
		if (pWnd == NULL)
		{
			pWnd = new CPopMenuInfoWnd();
		}
		return pWnd;
	}

	void SetValueInt(int i);

	void SetValueDescribe(CString str);

	void SetTitle(CString str);

	void SetLabel2Invisible();

public:
	CPopMenuInfoWnd();
	~CPopMenuInfoWnd();

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

