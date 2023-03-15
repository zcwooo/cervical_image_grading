#pragma once
#include "afxcmn.h"


// dlgSetting dialog

class dlgSetting : public CDialog
{
	DECLARE_DYNAMIC(dlgSetting)

public:
	dlgSetting(CWnd* pParent = NULL);   // standard constructor
	virtual ~dlgSetting();

// Dialog Data
	enum { IDD = IDD_DIALOG_SETTING };

private:
	CWnd* m_pParent;
	BOOL m_bInited;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnLeft();
	afx_msg void OnBnClickedBtnRight();
	afx_msg void OnBnClickedBtnForward();
	afx_msg void OnBnClickedBtnBack();
	afx_msg void OnBnClickedButtonUp();
	afx_msg void OnBnClickedButtonDown();
	DWORD m_nXStep;
	DWORD m_nYStep;
	CString m_strRadius;
	virtual BOOL OnInitDialog();
	BOOL m_bAutoCal;
};
