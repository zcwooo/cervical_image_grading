#pragma once

class CPopMenuSettingWnd : public WindowImplBase
{
public:
	int    m_iType;
	CString m_strDescribe;
	CLabelUI* m_pLabel1;
	CEditUI * m_pEdit1;

public:
	static CPopMenuSettingWnd* pWnd;
	static int MessageBox(HWND hParent = NULL)
	{
		if (pWnd == NULL)
		{
			pWnd = new CPopMenuSettingWnd();
		}
		pWnd->Create(hParent, _T("popmenusettingwnd"), WS_POPUP | WS_CLIPCHILDREN, WS_EX_TOOLWINDOW);
		pWnd->CenterWindow();
		return pWnd->ShowModal();
	}
	static CPopMenuSettingWnd* GetThis()
	{
		if (pWnd==NULL)
		{
			pWnd = new CPopMenuSettingWnd();
		}
		return pWnd;
	}
	void setType(int i = 0) //0 int 1double
	{
		m_iType = i;
	}
	void SetValueDouble(double dou);

	void SetValueInt(int i);

	void SetValueDescribe(CString str)
	{
		m_strDescribe = str;
	}
public:
	CPopMenuSettingWnd();
	~CPopMenuSettingWnd();

public:
	virtual void OnFinalMessage(HWND);
	virtual CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName(void) const;
	virtual void InitWindow();

	DUI_DECLARE_MESSAGE_MAP()
		virtual void OnClick(TNotifyUI& msg);

	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void GetTextData();
};

