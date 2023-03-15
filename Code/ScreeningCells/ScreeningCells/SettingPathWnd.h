#pragma once
#include "ControlEx.h"
class SettingPathWnd : public WindowImplBase
{
public:
	static int MessageBox(HWND hParent = NULL)
	{
		SettingPathWnd* pWnd = new SettingPathWnd();
		pWnd->Create(hParent, _T("SettingPathWnd"), WS_POPUP | WS_CLIPCHILDREN, WS_EX_TOOLWINDOW);
		pWnd->CenterWindow();
		return pWnd->ShowModal();
	}
public:
	SettingPathWnd();
	~SettingPathWnd();

	COptionUIEx* m_pOption;
	CLabelUI*	 m_pLabel;
	CString m_sSettingPath;


public:
	CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void OnFinalMessage(HWND);
	virtual CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName(void) const;
	virtual void InitWindow();

	DUI_DECLARE_MESSAGE_MAP()
	virtual void OnClick(TNotifyUI& msg);

	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

	CString GetOutFilePath();
};

