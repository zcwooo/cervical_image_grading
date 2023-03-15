
// MicroScope.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CMicroScopeApp:
// See MicroScope.cpp for the implementation of this class
//

class CMicroScopeApp : public CWinAppEx
{
public:
	CMicroScopeApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CMicroScopeApp theApp;