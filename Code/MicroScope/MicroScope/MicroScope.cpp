
// MicroScope.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "MicroScope.h"
#include "MicroScopeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMicroScopeApp

BEGIN_MESSAGE_MAP(CMicroScopeApp, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMicroScopeApp construction

CMicroScopeApp::CMicroScopeApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CMicroScopeApp object

CMicroScopeApp theApp;


// CMicroScopeApp initialization

BOOL CMicroScopeApp::InitInstance()
{
	if( !DShowLib::InitLibrary() )
	{
		AfxMessageBox( TEXT("The IC Imaging Control Class Library could not be initialized.\n(invalid license key?)") );
		exit( 1 );
	}
	atexit(DShowLib::ExitLibrary );	//放在此处，造成m_cz的OLE异常

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

//98.7.2
//防止运行一个应用程序的多个实列.3处

	//Create a mutex object. If the mutex object already exists,then
	//this is the second instance of the application. Note that the
	//mutex handle is automatically closed when the process terminates.
//第一处
	::CreateMutex(NULL,TRUE,m_pszExeName);
	if(GetLastError()==ERROR_ALREADY_EXISTS)
	{
		//find our previous application's main window
		CWnd* pPreWnd=CWnd::GetDesktopWindow()->GetWindow(GW_CHILD);
		while(pPreWnd)
		{
			//does this window have the 'previous instance tag' set?
			if(::GetProp(pPreWnd->GetSafeHwnd(),m_pszExeName))
			{
				//Found window,now set focus to the window
				//First restore window if it is curently iconic
				if(pPreWnd->IsIconic())
					pPreWnd->ShowWindow(SW_RESTORE);
				//set focus to the main window
				pPreWnd->SetForegroundWindow();
				//if window has a pop-up window,set focus to pop-up.
				pPreWnd->GetLastActivePopup()->SetForegroundWindow();

				return FALSE;
			}
			//Did not find window,get next window in list
			pPreWnd=pPreWnd->GetWindow(GW_HWNDNEXT);
		}
		TRACE("Could not find previous main window!\n");
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CMicroScopeDlg dlg;
	m_pMainWnd = &dlg;

	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

//防止运行一个应用程序的多个实列.3处
//第二处,第三处在CmainFrm.cpp,删除 'tag'
	//associate a 'tag' with the window so we can locate it later.
	::SetProp(AfxGetApp()->m_pMainWnd->GetSafeHwnd(),m_pszExeName,(HANDLE)1);

	//DShowLib::ExitLibrary();
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
