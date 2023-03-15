#pragma once
#include "CScan.h"
#include "CStage.h"
#include "CZ.h"

class CLoginSkipWnd : public WindowImplBase
{
public:
	static DWORD WINAPI NotifyLogin(LPVOID lpParameter)
	{
		CLoginSkipWnd* pThis = (CLoginSkipWnd*)lpParameter;
		if (pThis)
		{
			pThis->DetectLogin();

		}
		return 0;
	}
	static CLoginSkipWnd* pWnd;
	static int _MessageBox(HWND hParent = NULL)
	{
		pWnd = new CLoginSkipWnd();
		pWnd->Create(hParent, _T("loginskipwnd"), WS_POPUP | WS_CLIPCHILDREN, WS_EX_TOOLWINDOW);
		pWnd->CenterWindow();

		HANDLE hThread = CreateThread(NULL, 0, &NotifyLogin, (LPVOID)pWnd, 0, NULL);

		::SetWindowPos(NULL, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		return pWnd->ShowModal();

// 		CPopWnd* pPopWnd = new CPopWnd();
// 		pPopWnd->Create(m_hWnd, NULL, WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW | WS_EX_TOPMOST, 0, 0, 800, 572);
// 		pPopWnd->CenterWindow();
	}
public:
	CLoginSkipWnd();
	~CLoginSkipWnd();

public:
	virtual void OnFinalMessage(HWND);
	virtual CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName(void) const;
	virtual void InitWindow();

	DUI_DECLARE_MESSAGE_MAP()
	virtual void OnClick(TNotifyUI& msg);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	bool DetectLogin();

	//ºÏ≤‚œ‘Œ¢æµª∑æ≥
	CScan m_scan;
	CStage m_stage;
	CZ m_cz;
	bool DetectPrior();
	BOOL InitPrior(int nPort);
};

