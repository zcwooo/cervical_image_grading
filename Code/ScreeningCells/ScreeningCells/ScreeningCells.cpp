#include "StdAfx.h"
#include <exdisp.h>
#include <comdef.h>
#include "ControlEx.h"
#include "resource.h"
#include <ShellAPI.h>
#include "SkinFrame.h"
#include "PopWnd.h"
#include "SplashWnd.h"

#define _CRTDBG_MAP_ALLOC
#include<stdlib.h>
#include<crtdbg.h>

#include "LoginWnd.h"
#include "LoginSkipWnd.h"
#include "MainDlgWnd.h"

#include "HistogramWnd.h"

extern struct LoginParam gLoginParam;
extern bool gLoginSkipWndContinue;

void InitResource()
{
	// 资源类型
#ifdef _DEBUG
	CPaintManagerUI::SetResourceType(UILIB_FILE);
#else
	CPaintManagerUI::SetResourceType(UILIB_FILE);
#endif
	// 资源路径
	CDuiString strResourcePath = CPaintManagerUI::GetInstancePath();
	// 加载资源
	switch(CPaintManagerUI::GetResourceType())
	{
	case UILIB_FILE:
		{
			strResourcePath += _T("skin\\ScreeningCells\\");
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
			// 加载资源管理器
			CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
			break;
		}
	case UILIB_RESOURCE:
		{
			strResourcePath += _T("skin\\ScreeningCells\\");
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
			// 加载资源管理器
			CResourceManager::GetInstance()->LoadResource(_T("IDR_RES"), _T("xml"));
			break;
		}
	case UILIB_ZIP:
		{
			strResourcePath += _T("skin\\ScreeningCells\\");
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
			// 加密
			CPaintManagerUI::SetResourceZip(_T("ScreeningCells.zip"), true, _T("yzy_2019"));
			//CPaintManagerUI::SetResourceZip(_T("ScreeningCells.zip"), true);
			// 加载资源管理器
			CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
			break;
		}
	case UILIB_ZIPRESOURCE:
		{
			strResourcePath += _T("skin\\ScreeningCells\\");
			CPaintManagerUI::SetResourcePath(strResourcePath.GetData());
			HRSRC hResource = ::FindResource(CPaintManagerUI::GetResourceDll(), _T("IDR_ZIPRES"), _T("ZIPRES"));
			if( hResource != NULL ) {
				DWORD dwSize = 0;
				HGLOBAL hGlobal = ::LoadResource(CPaintManagerUI::GetResourceDll(), hResource);
				if( hGlobal != NULL ) {
					dwSize = ::SizeofResource(CPaintManagerUI::GetResourceDll(), hResource);
					if( dwSize > 0 ) {
						CPaintManagerUI::SetResourceZip((LPBYTE)::LockResource(hGlobal), dwSize);
						// 加载资源管理器
						CResourceManager::GetInstance()->LoadResource(_T("res.xml"), NULL);
					}
				}
				::FreeResource(hResource);
			}
		}
		break;
	}

	// 注册控件
	REGIST_DUICONTROL(CCircleProgressUI);
	REGIST_DUICONTROL(CMyComboUI);
	REGIST_DUICONTROL(CChartViewUI);
	REGIST_DUICONTROL(CWndUI);
	REGIST_DUICONTROL(CEditUIEx1);
}

static DWORD WINAPI NotifyLogin(LPVOID lpParameter)
{
	int i = -1;
	return 0;
}

CMainDlgWnd* gMainDlgWnd = NULL;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPSTR /*lpCmdLine*/, int nCmdShow)
{
	//_CrtSetBreakAlloc(295);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtDumpMemoryLeaks();

	HANDLE  hMutex = CreateMutex(NULL, FALSE, _T("ScreeningCells"));
	if (ERROR_ALREADY_EXISTS == GetLastError())
	{
		return 0;
	}

	//COM;
	//HRESULT Hr = ::CoInitialize(NULL);
	//if( FAILED(Hr) ) return 0;
	//// OLE
	//HRESULT hRes = ::OleInitialize(NULL);
	// 初始化UI管理器
	CPaintManagerUI::SetInstance(hInstance);
	// 初始化资源
	InitResource();

// 	CHistogramWnd* pHistogramWnd = new CHistogramWnd();
// 	// WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW WS_EX_TOPMOST
// 	pHistogramWnd->Create(NULL, NULL, WS_POPUP | WS_CLIPCHILDREN | WS_VISIBLE, WS_EX_TOOLWINDOW, 0, 0, 0, 0);
// 	pHistogramWnd->CenterWindow();
// 	SetWindowPos(pHistogramWnd->GetHWND(), NULL, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOACTIVATE);

 	int iret = CLoginWnd::MessageBox();
	if (gLoginSkipWndContinue == false)
		goto exeExit;

	HANDLE hThread = CreateThread(NULL, 0, &NotifyLogin, NULL, 0, NULL);

	CLoginSkipWnd::_MessageBox();
	if (gLoginSkipWndContinue== false)
		goto exeExit;

	gMainDlgWnd = new CMainDlgWnd();
	if (gMainDlgWnd == NULL) return 0;
	gMainDlgWnd->Create(NULL, _T("医之云"), UI_WNDSTYLE_FRAME, 0L, 0, 0, 1302, 752);
	gMainDlgWnd->CenterWindow();
	gMainDlgWnd->_SetFocus();
	//::SetWindowPos(gMainDlgWnd->GetHWND(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	CPaintManagerUI::MessageLoop();
	delete gMainDlgWnd;
	gMainDlgWnd = NULL;

exeExit:
	// 清理资源
	CPaintManagerUI::Term();
	// OLE
	//OleUninitialize();
	//// COM
	//::CoUninitialize();

	return 0;
}