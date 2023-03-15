#pragma once


class CPatientInfoWnd : public WindowImplBase
{
public:
	static int MessageBox(HWND hParent = NULL)
	{
		CPatientInfoWnd* pWnd = new CPatientInfoWnd();
		pWnd->Create(hParent, _T("patientinfownd"), WS_POPUP | WS_CLIPCHILDREN, WS_EX_TOOLWINDOW);
		pWnd->CenterWindow();
		return pWnd->ShowModal();
	}
public:
	CPatientInfoWnd();
	~CPatientInfoWnd();

	CEditUI* m_pEditName;
	COptionUI* m_pOptionUI1;
	COptionUI* m_pOptionUI2;
	CEditUI* m_pEditAge;
	CEditUI* m_pEditBingLiHao;
	CEditUI* m_pEditMobilePhone;
	COptionUI* m_pOption;
	CEditUI* m_pEditUI[5];
	CDateTimeUI* m_pDate1;


public:
	virtual void OnFinalMessage(HWND);
	virtual CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName(void) const;
	virtual void InitWindow();

	DUI_DECLARE_MESSAGE_MAP()
	virtual void OnClick(TNotifyUI& msg);

	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

	void CPatientInfoWnd::save();
};

