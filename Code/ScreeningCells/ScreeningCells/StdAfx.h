
#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#pragma once


////////
#define  VC_EXTRALEAN //从Windows标题中排除很少使用的东西
#include  <afx.h>
#include  <afxwin.h> // MFC核心和标准组件
#include  <afxext.h> // MFC扩展
#include  <afxdtctl.h> // MFC支持Internet Explorer 4 Common Controls
#ifndef  _AFX_NO_AFXCMN_SUPPORT
#include  <afxcmn.h> // MFC支持Windows公共控件
#endif  // _AFX_NO_AFXCMN_SUPPORT
/////


//#define WIN32_LEAN_AND_MEAN	
#define _CRT_SECURE_NO_DEPRECA
#include <windows.h>
#include <objbase.h>
#include <zmouse.h>

#include "..\DuiLib\UIlib.h"

using namespace DuiLib;

#ifdef _DEBUG
#   ifdef _UNICODE
#       pragma comment(lib, "..\\lib\\DuiLib_d.lib")
#   else
#       pragma comment(lib, "..\\lib\\DuiLibA_d.lib")
#   endif
#else
#   ifdef _UNICODE
#       pragma comment(lib, "..\\lib\\DuiLib.lib")
#   else
#       pragma comment(lib, "..\\lib\\DuiLibA.lib")
#   endif
#endif

#include "Global.h"

#import "C:\Program Files (x86)\Common Files\System\ado\msado15.dll" no_namespace rename("EOF", "adoEOF") 


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
