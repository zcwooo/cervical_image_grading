#include "stdafx.h"
#include "resource.h"
#include "MainDlgWnd.h"
#include "SkinFrame.h"
#include "CasesMgrWnd.h"
#include "HistogramWnd.h"
#include "DiagramWnd.h"
#include "PatientInfoWnd.h"
#include "PopExitWnd.h"
#include "RegisterWnd.h"
#include "ModifyPwdWnd.h"
#include "HelpWnd.h"
#include "DatabaseMgr.h"
#include "const_calc_data.h"
#include "SettingPathWnd.h"

#include <Windows.h>
#include <tlhelp32.h>
#include <tchar.h>
#include <ShellAPI.h>

#include <string>
using namespace std;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include "libharu/include/hpdf.h"
#include <setjmp.h>
#pragma comment(lib, "libharu/lib/libhpdf.lib")
#pragma comment(lib, "libharu/lib/libhpdfs.lib")
//USES_CONVERSION;
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <atlimage.h>
#include <fstream>
#include <io.h>
#include <conio.h>
#include "opencv2\opencv.hpp"
#include "cv.h"
#include <direct.h>
#include <atlconv.h>               //CString 转string
#include "PopMenuSettingWnd.h"
#include "PopMenuInfoWnd.h"
#include "CasesMgrWnd.h"

#include <afxwin.h>
//////////////////////////////////////////////////////////////////////////
//selection

#include "stdafx.h"
#include "afxdialogex.h"
#include <atlimage.h>
#include <fstream>
#include <io.h>
#include <conio.h>
#include "opencv2\opencv.hpp"
#include "cv.h"
#include <direct.h>
#include <atlconv.h>               //CString 转string
#include "qImgLib2User/qImgLib.h"
#include "libharu/include/hpdf.h"
#include <unordered_map>

#pragma comment(lib, "qImgLib2User/qImgLib.lib")
#pragma comment(lib, "libharu/lib/libhpdf.lib")
#pragma comment(lib, "libharu/lib/libhpdfs.lib")
extern int nCountNUMArray[81];
using namespace std;
using namespace cv;

extern int user_admin;
HANDLE hdib = NULL;
//统计数据集中上皮细胞个数，以及所有上皮细胞灰度总和
int nCountCells = 0;
int nCountCellsAll = 0;
//总的细胞个数				
//double DI_nextuse[100000] = { 0.0 };
//jmp_buf env;
// long MeanCellsGrayValue = 0;
vector<string> vCellsfullfilenames;			//?

//int nCellsPixelArea[100000] = {0};
//double DI[100000] = {0.0}; 
//double dCircleDegree[100000]={0.0};             // 每个上皮的圆度（yiger添加）
//double dCellgrayvar[100000] = { 0.0 };
//int dGrayValue1[100000]={0};                            // 每个上皮的灰度值（yiger添加）
//int SortIndex[100000]       = {0};

int *nCellsPixelArea = new int[100000]();				//细胞的面积区域
double *DI = new double[100000]();						//?
double *dCircleDegree = new double[100000]();
double *dCellgrayvar = new double[100000]();
int *dGrayValue1 = new int[100000]();
int *dODVar = new int[100000]();		//? 一个全局一个局部
int *dGrayMax1 = new int[100000]();
int *dgrayvar = new int[100000]();
int *SortIndex = new int[100000]();		//?
double *dBackgroundGrayMeanValue1 = new double[100000]();	//未使用
double *dEpithelialCellIOD1 = new double[100000]();
double *dEpithelialCellIOD2 = new double[100000]();		//后面生成
double *dLymphocyteCellIOD1 = new double[100000]();
double *dAOD1 = new double[100000]();

#define DI_X	2.0

long long MeanLymphocyteGrayValue = 0;		//未使用
long long SumLymphocyteGrayValue = 0;		//未使用
int nCountLymphocyte = 0;					//淋巴细胞总数		
long long SumLymphocyteAreaValue = 0;		//未使用
long long MeanLymphocyteIOD = 0;			//未使用
long long dLymphocyteIODSumValue = 0;	


///////////////
float cAod = 0;


//////////////////////////////////////////////////////////////////////////
///
TCHAR g_lpBinPath[MAX_PATH];	//执行文件路径
TCHAR g_lpINIPath[MAX_PATH];
TCHAR g_lpLogPath[MAX_PATH];

CString m_FileDir;  // 读入图片的路径
int nImageNumAll;	//处理的细胞图片

static TCHAR gszFolder[MAX_PATH * 2];//弹出文件夹 文件路径初始化

INT CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData)
{
	LPITEMIDLIST tmp = (LPITEMIDLIST)lp;
	switch (uMsg)
	{
	case   BFFM_INITIALIZED:
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)gszFolder);
		break;

	case   BFFM_SELCHANGED:
		TCHAR szc[MAX_PATH] = { 0 };
		if (SHGetPathFromIDList((LPITEMIDLIST)tmp, szc))
		{
			SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)szc);
		}
		break;
	}
	return   0;
}

//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
bool bMicroScopeEx;

CMyCriticalSection              MyCriticalSection; 

extern double m_valueDoubleSetting;
extern int    m_valueIntSetting;
extern PatientInfoCur g_PatientInfoCur;
extern ModifyReportInfo g_ModifyReportInfo;
extern struct _g_record g_record_temp;

WORD g_FLOW;
bool g_bScreening;

DWORD WINAPI doDispose(LPVOID lpParam)
{
	CMainDlgWnd *pDlg = (CMainDlgWnd*)lpParam;

	g_FLOW = FLOW_SCREENING;
	g_bScreening = true;
	pDlg->m_bIsDisposeBegin = true;
	pDlg->m_bIsDisposeEnd = false;
	pDlg->m_bIsDisposeCalc = true;

	pDlg->DisposeProgressEx();

	return 0;
}

vector<string> g_vec_file_name;
calc_data* g_p_calc_data;
vector<string> g_vec_calc_image;
vector<vector<string>> g_vvec_calc_data;

#define thread_count				16		 //16
#define thread_item_count			1

HANDLE g_thread_handle[thread_count];
DWORD  g_thread_id[thread_count];
HANDLE g_thread_perform;
HANDLE g_thread_handle_wait;
HANDLE g_thread_handle_progress;
WORD   g_thread_count;
int    g_thread_filename_index[thread_count];
CTime  g_calc_time;

DWORD WINAPI doDisposeEx(LPVOID lpParam)
{
	CMainDlgWnd *pDlg = (CMainDlgWnd*)lpParam;
	int i = 0;
	while (i++ < 5)
	{
		Sleep(1000);
	}

	return 0;
}
DWORD WINAPI doDisposeExWait(LPVOID lpParam)
{
	CMainDlgWnd *pDlg = (CMainDlgWnd*)lpParam;

	TCHAR lpPath[MAX_PATH];
	GetModuleFileName(NULL, lpPath, sizeof(lpPath)-1);
	PathRemoveFileSpec(lpPath);
	PathRemoveBackslash(lpPath);
	PathAddBackslash(lpPath);
	CStringA sBin(lpPath);
	CStringA sBinTemp(sBin + "Temp\\");
	if (_access(sBinTemp.GetString(), 0) != 0)
	{
		_mkdir(sBinTemp.GetString());
	}
	CStringA sp;

	CStringA sBin1(sBin + "Temp\\1\\");
	if (_access(sBin1.GetString(), 0) != 0)
	{
		_mkdir(sBin1.GetString());
	}
	CStringA sBin2(sBin + "Temp\\2\\");
	if (_access(sBin2.GetString(), 0) != 0)
	{
		_mkdir(sBin2.GetString());
	}
	CStringA sBin3(sBin + "Temp\\3\\");
	if (_access(sBin3.GetString(), 0) != 0)
	{
		_mkdir(sBin3.GetString());
	}
	CStringA sBin4(sBin + "Temp\\4\\");
	if (_access(sBin4.GetString(), 0) != 0)
	{
		_mkdir(sBin4.GetString());
	}
	CStringA sBin5(sBin + "Temp\\5\\");
	if (_access(sBin5.GetString(), 0) != 0)
	{
		_mkdir(sBin5.GetString());
	}
	for (int i = 0; i < g_thread_count; ++i)
	{
		sp.Format("%s%d\\", sBin1.GetString(), i);
		if (_access(sp.GetString(), 0) != 0)
		{
			_mkdir(sp.GetString());
		}

		sp.Format("%s%d\\", sBin2.GetString(), i);
		if (_access(sp.GetString(), 0) != 0)
		{
			_mkdir(sp.GetString());
		}

		sp.Format("%s%d\\", sBin3.GetString(), i);
		if (_access(sp.GetString(), 0) != 0)
		{
			_mkdir(sp.GetString());
		}

		sp.Format("%s%d\\", sBin4.GetString(), i);
		if (_access(sp.GetString(), 0) != 0)
		{
			_mkdir(sp.GetString());
		}

		sp.Format("%s%d\\", sBin5.GetString(), i);
		if (_access(sp.GetString(), 0) != 0)
		{
			_mkdir(sp.GetString());
		}
	}

	TCHAR name[] = _T("CalcClient.exe");
	CString sExe;
	sExe.Format(_T("%sCalcClient.exe"), pDlg->GetExePath());
	CStringA sExeA;
	sExeA = sExe;

	STARTUPINFO silA;
	PROCESS_INFORMATION pilA;
	ZeroMemory(&silA, sizeof(STARTUPINFO));
	ZeroMemory(&pilA, sizeof(PROCESS_INFORMATION));
	// 	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	// 	int iprio = GetPriorityClass(GetCurrentProcess());
	for (int i = 0; i < g_thread_count; ++i)
	{
		//ShellExecute(NULL, _T("open"), name, NULL, sExe.GetString(), SW_SHOWNORMAL);
		// 创建进程
		CreateProcess(NULL, (LPWSTR)sExe.GetString(), NULL, NULL, FALSE, 0, NULL, NULL, &silA, &pilA);
		// 设置进程优先级
		SetPriorityClass(pilA.hProcess, REALTIME_PRIORITY_CLASS);
		// 设置进程优先调整
		//SetProcessPriorityBoost(pilA.hProcess, true);

		// 		char buf[256] = { 0 }; 
		// 		sprintf_s(buf, "D:\\work1\\package\\PSTools\\psexec -s -i D:\\work\\ScreeningCells\\bin\\CalcClient.exe");
		// 		WinExec(buf, SW_NORMAL);
	}

	return 0;
}

unsigned WINAPI doDisposeExProgress(void* lpParam)
{
	CMainDlgWnd *pDlg = (CMainDlgWnd*)lpParam;

	while (true)
	{
		Sleep(1000);

		CStringA sa;
		string sPathFile, sPath, sDataFileName;
		sa = m_FileDir;
		sPathFile = sa.GetString();
		int nLocation = sPathFile.rfind("\\");
		sPath = sPathFile.substr(0, nLocation) + "\\";
		sDataFileName = sPathFile.substr(nLocation + 1, sPathFile.length());
		string StrTemp0 = sPath + sDataFileName + "Result";
		if (PathFileExistsA(StrTemp0.c_str()))
		{
			bool b=false;

			vector<string> vstr;

			CStringA stempa;
			stempa = m_FileDir;
			string stemp(stempa.GetString());
			string sPathFile;
			sPathFile = StrTemp0 + "\\Cells\\*.bmp";
			struct _finddata_t fileinfo;
			long fHandle;
			if ((fHandle = _findfirst(sPathFile.c_str(), &fileinfo)) == -1L)
			{
			}
			else
			{
				do
				{
					vstr.push_back(StrTemp0 + "\\Cells\\" + fileinfo.name);
					if (vstr.size() >= 100)
					{
						g_vec_calc_image = vstr;
						b = true;
						break;
					}
				} while (_findnext(fHandle, &fileinfo) == 0);
			}
			if (b)
			{
				break;
			}
		}
	}

	int len = strlen(__p_calc_data) + 1;
	char* pTemp = new char[len];
	memset(pTemp, 0, len);
	strncpy_s(pTemp, len, __p_calc_data, len);

	vector<string> vstr;
	pDlg->SplitString(vstr, pTemp, "]");
	if (vstr.size()>0)
	{
		for (int i = 0; i < vstr.size(); ++i)
		{
			int len1 = vstr[i].length() + 1;
			char* pTemp1 = new char[len1];
			memset(pTemp1, 0, len1);
			strncpy_s(pTemp1, len1, vstr[i].c_str(), len1);
			char* pdelete = pTemp1;
			pTemp1++;

			vector<string> vv;
			pDlg->SplitString(vv, pTemp1, "|");
			g_vvec_calc_data.push_back(vv);

			delete[] pdelete;
		}
	}

	delete[] pTemp;

	int img_cnt = g_vec_calc_image.size(), data_cnt = g_vvec_calc_data.size();
	srand((unsigned)time(NULL));
	int random_img[500] = { 0 }, random_data[500] = { 0 };
	if (img_cnt>500)
	{
		int adfsdfsd = -1;
	}
	for (int i = 0; i < img_cnt; ++i)
	{
		random_img[i] = rand() % img_cnt;
	}
	for (int i = 0; i < data_cnt; ++i)
	{
		random_data[i] = rand() % data_cnt;
	}

	int w_img = 0, w_data = 0;

	while (true)
	{
		if (!pDlg->IsDisposeCalc())
			break;

		w_img++;
		w_img = w_img % img_cnt;
		w_data++;
		w_data = w_data % data_cnt;

		for (int i = 0; i < 6; ++i)
		{
			int index1 = (w_img + i) % img_cnt, index2 = (w_data + i) % data_cnt;
			int a1 = random_img[index1];
			int a2 = random_data[index2];
			string s1, s2;
			s1 = g_vec_calc_image[a1];
			//pDlg->ShowOneCellSelectionImg(i, s1.c_str());
			::PostMessage(pDlg->GetHWND(), WM_USER + 700, i, a1);
			if (data_cnt>0)
			{
				int item_cnt = g_vvec_calc_data[0].size();
				CString str[13];
				for (int j = 0; j < item_cnt; ++j)
				{
					str[j] = g_vvec_calc_data[a2][j].c_str();
				}
				//pDlg->ShowOneCellSelectionData(i, str);
				::PostMessage(pDlg->GetHWND(), WM_USER + 701, i, a2);
			}
		}
		
		Sleep(1000);
	}

	return 0;
}


//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

//#include "atlconv.h"
int nCountNUMArray[81] = { 0 };

void InitPath(TCHAR * lpBin, TCHAR  *lpINI, TCHAR  *lpLog)
{
	TCHAR lpPath[MAX_PATH];
	GetModuleFileName(NULL, lpPath, sizeof(lpPath)-1);
	PathRemoveFileSpec(lpPath);
	PathRemoveBackslash(lpPath);
	PathAddBackslash(lpPath);

	if (lpBin != NULL)
		//sprintf(lpBin, "%s", lpPath);
		swprintf(lpBin, 200, _T("%s"), lpPath);
	if (lpINI != NULL)
		swprintf(lpINI, 200, _T("%s%s"), lpPath, "pst.ini");
	if (lpLog != NULL)
		swprintf(lpLog, 200, _T("%s"), lpPath);
}

vector<string> vCellsfullfilenames_nextuse;
double DI_nextuse[100000] = { 0.0 };
extern PatiengInfo g_PatiengInfo;

void TcharToChar(const TCHAR * tchar, char * _char)
{
	int iLength;
	//获取字节长度   
	iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);
	//将tchar值赋给_char  
	WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);
}
void CharToTchar(const char * _char, TCHAR * tchar)
{
	int iLength;

	iLength = MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, tchar, iLength);
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

DUI_BEGIN_MESSAGE_MAP(CMainPage, CNotifyPump)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_SELECTCHANGED, OnSelectChanged)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMCLICK, OnItemClick)
DUI_END_MESSAGE_MAP()

CMainPage::CMainPage()
{
	m_pPaintManager = NULL;
}

void CMainPage::SetPaintMagager(CPaintManagerUI* pPaintMgr)
{
	m_pPaintManager = pPaintMgr;
}

void CMainPage::OnClick(TNotifyUI& msg)
{

}

void CMainPage::OnSelectChanged(TNotifyUI &msg)
{
	int i = -1;
}

void CMainPage::OnItemClick(TNotifyUI &msg)
{
	int i = -1;
}

//////////////////////////////////////////////////////////////////////////
///
DUI_BEGIN_MESSAGE_MAP(CMainDlgWnd, WindowImplBase)
DUI_END_MESSAGE_MAP()

CMainDlgWnd::CMainDlgWnd()
{
	m_pMenu = NULL;
	m_pMenuFile = NULL;
	m_pMenuSetting = NULL;
	m_pMenuInfo = NULL;
	m_pMenuReport = NULL;
	m_pMenuScreening = NULL;
	m_pMenuHelp = NULL;

	m_MainPage.SetPaintMagager(&m_pm);
	AddVirtualWnd(_T("mainpage"), &m_MainPage);

	memset(m_lpResultPath, 0, sizeof(m_lpResultPath));

	CString path = GetExePath();
	_tcscpy(g_lpBinPath, path.GetString());

	g_FLOW = FLOW_INVALID;
	g_bScreening = false;

	m_iValidIndex = -1;

	g_p_calc_data = NULL;

	ZeroMemory(gszFolder, sizeof(gszFolder));

	bMicroScopeEx = true;

	int i = -1;
}

CMainDlgWnd::~CMainDlgWnd()
{
	CMenuWnd::DestroyMenu();
	if (m_pMenu != NULL) {
		delete m_pMenu;
		m_pMenu = NULL;
	}
	if (m_pMenuFile != NULL)
	{
		delete m_pMenuFile;
		m_pMenuFile = NULL;
	}
	if (m_pMenuSetting != NULL)
	{
		delete m_pMenuSetting;
		m_pMenuSetting = NULL;
	}
	if (m_pMenuInfo != NULL)
	{
		delete m_pMenuInfo;
		m_pMenuInfo = NULL;
	}
	if (m_pMenuReport != NULL)
	{
		delete m_pMenuReport;
		m_pMenuReport = NULL;
	}
	if (m_pMenuScreening != NULL)
	{
		delete m_pMenuScreening;
		m_pMenuScreening = NULL;
	}
	if (m_pMenuHelp != NULL)
	{
		delete m_pMenuHelp;
		m_pMenuHelp = NULL;
	}
	RemoveVirtualWnd(_T("mainpage"));
	if (g_p_calc_data != NULL)
	{
		for (int i = 0; i < g_thread_count; ++i)
		{
			g_p_calc_data[i].deldata();
		}
		delete[] g_p_calc_data;
		g_p_calc_data = NULL;
	}

#define SafeDel(obj) delete obj; obj=NULL; 

	SafeDel(nCellsPixelArea);
	SafeDel(DI);
	SafeDel(dCircleDegree);
	SafeDel(dCellgrayvar);
	SafeDel(dGrayValue1);
	SafeDel(dODVar);
	SafeDel(dGrayMax1);
	SafeDel(dgrayvar);
	SafeDel(SortIndex);
	SafeDel(dBackgroundGrayMeanValue1);
	SafeDel(dEpithelialCellIOD1);
	SafeDel(dEpithelialCellIOD2);
	SafeDel(dLymphocyteCellIOD1);
	SafeDel(dAOD1);

	delete[] m_pBlockPage;
	m_pBlockPage = NULL;
}

CControlUI* CMainDlgWnd::CreateControl(LPCTSTR pstrClass)
{
	if (lstrcmpi(pstrClass, _T("CircleProgress")) == 0) {
		return new CCircleProgressUI();
	}
	else if (lstrcmpi(pstrClass, _T("tab1")) == 0)
	{
		CDialogBuilder builder;
		CControlUI* pUI = builder.Create(_T("xmls/tab1.xml"));
		m_pTab1 = pUI;
		return pUI;
	}
	else if (lstrcmpi(pstrClass, _T("tab2")) == 0)
	{
		CDialogBuilder builder;
		CControlUI* pUI = builder.Create(_T("xmls/tab2.xml"));
		return pUI;
	}
	else if (lstrcmpi(pstrClass, _T("tab3")) == 0)
	{
		CDialogBuilder builder;
		CControlUI* pUI = builder.Create(_T("xmls/tab3.xml"));
		return pUI;
	}
	else if (lstrcmpi(pstrClass, _T("diagram")) == 0)
	{
		return new CDiagramUI();
	}
	else if (lstrcmpi(pstrClass, _T("VerticalLayoutEx")) == 0)
	{
		return new CVerticalLayoutUIEX();
	}
	else if (lstrcmpi(pstrClass, _T("CLabelUIEx")) == 0)
	{
		return new CLabelUIEx();
	}
 	else if (lstrcmpi(pstrClass, _T("CEditUIEx1")) == 0) {
 		CEditUIEx1 * pEditEx = new CEditUIEx1();
 		pEditEx->SetParentEx(this);
 		return pEditEx;
 	}
	return NULL;
}

void CMainDlgWnd::InitWindow()
{
	//SetWindowLong(m_hWnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW);

	SetIcon(IDR_MAINFRAME1);
	// 多语言接口
	CResourceManager::GetInstance()->SetTextQueryInterface(this);
	CResourceManager::GetInstance()->LoadLanguage(_T("lan_cn.xml"));
	// 皮肤接口
	CSkinManager::GetSkinManager()->AddReceiver(this);

	m_pCloseBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("closebtn")));
	m_pMaxBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("maxbtn")));
	m_pRestoreBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("restorebtn")));
	m_pMinBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("minbtn")));
	m_pSkinBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("skinbtn")));

	m_pTabLeft = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("switch_left")));
	m_pTabRight = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("switch_right")));

	// 注册托盘图标
	m_trayIcon.CreateTrayIcon(m_hWnd, IDR_MAINFRAME1, _T("医之云"));

	m_pOptionTabUI[0] = static_cast<COptionUI*>(m_pm.FindControl(_T("tab1")));
	m_pOptionTabUI[1] = static_cast<COptionUI*>(m_pm.FindControl(_T("tab2")));
	m_pOptionTabUI[2] = static_cast<COptionUI*>(m_pm.FindControl(_T("tab3")));

	COptionUI* pBtn1 = static_cast<COptionUI*>(m_pm.FindControl(_T("btn1")));
	//pBtn1->SetFocus();

	//path
	m_pLabelArchivePath = static_cast<CLabelUIEx*>(m_pm.FindControl(_T("archivepath")));
	TCHAR lpInfo[MAX_PATH] = { 0 };
	TCHAR lpBin[MAX_PATH];
	InitPath(lpBin, NULL, NULL);
	TCHAR lpINI[128];
	wsprintf(lpINI, _T("%s%s"), lpBin, _T("publish.ini"));
	GetPrivateProfileString(_T("archipath"), _T("path"), _T(""), lpInfo, sizeof(lpInfo)-1, lpINI);
	CString sArchi(lpInfo);
	if (PathFileExists(sArchi.GetString()))
	{
		if (sArchi.GetAt(sArchi.GetLength() - 1) == '\\' || sArchi.GetAt(sArchi.GetLength() - 1) == '/')
			sArchi = sArchi.Mid(0, sArchi.GetLength() - 1);
		m_pLabelArchivePath->SetText(sArchi.GetString());
		m_FileDir = sArchi;
		_tcscpy(gszFolder, sArchi.GetString());
	}
	m_pLabelArchivePath->SetFocus();

	m_pEditCurName = static_cast<CEditUI*>(m_pm.FindControl(_T("editcurname")));
	m_pEditCurBLH = static_cast<CEditUI*>(m_pm.FindControl(_T("editcurblh")));

	//selection
#define UPVALUE  (-23)
	bool bAdmin = IsAdmin(false);
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 13; j++)
		{
			CString str;
			str.Format(_T("edit_%d_%d"), j+3, i+1);
			m_pSelectEdit[i][j] = static_cast<CEditUI*>(m_pm.FindControl(str.GetString()));
			m_pSelectEdit[i][j]->SetFont(10);

			if (!bAdmin)
				m_pSelectEdit[i][j]->SetVisible(false);

			SIZE sz = m_pSelectEdit[i][j]->GetFixedXY();
			sz.cy += UPVALUE;
			m_pSelectEdit[i][j]->SetFixedXY(sz);

			if (!bAdmin)
				m_pSelectEdit[i][j]->SetVisible(false);
		}
		CString str;
		str.Format(_T("control_%d"), i+1);
		m_pSelectImg[i] = static_cast<CControlUI*>(m_pm.FindControl(str.GetString()));

		if (!bAdmin)
			m_pSelectImg[i]->SetVisible(false);
	}
	m_pProgress = static_cast<CLabelUI*>(m_pm.FindControl(_T("label_progress")));
	m_pProgressPercent = static_cast<CLabelUI*>(m_pm.FindControl(_T("label_percent")));
	m_pProgressBar = static_cast<CProgressUI*>(m_pm.FindControl(_T("progress_calc")));
	m_pGifAnim = static_cast<CGifAnimUI*>(m_pm.FindControl(_T("ani_clock1")));
	m_pGifAnim->SetVisible(false);
	m_pGifAnim1 = static_cast<CGifAnimUI*>(m_pm.FindControl(_T("ani_clock2")));
	m_pGifAnim1->SetVisible(false);
	m_pAniProgress = static_cast<CControlUI*>(m_pm.FindControl(_T("ani_progress")));
	m_pAniProgress->SetVisible(false);
	if (!bAdmin)
	{
		m_pGifAnim->SetVisible(true);
//		m_pGifAnim1->SetVisible(true);
		m_pAniProgress->SetVisible(true);
	}
// 	m_pclock_begin_end = static_cast<CControlUI*>(m_pm.FindControl(_T("clock_begin_end")));
// 	if (IsAdmin(false))
// 	{
// 		m_pclock_begin_end->SetVisible(false);
// 	}
	m_pProgressBar->SetValue(0);

	if (!bAdmin)
	{
		for (int i = 2; i < 16; ++i)
		{
			CString str;
			str.Format(_T("label_%d"), i);
			CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(str.GetString()));
			pLabel->SetVisible(false);
		}
	}

	for (int i = 0; i < 13;  ++i)
	{
		CString str;
		str.Format(_T("label_%d"), i + 3);
		CLabelUI* pLabel = static_cast<CLabelUI*>(m_pm.FindControl(str.GetString()));
		SIZE sz = pLabel->GetFixedXY();
		sz.cy += UPVALUE;
		pLabel->SetFixedXY(sz);
		pLabel->SetFont(11);
	}
	CButtonUI* pBtnUI = static_cast<CButtonUI*>(m_pm.FindControl(_T("tab2btn1")));
	SIZE sz = pBtnUI->GetFixedXY();
	sz.cy += UPVALUE+6;
	pBtnUI->SetFixedXY(sz);
	CLabelUI* pLabelProgress = static_cast<CLabelUI*>(m_pm.FindControl(_T("label_progress")));
	sz = pLabelProgress->GetFixedXY();
	sz.cy += UPVALUE+6;
	pLabelProgress->SetFixedXY(sz);

	m_bIsDisposeBegin = false;
	m_bIsDisposeEnd = false;
	m_bIsDisposeCalc = false;

	m_bSelFirstCell = false;

	//statistics
	m_pPageTab3 = static_cast<CControlUI*>(m_pm.FindControl(_T("pagetab3")));
// 	for (int i = 0; i < 32; ++i)
// 	{
// 		CString str;
// 		str.Format(_T("tab3_image%d"), i + 1);
// 		m_pStatisticsImg[i] = static_cast<CControlUI*>(m_pm.FindControl(str.GetString()));
// 		str.Format(_T("tab3_edit%d"), i + 1);
// 		m_pStatisticsEdit[i] = static_cast<CEditUI*>(m_pm.FindControl(str.GetString()));
// 		str.Format(_T("tab3_btndot%d"), i + 1);
// 		m_pStatisticsBtnShow[i] = static_cast<CButtonUI*>(m_pm.FindControl(str.GetString()));
// 		str.Format(_T("tab3_btnclose%d"), i + 1);
// 		m_pStatisticsBtnDelete[i] = static_cast<CButtonUI*>(m_pm.FindControl(str.GetString()));
// 	}
	m_pBtnLastPage = static_cast<CButtonUI*>(m_pm.FindControl(_T("lastpage")));
	m_pBtnNextPage = static_cast<CButtonUI*>(m_pm.FindControl(_T("text_page")));
	m_pLabelPage = static_cast<CLabelUI*>(m_pm.FindControl(_T("text_page")));
	m_pLabelSelCellCount = static_cast<CLabelUI*>(m_pm.FindControl(_T("text_sel_cell")));
	m_pLabelSelCellCount->SetText(_T("已选细胞个数为:0"));
	m_pLabelPageSkip = static_cast<CEditUIEx1*>(m_pm.FindControl(_T("skip_page")));
	m_pBtnPageSkip = static_cast<CButtonUI*>(m_pm.FindControl(_T("btnskip")));

	m_wCurShowPageCount = _cnt_show_page;
	m_wSelPage = 0;

	m_pBlockPage = new BlockPage[_page];
	memset(m_pBlockPage, 0, sizeof(BlockPage*)*_page);
	m_pTab3Page = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("page_vertical")));
	for (int i = 0; i < _row_cnt; ++i)
	{
		TCHAR buf[52] = { 0 };
		_stprintf(buf, _T("tab3_page%d"), i + 1);
		m_pTab3Hori[i] = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(buf));
	}
	int h = m_pTab3Page->GetFixedHeight();
	int w = m_pTab3Page->GetFixedWidth();
	for (int i = 0; i < _row_cnt; ++i)
	{
		for (int j = 0; j < _column_cnt; ++j)
		{
			TCHAR buf[64] = { 0 };

			int w1 = w / _column_cnt;
			CVerticalLayoutUI* pUI = new CVerticalLayoutUI;
			pUI->SetFixedWidth(w1);
			pUI->SetFixedHeight(h);
			RECT r; r.left = 0; r.top = 0; r.right = 1; r.bottom = 1;
			if (i == 0)
				r.top = 1;
			if (j == 0)
				r.left = 1;
			pUI->SetBorderSize(r);
			pUI->SetBorderColor(GetColorDword(_T("ffcccccc")));
			pUI->SetChildPadding(2);
			//RECT rc; rc.left = 10; rc.top = 0; rc.right = 10; rc.bottom = 5;
			//pUI->SetInset(rc);

			CControlUI * pAdjust = new CControlUI;
// 			pAdjust->SetBorderSize(1);
// 			pAdjust->SetBorderColor(GetColorDword(_T("ff0000ff")));
			pUI->Add(pAdjust);

#define img_w 60
#define img_h 60
			CHorizontalLayoutUI* pUI1 = new CHorizontalLayoutUI;
			pUI1->SetFixedHeight(img_h);
			pUI1->SetChildPadding(4);
			pUI->Add(pUI1);
			CControlUI* pT = new CControlUI;
			pT->SetFixedWidth(20);
			pUI1->Add(pT);
			CControlUI* pControl = new CControlUI;
			pControl->SetFixedHeight(img_h);
			pControl->SetFixedWidth(img_w);
			pControl->SetBorderSize(1);
			pControl->SetBorderColor(GetColorDword(_T("ffcccccc")));
			_stprintf(buf, _T("btnimage_%d_%d"), i, j);
			pControl->SetName(buf);
			//pControl->SetBkImage(_T("E:\\work11\\TestResult\\Cells\\000-013_65.bmp"));
			m_pBlockPage[0].image[i][j] = pControl;
			int xCur = (w1 - img_w) / 2;
			int yCur = 0;
			pUI1->Add(pControl);
			CButtonUI* pButton = new CButtonUI;
			pButton->SetBorderSize(1);
			//pButton->SetBorderColor(GetColorDword(_T("ffff0000")));
			pButton->SetFloat(true);
			pButton->SetAttribute(_T("float"), _T("true"));
			pButton->SetAttribute(_T("pos"), _T("24,0,84,60"));
			_stprintf(buf, _T("btnimage1_%d_%d"), i, j);
			pButton->SetName(buf);
			m_pBlockPage[0].btnsel[i][j] = pButton;
			pUI1->Add(pButton);
			CVerticalLayoutUI* pVer = new CVerticalLayoutUI;
			CControlUI* ptem = new CControlUI;
			ptem->SetFixedHeight(36);
			pVer->Add(ptem);
			CButtonUI* pButton1 = new CButtonUI;
			pButton1->SetFixedWidth(30);
			pButton1->SetFixedHeight(24);
			pButton1->SetAttribute(_T("normalimage"), _T("file='dlgmain\\tab3Dot.png' dest='0,0,30,24' source='0,0,30,24'"));
			pButton1->SetAttribute(_T("hotimage"), _T("file='dlgmain\\tab3Dot.png' dest='0,0,30,24' source='30,0,60,24'"));
			pButton1->SetAttribute(_T("pushedimage"), _T("file='dlgmain\\tab3Dot.png' dest='0,0,30,24' source='60,0,90,24'"));
			_stprintf(buf, _T("btnshowsel_%d_%d"), i, j);
			pButton1->SetName(buf);
			m_pBlockPage[0].btndot[i][j] = pButton1;
			pVer->Add(pButton1);
			pUI1->Add(pVer);
			
			CHorizontalLayoutUI* pUI2 = new CHorizontalLayoutUI;
			pUI2->SetFixedHeight(18);
			CControlUI* pT1 = new CControlUI;
			pT1->SetFixedWidth(10);
			pUI2->Add(pT1);
			CEditUI* pEdit = new CEditUI;
			pEdit->SetFixedWidth(110);
			pEdit->SetFixedHeight(18);
			pEdit->SetBkImage(_T("login/edit.png"));
			pEdit->SetReadOnly(true);
			pEdit->SetAttribute(_T("align"), _T("center"));
			//pEdit->SetAttribute(_T("textpadding"), _T("8,0,0,0"));
			//pEdit->SetText(_T("3.056458"));
			_stprintf(buf, _T("edititem1_%d_%d"), i, j);
			pEdit->SetName(buf);
			pEdit->SetFont(10);
			m_pBlockPage[0].edit[i][j] = pEdit;
			pUI2->Add(pEdit);
			pUI->Add(pUI2);

			CControlUI * pAdjust1 = new CControlUI;
// 			pAdjust1->SetBorderSize(1);
// 			pAdjust1->SetBorderColor(GetColorDword(_T("ff0000ff")));
			pUI->Add(pAdjust1);

			m_pTab3Hori[i]->AddAt(pUI, j);
		}
	}

	_iBingLiSel = -1;

	_iMenuSel = -1;

	g_vec_file_name.swap(vector<string>());

	SetTimer(m_hWnd, 20413, 100, NULL);

	//CDatabaseMgr* pMgr = CDatabaseMgr::GetInstance();
	//_tcscpy_s(g_PatiengInfo.lpBLId, sizeof(g_PatiengInfo.lpBLId), _T("bl123456"));
	//g_record_temp.reset();
	//g_record_temp.setdata(_T("BL_ID"), _T("bl123456"), -1);
	//g_record_temp.setdata(_T("pdf_img2"), _T("pdf_img2"), -1);
	//g_record_temp.setdata(_T("patient_name"), _T("张安地"), -1);
	//pMgr->SaveDataInfo(g_record_temp);

	//_g_record temp;
	//pMgr->FindInfoByID(72, temp);
	//CString record[33];
	//temp.convertdata_string(temp, record);

	//Mat mat;
	//mat = imread("E:\\work11\\被删细胞\\3.png", 0);
	//bool b = IsInvalidCell(mat);
	int i = -1;
}

BOOL CMainDlgWnd::Receive(SkinChangedParam param)
{
	CControlUI* pRoot = m_pm.FindControl(_T("root"));
	if (pRoot != NULL) {
		if (param.bColor) {
			pRoot->SetBkColor(param.bkcolor);
			pRoot->SetBkImage(_T(""));
		}
		else {
			pRoot->SetBkColor(0);
			pRoot->SetBkImage(param.bgimage);
			//m_pm.SetLayeredImage(param.bgimage);
		}
	}
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////

HRESULT STDMETHODCALLTYPE CMainDlgWnd::UpdateUI(void)
{
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CMainDlgWnd::GetHostInfo(CWebBrowserUI* pWeb,
	/* [out][in] */ DOCHOSTUIINFO __RPC_FAR *pInfo)
{
	if (pInfo != NULL) {
		pInfo->dwFlags |= DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_NO3DOUTERBORDER;
	}
	return S_OK;
}
HRESULT STDMETHODCALLTYPE CMainDlgWnd::ShowContextMenu(CWebBrowserUI* pWeb,
	/* [in] */ DWORD dwID,
	/* [in] */ POINT __RPC_FAR *ppt,
	/* [in] */ IUnknown __RPC_FAR *pcmdtReserved,
	/* [in] */ IDispatch __RPC_FAR *pdispReserved)
{
	return E_NOTIMPL;
	//返回 E_NOTIMPL 正常弹出系统右键菜单
	//返回S_OK 则可屏蔽系统右键菜单
}

DuiLib::CDuiString CMainDlgWnd::GetSkinFile()
{
	return _T("XML_MAIN");
}

LPCTSTR CMainDlgWnd::GetWindowClassName() const
{
	return _T("MainWndScreening");
}

UINT CMainDlgWnd::GetClassStyle() const
{
	return CS_DBLCLKS;
}

void CMainDlgWnd::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
}

LPCTSTR CMainDlgWnd::QueryControlText(LPCTSTR lpstrId, LPCTSTR lpstrType)
{
	return NULL;
}

void CMainDlgWnd::Notify(TNotifyUI& msg)
{
	CDuiString name = msg.pSender->GetName();
	if (msg.sType == _T("windowinit")) {
	}
	else if (msg.sType == _T("alt"))
	{
		SetFocus(m_hWnd);
 		ClearMenu();
		SetFocus(m_hWnd);
	}
	else if (msg.sType == _T("colorchanged"))
	{
		CControlUI* pRoot = m_pm.FindControl(_T("root"));
		if (pRoot != NULL) {
			CColorPaletteUI* pColorPalette = (CColorPaletteUI*)m_pm.FindControl(_T("Pallet"));
			pRoot->SetBkColor(pColorPalette->GetSelectColor());
			pRoot->SetBkImage(_T(""));
		}
	}
	else if (msg.sType == DUI_MSGTYPE_ITEMACTIVATE) {
		if (MSGID_OK == CMsgWnd::MessageBox(m_hWnd, _T("退出"), _T("确定退出？")))
		{
			::DestroyWindow(m_hWnd);
		}
	}
	else if (msg.sType == _T("showactivex"))
	{
		if (name.CompareNoCase(_T("ani_flash")) == 0) {
			IShockwaveFlash* pFlash = NULL;
			CActiveXUI* pActiveX = static_cast<CActiveXUI*>(msg.pSender);
			pActiveX->GetControl(__uuidof(IShockwaveFlash), (void**)&pFlash);
			if (pFlash != NULL)  {
				pFlash->put_WMode(_bstr_t(_T("Transparent")));
				pFlash->put_Movie(_bstr_t(CPaintManagerUI::GetInstancePath() + _T("\\skin\\ScreeningCells\\other\\waterdrop.swf")));
				pFlash->DisableLocalSecurity();
				pFlash->put_AllowScriptAccess(L"always");
				BSTR response;
				pFlash->CallFunction(L"<invoke name=\"setButtonText\" returntype=\"xml\"><arguments><string>Click me!</string></arguments></invoke>", &response);
				pFlash->Release();
			}
		}
	}
	else if (msg.sType == _T("click"))
	{
		if (name.CompareNoCase(_T("tab3_image1"))==0 )
		{
			m_pStatisticsImg[0]->SetBorderSize(1);
			m_pStatisticsImg[0]->SetBorderColor(GetColorDword(_T("FFFF0000")));
			int i = -1;
		}
		if (name.CompareNoCase(_T("closebtn")) == 0)
		{
			//if (MSGID_OK == CMsgWnd::MessageBox(m_hWnd, _T("退出"), _T("确定退出？")))
			if (MSGID_OK == CPopExitWnd::MessageBox(m_hWnd))
			{
				::DestroyWindow(m_hWnd);
			}
			return;
		}
		else if (msg.pSender == m_pMinBtn) {
			SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0); return;
		}
		else if (msg.pSender == m_pMaxBtn) {
			CDuiRect rcWorkArea;
			//// 获取工作区的大小
			//SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkArea, 0);
			//MoveWindow(m_hWnd, rcWorkArea.left, rcWorkArea.top, rcWorkArea.right, rcWorkArea.bottom, true);
			SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0); return;
		}
		else if (msg.pSender == m_pRestoreBtn) {
			SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0); return;
		}
		else if (msg.pSender == m_pSkinBtn) {
			new CSkinFrame(m_hWnd, m_pSkinBtn);
		}
		// 按钮消息
		OnLClick(msg.pSender);
	}

	else if (msg.sType == _T("selectchanged"))
	{
		if (name.CompareNoCase(_T("tab1")) == 0)
			m_pTabRight->SelectItem(0);
		else if (name.CompareNoCase(_T("tab2")) == 0)
			m_pTabRight->SelectItem(1);
		else if (name.CompareNoCase(_T("tab3")) == 0)
			m_pTabRight->SelectItem(2);
		if (name.CompareNoCase(_T("btn1")) == 0)
			m_pTabLeft->SelectItem(0);
		else if (name.CompareNoCase(_T("btn2")) == 0)
			m_pTabLeft->SelectItem(1);
		else if (name.CompareNoCase(_T("btn3")) == 0)
			m_pTabLeft->SelectItem(2);
		else if (name.CompareNoCase(_T("btn4")) == 0)
			m_pTabLeft->SelectItem(3);
		else if (name.CompareNoCase(_T("btn5")) == 0)
			m_pTabLeft->SelectItem(4);
		else if (name.CompareNoCase(_T("btn6")) == 0)
			m_pTabLeft->SelectItem(5);
	}
	else if (msg.sType == _T("valuechanged"))
	{
		CProgressUI* pSlider = static_cast<CProgressUI*>(m_pm.FindControl(_T("slider")));
		CProgressUI* pPro1 = static_cast<CProgressUI*>(m_pm.FindControl(_T("progress")));
		CProgressUI* pPro2 = static_cast<CProgressUI*>(m_pm.FindControl(_T("circle_progress")));
		pPro1->SetValue(pSlider->GetValue());
		pPro2->SetValue(pSlider->GetValue());
	}
	else if (msg.sType == _T("predropdown") && name == _T("font_size"))
	{
		CComboUI* pFontSize = static_cast<CComboUI*>(m_pm.FindControl(_T("font_size")));
		if (pFontSize)
		{
			pFontSize->RemoveAll();
			for (int i = 0; i < 10; i++) {
				CListLabelElementUI * pElement = new CListLabelElementUI();
				pElement->SetText(_T("测试长文字"));
				pElement->SetFixedHeight(30);
				pFontSize->Add(pElement);
			}
			pFontSize->SelectItem(0);
		}
	}

	return WindowImplBase::Notify(msg);
}
void CMainDlgWnd::OnLClick(CControlUI *pControl)
{
	CDuiString sName = pControl->GetName();

	CString sDest(sName.GetData());
	CString s1(_T("btnimage1_")), s2(_T("btnshowsel_"));
	int ibtncellitem1 = -1, ibtncellitem2 = -1;
	ibtncellitem1 = sDest.Find(s1);
	ibtncellitem2 = sDest.Find(s2);
	
	if (sName.CompareNoCase(_T("btnfile")) == 0 ||
		sName.CompareNoCase(_T("btnsetting")) == 0 ||
		sName.CompareNoCase(_T("btninfo")) == 0 || //
		sName.CompareNoCase(_T("btnreport")) == 0 || 
		sName.CompareNoCase(_T("btnscreening")) == 0 || //
		sName.CompareNoCase(_T("btnhelp")) == 0)
	{
		CDuiPoint point;
		::GetCursorPos(&point);
		RECT rc;
		::GetWindowRect(m_hWnd, &rc);
		RECT rc1 = pControl->GetClientPos();
		int w = pControl->GetWidth();
		int h = pControl->GetHeight();
		point.x = rc.left + rc1.left;
		point.y = rc.top + rc1.top + h + 2;

		if (sName.CompareNoCase(_T("btnfile")) == 0)
		{
			_iMenuSel = 0;

			CMenuWndEx::GetGlobalContextMenuObserver().SetMenuCheckInfo(&m_MenuFileMap);
			ClearMenu();
			m_pMenuFile = new CMenuWndEx();
			m_pMenuFile->SetParentWnd(m_hWnd);
			m_pMenuFile->Init(NULL, _T("xmls/menufile.xml"), point, &m_pm);
			m_pMenuFile->GetMenuUI()->SelectItem(0, true);
		}
		else if (sName.CompareNoCase(_T("btnsetting")) == 0)
		{
			_iMenuSel = 1;

			if (IsAdmin())
			{
				CMenuWndEx::GetGlobalContextMenuObserver().SetMenuCheckInfo(&m_MenuSettingMap);
				ClearMenu();
				m_pMenuSetting = new CMenuWndEx();
				m_pMenuSetting->SetParentWnd(m_hWnd);
				m_pMenuSetting->Init(NULL, _T("xmls/menusetting.xml"), point, &m_pm);
				//m_pMenuSetting->GetMenuUI()->SelectItem(0, true);
			}
		}
		else if (sName.CompareNoCase(_T("btninfo")) == 0)
		{
// 			CMenuWndEx::GetGlobalContextMenuObserver().SetMenuCheckInfo(&m_MenuInfoMap);
// 			ClearMenu();
// 			m_pMenuInfo = new CMenuWndEx();
// 			m_pMenuInfo->SetParentWnd(m_hWnd);
// 			m_pMenuInfo->Init(NULL, _T("xmls/menuinfo.xml"), point, &m_pm);
		}
		else if (sName.CompareNoCase(_T("btnreport")) == 0)
		{
			_iMenuSel = 2;

			CMenuWndEx::GetGlobalContextMenuObserver().SetMenuCheckInfo(&m_MenuReportMap);
			ClearMenu();
			m_pMenuReport = new CMenuWndEx();
			m_pMenuReport->SetParentWnd(m_hWnd);
			m_pMenuReport->Init(NULL, _T("xmls/menureport.xml"), point, &m_pm);
		}
		else if (sName.CompareNoCase(_T("btnscreening")) == 0)
		{
			//CMenuWndEx::GetGlobalContextMenuObserver().SetMenuCheckInfo(&m_MenuScreeningMap);
			//ClearMenu();
			//m_pMenuScreening = new CMenuWndEx();
			//m_pMenuScreening->SetParentWnd(m_hWnd);
			//m_pMenuScreening->Init(NULL, _T("xmls/menuscreening.xml"), point, &m_pm);
		}
		else if (sName.CompareNoCase(_T("btnhelp")) == 0)
		{
			_iMenuSel = 3;

			CMenuWndEx::GetGlobalContextMenuObserver().SetMenuCheckInfo(&m_MenuHelpMap);
			ClearMenu();
			m_pMenuHelp = new CMenuWndEx();
			m_pMenuHelp->SetParentWnd(m_hWnd);
			m_pMenuHelp->Init(NULL, _T("xmls/menuhelp.xml"), point, &m_pm);
		}
	}
	///////////////////////////////////////
	//新建
	else if (sName.CompareNoCase(_T("btn1")) == 0)
	{
		OnBnClickedBtnInputinfo();
	}
	//制片
	else if (sName.CompareNoCase(_T("btn2")) == 0)
	{
		OnBnClickedBtnMicroscope();
	}
	//档案
	else if (sName.CompareNoCase(_T("btn3")) == 0)
	{
		OnBnClickedButton1();
	}
	//报告
	else if (sName.CompareNoCase(_T("btn4")) == 0)
	{
		//OnBnClickedButton19();
		if (g_PatientInfoCur.sName.IsEmpty() || g_PatientInfoCur.sBLH.IsEmpty())
		{
			SOUND_WARNING
			::MessageBox(m_hWnd, _T("请新建一个用户或者病历管理选择一个用户样本"), _T("警告"), MB_OKCANCEL);
			return;
		}

		CDatabaseMgr* pMgr = CDatabaseMgr::GetInstance();
		_g_record t;
		pMgr->FindInfoByID(g_PatientInfoCur.sBLH, t);
		CString a;
		int b;
		bool ret = t.getdata(_T("path"), a, b);
		assert(ret);

		TCHAR lpBin[MAX_PATH] = { 0 }, buf[MAX_PATH] = { 0 };
		InitPath(lpBin, NULL, NULL);
		if (!PathFileExists(a.GetString()))
		{
			SOUND_WARNING
			::MessageBox(m_hWnd, _T("报告文件不存在."), _T("警告"), MB_OKCANCEL);
			return;
		}
		CString name = _T("AcroRd32.exe");
		CString name1;
		name1 += a;
		ShellExecute(NULL, _T("open"), name, name1, lpBin, SW_SHOWNORMAL);
	}
	//参数
	else if (sName.CompareNoCase(_T("btn5")) == 0)
	{

	}
	//病历管理
	else if (sName.CompareNoCase(_T("btn6")) == 0)
	{
		OnBnClickedBtnBl();
	}
	//开始处理
	else if (sName.CompareNoCase(_T("tab2btn1")) == 0)
	{
		OnDisposeProgressEx(); 
	}
	//tab3
	else if (-1 != ibtncellitem1 || -1 != ibtncellitem2)
	{
		CString stemp1;
		if (-1 != ibtncellitem1)
			stemp1 = sDest.Mid(s1.GetLength());
		else
			stemp1 = sDest.Mid(s2.GetLength());
		int pos1 = stemp1.Find(_T("_"));
		CString stemp2 = stemp1.Mid(0, pos1);
		CString stemp3 = stemp1.Mid(pos1 + 1);
		int row = _ttoi(stemp2.GetString());
		int column = _ttoi(stemp3.GetString());
		if (m_iValidIndex >= 0 && m_iValidIndex>= row*_column_cnt + column)
		{
			//选择细胞
			if (-1 != ibtncellitem1)
			{
				SelImg(row, column);
			}
			//显示细胞图片
			else if (-1 != ibtncellitem2)
			{
// 				int page, uiIndex;
// 				bool b = FindIndex(0, page, uiIndex);
// 				if (b)
// 				{
// 
// 				}
				SetShowCell(row, column);
			}
		}
	}
	//上一页
	else if (sName.CompareNoCase(_T("lastpage"))==0)
	{
		if (m_wSelPage > 0)
		{
			m_wSelPage--;
			SelPage(m_wSelPage);
		}
	}
	//下一页
	else if (sName.CompareNoCase(_T("nextpage")) == 0)
	{
		if (m_wSelPage< m_wCurShowPageCount - 1)
		{
			m_wSelPage++;
			SelPage(m_wSelPage);
		}
	}
	//跳转
	else if (sName.CompareNoCase(_T("btnskip")) == 0)
	{
		CString str = m_pLabelPageSkip->GetText();
		if (!str.IsEmpty())
		{
			int iPage = _ttoi(str.GetString());
			if (iPage>=1 && iPage<=m_wCurShowPageCount)
			{
				int pg = iPage - 1;
				if (pg != m_wSelPage)
				{
				}
				m_wSelPage = pg;
				SelPage(m_wSelPage);
			}
		}
	}
	//统计信息
	else if (sName.CompareNoCase(_T("tab3btn1")) == 0)
	{
		//StatisticsInfo();
	}
	//显示统计结果
	else if (sName.CompareNoCase(_T("tab3btn2")) == 0)
	{
		ShowStatisticResult();

		//CHistogramWnd* pHistogramWnd = new CHistogramWnd();
		//// WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW WS_EX_TOPMOST
		//pHistogramWnd->Create(NULL, NULL, WS_POPUP | WS_CLIPCHILDREN | WS_VISIBLE, WS_EX_TOOLWINDOW, 0, 0, 602, 482);
		//pHistogramWnd->CenterWindow();
		//SetWindowPos(pHistogramWnd->GetHWND(), NULL, 0, 0, 602, 482, SWP_NOZORDER | SWP_NOACTIVATE);

		//CDiagramWnd* pDiagramWnd = new CDiagramWnd();
		//pDiagramWnd->Create(NULL, NULL, WS_POPUP | WS_CLIPCHILDREN | WS_VISIBLE, WS_EX_TOOLWINDOW, 0, 0, 602, 482);
		//pDiagramWnd->CenterWindow();
		//SetWindowPos(pDiagramWnd->GetHWND(), NULL, 603, 0, 602, 482, SWP_NOZORDER | SWP_NOACTIVATE);
	}
	//生成报告
	else if (sName.CompareNoCase(_T("tab3btn3")) == 0)
	{
		OnBnClickedButton19();
	}
}


void CMainDlgWnd::ClickMenuHelp(LPCTSTR name)
{
	//文件
	if (_tcscmp(_T("file1"), name)==0)
	{
		OnBnClickedButton1();
	}
	//设置
	else if (_tcscmp(_T("setting1"), name) == 0) //显微镜
	{
		On32813();
	}
	else if (_tcscmp(_T("setting2"), name) == 0) //测量半径
	{
		On32825(); 
	}
	else if (_tcscmp(_T("setting3"), name) == 0) //x向步长
	{
		On32840(); 
	}
	else if (_tcscmp(_T("setting4"), name) == 0) //y向步长
	{
		On32841(); 
	}
	else if (_tcscmp(_T("setting5"), name) == 0) //停止阔值
	{
		On32842();
	}
	//报告
	else if (_tcscmp(_T("report1"), name) == 0) //生成报告
	{
		On32815(); 
	}
	else if (_tcscmp(_T("report3"), name) == 0) //打印报告
	{
		On32826();
	}
	//帮助
	else if (_tcscmp(_T("help1"), name) == 0) //技术支持
	{
		On32818(); 
	}
	else if (_tcscmp(_T("help2"), name) == 0) //联系我们
	{
		On32819(); 
	}
	else if (_tcscmp(_T("help3"), name) == 0) //注册账号
	{
		OnRegister(); 
	}
	else if (_tcscmp(_T("help4"), name) == 0) //修改密码
	{
		OnModifyPwd(); 
	}
	else if (_tcscmp(_T("help5"), name) == 0) //剩余次数
	{
		OnLeftCount();
	}
}

LRESULT CMainDlgWnd::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	m_trayIcon.DeleteTrayIcon();
	bHandled = FALSE;
	// 退出程序
	PostQuitMessage(0);
	return 0;
}


LRESULT CMainDlgWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}


LRESULT CMainDlgWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return WindowImplBase::OnSize(uMsg, wParam, lParam, bHandled);
}

LPCTSTR GetMenuButtonName(int sel)
{
	if (0 == sel) return _T("btnfile");
	else if (1 == sel) return _T("btnsetting");
	else if (2 == sel) return _T("btnreport");
	else if (3 == sel) return _T("btnhelp");
	return NULL;
}

LRESULT CMainDlgWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//关闭菜单
    if (uMsg == WM_USER + 2049)
	{  
		if (_iMenuSel>=0)
		{
			LPCTSTR pStr = NULL;
			if (wParam == VK_LEFT)
			{
				int iSel = _iMenuSel - 1;
				if (iSel >= 0)
				{
					if (!IsAdmin(false) && iSel == 1)
						iSel -= 1;
					pStr = GetMenuButtonName(iSel);
				}
			}
			else if (wParam == VK_RIGHT)
			{
				int iSel = _iMenuSel + 1;
				if (iSel <= 3)
				{
					if (!IsAdmin(false) && iSel == 1)
						iSel += 1;
				
					pStr = GetMenuButtonName(iSel);
				}
			}
			if (pStr)
			{
				if (m_pMenuFile)
					m_pMenuFile->Close();
				if (m_pMenuSetting)
					m_pMenuSetting->Close();
				if (m_pMenuReport)
					m_pMenuReport->Close();
				if (m_pMenuHelp)
					m_pMenuHelp->Close();
				CButtonUI* pBtn = static_cast<CButtonUI*>(m_pm.FindControl(pStr));
				OnLClick(pBtn);
			}
		}
	}
	// 关闭窗口，退出程序
	else if (uMsg == WM_USER+2000)
	{
		bHandled = true;

		int val = (int)wParam;
		if (10 == val) //打开路径
		{
			CString path = m_pLabelArchivePath->GetText();
			if (path.GetLength() > 0 && !PathFileExists(path.GetString()))
			{
				SOUND_WARNING;
				::MessageBox(m_hWnd, _T("文件路径错误."), _T("警告"), MB_OKCANCEL);
				return 0;
			}
			ShellExecute(NULL, _T("open"), NULL, NULL, path.GetString(), SW_SHOWNORMAL);
		}
		else if (11 == val)//复制路径
		{
			if (m_FileDir.IsEmpty())
			{
				return 0;
			}
			int len = m_FileDir.GetLength() * 2;
			char cp[] = { 0 };
			CStringA stra;
			stra = gszFolder;
			strcpy(cp, stra.GetString());
			CopyToClipboard(cp, len);
		}
		else if (12 == val)//设置路径
		{
			SettingPathWnd::MessageBox(m_hWnd);
		}
		return 0;
	}
	if (uMsg == WM_USER + 1346)
	{
		int i = -1;
	}
	if (uMsg == WM_USER_CAL)
	{
		::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		SetTimer(gMainDlgWnd->GetHWND(), 386, 300, NULL);
	}
	else if (uMsg == MSG_USER_SWITCH_MAIN)
	{
		SetTimer(m_hWnd, TIMER_USER_SWITCH_MAIN, 500, NULL);
	}
	else if (uMsg == MSG_USER_SWITCH_MAIN1)
	{
		SetTimer(m_hWnd, TIMER_USER_SWITCH_MAIN1, 500, NULL);
	}
	else if (uMsg == WM_DESTROY)
	{
		::PostQuitMessage(0L);
		bHandled = TRUE;
		return 0;
	}
	//进度
	else if (uMsg == WM_USER + 456)
	{
		ShowProgressEx(m_aProgress);
	}
	else if (uMsg == WM_USER + 700)
	{
		int i = (int)wParam;
		int a1 = (int)lParam;
		string s1, s2;
		s1 = g_vec_calc_image[a1];
		if (PathFileExistsA(s1.c_str()))
		{
			ShowOneCellSelectionImg(i, s1.c_str());
		}
		bHandled = TRUE;
		return 0;
	}
	else if (uMsg == WM_USER + 701)
	{
		int item_cnt = g_vvec_calc_data[0].size();
		if (item_cnt > 0)
		{
			int i = (int)wParam;
			int a2 = (int)lParam;
			CString str[13];
			for (int j = 0; j < item_cnt; ++j)
			{
				str[j] = g_vvec_calc_data[a2][j].c_str();
			}
			ShowOneCellSelectionData(i, str);
		}
		bHandled = TRUE;
		return 0;
	}
	else if (uMsg == WM_TIMER)
	{
		if (wParam == 20413)
		{
			KillTimer(m_hWnd, 20413);

			CVerticalLayoutUI* bodyright = static_cast<CVerticalLayoutUI*>(m_pm.FindControl(_T("bodyright")));
			bodyright->SetFocus();
		}
		//bMicroScopeEx控制按钮点击
		else if (wParam == 2020)
		{
			KillTimer(m_hWnd, 2020);
			bMicroScopeEx = true;
		}
		//重新计算
		else if (wParam == TIMER_USER_SWITCH_MAIN)
		{
			KillTimer(m_hWnd, TIMER_USER_SWITCH_MAIN);
			gMainDlgWnd->SwitchTab(1);
			gMainDlgWnd->OnDisposeProgressEx(); //开始处理
		}
		//重新选择档案
		else if (wParam == TIMER_USER_SWITCH_MAIN1)
		{
			KillTimer(m_hWnd, TIMER_USER_SWITCH_MAIN1);
			gMainDlgWnd->SwitchTab(0);
			gMainDlgWnd->OnBnClickedButton1(); //档案
		}
		//扫描程序通知
		else if (wParam == 386)
		{
			KillTimer(m_hWnd, 386);
			::SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

			if (gMainDlgWnd->IsDisposeCalc())
			{
				SOUND_WARNING
				::MessageBox(m_hWnd, _T("当前样本检测中，请等待操作完成."), _T("警告"), MB_OKCANCEL);
				return 0;
			}

			TCHAR lpBin[MAX_PATH];
			InitPath(lpBin, NULL, NULL);
			TCHAR lpini[MAX_PATH];
			wsprintf(lpini, _T("%s%s"), lpBin, _T("number.ini"));
			TCHAR buf[MAX_PATH] = { 0 };
			GetPrivateProfileString(_T("path"), _T("number"), _T(""), buf, MAX_PATH - 1, lpini);
			CString path(buf);
			if (PathFileExists(path.GetString()))
			{
				int pos = path.Find(_T("-num"));
				CString strBLH(path.Mid(pos + 4));
				if (strBLH.GetLength() && strBLH.GetAt(strBLH.GetLength()-1)=='\\' )
				{
					strBLH = strBLH.Mid(0, strBLH.GetLength() - 1);
				}
				if (strBLH.IsEmpty())
				{
					return 0;
				}
				bool bCalc = false;
				if (g_PatientInfoCur.sName.IsEmpty() || g_PatientInfoCur.sBLH.IsEmpty())
				{
					SOUND_WARNING
					::MessageBox(m_hWnd, _T("请新建一个用户或者病历管理选择一个用户样本,然后手动选择扫描图片路径计算."), _T("警告"), MB_OKCANCEL);
					return 0;
				}
				else
				{
					if (g_PatientInfoCur.sBLH.Compare(strBLH)==0)
					{
						bCalc = true;
					}
					else
					{
						SOUND_WARNING
						::MessageBox(m_hWnd, _T("当前计算程序选择用户的病历号与扫描程序不相同,请手动选择扫描图片路径计算."), _T("警告"), MB_OKCANCEL);
						return 0;
					}
				}
				m_bIsDisposeBegin = false;
				m_bIsDisposeEnd = false;
				m_bIsDisposeCalc = false;
				OpenFolder(path);
				if (bCalc)
					OnDisposeProgressEx();
			}
		}
		else if (wParam == 416)
		{
			if (m_bSuc)
			{
				KillTimer(m_hWnd, 416);
			}
			CTime bTime = CTime::GetCurrentTime();
			bTime.GetTime();
			CTimeSpan bbb;
			bbb = bTime - m_aTime;
			CString strtime;
			strtime.Format(_T("  Time=%dM:%dS\n"), bbb.GetMinutes(), bbb.GetSeconds());
			CString sTime;
			sTime += strtime;
			ShowPregress(sTime, m_aProgress);
		}
		bHandled = FALSE;
	}
	else if (uMsg == WM_SHOWWINDOW)
	{
		bHandled = FALSE;
		m_pMinBtn->NeedParentUpdate();
		InvalidateRect(m_hWnd, NULL, TRUE);
	}
	else if (uMsg == WM_SYSKEYDOWN || uMsg == WM_KEYDOWN) {
		int a = 0;
	}
	else if (uMsg == WM_MENUCLICK)
	{
		MenuCmd* pMenuCmd = (MenuCmd*)wParam;
		if (pMenuCmd != NULL)
		{
			BOOL bChecked = pMenuCmd->bChecked;
			CDuiString sMenuName = pMenuCmd->szName;
			CDuiString sUserData = pMenuCmd->szUserData;
			CDuiString sText = pMenuCmd->szText;
			m_pm.DeletePtr(pMenuCmd);

			if (0 == sMenuName.CompareNoCase(_T("exit"))) {
				Close(0);
			}
			
			ClickMenuHelp(sMenuName);
		}
		bHandled = TRUE;
		return 0;
	}
	else if (uMsg == UIMSG_TRAYICON)
	{
		UINT uIconMsg = (UINT)lParam;
		if (uIconMsg == WM_LBUTTONUP) {
			BOOL bVisible = IsWindowVisible(m_hWnd);
			::ShowWindow(m_hWnd, !bVisible ? SW_SHOW : SW_HIDE);
		}
		else if (uIconMsg == WM_RBUTTONUP) {
			if (m_pMenu != NULL) {
				delete m_pMenu;
				m_pMenu = NULL;
			}
			m_pMenu = new CMenuWnd();
			CDuiPoint point;
			::GetCursorPos(&point);
			//point.y -= 100;
			m_pMenu->Init(NULL, _T("menu.xml"), point, &m_pm);
			// 动态添加后重新设置菜单的大小
			m_pMenu->ResizeMenu();
		}
	}
	else if (uMsg == WM_DPICHANGED) {
		m_pm.SetDPI(LOWORD(wParam));  // Set the new DPI, retrieved from the wParam
		m_pm.ResetDPIAssets();
		RECT* const prcNewWindow = (RECT*)lParam;
		SetWindowPos(m_hWnd, NULL, prcNewWindow->left, prcNewWindow->top, prcNewWindow->right - prcNewWindow->left, prcNewWindow->bottom - prcNewWindow->top, SWP_NOZORDER | SWP_NOACTIVATE);
		if (m_pm.GetRoot() != NULL) m_pm.GetRoot()->NeedUpdate();
	}
	else if (uMsg == WM_MOUSELEAVE)
	{
		_bMouseTrack = TRUE;
		int i = -1;
	}
	bHandled = FALSE;
	return 0;
}


LRESULT CMainDlgWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	WindowImplBase::OnCreate(uMsg, wParam, lParam, bHandled);
	return 0;
}


LRESULT CMainDlgWnd::OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

	bHandled = FALSE;
	return 0;
}


LRESULT CMainDlgWnd::OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	if (_bMouseTrack)     // 若允许 追踪，则。 
	{
		TRACKMOUSEEVENT csTME;
		csTME.cbSize = sizeof (csTME);
		csTME.dwFlags = TME_LEAVE | TME_HOVER;
		csTME.hwndTrack = m_hWnd;// 指定要 追踪 的窗口 
		csTME.dwHoverTime = 10;  // 鼠标在按钮上停留超过 10ms ，才认为状态为 HOVER
		::_TrackMouseEvent(&csTME); // 开启 Windows 的 WM_MOUSELEAVE ， WM_MOUSEHOVER 事件支持


		_bMouseTrack = FALSE;   // 若已经 追踪 ，则停止 追踪 
	}
	bHandled = FALSE;
	return 0;
}


BOOL CMainDlgWnd::OnMainWndMaxMinEx(bool bMax)
{
	if (m_pTab1)
	{
		if (bMax)
			m_pTab1->SetBkImage(_T("dlgmain/bkg2.png"));
		else
			m_pTab1->SetBkImage(_T("dlgmain/bkg1.png"));
	}
	return TRUE;
}

LRESULT CMainDlgWnd::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	MONITORINFO oMonitor = {};
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
	CDuiRect rcWork = oMonitor.rcWork;
	rcWork.Offset(-oMonitor.rcMonitor.left, -oMonitor.rcMonitor.top);

	LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
	lpMMI->ptMaxPosition.x = rcWork.left;
	lpMMI->ptMaxPosition.y = rcWork.top;
	lpMMI->ptMaxSize.x = rcWork.right;
	lpMMI->ptMaxSize.y = rcWork.bottom;

	bHandled = FALSE;
	return 0;
}


void CMainDlgWnd::ClearMenu()
{
	if (m_pMenuFile != NULL) {
		delete m_pMenuFile;
		m_pMenuFile = NULL;
	}
	if (m_pMenuSetting != NULL) {
		delete m_pMenuSetting;
		m_pMenuSetting = NULL;
	}
	if (m_pMenuInfo != NULL) {
		delete m_pMenuInfo;
		m_pMenuInfo = NULL;
	}
	if (m_pMenuReport != NULL) {
		delete m_pMenuReport;
		m_pMenuReport = NULL;
	}
	if (m_pMenuScreening != NULL) {
		delete m_pMenuScreening;
		m_pMenuScreening = NULL;
	}
	if (m_pMenuHelp != NULL) {
		delete m_pMenuHelp;
		m_pMenuHelp = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////////////////
///////////////////////\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void CMainDlgWnd::OpenFolder(CString& szFolder)
{
	if (gMainDlgWnd->IsDisposeProgress())
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("当前样本进行中，无法选择文件."), _T("警告"), MB_OKCANCEL);
		return;
	}

	//文件夹相关路径
	string sPath;
	string sPathFile;
	string sFilePathName;
	string sFileName;
	string sPathOtus;
	string sPathImageDrawFirst;
	string sPathImageDrawFinal;
	string sPathROICells;
	string sPathROILymphocyte;
	string sPathROIOthers;
	string sDataFileName;
	string sDataFileResultName;

	if (szFolder.GetAt(szFolder.GetLength() - 1) == '\\' || szFolder.GetAt(szFolder.GetLength() - 1) == '/')
		szFolder = szFolder.Mid(0, szFolder.GetLength() - 1);
	m_FileDir.Format(_T("%s"), szFolder);

	USES_CONVERSION;
	sPathFile = W2A(m_FileDir);
	int nLocation = sPathFile.rfind("\\");
	sPath = sPathFile.substr(0, nLocation) + "\\";
	sDataFileName = sPathFile.substr(nLocation + 1, sPathFile.length());
	sDataFileResultName = sDataFileName + "Result\\";
	string sss = sPathFile;
	sss = sss + "Result\\";
	//m_lpResultP = c_str(sss);
	sss = sss + "Result\\";
	sprintf(m_lpResultPath, ("%s"), sss.data());

	sPathFile = sPathFile + "\\*.jpg";

	//统计图片数量
	nImageNumAll = 0;
	//文件存储信息结构体 
	struct _finddata_t fileinfo;
	//保存文件句柄 
	long fHandle;
	if ((fHandle = _findfirst(sPathFile.c_str(), &fileinfo)) == -1L)
	{
		printf("当前目录下没有bmp文件\n");
	}
	else
	{
		//建立数据结果及过程中的文件及数据
		string StrTemp0 = sPath + sDataFileName + "Result";
		_mkdir(StrTemp0.c_str());
		string StrTemp = sPath + sDataFileResultName + "Lymphocyte";
		_mkdir(StrTemp.c_str());
		StrTemp = sPath + sDataFileResultName + "Cells";
		_mkdir(StrTemp.c_str());
		StrTemp = sPath + sDataFileResultName + "ImageDrawFinal";
		_mkdir(StrTemp.c_str());
		StrTemp = sPath + sDataFileResultName + "Result";
		_mkdir(StrTemp.c_str());
		StrTemp = sPath + sDataFileResultName + "Others";
		_mkdir(StrTemp.c_str());

		do
		{
			nImageNumAll++;
		} while (_findnext(fHandle, &fileinfo) == 0);
	}

	m_pLabelArchivePath->SetText(m_FileDir.GetString());

	TCHAR lpInfo[MAX_PATH] = { 0 };
	TCHAR lpBin[MAX_PATH];
	InitPath(lpBin, NULL, NULL);
	TCHAR lpINI[128];
	wsprintf(lpINI, _T("%s%s"), lpBin, _T("publish.ini"));
	WritePrivateProfileString(_T("archipath"), _T("path"), m_FileDir.GetString(), lpINI);

	//切换标签页
	SwitchTab(1);
}

BOOL CMainDlgWnd::OpenDB()
{
	BOOL bOK = FALSE;
	m_pConnection = _ConnectionPtr(__uuidof(Connection));
	try
	{
		CString strDBName;
		strDBName = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=";
		strDBName += g_lpBinPath;
		strDBName += "patient.mdb;";
		strDBName += "User ID='admin';Password=;Jet OLEDB:Database Password='yzy_db_2019'";

		m_pConnection->ConnectionString = (_bstr_t)strDBName;
		//m_pConnection->ConnectionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=patient.mdb;";
		m_pConnection->Open("", "", "", adConnectUnspecified);
		bOK = TRUE;
	}
	catch (_com_error *e)
	{
		SOUND_WARNING
		AfxMessageBox(e->ErrorMessage());
		bOK = FALSE;
	}

	/* OK
	//CString csFilter;
	//csFilter = ("ID=2");
	//CString csFilter;
	//csFilter = ("patient_name='张三'");

	//CString csSQLstr;
	//csSQLstr = "SELECT * FROM patient_info where ";
	//csSQLstr += csFilter;
	*/
	CString csSQLstr;
	if (m_strFilter.GetLength() == 0)
	{
		csSQLstr = "SELECT * FROM patient_info";
		csSQLstr += " order by ID desc";

	}
	else
		csSQLstr = m_strFilter;

	m_pRecordset.CreateInstance(__uuidof(Recordset));
	try
	{
		m_pRecordset->Open((_bstr_t)csSQLstr, m_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
		bOK = TRUE;
	}
	catch (_com_error *e)
	{
		SOUND_WARNING
		AfxMessageBox(e->ErrorMessage());
		bOK = FALSE;
	}
	return bOK;
}

void CMainDlgWnd::ExitADOConn()
{
	CloseDB();
	//CoUninitialize();
}

void CMainDlgWnd::CloseDB()
{
	if (m_pRecordset != NULL)
	{
		try
		{
			if (m_pRecordset->State)
			{
				m_pRecordset->Close();//关闭记录集
			}
			m_pRecordset = NULL;
		}
		catch (_com_error e)
		{
			SOUND_WARNING
			AfxMessageBox(e.ErrorMessage());
		}

	}

	if (m_pConnection != NULL)
	{
		try
		{
			if (m_pConnection->State)
			{
				m_pConnection->Close();
			}
			m_pConnection = NULL;
		}
		catch (_com_error e)
		{
			SOUND_WARNING
			AfxMessageBox(e.ErrorMessage());
		}
	}
}

void CMainDlgWnd::MkFilter(int nID)
{
	if (nID == 0)	//
	{
		m_strFilter = "";
	}
	else if (nID == 1)	//姓名
	{
		//m_strFilter.Format(_T("[姓名] LIKE '%s'"), m_strName);
		m_strFilter = "SELECT * FROM patient_info where [patient_name] LIKE '%";
		//m_strFilter = "SELECT * FROM patient_info where patient_name='";
		m_strFilter += m_strName;
		m_strFilter += "%'";
		m_strFilter += " order by ID desc";

	}
	else if (nID == 2)	//性别
	{
		m_strFilter = "SELECT * FROM patient_info where sex=";
		m_strFilter += m_strSex_filter;
		m_strFilter += " order by ID desc";

	}
	else if (nID == 3)	//年龄
	{
		m_strFilter = "SELECT * FROM patient_info where age BETWEEN ";
		m_strFilter += m_strAgeLow_filter;
		m_strFilter += "AND ";
		m_strFilter += m_strAgeHigh_filter;
		m_strFilter += " order by ID desc";

	}
	else if (nID == 4)	//病历号
	{
		m_strFilter = "SELECT * FROM patient_info where BL_ID LIKE '%";
		m_strFilter += m_strBLNo;
		m_strFilter += "%'";
		m_strFilter += " order by ID desc";

	}
	/*
	else if (nID == 5)	//送检日期
	{
	m_strFilter = "SELECT * FROM patient_info where [create_date] BETWEEN # ";
	//m_strFilter += m_strTimeLow_filter;
	m_strFilter += m_TimeLow.Format();
	m_strFilter += " # AND # ";
	//m_strFilter += m_strTimeHigh_filter;
	m_strFilter += m_TimeHigh.Format();
	m_strFilter += " #";
	}
	*/
	else if (nID == 5)	//送检日期
	{
		m_strFilter = "SELECT * FROM patient_info where format(create_date, 'yyyy-mm-dd') BETWEEN format(#";
		m_strFilter += m_strTimeLow_filter;
		m_strFilter += "#, 'yyyy-mm-dd') and format(#";
		m_strFilter += m_strTimeHigh_filter;
		m_strFilter += "#, 'yyyy-mm-dd')";
		m_strFilter += " order by ID desc";

	}
	else if (nID == 6)	//审核
	{
		m_strFilter = "SELECT * FROM patient_info where check_ID=";
		m_strFilter += m_strCheck_filter;
		m_strFilter += " order by ID desc";

	}
	else if (nID == 7)
	{
		m_strFilter = "SELECT * FROM patient_info where BL_ID=";
		m_strFilter += m_strBLH_filter;
	}
}

void CMainDlgWnd::Save_info_todb(void)
{
	if (g_PatiengInfo.db_ID == 2)
	{
		return;
	}
	else if (g_PatiengInfo.db_ID == 0 || g_PatiengInfo.db_ID == 1)	//0新增 1修改
	{
		if (!OpenDB())
			return;

		bool bSuccess = false;
		bool bFindName = false;

		try
		{
			if (!m_pRecordset->BOF)
				m_pRecordset->MoveFirst();
			else
			{
				//::MessageBox(m_hWnd, _T("未查询符合条件的数据"), _T("警告"), MB_OK);
				return;
			}
			CString strBLHFind;
			int row = 0;
			int nCol = 0;
			_variant_t var;
			while (!m_pRecordset->adoEOF)
			{
				nCol = 0;
				var = m_pRecordset->GetCollect("BL_ID");
				if (var.vt != VT_NULL)
				{
					strBLHFind = (LPCSTR)_bstr_t(var);
				}

				if (strBLHFind.Compare(g_PatiengInfo.lpBLId) == 0)
				{
					bFindName = true;
					break;
				}
				else
					m_pRecordset->MoveNext();
			}
		}
		catch (_com_error *e)
		{
			SOUND_WARNING
			AfxMessageBox(e->ErrorMessage());
		}

		if (!bFindName)
		{
			m_pRecordset->AddNew();///插入新记录
		}
		m_pRecordset->PutCollect("patient_name", g_PatiengInfo.lpName);
		if (g_PatiengInfo.nSex)
			m_pRecordset->PutCollect("sex", int(1));
		else
			m_pRecordset->PutCollect("sex", int(0));

		m_pRecordset->PutCollect("age", int(g_PatiengInfo.nAge));
		m_pRecordset->PutCollect("BL_ID", g_PatiengInfo.lpBLId);
		m_pRecordset->PutCollect("PhoneNum", g_PatiengInfo.lpPhoneNum);
		COleDateTime	bTime = COleDateTime::GetCurrentTime();
		m_pRecordset->PutCollect("create_date", _variant_t(bTime));
		m_pRecordset->PutCollect("check_ID", int(0));
		//m_pRecordset->PutCollect("diag", "diag_1111");
 		CString name1;
		name1 += m_lpResultPath;
		int name1length = name1.GetLength();
		name1length = name1length - 7;
		name1 = name1.Left(name1length);
		//name1= name1.Left(20);
		name1 += g_PatiengInfo.lpBLId;
		name1 += _T(".pdf");
		if (!bFindName)
			name1 = _T("");
		m_pRecordset->PutCollect("path", (_bstr_t)name1);
		m_pRecordset->PutCollect("check_ID", int(g_PatiengInfo.wCheckId));

		m_pRecordset->Update();
		CloseDB();

		g_PatiengInfo.db_ID = 1;
		m_nLastID = GetLastID();
	}
}

bool CMainDlgWnd::find_info_toid(CString sBLHID, CString& sID)
{
	bool bRet = false;
	if (!gMainDlgWnd->OpenDB())
		return false;
	_ConnectionPtr m_pConnection = gMainDlgWnd->m_pConnection;
	_RecordsetPtr m_pRecordset = gMainDlgWnd->m_pRecordset;
	try
	{
		if (!m_pRecordset->BOF)
			m_pRecordset->MoveFirst();
		else
		{
			//::MessageBox(m_hWnd, _T("表内数据为空"), _T("警告"), MB_OK);
			return false;
		}

		CString strBLNo;

		int row = 0;
		int nCol = 0;
		_variant_t var;
		while (!m_pRecordset->adoEOF)
		{
			nCol = 0;
			var = m_pRecordset->GetCollect("BL_ID");
			if (var.vt != VT_NULL)
			{
				strBLNo = (LPCSTR)_bstr_t(var);
				if (strBLNo.Compare(sBLHID)==0)
				{
					var = m_pRecordset->GetCollect("ID");
					if (var.vt != VT_NULL)
					{
						int nID = (int)(var);
						sID.Format(_T("%d"), nID);
						bRet = true;
						break;
					}
				}
			}
			m_pRecordset->MoveNext();
		}
	}
	catch (_com_error *e)
	{
		SOUND_WARNING
		AfxMessageBox(e->ErrorMessage());
	}

	gMainDlgWnd->CloseDB();

	return bRet;
}

int CMainDlgWnd::GetLastID()
{
	if (!OpenDB())
		return -1;
	int nID = -1;

	/*_ConnectionPtr m_pConnection = m_pParent->m_pConnection;
	_RecordsetPtr m_pRecordset = m_pParent->m_pRecordset;*/
	int     nItemIndex = -1;//当前所选记录的索引
	BOOL    hasDel = false;
	BOOL  	hasClear = false;
	BOOL    IsBottom = false;
	int     colomn = -1;

	try
	{
		if (!m_pRecordset->BOF)
			m_pRecordset->MoveFirst();
		//m_pRecordset->MoveLast();
		else
		{
			//AfxMessageBox(_T("表内数据为空"));
			//::MessageBox(m_hWnd, _T("未查询符合条件的数据"), _T("警告"), MB_OK);
			return -1;
		}
		// 读入库中各字段并加入列表框中

		_variant_t var;
		var = m_pRecordset->GetCollect("ID");
		nID = (int)(var);
	}
	catch (_com_error *e)
	{
		SOUND_WARNING
		AfxMessageBox(e->ErrorMessage());
	}
	CloseDB();

	return nID;
}

void CMainDlgWnd::ModifyDBByID(int nModifyID, PatiengInfo* pInfo)
{
	if (!OpenDB())
		return;
	try
	{
		if (!m_pRecordset->BOF)
			m_pRecordset->MoveFirst();
		else
		{
			//AfxMessageBox(_T("表内数据为空"));
			//::MessageBox(m_hWnd, _T("未查询符合条件的数据"), _T("警告"), MB_OK);
			return;
		}
		// 读入库中各字段并加入列表框中

		int row = 0;
		int nCol = 0;
		_variant_t var;
		while (!m_pRecordset->adoEOF)
		{
			nCol = 0;
			var = m_pRecordset->GetCollect("ID");
			if (var.vt != VT_NULL)
			{
				int nID = (int)(var);
				if (nID == nModifyID)
				{
					m_pRecordset->PutCollect("patient_name", pInfo->lpName);
					if (pInfo->nSex)
						m_pRecordset->PutCollect("sex", int(1));
					else
						m_pRecordset->PutCollect("sex", int(0));

					m_pRecordset->PutCollect("age", pInfo->nAge);
					m_pRecordset->PutCollect("BL_ID", pInfo->lpBLId);
					m_pRecordset->PutCollect("PhoneNum", pInfo->lpPhoneNum);
					COleDateTime	bTime = COleDateTime::GetCurrentTime();
					m_pRecordset->PutCollect("create_date", _variant_t(bTime));
					m_pRecordset->PutCollect("check_ID", pInfo->wCheckId);
					g_PatiengInfo.db_ID = 1;
					m_pRecordset->PutCollect("diag", "");
					CString name1;
					name1 += m_lpResultPath;
					int name1length = name1.GetLength();
					name1length = name1length - 7;
					name1 = name1.Left(name1length);
					//name1= name1.Left(20);
					name1 += g_PatiengInfo.lpBLId;
					name1 += _T(".pdf");
					if (PathFileExists(name1.GetString()))
					{
						m_pRecordset->PutCollect("path", name1.GetString());
					}
					else
						m_pRecordset->PutCollect("path", _T(""));
					m_pRecordset->Update();
					break;
				}
			}
			m_pRecordset->MoveNext();
		}
	}
	catch (_com_error *e)
	{
		SOUND_WARNING
		AfxMessageBox(e->ErrorMessage());
	}

	CloseDB();
}

void CMainDlgWnd::OnBnClickedButton1()
{
	g_FLOW = FLOW_ARCHIEVEOPEN;

	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.pidlRoot = NULL;
	bi.hwndOwner = m_hWnd;
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
	bi.lpfn = BrowseCallbackProc;
	bi.lParam = (LPARAM)&gszFolder;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	BOOL bRet = FALSE;
	if (pidl)
	{
		if (SHGetPathFromIDList(pidl, gszFolder))bRet = TRUE;
 		IMalloc *pMalloc = NULL;
		if (SUCCEEDED(SHGetMalloc(&pMalloc)) && pMalloc)
		{
			pMalloc->Free(pidl);
			pMalloc->Release();
		}
	}
	CString str = gszFolder;

	if (bRet)
		OpenFolder(str);
}

void CMainDlgWnd::OnBnClickedBtnMicroscope()
{
	if (g_PatientInfoCur.sName.IsEmpty() || g_PatientInfoCur.sBLH.IsEmpty())
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("请新建一个用户或者病历管理选择一个用户样本"), _T("警告"), MB_OKCANCEL);
		return;
	}

	g_FLOW = FLOW_SAOMIAO;

	//HANDLE  hMutex = CreateMutex(NULL, FALSE, _T("MicroScopeEx"));
	//if (ERROR_ALREADY_EXISTS == GetLastError())
	//{
	//	return;
	//}
	TCHAR processName[] = _T("MicroScopeEx.exe");
	DWORD dwPid;
	bool bFind = FindProcess(processName, dwPid);
	//HWND hwnd = FindWindow(_T("MainMicroScopeEx"), _T("医之云扫描"));
	if (bMicroScopeEx)
	{
		bMicroScopeEx = false;
		SetTimer(m_hWnd, 2020, 5000, NULL);

		TCHAR lpPath[MAX_PATH];
		InitPath(lpPath, NULL, NULL);
		_stprintf(lpPath, _T("%spublish.ini"), lpPath);
		TCHAR buf[64] = { 0 };
		int sz = GetPrivateProfileString(_T("exeauto"), _T("isclose"), _T("0"), buf, 64, lpPath);
		int iRet = _ttoi(buf);
		if (iRet && bFind)
		{
			KillProcess(dwPid);
		}
		
		bFind = FindProcess(processName, dwPid);
		if (bFind)
		{
			return;
		}
	}
	else
		return;


	// TODO:  在此添加控件通知处理程序代码
	//ShellExecute(NULL, "open ", "D:\\癌细胞项目\\李正义\\程序\\Debug\\MicroScope.exe", NULL, NULL, SW_SHOWNORMAL);
	TCHAR lpBin[MAX_PATH];
	InitPath(lpBin, NULL, NULL);
	CString name = _T("MicroScopeEx.exe");
	//CString path = _T("D:\\癌细胞项目\\李正义\\程序\\Debug\\");
	//CString path = widechar(lpBin);
	//ShellExecuteExOpen(name,path);
//	ShellExecute(NULL, _T("open"), name, NULL, lpBin, SW_SHOWNORMAL);
	//CString Exec = _T("MicroScope.exe ");
	//WinExec(Exec, SW_SHOW);

	//PVOID OldValue = NULL;
	//BOOL bRet = Wow64DisableWow64FsRedirection(&OldValue);
	//bool b = ShellExecute(NULL, _T("open"), name, NULL, lpBin, SW_SHOWNORMAL);
	//if (bRet)
	//{
	//	Wow64RevertWow64FsRedirection(OldValue);
	//}
	//OldValue = NULL;

	/*
	char lpExe[MAX_PATH];
	sprintf(lpExe, "%s", "MicroScope.exe ");
	WinExec(lpExe, SW_SHOW);
	*/

 	CString sExe;
 	sExe.Format(_T("%sMicroScopeEx.exe"), GetExePath());
 	CStringA sExeA;
 	sExeA = sExe;
 	// 创建进程
 	STARTUPINFO silA;
 	PROCESS_INFORMATION pilA;
 	ZeroMemory(&silA, sizeof(STARTUPINFO));
 	ZeroMemory(&pilA, sizeof(PROCESS_INFORMATION));
 	bool bRet = CreateProcess(NULL, (LPWSTR)sExe.GetString(), NULL, NULL, FALSE, 0, NULL, NULL, &silA, &pilA);
 	// 设置进程优先级
 	SetPriorityClass(pilA.hProcess, REALTIME_PRIORITY_CLASS);
	OutputDebugString(_T("sMicroScopeExsMicroScopeExsMicroScopeExsMicroScopeEx\n"));
}

void CMainDlgWnd::OnBnClickedBtnInputinfo()
{
	g_FLOW = FLOW_PATIENTINFO;
	CPatientInfoWnd::MessageBox(m_hWnd);
}

jmp_buf env;
int pdf_err = 0;
void error_handler(HPDF_STATUS   error_no,
	HPDF_STATUS   detail_no,
	void         *user_data)
{
	printf("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no,
		(HPDF_UINT)detail_no);
	if (error_no==4119 && detail_no==13)
	{
		pdf_err = 4119;
		::MessageBox(gMainDlgWnd->GetHWND(), _T("请先关闭PDF阅读器,在生成报告."), _T("提示"), MB_OKCANCEL);
	}
	longjmp(env, 1);
}


void draw_image(HPDF_Doc     pdf, const char  *filename, float x, float y, const char  *text)
{
#ifdef __WIN32__
	const char* FILE_SEPARATOR = "\\";
#else
	const char* FILE_SEPARATOR = "/";
#endif
	char filename1[255];

	HPDF_Page page = HPDF_GetCurrentPage(pdf);
	HPDF_Image image;

	//strcpy(filename1, "images");
	//strcat(filename1, FILE_SEPARATOR);
	//strcat(filename1, filename);
	strcpy(filename1, filename);

	image = HPDF_LoadJpegImageFromFile(pdf, filename1);

	/* Draw image to the canvas. */
	// HPDF_Page_DrawImage (page, image, x, y, HPDF_Image_GetWidth (image),HPDF_Image_GetHeight (image));
	HPDF_Page_DrawImage(page, image, x, y, 240, 160);

	/* Print the text. */
	HPDF_Page_BeginText(page);
	//HPDF_Page_MoveTextPos (page, x+50, y-10);
	//HPDF_Page_ShowTextNextLine (page, filename);
	HPDF_Page_MoveTextPos(page, x + 90, y - 10);
	HPDF_Page_ShowTextNextLine(page, text);
	HPDF_Page_EndText(page);
}

void draw_image2(HPDF_Doc     pdf, const char  *filename, float x, float y, const char  *text)
{
#ifdef __WIN32__
	const char* FILE_SEPARATOR = "\\";
#else
	const char* FILE_SEPARATOR = "/";
#endif
	char filename1[255];

	HPDF_Page page = HPDF_GetCurrentPage(pdf);
	HPDF_Image image;

	//strcpy(filename1, "images");
	//strcat(filename1, FILE_SEPARATOR);
	//strcat(filename1, filename);
	strcpy(filename1, filename);

	image = HPDF_LoadJpegImageFromFile(pdf, filename1);

	/* Draw image to the canvas. */
	// HPDF_Page_DrawImage (page, image, x, y, HPDF_Image_GetWidth (image),HPDF_Image_GetHeight (image));
	HPDF_Page_DrawImage(page, image, x, y, 200, 170);

	/* Print the text. */
	HPDF_Page_BeginText(page);
	//HPDF_Page_MoveTextPos (page, x+50, y-10);
	//HPDF_Page_ShowTextNextLine (page, filename);
	HPDF_Page_MoveTextPos(page, x + 25, y - 10);
	HPDF_Page_ShowTextNextLine(page, text);
	HPDF_Page_EndText(page);
}
void draw_image1(HPDF_Doc     pdf, const char  *filename, float x, float y, const char  *text)
{
#ifdef __WIN32__
	const char* FILE_SEPARATOR = "\\";
#else
	const char* FILE_SEPARATOR = "/";
#endif
	char filename1[255];

	HPDF_Page page = HPDF_GetCurrentPage(pdf);
	HPDF_Image image;

	//strcpy(filename1, "images");
	//strcat(filename1, FILE_SEPARATOR);
	//strcat(filename1, filename);
	strcpy(filename1, filename);

	image = HPDF_LoadJpegImageFromFile(pdf, filename1);

	/* Draw image to the canvas. */
	// HPDF_Page_DrawImage (page, image, x, y, HPDF_Image_GetWidth (image),HPDF_Image_GetHeight (image));
	HPDF_Page_DrawImage(page, image, x, y, 130, 53);

	/* Print the text. */
	HPDF_Page_BeginText(page);
	//HPDF_Page_MoveTextPos (page, x+50, y-10);
	//HPDF_Page_ShowTextNextLine (page, filename);
	HPDF_Page_MoveTextPos(page, x + 25, y - 10);
	HPDF_Page_ShowTextNextLine(page, text);
	HPDF_Page_EndText(page);
}

void CMainDlgWnd::OnBnClickedButton19()
{
	On32815();
}

void CMainDlgWnd::OnBnClickedButton4()
{

}

void CMainDlgWnd::OnBnClickedButton3()
{

}

void CMainDlgWnd::OnBnClickedBtnBl()
{
	CCasesMgrWnd* pCasesMgrWnd = new CCasesMgrWnd();
	pCasesMgrWnd->Create(m_hWnd, _T("医之云"), WS_POPUP | WS_VISIBLE, WS_EX_TOOLWINDOW, 0, 0, 1200, 800);
	pCasesMgrWnd->CenterWindow();
	pCasesMgrWnd->ShowModal();
}

void CMainDlgWnd::On32813()
{
	TCHAR lpBin[MAX_PATH];
	InitPath(lpBin, NULL, NULL);
	CString name = _T("MicroScopeEx.exe");
	ShellExecute(NULL, _T("open"), name, NULL, lpBin, SW_SHOWNORMAL);
}

void CMainDlgWnd::On32825()
{
	TCHAR lpInfo[128];
	TCHAR lpBin[MAX_PATH];
	InitPath(lpBin, NULL, NULL);
	TCHAR lpINI[128];
	wsprintf(lpINI, _T("%s%s"), lpBin, _T("pst.ini"));
	GetPrivateProfileString(_T("StagePara"), _T("radius_mm"), _T("5"), lpInfo, sizeof(lpInfo) - 1, lpINI);
	double m_dRadius_mm = _tstof(lpInfo);
	if (m_dRadius_mm > MAX_RADIUS)
	{
		m_dRadius_mm = 4.0;
		OutputDebugString(_T("Radius too large,set to 4.5mm.\n"));
	}

	CPopMenuSettingWnd * pThis = CPopMenuSettingWnd::GetThis();
	pThis->setType(1);
	pThis->SetValueDouble(m_dRadius_mm);
	pThis->SetValueDescribe(_T("测量半径（mm）"));
	int rc = CPopMenuSettingWnd::MessageBox(m_hWnd);

	if (rc == MSGID_OK)
	{
		m_dRadius_mm = m_valueDoubleSetting;
		CString str;
		str.Format(_T("%.2f"), m_dRadius_mm);
		WritePrivateProfileString(_T("StagePara"), _T("radius_mm"), str.GetString(), lpINI);

	}
}

void CMainDlgWnd::On32840()
{
	TCHAR lpInfo[128];
	TCHAR lpBin[MAX_PATH];
	InitPath(lpBin, NULL, NULL);
	TCHAR lpINI[128];
	wsprintf(lpINI, _T("%s%s"), lpBin, _T("pst.ini"));
	int m_nX_step_um = GetPrivateProfileInt(_T("StagePara"), _T("X_step_um"), 300, lpINI);

	CPopMenuSettingWnd * pThis = CPopMenuSettingWnd::GetThis();
	pThis->setType(0);
	pThis->SetValueInt(m_nX_step_um);
	pThis->SetValueDescribe(_T("x向步长（um）"));
	int rc = CPopMenuSettingWnd::MessageBox(m_hWnd);

	if (rc == IDOK)
	{
		m_nX_step_um = m_valueIntSetting;
		CString str;
		str.Format(_T("%d"), m_nX_step_um);
		WritePrivateProfileString(_T("StagePara"), _T("X_step_um"), str, lpINI);
	}
}

void CMainDlgWnd::On32841()
{
	TCHAR lpInfo[128];
	TCHAR lpBin[MAX_PATH];
	InitPath(lpBin, NULL, NULL);
	TCHAR lpINI[128];
	wsprintf(lpINI, _T("%s%s"), lpBin, _T("pst.ini"));
	int m_nY_step_um = GetPrivateProfileInt(_T("StagePara"), _T("Y_step_um"), 300, lpINI);

	CPopMenuSettingWnd * pThis = CPopMenuSettingWnd::GetThis();
	pThis->setType(0);
	pThis->SetValueInt(m_nY_step_um);
	pThis->SetValueDescribe(_T("y向步长（um）"));
	int rc = CPopMenuSettingWnd::MessageBox(m_hWnd);

	if (rc == IDOK)
	{
		m_nY_step_um = m_valueIntSetting;
		CString str;
		str.Format(_T("%d"), m_nY_step_um);
		WritePrivateProfileString(_T("StagePara"), _T("Y_step_um"), str, lpINI);

	}
}

void CMainDlgWnd::On32842()
{
	TCHAR lpInfo[128];
	TCHAR lpBin[MAX_PATH];
	InitPath(lpBin, NULL, NULL);
	TCHAR lpINI[128];
	wsprintf(lpINI, _T("%s%s"), lpBin, _T("pst.ini"));
	int nValue = GetPrivateProfileInt(_T("CalCell"), _T("Shangpi_Num_Limit"), 8000, lpINI);

	CPopMenuSettingWnd * pThis = CPopMenuSettingWnd::GetThis();
	pThis->setType(0);
	pThis->SetValueInt(nValue);
	pThis->SetValueDescribe(_T("停止阈值"));
	int rc = CPopMenuSettingWnd::MessageBox(m_hWnd);

	if (rc == IDOK)
	{
		nValue = m_valueIntSetting;
		CString str;
		str.Format(_T("%d"), nValue);
		WritePrivateProfileString(_T("CalCell"), _T("Shangpi_Num_Limit"), str, lpINI);

	}
}

void CMainDlgWnd::On32814()
{
	OnBnClickedBtnInputinfo();
}

void CMainDlgWnd::On32830()
{
	TCHAR lpBin[MAX_PATH];
	InitPath(lpBin, NULL, NULL);
	TCHAR lpFName[256];
	wsprintf(lpFName, _T("%s%s"), lpBin, _T("cell_result.ini"));
	int linbaResult = GetPrivateProfileInt(_T("result"), _T("linba"), 0, lpFName);
	TCHAR lplinbaResult[64];

	CPopMenuInfoWnd::GetThis()->SetValueInt(linbaResult);
	CPopMenuInfoWnd::GetThis()->SetValueDescribe(_T("淋巴细胞数:"));
	CPopMenuInfoWnd::GetThis()->SetTitle(_T("淋巴细胞数"));
	CPopMenuInfoWnd::MessageBox(m_hWnd);
}

void CMainDlgWnd::On32831()
{
	TCHAR lpBin[MAX_PATH];
	InitPath(lpBin, NULL, NULL);
	TCHAR lpFName[128];
	wsprintf(lpFName, _T("%s%s"), lpBin, _T("cell_result.ini"));
	int shangpiResult = GetPrivateProfileInt(_T("result"), _T("linba"), 0, lpFName);
	TCHAR lplinbaResult[64];

	CPopMenuInfoWnd::GetThis()->SetValueInt(shangpiResult);
	CPopMenuInfoWnd::GetThis()->SetValueDescribe(_T("上皮细胞数:"));
	CPopMenuInfoWnd::GetThis()->SetTitle(_T("上皮细胞数"));
	CPopMenuInfoWnd::MessageBox(m_hWnd);
}

void CMainDlgWnd::On32836()
{
	TCHAR lpBin[MAX_PATH];
	InitPath(lpBin, NULL, NULL);
	TCHAR lpFName[128];
	wsprintf(lpFName, _T("%s%s"), lpBin, _T("cell_result.ini"));
	int danbeitiResult = GetPrivateProfileInt(_T("result"), _T("danbeiti"), 0, lpFName);

	CPopMenuInfoWnd::GetThis()->SetValueInt(danbeitiResult);
	CPopMenuInfoWnd::GetThis()->SetValueDescribe(_T("单倍体细胞数:"));
	CPopMenuInfoWnd::GetThis()->SetTitle(_T("单倍体细胞数"));
	CPopMenuInfoWnd::MessageBox(m_hWnd);
}

void CMainDlgWnd::On32837()
{
	TCHAR lpBin[MAX_PATH];
	InitPath(lpBin, NULL, NULL);
	TCHAR lpFName[128];
	wsprintf(lpFName, _T("%s%s"), lpBin, _T("cell_result.ini"));
	int erbeitiResult = GetPrivateProfileInt(_T("result"), _T("erbeiti"), 0, lpFName);

	CPopMenuInfoWnd::GetThis()->SetValueInt(erbeitiResult);
	CPopMenuInfoWnd::GetThis()->SetValueDescribe(_T("二倍体细胞数:"));
	CPopMenuInfoWnd::GetThis()->SetTitle(_T("二倍体细胞数"));
	CPopMenuInfoWnd::MessageBox(m_hWnd);
}

void CMainDlgWnd::On32838()
{
	TCHAR lpBin[MAX_PATH];
	InitPath(lpBin, NULL, NULL);
	TCHAR lpFName[128];
	wsprintf(lpFName, _T("%s%s"), lpBin, _T("cell_result.ini"));
	int sanbeitiResult = GetPrivateProfileInt(_T("result"), _T("sanbeiti"), 0, lpFName);

	CPopMenuInfoWnd::GetThis()->SetValueInt(sanbeitiResult);
	CPopMenuInfoWnd::GetThis()->SetValueDescribe(_T("三倍体细胞数:"));
	CPopMenuInfoWnd::GetThis()->SetTitle(_T("三倍体细胞数"));
	CPopMenuInfoWnd::MessageBox(m_hWnd);
}

void CMainDlgWnd::On32839()
{
	TCHAR lpBin[MAX_PATH];
	InitPath(lpBin, NULL, NULL);
	TCHAR lpFName[128];
	wsprintf(lpFName, _T("%s%s"), lpBin, _T("cell_result.ini"));
	int duobeitiResult = GetPrivateProfileInt(_T("result"), _T("duobeiti"), 0, lpFName);

	CPopMenuInfoWnd::GetThis()->SetValueInt(duobeitiResult);
	CPopMenuInfoWnd::GetThis()->SetValueDescribe(_T("多倍体细胞数:"));
	CPopMenuInfoWnd::GetThis()->SetTitle(_T("多倍体细胞数"));
	CPopMenuInfoWnd::MessageBox(m_hWnd);

}

void CMainDlgWnd::On32843()
{
	TCHAR lpFName[MAX_PATH];
	TCHAR lpPath[MAX_PATH];
	CharToTchar(m_lpResultPath, lpPath);
	wsprintf(lpFName, _T("%s%s"), lpPath, _T("CellsLambda.txt"));
	ShellExecute(NULL, _T("open"), lpFName, NULL, NULL, SW_SHOWNORMAL);
}

void CMainDlgWnd::On32844()
{
	TCHAR lpFName[MAX_PATH];
	TCHAR lpPath[MAX_PATH];
	CharToTchar(m_lpResultPath, lpPath);
	wsprintf(lpFName, _T("%s%s"), lpPath, _T("CellsName.txt"));
	ShellExecute(NULL, _T("open"), lpFName, NULL, NULL, SW_SHOWNORMAL);
}

void CMainDlgWnd::On32845()
{
	TCHAR lpFName[MAX_PATH];
	TCHAR lpPath[MAX_PATH];
	CharToTchar(m_lpResultPath, lpPath);
	wsprintf(lpFName, _T("%s%s"), lpPath, _T("CellsPixelArea.txt"));
	ShellExecute(NULL, _T("open"), lpFName, NULL, NULL, SW_SHOWNORMAL);
}

void CMainDlgWnd::On32846()
{
	TCHAR lpFName[MAX_PATH];
	TCHAR lpPath[MAX_PATH];
	CharToTchar(m_lpResultPath, lpPath);
	wsprintf(lpFName, _T("%s%s"), lpPath, _T("SortIndex.txt"));
	ShellExecute(NULL, _T("open"), lpFName, NULL, NULL, SW_SHOWNORMAL);
}

//生成报告
bool CMainDlgWnd::On32815(bool bModify, CString sBLH, CString sPropose)
{
	CDatabaseMgr* pMgr = CDatabaseMgr::GetInstance();
	_g_record temp;

	//
	if (!bModify )
	{
		if (gMainDlgWnd->IsDisposeCalc())
		{
			SOUND_WARNING
			::MessageBox(m_hWnd, _T("当前样本检测中，请等待操作完成."), _T("警告"), MB_OKCANCEL);
			return false;
		}

		if (g_PatientInfoCur.sName.IsEmpty() || g_PatientInfoCur.sBLH.IsEmpty())
		{
			SOUND_WARNING
			::MessageBox(m_hWnd, _T("请新建一个用户或者病历管理选择一个用户样本"), _T("警告"), MB_OKCANCEL);
			return false;
		}

		string sPathFile;
		USES_CONVERSION;
		sPathFile = W2A(m_FileDir);
		if (sPathFile.length() == 0)
		{
			::MessageBox(m_hWnd, _T("请选择制片或者打开档案路径"), _T("警告"), MB_OKCANCEL);
			return false;
		}
		if (vCellsfullfilenames.size() == 0)
		{
			SOUND_WARNING
			::MessageBox(m_hWnd, _T("请先进行实时筛选"), _T("警告"), MB_OKCANCEL);
			return false;
		}
		// 	if (!m_bTatisticsInfo)
		// 	{
		// 		::MessageBox(m_hWnd, _T("请查看统计信息"), _T("警告"), MB_OK);
		// 		return;
		// 	}
		if (m_pBlockPage[0].wSelCount < _cnt_sel_cell)
		{
			SOUND_WARNING
			::MessageBox(m_hWnd, _T("请选择20个合适的细胞."), _T("警告"), MB_OKCANCEL);
			return false;
		}
		if (!m_bStatisticsShow)
		{
			SOUND_WARNING
			int iRet = ::MessageBox(m_hWnd, _T("请查看统计结果"), _T("警告"), MB_OKCANCEL);
			if (iRet == IDOK)
			{
				ShowStatisticResult();
			}
			return false;
		}

		CStringA aPath;
		aPath = m_FileDir;
		string _filepath;
		_filepath = aPath + "Result\\";
		string c_s5 = "2.jpg";
		string c_s7 = _filepath + c_s5;
		CString sFile(c_s7.c_str());
		if (!PathFileExists(sFile.GetString()) || !m_bSelFirstCell)
		{
			SOUND_WARNING
			::MessageBox(m_hWnd, _T("请查看第一个选择的细胞图片"), _T("警告"), MB_OKCANCEL);
			int page, uiIndex;
			bool b = FindIndex(0, page, uiIndex);
			if (b)
			{
				m_wSelPage = page;
				SelPage(m_wSelPage);

				int row = uiIndex / _column_cnt;
				int column = uiIndex % _column_cnt;
				SetShowCell(row, column);
			}
			return false;
		}

		CString path[_cnt_sel_cell], edit[_cnt_sel_cell];
		bool b = GetSelCell(path, edit);
		if (b)
		{
			CStringA aPath;
			aPath = m_FileDir;
			string simg = aPath + "Result\\1.jpg";
			CString str(simg.c_str());
			SaveCellImage(path, edit, str);
		}
	}

	DWORD pid;
	bool bFind = FindProcess(_T("AcroRd32.exe"), pid);
	if (bFind)
	{
		KillProcess(pid);
	}
	bFind = FindProcess(_T("AcroRd32.exe"), pid);
	if (bFind)
	{
		KillProcess(pid);
	}

	HPDF_Doc pdf;
	pdf_err = 0;
	pdf = HPDF_New(error_handler, NULL);
	if (!pdf)
	{
		//AfxMessageBox("错误:创建PDF失败！");
		SOUND_WARNING
		MessageBoxW(m_hWnd, _T("错误:创建PDF失败！"), _T(""), 0);
		return false;//FALSE;
	}
	if (setjmp(env))
	{
		HPDF_Free(pdf);
		if (pdf_err)
		{
			//AfxMessageBox("错误:生成PDF过程中遇见错误！");
		//	MessageBoxW(m_hWnd, _T("错误:生成PDF过程中遇见错误！"), _T(""), 0);
			//::MessageBox(NULL, _T("请查看[统计信息][查看统计结果][查看第一个细胞图片(最后一个查看)]"), _T("警告"), MB_OK);
		}
		else
		{
			SOUND_WARNING
			MessageBoxW(m_hWnd, _T("错误:生成PDF过程中遇见错误！"), _T(""), 0);
		}
		return false;//FALSE;
	}

	//2.设置文档对象/////////////////////////////////////////////////////////////////////////////////
	//set compression mode 
	HPDF_SetCompressionMode(pdf, HPDF_COMP_ALL);
	//set page mode to use outlines
	HPDF_SetPageMode(pdf, HPDF_PAGE_MODE_USE_OUTLINE);

	//3.创建Page对象////////////////////////////////////////////////////////////////////////////////
	HPDF_Page page;
	page = HPDF_AddPage(pdf);
	HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A4, HPDF_PAGE_PORTRAIT);
	///HPDF_PAGE_SIZE_A4  595*841(in pixel)
	///HPDF_PAGE_LANDSCAPE A4倒方向 ；HPDF_PAGE_PORTRAIT 正向方向

	///3.1输出字符串//////////////////////////////////////////////
	CString str_Showtxt;
	HPDF_Page_BeginText(page);
	///使用条件：当前graphics mode为HPDF_GMODE_PAGE_DESCRIPTION,函数将当前模式改为HPDF_GMODE_TEXT_OBJECT
	///开始text对象，设置当前text的位置到点（0，0）
	HPDF_Font font;
	/* get default font */
	HPDF_UseCNSFonts(pdf);
	HPDF_UseCNSEncodings(pdf);
	font = HPDF_GetFont(pdf, "SimHei", "GB-EUC-H");///SimSun宋体、SimHei黑体

	///显示报告标题
	//HPDF_Page_SetFontAndSize (page, font, 24);//注：字体显示的起始位置指字体左下角的位置
	//HPDF_Page_MoveTextPos (page, HPDF_Page_GetWidth(page)/4, HPDF_Page_GetHeight(page)-60);
	//	/////使用条件：graphics mode of the page is in HPDF_GMODE_TEXT_OBJECT
	//	/////当前位置（x1,y1）,HPDF_Page_MoveTextPos (page, x ,y)移动后的位置（x1+x,y1+y）
	//str_Showtxt="xxx检测结果";
	/*char lpStr[128];
	sprintf(lpStr, "%s", str_Showtxt);
	HPDF_Page_ShowText(page, lpStr);///显示字符串方法1*/

	CTime Curtime;
	Curtime = CTime::GetCurrentTime();
	CString strCurT, time1, time2;
	time1 = Curtime.Format("%Y-%m-%d,");
	time2 = Curtime.Format("%H:%M:%S");
	strCurT = time1 + time2;
	///显示测试时间
	/*HPDF_Page_SetFontAndSize(page, font, 12);
	char lpT[128];
	sprintf(lpT, "%s", strCurT);
	HPDF_Page_TextOut(page, HPDF_Page_GetWidth(page) * 3 / 4, HPDF_Page_GetHeight(page) - 80, lpT);///显示字符串方法2*/



	HPDF_Page_EndText(page);
	///将graphics mode 改为:HPDF_GMODE_PAGE_DESCRIPTION
	///必须有HPDF_Page_BeginText (page);HPDF_Page_EndText (page);否则保存PDF失败。


	///3.2 画线/////////////////////////////////////////////////
	///画线4步骤：设置、起点、LineTo、Stroke
	int i = 0;
	///*3.2.1 Draw horizontal lines. */
	///设置线宽
	HPDF_Page_SetLineWidth(page, 1.5);
	//起点
	HPDF_Page_MoveTo(page, 40, HPDF_Page_GetHeight(page) - 150);//横线1
	//画线
	HPDF_Page_LineTo(page, HPDF_Page_GetWidth(page) - 40, HPDF_Page_GetHeight(page) - 150);

	HPDF_Page_MoveTo(page, 40, HPDF_Page_GetHeight(page) - 220);//横线1
	//画线
	HPDF_Page_LineTo(page, HPDF_Page_GetWidth(page) - 40, HPDF_Page_GetHeight(page) - 220);

	HPDF_Page_MoveTo(page, 50, 230);//横线1
	//画线
	HPDF_Page_LineTo(page, HPDF_Page_GetWidth(page) - 50, 230);

	HPDF_Page_MoveTo(page, 50, 166);//横线1
	//画线
	HPDF_Page_LineTo(page, HPDF_Page_GetWidth(page) - 50, 166);

	HPDF_Page_MoveTo(page, 50, HPDF_Page_GetHeight(page) - 735);//横线1
	//画线
	HPDF_Page_LineTo(page, HPDF_Page_GetWidth(page) - 50, HPDF_Page_GetHeight(page) - 735);
	///* 3.2.2 Draw vertical lines. */
	//HPDF_Page_MoveTo (page, 50, HPDF_Page_GetHeight(page)-90);//第一根竖线
	//HPDF_Page_LineTo (page, 50, HPDF_Page_GetHeight(page)-495);

	///画线结束
	///change the graphics to HPDF_GMODE_PAGE_DESCRIPION
	HPDF_Page_Stroke(page);

	//查找首个细胞的索引
	int uipage=-1, uiIndex=-1, lastIndex=-1;
	bool bFindIndex = FindIndex(1, uipage, uiIndex);
	if (bFindIndex)
	{
		lastIndex = uipage * _cnt() + uiIndex;
	}

	//4.Save PDF//////////////////////////////////////////////////////////////////////////////////////
	///字符数组拼接字符串的方法：///char fname[256]; strcpy (fname, argv[0]); strcat (fname, ".pdf");
	///文件夹必须是之前创建好的，如果文件夹不存在，不会自动创建
	///路径不存在就创建,PathFileExists检验路径是否有效，有效返回true
	/*
	if (!PathFileExists("D:\\TestPdf\\"))
	{//当天保存目录
	_mkdir("D:\\TestPdf\\");
	}
	*/

	int num_remaincells = nCountCells - lastIndex + 1 + 20;
	int sum1 = nCountNUMArray[80];/*sum1为c值大于8细胞总和*/
	int sum2 = 0;//sum1为c值大于5的细胞总数
	for (int i = 50; i < 80; ++i)
	{
		sum2 += nCountNUMArray[i];
	}
	int sum3 = 0;//sum2为c值大于4的细胞总数
	for (int i = 35; i <= 49; ++i)
	{
		sum3 += nCountNUMArray[i];
	}
	int sum4 = 0;
	for (int i = 25; i <= 34; ++i)//sum3为c值大于2.5的细胞总数
	{
		sum4 += nCountNUMArray[i];
	}
	int sum5 = 0;//sum4为c大于1.5的细胞总数
	for (int i = 15; i <= 24; ++i)
	{
		sum5 += nCountNUMArray[i];
	}
	char zd[128];
	char jy[128];
	if (sum1 > 0)
	{
		sprintf(zd, "%s%s", " 诊断结果:","多为肿瘤细胞");
		sprintf(jy, "%s%s", " 建    议:","请做阴道镜检查及活体组织检查");
	}
	else if ( sum2 < 3 && sum2 > 0)
	{
		sprintf(zd, "%s%s", " 诊断结果:","HPV感染");
		sprintf(jy, "%s%s", " 建    议:","4-6个月复查");
	}
	else if (sum2 >= 3)
	{
		sprintf(zd, "%s%s", " 诊断结果:","肿瘤或癌前病变");
		sprintf(jy, "%s%s", " 建    议:","请做阴道镜检查及活体组织检查");
	}
	else if (sum3 >= num_remaincells * 0.1)
	{
		sprintf(zd, "%s%s", " 诊断结果:","肿瘤或癌前病变");
		sprintf(jy, "%s%s", " 建    议:","请做阴道镜检查及活体组织检查");
	}
	else if (num_remaincells * 0.05 <= sum3 && sum3 < num_remaincells * 0.1)
	{
		sprintf(zd, "%s%s", " 诊断结果:","部分细胞为增生");
		sprintf(jy, "%s%s", " 建    议:","定期复查(1年)");
	}
	else if (sum3 <= num_remaincells * 0.05 && sum3 > 0)
	{
		sprintf(zd, "%s%s", " 诊断结果:","正常增殖周期细胞");
		sprintf(jy, "%s%s", " 建    议:","定期复查(1年)");
	}
	else if (sum4 > num_remaincells * 0.1)
	{
		sprintf(zd, "%s%s", " 诊断结果:","恶性肿瘤可能性大");
		sprintf(jy, "%s%s", " 建    议:","请做阴道镜检查及活体组织检查");
	}
	else if (num_remaincells * 0.05 >= sum4 && sum4 > 0)
	{
		sprintf(zd, "%s%s", " 诊断结果:","正常细胞");
		sprintf(jy, "%s%s", " 建    议:","定期复查(1年)");
	}
	else
	{
		sprintf(zd, "%s%s", " 诊断结果:","正常细胞");
		sprintf(jy, "%s%s", " 建    议:","定期复查(1年)");
	}

	//新的诊断结果和建议
	int nCnt[6] = { 0 };
	for (int i = 0; i < nCountCells; i++)
	{
		if (!IsDiIndex(i))
			continue;
		double d = DI[i];
		if (0.5<=DI[i] && DI[i]<=1.25 ) //0  //二倍体细胞
		{
			nCnt[0]++;
		}
		else if (1.25 < DI[i] && DI[i]<1.8) //1
		{
			nCnt[1]++;
		}
		else if (1.8 <= DI[i] && DI[i]<2.3) //2
		{
			nCnt[2]++;
		}
		else if (2.5 <= DI[i]) //3
		{
			nCnt[3]++;
		}
		else if (4.5 <= DI[i])//4
		{
			nCnt[4]++;
		}
		else
		{
			TCHAR buf[256] = { 0 };
			_stprintf(buf, _T("KKKK:%0.3f\n"), d);
			OutputDebugString(buf);
			nCnt[5]++;
		}
	}
	int result[11][4] = { 0 }; //0 col DNA诊断结果索引  1 col DNA诊断结果[0或1] 2 col处理意见[0-2] 3 col di等级[0-5]
	//int r1[11] = { 0, 1, 6, 3, 4, 7, 2, 5, 8, 9, 10 };
	for (int i = 0; i < 11; ++i)
		result[i][0] = i /*r1[i]*/;
	CStringA sResult1[11] = 
	{ 
	"未见DNA倍体异常细胞",		//[0]  2c  定期复查(1年)										
	"未见DNA倍体异常细胞",		//[1]  3c  定期复查(1年)	   (3c细胞数小于二倍体细胞数的10%)		
	"有异倍体细胞峰值出现",		//[2]  3c  阴道镜检查及活检	   (3c细胞数大于等于二倍体细胞数的10%)	
	"未见DNA倍体异常细胞",		//[3]  4c  定期复查(1年)	   (4c细胞小于二倍体细胞数的5%)			
	"有部分细胞为增生",			//[4]  4c  3-6月复查		   (4c细胞数在二倍体细胞数的5%和10%之间)	
	"有部分细胞为异常增生",		//[5]  4c  阴道镜检查及活检    (4c细胞数大于等于二倍体细胞数的10%)	
	"未见DNA倍体异常细胞",		//[6]  5c  定期复查(1年)	   (5c细胞数为0个)	 未见5c细胞					
	"可见少量DNA倍体异常细胞",  //[7]  5c  3-6月复查		   (5c细胞数为1至2个)						
	"可见DNA倍体异常细胞",		//[8]  5c  阴道镜检查及活检    (5c细胞数为3至15之间)                  
	"可见大量DNA倍体异常细胞",  //[9]  5c  阴道镜检查及活检    (5c细胞数大于15个)						
	"疑似肿瘤细胞"			   //[10]  9c  阴道镜检查及活检    (9c细胞大于0)			                	                                                                                             
	},
		sResult2[3] = { //0-2
		"定期复查(1年)",
		"3～6月复查",
		"阴道镜检查及活体组织检查"
		};
	BYTE by[11] = { 0, 0, 2, 0, 1, 2, 0, 1, 2, 2, 2 };
	//数目判断条件 
	//9c细胞
	if (nCnt[4]>0) 
	{
		result[10][1] = 1;
		result[10][2] = by[10];
		result[10][3] = 4;
	}
	//5c细胞
	int nTemp = nCnt[3];
	if (nTemp == 0) 
	{
		result[6][1] = 1;
		result[6][2] = by[6];
		result[6][3] = 3;
	}
	else if (nTemp >= 1 && nTemp <= 2)
	{
		result[7][1] = 1;
		result[7][2] = by[7];
		result[7][3] = 3;
	}
	else if (nTemp>=3 && nTemp<=15)
	{
		result[8][1] = 1;
		result[8][2] = by[8];
		result[8][3] = 3;
	}
	else if (nTemp>15)
	{
		result[9][1] = 1;
		result[9][2] = by[9];
		result[9][3] = 3;
	}
	//4c细胞
	nTemp = nCnt[2];
	int n2CCount = nCnt[0];
	if (nTemp>0 && nTemp < n2CCount*1.0 / 100 * 5)
	{
		result[3][1] = 1;
		result[3][2] = by[3];
		result[3][3] = 2;
	}
	else if (nTemp >= n2CCount*1.0 / 100 * 5 && nTemp <= n2CCount*1.0 / 100 * 10)
	{
		result[4][1] = 1;
		result[4][2] = by[4];
		result[4][3] = 2;
	}
	else if (nTemp > n2CCount*1.0 / 100 * 10)
	{
		result[5][1] = 1;
		result[5][2] = by[5];
		result[5][3] = 2;
	}
	//非整倍体细胞 3c
	nTemp = nCnt[1];
	if (nTemp > 0 && nTemp < n2CCount*1.0 / 100 * 10)
	{
		result[1][1] = 1;
		result[1][2] = by[1];
		result[1][3] = 1;
	}
	else if (nTemp >= n2CCount*1.0 / 100 * 10)
	{
		result[2][1] = 1;
		result[2][2] = by[2];
		result[2][3] = 1;
	}
	//2c
	if (n2CCount>0)
	{
		result[0][1] = 1;
		result[0][2] = by[0];
		result[0][3] = 0;
	}
	int a[11] = { -1 };
	int aC = 0;
	int iYJ = -1;
	for (int i = 0; i < 11; ++i)
	{
		if (result[i][1]==1 && result[i][2] > iYJ)
		{
			iYJ = result[i][2];
		}
	}
	for (int i = 0; i < 11; ++i)
	{
		if (result[i][1] == 1 && result[i][2] == iYJ)
		{
			a[aC++] = i;
		}
	}
	if (aC>0)
	{
		int index = a[aC - 1];
		int idx = result[index][0];
		CStringA s1 = sResult1[idx];
		int idx1 = result[index][2];
		CStringA s2 = sResult2[idx1];
		//一年复查修正
		if (idx1 == 0 && nCnt[2]>=15)
		{
// 			idx1 = 1;
// 			s2 = sResult2[idx1];
//			MessageBox(NULL, _T("aaaa"), _T(""), MB_OK);
		}
		OutputDebugStringA(s1.GetString());
		OutputDebugStringA("\n");
		OutputDebugStringA(s2.GetString());
		OutputDebugStringA("\n");

		sprintf(zd, "%s%s", " 诊断结果:", s1.GetString());
		sprintf(jy, "%s%s", " 建    议:", s2.GetString());
	}


	if (!bModify)
		sBLH = g_PatientInfoCur.sBLH;
	_g_record t;
	pMgr->FindInfoByID(sBLH, t);
	if (bModify)
	{
		CStringA strA = (CStringA)sPropose;
		sprintf(jy, "%s%s", " 建    议: ", strA.GetString());
	}
	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 26);
	float fw = HPDF_Page_TextWidth(page, "细胞DNA定量分析报告");
	float pdf_w = HPDF_Page_GetWidth(page);
	HPDF_Page_MoveTextPos(page, (pdf_w-fw)/2, HPDF_Page_GetHeight(page) - 130);
	HPDF_Page_ShowText(page, "细胞DNA定量分析报告");
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 50, HPDF_Page_GetHeight(page) - 167);
	HPDF_Page_ShowText(page, " 标本编号:  ");
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 105, HPDF_Page_GetHeight(page) - 167);
	CStringA sBingLiHao;
// 	if (g_ModifyReportInfo.bValid)
// 		sBingLiHao = (CStringA)g_ModifyReportInfo.bingLiHao;
// 	else
// 		sBingLiHao = g_PatiengInfo.lpBLId;
	sBingLiHao = sBLH;

	HPDF_Page_ShowText(page, sBingLiHao.GetString());
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 50, HPDF_Page_GetHeight(page) - 187);
	HPDF_Page_ShowText(page, " 标本类型:  ");
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 50, HPDF_Page_GetHeight(page) - 207);
	HPDF_Page_ShowText(page, " 联系方式:  ");
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 105, HPDF_Page_GetHeight(page) - 207);
	CStringA sMobile(g_PatiengInfo.lpPhoneNum);
	if (bModify)
	{
		CString a;
		int b;
		bool ret = t.getdata(_T("PhoneNum"), a, b);
		assert(ret);
		sMobile = (CStringA)a;
	}
	temp.setdata(_T("PhoneNum"), sMobile.GetString(), -1);
	HPDF_Page_ShowText(page, sMobile.GetString());
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 175, HPDF_Page_GetHeight(page) - 167);
	HPDF_Page_ShowText(page, " 姓名:  ");
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 210, HPDF_Page_GetHeight(page) - 167);
	CStringA slpName(g_PatiengInfo.lpName);
	if (bModify)
	{
		CString a;
		int b;
		bool ret = t.getdata(_T("patient_name"), a, b);
		assert(ret);
		slpName = (CStringA)a;
	}
	temp.setdata(_T("patient_name"), slpName.GetString(), -1);
	HPDF_Page_ShowText(page, slpName.GetString());
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 175, HPDF_Page_GetHeight(page) - 187);
	HPDF_Page_ShowText(page, " 年龄:  ");
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 210, HPDF_Page_GetHeight(page) - 187);
	CStringA sAge;
	sAge.Format("%d", g_PatiengInfo.nAge);
	if (bModify)
	{
		CString a;
		int b;
		bool ret = t.getdata(_T("age"), a, b);
		assert(ret);
		sAge = (CStringA)a;
	}
	int iage = atoi(sAge.GetString());
	temp.setdata(_T("age"), _T(""), iage);
	HPDF_Page_ShowText(page, sAge.GetString());
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 175, HPDF_Page_GetHeight(page) - 207);
	HPDF_Page_ShowText(page, " 性别:  ");
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 210, HPDF_Page_GetHeight(page) - 207);
	CStringA sSex = _T("男");
	if (g_PatiengInfo.nSex == 1)
		sSex = _T("女");
	int isex = 1;
	if (bModify)
	{
		CString a;
		int b;
		bool ret = t.getdata(_T("sex"), a, b);
		assert(ret);
		isex = b;
		if (b == 1)
			sSex = "女";
	}
	temp.setdata(_T("sex"), _T(""), isex);
	HPDF_Page_ShowText(page, sSex.GetString());
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 300, HPDF_Page_GetHeight(page) - 167);
	HPDF_Page_ShowText(page, " 住院号:  ");
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 354, HPDF_Page_GetHeight(page) - 167);
	CStringA azyh;
	{
		CString a;
		int b;
		bool ret = t.getdata(_T("zhu_yuan_hao"), a, b);
		assert(ret);
		azyh = (CStringA)a;
	}
	temp.setdata(_T("zhu_yuan_hao"), azyh.GetString(), -1);
	HPDF_Page_ShowText(page, azyh.GetString());
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 300, HPDF_Page_GetHeight(page) - 187);
	HPDF_Page_ShowText(page, " 床号:  ");
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 354, HPDF_Page_GetHeight(page) - 187);
	CStringA sch;
	{
		CString a;
		int b;
		bool ret = t.getdata(_T("chuang_hao"), a, b);
		assert(ret);
		sch = (CStringA)a;
	}
	temp.setdata(_T("chuang_hao"), sch.GetString(), -1);
	HPDF_Page_ShowText(page, sch.GetString());
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 300, HPDF_Page_GetHeight(page) - 207);
	HPDF_Page_ShowText(page, " 取样日期:  ");
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 354, HPDF_Page_GetHeight(page) - 207);
	CStringA sdate;
	{
		CString a;
		int b;
		bool ret = t.getdata(_T("create_date"), a, b);
		assert(ret);
		sdate = (CStringA)a;
	}
	sdate.Trim();
	int ppos = sdate.Find(' ', 1);
	int iloc1 = 10;
	if (ppos > 0)
		iloc1 = ppos;
	temp.setdata(_T("create_date"), sdate.GetString(), -1);
	CStringA sdate1 = sdate.Mid(0, iloc1);
	HPDF_Page_ShowText(page, sdate1.GetString());
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 425, HPDF_Page_GetHeight(page) - 167);
	HPDF_Page_ShowText(page, " 送检医院:  ");
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 479, HPDF_Page_GetHeight(page) - 167);
	CStringA shospital;
	{
		CString a;
		int b;
		bool ret = t.getdata(_T("checking_hospital"), a, b);
		assert(ret);
		shospital = (CStringA)a;
	}
	temp.setdata(_T("checking_hospital"), shospital.GetString(), -1);
	HPDF_Page_ShowText(page, shospital.GetString());
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 425, HPDF_Page_GetHeight(page) - 187);
	HPDF_Page_ShowText(page, " 送检科: ");
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 479, HPDF_Page_GetHeight(page) - 187);
	CStringA sdepart;
	{
		CString a;
		int b;
		bool ret = t.getdata(_T("checking_depart"), a, b);
		assert(ret);
		sdepart = (CStringA)a;
	}
	temp.setdata(_T("checking_depart"), sdepart.GetString(), -1);
	HPDF_Page_ShowText(page, sdepart.GetString());
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 425, HPDF_Page_GetHeight(page) - 207);
	HPDF_Page_ShowText(page, " 送检医生: ");
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 479, HPDF_Page_GetHeight(page) - 207);
	CStringA sdoctor;
	{
		CString a;
		int b;
		bool ret = t.getdata(_T("checking_doctor"), a, b);
		assert(ret);
		sdoctor = (CStringA)a;
	}
	temp.setdata(_T("checking_doctor"), sdoctor.GetString(), -1);
	HPDF_Page_ShowText(page, sdoctor.GetString());
	HPDF_Page_EndText(page);

	HPDF_Page_SetFontAndSize(page, font, 8);
	CString str1;

	//draw_image(pdf, W2A(str1.GetBuffer), 110, 250, "");
	CStringA aPath;
	aPath = m_FileDir;
	string _filepath;
	_filepath = aPath + "Result\\";
	string c_s3 = "DNA_Content_Cell_Area_Scatter_Diagram.jpg";
	string c_s4 = "DNA_Content_Cell_Number_Histogram.jpg";
	string c_s5 = "1.jpg";
	string c_s6 = "2.jpg";
	string c_s1 = _filepath + c_s3;
	string c_s2 = _filepath + c_s4;
	string c_s7 = _filepath + c_s5;
	string c_s8 = _filepath + c_s6;

	bool bExist = true;
	if (bModify)
	{
		CString a; int b; CStringA str; bool ret = false;
		CString img1, img2, img3, img4;
		ret = t.getdata(_T("pdf_img1"), a, b); assert(ret);
		img1 = a;
		ret = t.getdata(_T("pdf_img2"), a, b); assert(ret);
		img2 = a;
		ret = t.getdata(_T("pdf_img3"), a, b); assert(ret);
		img3 = a;
		ret = t.getdata(_T("pdf_img4"), a, b); assert(ret);
		img4 = a;
		if (!PathFileExists(img1.GetString()) || !PathFileExists(img2.GetString()) ||
			!PathFileExists(img3.GetString()) || !PathFileExists(img4.GetString()))
		{
			bExist = false;
		}
	}
	else
	{
		if (!PathFileExistsA(c_s1.c_str()) || !PathFileExistsA(c_s2.c_str()) ||
			!PathFileExistsA(c_s7.c_str()) || !PathFileExistsA(c_s8.c_str()))
		{
			bExist = false;
		}
	}
	if (!bExist)
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("当前样本检测结果数据损坏，请重新检测!"), _T("提示"), MB_OKCANCEL);
		m_bIsDisposeBegin = false;
		m_bIsDisposeEnd = false;
		m_bIsDisposeCalc = false;
		return false;
	}

	Mat SrcImage = imread(c_s7, CV_LOAD_IMAGE_UNCHANGED);
	int height = SrcImage.rows;
	int width = SrcImage.cols;

	cv::imwrite(c_s7, SrcImage);

	const char* p1 = c_s1.data(); //diagram 散点图
	const char* p2 = c_s2.data(); //histogram 直方图
	const char* p3 = c_s7.data(); //1.jpg 选择的细胞 
	const char* p4 = c_s8.data(); //2.jpg 第一张选择的细胞
	char buff[1000];
	_getcwd(buff, 1000);
	string	path3 = buff;
	path3 = path3 + "\\LOGO.jpg";
	const char* p5 = path3.data();
	draw_image1(pdf, p5, 30, 740, "");

	if (bModify)
	{
		CString a;
		int b;
		CStringA str;
		bool ret = false;

		ret = t.getdata(_T("pdf_img1"), a, b);
		assert(ret);
		temp.setdata(_T("pdf_img1"), a, -1);
		str = (CStringA)a;
		draw_image(pdf, str.GetString(), 110, 440, " DNA含量（c）正常<5C  ");

		ret = t.getdata(_T("pdf_img2"), a, b);
		assert(ret);
		temp.setdata(_T("pdf_img2"), a, -1);
		str = (CStringA)a;
		draw_image(pdf, str.GetString(), 320, 440, " DNA含量（c）正常<5C ");

		ret = t.getdata(_T("pdf_img3"), a, b);
		assert(ret);
		temp.setdata(_T("pdf_img3"), a, -1);
		str = (CStringA)a;
		draw_image2(pdf, str.GetString(), 80, 250, ""); 

		ret = t.getdata(_T("pdf_img4"), a, b);
		assert(ret);
		temp.setdata(_T("pdf_img4"), a, -1);
		str = (CStringA)a;
		draw_image2(pdf, str.GetString(), 310, 250, ""); 
	}
	else
	{
		draw_image(pdf, p1,60, 440, " DNA含量（c）正常<5C  ");
		draw_image(pdf, p2, 289, 440, " DNA含量（c）正常<5C ");
		draw_image2(pdf, p3, 80, 250, ""); 
		draw_image2(pdf, p4, 310, 250, ""); 

		temp.setdata(_T("pdf_img1"), p1, -1);
		temp.setdata(_T("pdf_img2"), p2, -1);
		temp.setdata(_T("pdf_img3"), p3, -1);
		temp.setdata(_T("pdf_img4"), p4, -1);
	}

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 60, 210);
	HPDF_Page_ShowText(page, " 细胞种类:");
	HPDF_Page_EndText(page);

#define delta_x_kind 20
#define x_2 143
#define x_3 187
#define x_4 228
#define x_5 294
#define x_6 369
#define x_7 462
	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, x_2, 210);
	HPDF_Page_ShowText(page, "2c");
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, x_3, 210);
	HPDF_Page_ShowText(page, "3c");
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, x_4, 210);
	HPDF_Page_ShowText(page, "多倍体");
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, x_5, 210);
	HPDF_Page_ShowText(page, "淋巴细胞");
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, x_6, 210);
	HPDF_Page_ShowText(page, "有效上皮细胞");
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, x_7, 210);
	HPDF_Page_ShowText(page, "上皮细胞总数");
	HPDF_Page_EndText(page);

	TCHAR lpBin[MAX_PATH];
	InitPath(lpBin, NULL, NULL);
	TCHAR lpFName[128];
	wsprintf(lpFName, _T("%s\\%s"), m_FileDir.GetString(), _T("cell_result.ini"));
	int ShangPiResult = GetPrivateProfileInt(_T("result"), _T("shangpi"), 0, lpFName);
	int linbaResult = GetPrivateProfileInt(_T("result"), _T("linba"), 0, lpFName);
	int danbeitiResult = GetPrivateProfileInt(_T("result"), _T("danbeiti"), 0, lpFName);
	int erbeitiResult = GetPrivateProfileInt(_T("result"), _T("erbeiti"), 0, lpFName);
	int sanbeitiResult = GetPrivateProfileInt(_T("result"), _T("sanbeiti"), 0, lpFName);
	int duobeitiResult = GetPrivateProfileInt(_T("result"), _T("duobeiti"), 0, lpFName);
	int xianxingliResult = GetPrivateProfileInt(_T("result"), _T("xianxingli"), 0, lpFName);

	char lpShangPiResult[64];
	sprintf(lpShangPiResult, "%d", erbeitiResult+sanbeitiResult+duobeitiResult);
	char lplinbaResult[64];
	sprintf(lplinbaResult, "%d", linbaResult);
	char lpdanbeitiResult[64];
	sprintf(lpdanbeitiResult, "%d", danbeitiResult);
	char lperbeitiResult[64];
	sprintf(lperbeitiResult, "%d", erbeitiResult);
	char lpsanbeitiResult[64];
	sprintf(lpsanbeitiResult, "%d", sanbeitiResult);
	char lpduobeitiResult[64];
	sprintf(lpduobeitiResult, "%d", duobeitiResult);
	char lpxianxingliResult[64];
	sprintf(lpxianxingliResult, "%d", xianxingliResult);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 60, 185);
	HPDF_Page_ShowText(page, " 细胞数量:");
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, x_2, 185);
	CStringA sAtt1(lperbeitiResult);
	if (bModify)
	{
		CString a; int b;
		bool ret = t.getdata(_T("attribute1"), a, b);
		assert(ret);
		sAtt1 = (CStringA)a;
	}
	temp.setdata(_T("attribute1"), sAtt1.GetString(), -1);
	HPDF_Page_ShowText(page, sAtt1.GetString());
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, x_3, 185);
	CStringA sAtt2(lpsanbeitiResult);
	if (bModify)
	{
		CString a; int b;
		bool ret = t.getdata(_T("attribute2"), a, b);
		assert(ret);
		sAtt2 = (CStringA)a;
	}
	temp.setdata(_T("attribute2"), sAtt2.GetString(), -1);
	HPDF_Page_ShowText(page, sAtt2.GetString());
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, x_4, 185);
	CStringA sAtt3(lpduobeitiResult);
	if (bModify)
	{
		CString a; int b;
		bool ret = t.getdata(_T("attribute3"), a, b);
		assert(ret);
		sAtt3 = (CStringA)a;
	}
	temp.setdata(_T("attribute3"), sAtt3.GetString(), -1);
	HPDF_Page_ShowText(page, sAtt3.GetString());
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, x_5, 185);
	CStringA sAtt4(lplinbaResult);
	if (bModify)
	{
		CString a; int b;
		bool ret = t.getdata(_T("attribute4"), a, b);
		assert(ret);
		sAtt4 = (CStringA)a;
	}
	temp.setdata(_T("attribute4"), sAtt4.GetString(), -1);
	HPDF_Page_ShowText(page, sAtt4.GetString());
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, x_6, 185);
	CStringA sAtt5(lpShangPiResult);
	if (bModify)
	{
		CString a; int b;
		bool ret = t.getdata(_T("attribute5"), a, b);
		assert(ret);
		sAtt5 = (CStringA)a;
	}
	temp.setdata(_T("attribute5"), sAtt5.GetString(), -1);
	HPDF_Page_ShowText(page, sAtt5.GetString());
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, x_7, 185);
	CStringA sAtt6;
	sAtt6.Format("%d", nCountCellsAll);
	if (bModify)
	{
		CString a; int b;
		bool ret = t.getdata(_T("reseve1"), a, b);
		assert(ret);
		sAtt6 = (CStringA)a;
	}
	temp.setdata(_T("reseve1"), sAtt6.GetString(), -1);
	HPDF_Page_ShowText(page, sAtt6.GetString());
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 60, 140);
	//HPDF_Page_ShowText(page, " 诊断结果： DNA意见 ");
	CStringA strZD(zd);
	if (bModify)
	{
		CString a; int b;
		bool ret = t.getdata(_T("diagnose_result"), a, b);
		assert(ret);
		strZD = (CStringA)a;
	}
	temp.setdata(_T("diagnose_result"), strZD.GetString(), -1);
	HPDF_Page_ShowText(page, strZD.GetString());
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 420, 140);
	HPDF_Page_ShowText(page, " 报告医师签名: ");
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 60, 120);
	//HPDF_Page_ShowText(page, " 建   议: ");
	CStringA strJY(jy);
	if (bModify)
	{
// 		CString a; int b;
// 		bool ret = t.getdata(_T("diagnose_propose"), a, b);
		assert(ret);
// 		strJY = (CStringA)
		strJY = jy;
	}
	temp.setdata(_T("diagnose_propose"), strJY.GetString(), -1);
	HPDF_Page_ShowText(page, strJY.GetString());
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 420, 120);
	HPDF_Page_ShowText(page, " 日       期: ");
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 60, 85);
	HPDF_Page_ShowText(page, " 报告说明: ");
	HPDF_Page_EndText(page);

	HPDF_Page_BeginText(page);
	HPDF_Page_SetFontAndSize(page, font, 10);
	HPDF_Page_MoveTextPos(page, 60, 65);
	HPDF_Page_ShowText(page, " 本报告只对此标本负责，仅供临床医生参考。 ");
	HPDF_Page_EndText(page);

	char Save_path[128];

	const char* p6 = _filepath.data();
	char name2[MAX_PATH];
	TcharToChar(g_PatiengInfo.lpBLId, name2);
	//sprintf(Save_path,p6 , time1, "xxx.pdf");
	sprintf(Save_path, "%s%s.pdf", p6, name2);
	if (bModify)
	{
		CStringA str66;
		CString a; int b;
		bool ret = t.getdata(_T("path"), a, b);
		assert(ret);
		str66 = (CStringA)a;
		sprintf(Save_path, "%s", str66.GetString());
	}

	///
	HPDF_SaveToFile(pdf, Save_path);

	//5.Cleanup////////////////////////////////////////////////////////////////////////////////////////

	HPDF_Free(pdf);

	temp.setdata(_T("path"), Save_path, -1);

	/////////////////////////////////

	g_FLOW = FLOW_REPORTE;
	g_bScreening = false;

	temp.copydata(g_record, temp);
	CString blh;
	blh = g_PatiengInfo.lpBLId;
	if (bModify)
		blh = sBLH;
	pMgr->SaveDataInfo(temp, blh);

	SOUND_SUCCEED

	if (!bModify)
	{
		MessageBoxW(m_hWnd, _T("报告生成成功"), _T("提示"), 0);
		cv::destroyAllWindows();
	}

	m_bIsDisposeBegin = false;
	m_bIsDisposeEnd = true;

	return true;
}

void CMainDlgWnd::Oncheckreport()
{
	if (m_nLastID < 0)
	{
		CPopMenuInfoWnd::GetThis()->SetValueDescribe(_T("请先保存病历！"));
		CPopMenuInfoWnd::GetThis()->SetTitle(_T("提示"));
		CPopMenuInfoWnd::MessageBox(m_hWnd);
	}
	else if (g_PatiengInfo.db_ID == 0)
	{
// 		CString a;
// 		a.Format(_T("nLastID=%d\n"), m_nLastID);
// 		OutputDebugString(a);
// 		ModifyDBByID(m_nLastID, &g_PatiengInfo);
	}
	else if (g_PatiengInfo.db_ID == 1){
		::MessageBox(m_hWnd, _T("该记录已审核!"), _T("提示"), MB_OKCANCEL);
// 		CPopMenuInfoWnd::GetThis()->SetValueDescribe(_T("该记录已审核！"));
// 		CPopMenuInfoWnd::GetThis()->SetTitle(_T("提示"));
// 		CPopMenuInfoWnd::MessageBox(m_hWnd);
	}
}

void CMainDlgWnd::On32826()
{
	/*
	//TCHAR lpBin[MAX_PATH];
	InitPath(lpBin, NULL, NULL);
	//CString name = _T("AcroRd32.exe D:\1.pdf");
	CString name = _T("AcroRd32.exe D:\1.pdf");
	ShellExecute(NULL, _T("open"), name, NULL, lpBin, SW_SHOWNORMAL);// TODO:  在此添加命令处理程序代码
	*/
	//-------------------------------------------------------------
	//CString param;
	//param.Format(_T(" %s"), _T("D:\\1.pdf"));

	//SHELLEXECUTEINFO ShExecInfoPrintf = { 0 };
	//ShExecInfoPrintf.cbSize = sizeof(SHELLEXECUTEINFO);
	//ShExecInfoPrintf.fMask = SEE_MASK_NOCLOSEPROCESS;
	//ShExecInfoPrintf.hwnd = NULL;
	//ShExecInfoPrintf.lpVerb = NULL;
	//ShExecInfoPrintf.lpFile = _T("AcroRd32");	//此处是待打印的 pdf文档	
	//ShExecInfoPrintf.lpParameters = param;
	//ShExecInfoPrintf.lpDirectory = NULL;
	//ShExecInfoPrintf.nShow = SW_HIDE;
	//ShExecInfoPrintf.hInstApp = NULL;
	//ShellExecuteEx(&ShExecInfoPrintf); //可添加 bool变量，判断是否成功。
	//if (WaitForSingleObject(ShExecInfoPrintf.hProcess, 50000) == WAIT_TIMEOUT)
	//{
	//	TerminateProcess(ShExecInfoPrintf.hProcess, 0);
	//}
	//-------------------------------------------------------------
	TCHAR lpBin[MAX_PATH];
	InitPath(lpBin, NULL, NULL);
	CString name = _T("AcroRd32.exe");

	CString name1 = _T("/p /h ");
	name1 += m_lpResultPath;
	int name1length = name1.GetLength();
	name1length = name1length - 7;
	name1 = name1.Left(name1length);
	//name1= name1.Left(20);
	name1 += g_PatiengInfo.lpBLId;
	name1 += _T(".pdf");

	ShellExecute(NULL, _T("open"), name, name1, lpBin, SW_SHOWNORMAL);
}

void CMainDlgWnd::On32817()
{
	OnDisposeProgressEx();
}

void CMainDlgWnd::On32818()
{
	ShellExecute(NULL, _T("open"), _T("explorer.exe"), _T("http://about.easvian.com/index.html"), NULL, SW_SHOW);
}

void CMainDlgWnd::On32819()
{
	ShellExecute(NULL, _T("open"), _T("explorer.exe"), _T("http://about.easvian.com/contact.html"), NULL, SW_SHOW);
	//HelpWnd::MessageBox(m_hWnd);
}


void CMainDlgWnd::OnRegister()
{
	if (IsAdmin())
		CRegisterWnd::MessageBox(m_hWnd);
}


void CMainDlgWnd::OnModifyPwd()
{
	//if (IsAdmin())
	CModifyPwdWnd::MessageBox(m_hWnd);
}


void CMainDlgWnd::OnLeftCount()
{
	int iLeft = gEncryptionDogLeftCount();
	if (iLeft < 0)
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("请检查是否插入加密狗."), _T("警告"), MB_OKCANCEL);
		return ;
	}
	CPopMenuInfoWnd::GetThis()->SetValueInt(iLeft);
	CPopMenuInfoWnd::GetThis()->SetValueDescribe(_T("剩余次数:"));
	CPopMenuInfoWnd::GetThis()->SetTitle(_T("剩余次数"));
	CPopMenuInfoWnd::MessageBox(m_hWnd);
}

void CMainDlgWnd::SwitchTab(int ipage)
{
	if (!(ipage >= 0 && ipage <= 2))
		return;
	m_pTabRight->SelectItem(ipage);
	for (int i = 0; i < 3; ++i)
	{
		if (i==ipage)
			m_pOptionTabUI[i]->Selected(true);
		else
			m_pOptionTabUI[i]->Selected(false);
	}
	
}


void CMainDlgWnd::ShowCurUser()
{
	m_pEditCurName->SetText(g_PatientInfoCur.sName.GetString());
	m_pEditCurBLH->SetText(g_PatientInfoCur.sBLH.GetString());
}


void CMainDlgWnd::ClearCurUser()
{
	m_pEditCurName->SetText(_T(""));
	m_pEditCurBLH->SetText(_T(""));
}

/////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
//selection

void CMainDlgWnd::OnDisposeProgressEx()
{
	if (gMainDlgWnd->IsDisposeCalc())
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("当前样本检测中，请等待操作完成."), _T("警告"), MB_OKCANCEL);
		return;
	}

	bool bret = ResetDataParam();
	if (!bret)
	{
		::MessageBox(m_hWnd, _T("当前样本目录中图片太模糊，请重新扫片子在计算."), _T("警告"), MB_OKCANCEL);
		return;
	}

	if (nImageNumAll <= 0)
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("当前路径没有发现处理的细胞图片."), _T("警告"), MB_OKCANCEL);
		return;
	}

	if (IsDog)
	{
		haspStatus status = gEncryptionDogVertify();
		//加密狗key没有找到
		if (status == HASP_HASP_NOT_FOUND)
		{
			SOUND_WARNING
			::MessageBox(m_hWnd, _T("加密狗KEY没有找到"), _T("警告"), MB_OK | MB_SYSTEMMODAL);
			return;
		}
		else if (status == HASP_FEATURE_EXPIRED)
		{
			SOUND_WARNING
			::MessageBox(m_hWnd, _T("加密狗授权到期"), _T("警告"), MB_OK | MB_SYSTEMMODAL);
			return;
		}
		if (status != HASP_STATUS_OK)
		{
			TCHAR buf[128] = { 0 };
			_stprintf(buf, _T("加密狗未知错误，status=%d"), status);
			::MessageBox(m_hWnd, buf, _T("警告"), MB_OK | MB_SYSTEMMODAL);
			return;
		}
	}

	if (g_PatientInfoCur.sName.IsEmpty() || g_PatientInfoCur.sBLH.IsEmpty())
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("请新建一个用户或者病历管理选择一个用户样本"), _T("警告"), MB_OKCANCEL);
		return;
	}

	USES_CONVERSION;
	string sPathFile;
	sPathFile = W2A(m_FileDir);
	if (sPathFile.length() == 0)
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("请选择制片或者打开档案路径"), _T("警告"), MB_OKCANCEL);
		return;
	}

	if (!IsAdmin(false))
	{
		m_pGifAnim->SetVisible(true);
//		m_pGifAnim1->SetVisible(true);
		m_pAniProgress->SetVisible(true);
	}
	UpdateWindow(m_hWnd);

	vCellsfullfilenames.swap(vector<string>());

	g_vec_calc_image.swap(vector<string>());
	g_vvec_calc_data.swap(vector<vector<string>>());

	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	printf("now system cpu num is %d\n", sysInfo.dwNumberOfProcessors);

	g_calc_time = CTime::GetCurrentTime();
	memset(g_thread_handle, 0, sizeof(g_thread_handle));
	memset(g_thread_id, 0, sizeof(g_thread_id));
	g_thread_handle_wait = NULL;
	g_thread_count = thread_count;
	int iDo = sysInfo.dwNumberOfProcessors / 4;
	if (iDo==1)
	{
		//g_thread_count = thread_count / 3;
	}
	if (g_p_calc_data==NULL)
	{
		g_p_calc_data = new calc_data[g_thread_count]();
		for (int i = 0; i < g_thread_count; ++i)
			g_p_calc_data[i].newdata();
	}
	for (int i = 0; i < g_thread_count; ++i)
	{
		g_p_calc_data[i].reset();
		g_p_calc_data[i].image_count = thread_item_count;
	}
	int iRemainder = nImageNumAll % g_thread_count;
	int iMerchant = nImageNumAll / g_thread_count;
	int cnt = 0;
	if (iMerchant == 0)
	{
		g_thread_count = iRemainder;
		cnt = iRemainder;
	}
	else
	{
		for (int i = 0; i < g_thread_count; ++i)
		{
			if (i < iRemainder)
				g_p_calc_data[i].image_count = iMerchant + 1;
			else
				g_p_calc_data[i].image_count = iMerchant;
			cnt += g_p_calc_data[i].image_count;
		}
	}
	assert(false);
	assert(cnt == nImageNumAll);
	if (cnt != nImageNumAll)
	{
		assert(false);
	}

	//g_thread_count = 1;
	memset(g_thread_filename_index, -1, sizeof(g_thread_filename_index));

	int index = 0;
	for (int i = 0; i < g_thread_count; ++i)
	{
		for (int j = 0; j < g_p_calc_data[i].image_count; ++j)
		{
			g_p_calc_data[i].image_index[j] = index++;
		}
	}
	if (index == g_vec_file_name.size())
	{
		int j = -1;
	}
	
	m_aTime = CTime::GetCurrentTime();
	CTime bTime = CTime::GetCurrentTime();
	bTime.GetTime();
	CTimeSpan bbb;
	bbb = bTime - m_aTime;
	CString strtime;
	strtime.Format(_T("  Time=%dM:%dS\n"), bbb.GetMinutes(), bbb.GetSeconds());
	CString sTime;
	sTime += strtime;
	m_aProgress = 0;
	m_bSuc = false;
	ShowPregress(sTime, m_aProgress);

	TCHAR processName[] = _T("CalcClient.exe");
	DWORD dwPid;
	bool bKill = FindProcess(processName, dwPid);
	while (bKill)
	{
		KillProcess(dwPid);
		bKill = FindProcess(processName, dwPid);
	}

	DWORD ThreadID;
	g_thread_perform = ::CreateThread(NULL, NULL, doDispose, (void*)this, 0, (LPDWORD)&g_thread_id[0]);

	g_thread_handle_wait = ::CreateThread(NULL, NULL, doDisposeExWait, (void*)this, 0, (LPDWORD)&ThreadID);

	unsigned threadID;
	g_thread_handle_progress = (HANDLE)_beginthreadex(NULL, 0, doDisposeExProgress, (void*)this, 0, &threadID);

	SetTimer(gMainDlgWnd->GetHWND(), 416, 1000, NULL);

	int i = -1;
}


void CMainDlgWnd::SetThreadItemFileName(int index, char name[buf_send_recv_len], char* head)
{
	if (index >= g_thread_count || index<0)
		return;

	g_thread_filename_index[index] = index;

	memset(name, 0, sizeof(char)*buf_send_recv_len);
	string temp;
	temp.append(head);
	for (int j = 0; j < g_p_calc_data[index].image_count; ++j)
	{
		int idx = g_p_calc_data[index].image_index[j];
		temp.append(g_vec_file_name[idx]);
	}
	temp.append("|");
	CStringA sA;
	sA = m_FileDir;
	temp.append(sA.GetString());
	temp.append("|");
	char buf[20] = { 0 };
	sprintf_s(buf, sizeof(buf), "%d", index);
	temp.append(buf);
	sprintf_s(name, buf_send_recv_len,  "%s", temp.c_str());
}


void CMainDlgWnd::SetThreadItemPID(int index, int pid)
{
	if (index >= g_thread_count || index < 0)
		return;

	g_p_calc_data[index].pid = pid;
}


void CMainDlgWnd::SetThreadItemResult(int index, const char result[buf_send_recv_len])
{
	if (index >= g_thread_count || index < 0)
		return;

	memcpy(g_p_calc_data[index].buf, result, buf_send_recv_len);
}


void CMainDlgWnd::SetThreadItemEnd(int index)
{
	if (index >= g_thread_count || index < 0)
		return;

	g_p_calc_data[index].bend = true;
}

bool CMainDlgWnd::CheckThreadItemsEnd()
{
	for (int i = 0; i < g_thread_count; ++i)
	{
		if (! g_p_calc_data[i].bend)
		{
			return false;
		}
	}
	return true;
}

bool CMainDlgWnd::GetThreadItemDataIndex(int pid, int &index)
{
	for (int i = 0; i < g_thread_count; ++i)
	{
		if (g_p_calc_data[i].pid == pid)
		{
			index = i;
			return true;
		}
	}
	return false;
}

bool CMainDlgWnd::GetThreadItemFileName(const char srcbuf[buf_send_recv_len], vector<string> &vname, string& path, int& index)
{
	g_vec_file_name.swap(vector<string>());
	vname.swap(vector<string>());
	path = "";
	index = -1;
	int len = strlen(srcbuf);
	if (len <= 0)
		return false;

	CString dest(srcbuf);
	int pos = dest.Find(_T("|"));
	if (pos == -1)
		return false;
	CString temp;
	temp = dest.Mid(pos + 1);
	int loc = temp.Find(_T("|"));
	if (loc == -1)
		return false;
	CString strIndex;
	strIndex = temp.Mid(loc + 1);
	index = _ttoi(strIndex.GetString());
	temp = temp.Mid(loc);
	CStringA strA;
	strA = temp;
	path = strA.GetString();
	temp = dest.Mid(0, pos);
	int pos1 = temp.Find(_T(".jpg"));
	while (pos1 > 0)
	{
		CString str1, str2;
		int p1, p2;
		str1 = temp.Mid(0, pos1 + 4);
		str2 = temp.Mid(pos1 + 4);
		temp = str2;
		pos1 = temp.Find(_T(".jpg"));
		CStringA sA;
		sA = str1;
		vname.push_back(sA.GetString());
	}

	g_vec_file_name = vname;
	m_FileDir = path.c_str();

	return true;
}


bool CMainDlgWnd::GetThreadItemFileNameIndex(int &index)
{
	index = 1;
	for (int i = 0; i < g_thread_count; ++i)
	{
		if (g_thread_filename_index[i] < 0)
		{
			g_thread_filename_index[i] = i;
			index = i;
			break;
		}
	}
	return true;
}

void initialization() {
	//初始化套接字库
	WORD w_req = MAKEWORD(2, 2);//版本号
	WSADATA wsadata;
	int err;
	err = WSAStartup(w_req, &wsadata);
	if (err != 0) {
		cout << "初始化套接字库失败！" << endl;
	}
	else {
		cout << "初始化套接字库成功！" << endl;
	}
	//检测版本号
	if (LOBYTE(wsadata.wVersion) != 2 || HIBYTE(wsadata.wHighVersion) != 2) {
		cout << "套接字库版本号不符！" << endl;
		WSACleanup();
	}
	else {
		cout << "套接字库版本正确！" << endl;
	}

}


int CMainDlgWnd::DisposeProgressEx()
{
// 	CPipeMgr pipe;
// 	pipe.StartSvr();
// 	WaitForSingleObject(pipe.GetThreadHandleSvr(), INFINITE	

	//定义长度变量
	int send_len = 0;
	int recv_len = 0;
	int _len = 0;
	//定义发送缓冲区和接受缓冲区
	char send_buf[buf_send_recv_len];
	char recv_buf[buf_send_recv_len];
	//定义服务端套接字，接受请求套接字
	SOCKET s_server;
	SOCKET s_accept;
	//服务端地址客户端地址
	SOCKADDR_IN server_addr;
	SOCKADDR_IN accept_addr;
	initialization();
	//填充服务端信息
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(54321);
	//创建套接字
	s_server = socket(AF_INET, SOCK_STREAM, 0);
	if (bind(s_server, (SOCKADDR *)&server_addr, sizeof(SOCKADDR)) == SOCKET_ERROR) {
		cout << "套接字绑定失败！" << endl;
		WSACleanup();
	}
	else {
		cout << "套接字绑定成功！" << endl;
	}
	//设置套接字为监听状态
	if (listen(s_server, SOMAXCONN) < 0) {
		cout << "设置监听状态失败！" << endl;
		WSACleanup();
	}
	else {
		cout << "设置监听状态成功！" << endl;
	}
	cout << "服务端正在监听连接，请稍候...." << endl;

	int iRecvClientCount = 0;

	map<int, vector<string>> mapVStr;

	int ll = 1000000;
	char* pRecv = new char[ll];
	int iLeftProcess = 0;
	while (true)
	{
		//检测是否完成 
		if (gMainDlgWnd->CheckThreadItemsEnd())
		{
			break;
		}
		//接受连接请求
		_len = sizeof(SOCKADDR);
		s_accept = accept(s_server, (SOCKADDR *)&accept_addr, &_len);
		if (s_accept == SOCKET_ERROR) {
			cout << "连接失败！" << endl;
			WSACleanup();
			return 0;
		}
		cout << "连接建立，准备接受数据" << endl;

		int count = 0;
		
		//接收数据
		while (1)
		{
			//memset(recv_buf, 0, sizeof(recv_buf));
			//recv_len = recv(s_accept, recv_buf, buf_send_recv_len, 0);
			//if (recv_len < 0) {
			//	cout << "接受失败！" << endl;
			//	break;
			//}
			//else {
			//	cout << "客户端信息:" << recv_buf << endl;
			//}

			
			memset(pRecv, 0, ll);
			_ReadSocket(s_accept, pRecv, ll);
			string srecv(pRecv);

			srecv = srecv.substr(8);
			string shead = srecv.substr(0, msg_len);
			if (shead.compare(msg_c_s_init) == 0)
			{
				int index = -1;
				gMainDlgWnd->GetThreadItemFileNameIndex(index);
				string spid = srecv.substr(2);
				int pid = atoi(spid.c_str());
				gMainDlgWnd->SetThreadItemPID(index, pid);

				memset(send_buf, 0, sizeof(send_buf));
				gMainDlgWnd->SetThreadItemFileName(index, send_buf, msg_s_c_calcfilename);
				send_len = send(s_accept, send_buf, buf_send_recv_len, 0);
				if (send_len < 0) {
					cout << "发送失败！" << endl;
					break;
				}

				closesocket(s_accept);
				break;
			}
			else if (shead.compare(msg_c_s_calcprocess) == 0)
			{
				string stemp = srecv.substr(2);
				string::size_type  sz = stemp.find("|");
				string str1, str2;
				if (sz!=string::npos)
				{
					str1 = stemp.substr(0, sz);
					str2 = stemp.substr(sz + 1);
				}

				closesocket(s_accept);
				m_aProgress = atoi(str1.c_str());
				PostMessage(WM_USER + 456, 0, 0);

				break;
			}
			else if (shead.compare(msg_c_s_calcresult) == 0)
			{
				iRecvClientCount++;

				string::size_type st = srecv.find("|");
				if (st == string::npos)
				{
					cout << "解析错误1" << endl;
					break;
				}
				string spid = srecv.substr(2, st - 2);
				if (spid.size() == 0)
				{
					break;
				}
				int pid = atoi(spid.c_str());
				int index = -1;
				//bool bret = gMainDlgWnd->GetThreadItemDataIndex(pid, index);
				//if (bret==false)
				//{
				//	int ierror = -1;
				//}
				string sdata = srecv.substr(st + 1);
				string::size_type sz = sdata.find("|");
				if (sz == string::npos)
				{
					break;
				}
				string sdata1 = sdata.substr(0, sz);
				index = atoi(sdata1.c_str());
				g_p_calc_data[index].index = index;
				string sdata2 = sdata.substr(sz + 1);
				if (sdata2.compare("endendend") == 0)
				{
					iLeftProcess++;
				}
				else
				{
					string::size_type sz1 = sdata2.find("|");
					if (sz1 == string::npos)
					{
						break;
					}
					string sdata3 = sdata2.substr(0, sz1);//数据文件路径
					string sdata4 = sdata2.substr(sz1 + 1);
					string::size_type sz2 = sdata4.find("[");
					if (string::npos == sz2)
					{
						break;
					}
					string sdata5 = sdata4.substr(0, sz2); //文件名解析
					int iLenTemp = sdata5.length() + 1;
					char* pTemp = new char[iLenTemp];
					memset(pTemp, 0, iLenTemp);
					strncpy(pTemp, sdata5.c_str(), iLenTemp - 1);
					vector<string> vstr;
					SplitString(vstr, pTemp, "|");
					delete[] pTemp;
					pTemp = NULL;
					vector<string> _v;
					for (int i = 0; i < vstr.size(); ++i)
					{
						//vCellsfullfilenames.push_back(sdata3 + vstr[i] + ".bmp");
						_v.push_back(sdata3 + vstr[i] + ".bmp");
					}
					mapVStr[index] = _v;

					//其它计算数据
					string sdata6 = sdata4.substr(sz2 + 1);

					iLenTemp = sdata6.length() + 1;
					pTemp = new char[iLenTemp];
					memset(pTemp, 0, iLenTemp);
					strncpy(pTemp, sdata6.c_str(), iLenTemp - 1);
					vstr.swap(vector<string>());
					SplitString(vstr, pTemp, "]");
					delete[] pTemp;
					pTemp = NULL;

					for (int i = 0; i < vstr.size(); ++i)
					{
						iLenTemp = vstr[i].length() + 1;
						pTemp = new char[iLenTemp];
						memset(pTemp, 0, iLenTemp);
						strncpy(pTemp, vstr[i].c_str(), iLenTemp - 1);
						vector<string> vItem;
						char* pTemp1 = pTemp + 1;
						SplitString(vItem, pTemp1, "|");
						delete[] pTemp;
						pTemp = NULL;

						//d lld d
						int _int = 0;
						long long _longlong = 0;
						double _double = 0.0;
						if (i == 0)
						{
							_int = atoi(vItem[0].c_str());
							g_p_calc_data[index]._nCountLymphocyte = _int;
							_longlong = atol(vItem[1].c_str());
							g_p_calc_data[index]._dLymphocyteIODSumValue = _longlong;
							_int = atoi(vItem[2].c_str());
							g_p_calc_data[index]._nCountCells = _int;
							_int = atoi(vItem[3].c_str());
							g_p_calc_data[index]._nCountCellsAll = _int;
						}
						//1
						else if (i == 1)
						{
							for (int j = 0; j < vItem.size(); ++j)
							{
								_int = atoi(vItem[j].c_str());
								g_p_calc_data[index]._p_nCellsPixelArea[g_p_calc_data[index]._p_nCellsPixelArea_count++] = _int;
							}
						}
						//2
						else if (i == 2)
						{
							for (int j = 0; j < vItem.size(); ++j)
							{
								_double = atof(vItem[j].c_str());
								g_p_calc_data[index]._p_dCircleDegree[g_p_calc_data[index]._p_dCircleDegree_count++] = _double;
							}
						}
						//3
						else if (i == 3)
						{
							for (int j = 0; j < vItem.size(); ++j)
							{
								_double = atof(vItem[j].c_str());
								g_p_calc_data[index]._p_dCellgrayvar[g_p_calc_data[index]._p_dCellgrayvar_count++] = _double;
							}
						}
						//4
						else if (i == 4)
						{
							for (int j = 0; j < vItem.size(); ++j)
							{
								_int = atoi(vItem[j].c_str());
								g_p_calc_data[index]._p_dGrayValue1[g_p_calc_data[index]._p_dGrayValue1_count++] = _int;
							}
						}
						//5
						else if (i == 5)
						{
							for (int j = 0; j < vItem.size(); ++j)
							{
								_int = atoi(vItem[j].c_str());
								g_p_calc_data[index]._p_dGrayMax1[g_p_calc_data[index]._p_dGrayMax1_count++] = _int;
							}
						}
						//6
						else if (i == 6)
						{
							for (int j = 0; j < vItem.size(); ++j)
							{
								_double = atof(vItem[j].c_str());
								g_p_calc_data[index]._p_dEpithelialCellIOD1[g_p_calc_data[index]._p_dEpithelialCellIOD1_count++] = _double;
							}
						}
						//7
						else if (i == 7)
						{
							for (int j = 0; j < vItem.size(); ++j)
							{
								_double = atof(vItem[j].c_str());
								g_p_calc_data[index]._p_dAOD1[g_p_calc_data[index]._p_dAOD1_count++] = _double;
							}
						}
						//8
						else if (i == 8)
						{
							dLymphocyteCellIOD1;
							for (int j = 0; j < vItem.size(); ++j)
							{
								_double = atof(vItem[j].c_str());
								g_p_calc_data[index]._p_dLymphocyteCellIOD1[g_p_calc_data[index]._p_dLymphocyteCellIOD1_count++] = _double;
							}
						}
					}
				}

				//gMainDlgWnd->SetThreadItemResult(index, sdata6.c_str());

				memset(send_buf, 0, sizeof(send_buf));
				sprintf_s(send_buf, sizeof(send_buf), "%s,pid=%d", msg_s_c_close, pid);
				send_len = send(s_accept, send_buf, buf_send_recv_len, 0);
				if (send_len < 0) {
					cout << "发送失败！" << endl;
					break;
				}

				gMainDlgWnd->SetThreadItemEnd(index);

				TCHAR buf1[256] = { 0 };
				_stprintf(buf1, _T("client success index %d\n"), index);
				OutputDebugString(buf1);

				closesocket(s_accept);
				break;
			}
		}
		closesocket(s_accept);
	}

	delete[] pRecv;
	pRecv = NULL;

	CTime endTime = CTime::GetCurrentTime();
	CTimeSpan span;
	span = g_calc_time - endTime;
	CString strtime;
	strtime.Format(_T("Time=%dM:%dS\n"), span.GetMinutes(), span.GetSeconds());
	OutputDebugString(strtime);

	//关闭套接字
	closesocket(s_server);
	closesocket(s_accept);
	//释放DLL资源
	WSACleanup();

	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//整合数据
	//////////////////////////////////////////////////////////////////////////////////////////////////////
	memset(nCellsPixelArea, 0, sizeof(int) * 100000);	//
	memset(DI, 0, sizeof(double) * 100000);
	memset(dCircleDegree, 0, sizeof(double) * 100000);	//
	memset(dCellgrayvar, 0, sizeof(double) * 100000);	//
	memset(dGrayValue1, 0, sizeof(int) * 100000);	//
	memset(dODVar, 0, sizeof(int) * 100000);
	memset(dGrayMax1, 0, sizeof(int) * 100000);	//没用
	memset(dgrayvar, 0, sizeof(int) * 100000);
	memset(SortIndex, 0, sizeof(int) * 100000);
	memset(dBackgroundGrayMeanValue1, 0, sizeof(double) * 100000);
	memset(dEpithelialCellIOD1, 0, sizeof(double) * 100000);	//
	memset(dEpithelialCellIOD2, 0, sizeof(double) * 100000);
	memset(dLymphocyteCellIOD1, 0, sizeof(double) * 100000);
	memset(dAOD1, 0, sizeof(double) * 100000);	//

	MeanLymphocyteGrayValue = 0;
	SumLymphocyteGrayValue = 0;
	nCountLymphocyte = 0;		//
	SumLymphocyteAreaValue = 0;
	MeanLymphocyteIOD = 0;
	dLymphocyteIODSumValue = 0;	//

	nCountCells = 0;
	nCountCellsAll = 0;

	int check_count = iLeftProcess;
	for (int i = 0; i < g_thread_count; ++i)
	{
		if (mapVStr[i].size()>0)
		{
			TCHAR lpBin[MAX_PATH];
			InitPath(lpBin, NULL, NULL);
			TCHAR lpini[MAX_PATH];
			wsprintf(lpini, _T("%s%s"), lpBin, _T("number.ini"));
			TCHAR buf[128] = { 0 };
			GetPrivateProfileString(_T("select_path_out"), _T("select_box"), _T("1"), buf, 128, lpini);
			int iSel = _ttoi(buf);
			CString path1, path2, path1name;
			bool bEqual = true;
			if (1 == iSel)
			{
				TCHAR buf1[256] = { 0 };
				GetPrivateProfileString(_T("select_path_save"), _T("select_path"), _T(""), buf1, 256, lpini);
				path2.Append(buf1);
				if (PathFileExists(m_FileDir) && PathFileExists(path2))
				{
					if (m_FileDir.GetAt(m_FileDir.GetLength() - 1) == '\\' || m_FileDir.GetAt(m_FileDir.GetLength() - 1) == '/')
						m_FileDir.SetAt(m_FileDir.GetLength() - 1, '\0');
					path1name = m_FileDir.Mid(m_FileDir.ReverseFind('\\') + 1);
					path1 = m_FileDir.Mid(0, m_FileDir.ReverseFind('\\'));
					if (path1.CompareNoCase(path2) != 0)
					{
						bEqual = false;
					}
				}
			}

			check_count++;
			for (int j = 0; j < mapVStr[i].size(); ++j)
			{
				CString sTemp(mapVStr[i][j].c_str());
				if (!bEqual)
				{
					int pos = sTemp.Find(path1name + _T("Result"));
					CString strTail;
					if (pos >= 0)
					{
						int pos1 = sTemp.Find(_T("\\"), pos);
						if (pos1>0)
						{
							strTail = sTemp.Mid(pos1);
							if (!strTail.IsEmpty())
							{
								CString s1;
								s1.Append(path2);
								s1.Append(_T("\\"));
								s1.Append(path1name);
								s1.Append(_T("Result"));
								s1.Append(strTail);
								sTemp = s1;
							}
						}
					}
				}
				CStringA sTempA;
				sTempA = sTemp;
				vCellsfullfilenames.push_back(sTempA.GetString());
			}
		}
	}
	if (check_count==g_thread_count)
	{
		OutputDebugString(_T("接收数据完成\n"));
	}

	int a1 = 0, a2 = 0, a3 = 0, a4 = 0, a5 = 0, a6 = 0, a7 = 0, a8 = 0, a9 = 0, a10 = 0, a11 = 0, a12 = 0, a13 = 0;
	int aLymphocyteC = 0;
	for (int i = 0; i < g_thread_count; ++i)
	{
		for (int j = 0; j < g_p_calc_data[i]._p_nCellsPixelArea_count; ++j)
		{
			nCellsPixelArea[a1++] += g_p_calc_data[i]._p_nCellsPixelArea[j];
		}
// 		for (int j = 0; j < g_p_calc_data[i]._p_DI_count; ++j)
// 		{
// 			DI[a2++] += g_p_calc_data[i]._p_DI[j];
// 		}
		for (int j = 0; j < g_p_calc_data[i]._p_dCircleDegree_count; ++j)
		{
			dCircleDegree[a3++] += g_p_calc_data[i]._p_dCircleDegree[j];
		}
		for (int j = 0; j < g_p_calc_data[i]._p_dCellgrayvar_count; ++j)
		{
			dCellgrayvar[a4++] += g_p_calc_data[i]._p_dCellgrayvar[j];
		}
		for (int j = 0; j < g_p_calc_data[i]._p_dGrayValue1_count; ++j)
		{
			dGrayValue1[a5++] += g_p_calc_data[i]._p_dGrayValue1[j];
		}
//		for (int j = 0; j < g_p_calc_data[i]._p_dODVar_count; ++j)
// 		{
// 			dODVar[a6++] += g_p_calc_data[i]._p_dODVar[j];
// 		}
		for (int j = 0; j < g_p_calc_data[i]._p_dGrayMax1_count; ++j)
		{
			dGrayMax1[a7++] += g_p_calc_data[i]._p_dGrayMax1[j];
		}
// 		for (int j = 0; j < g_p_calc_data[i]._p_dgrayvar_count; ++j)
// 		{
// 			dgrayvar[a8++] += g_p_calc_data[i]._p_dgrayvar[j];
// 		}
// 		for (int j = 0; j < g_p_calc_data[i]._p_SortIndex_count; ++j)
// 		{
// 			SortIndex[a9++] += g_p_calc_data[i]._p_SortIndex[j];
// 		}
// 		for (int j = 0; j < g_p_calc_data[i]._p_dBackgroundGrayMeanValue1_count; ++j)
// 		{
// 			dBackgroundGrayMeanValue1[a10++] += g_p_calc_data[i]._p_dBackgroundGrayMeanValue1[j];
// 		}
		for (int j = 0; j < g_p_calc_data[i]._p_dEpithelialCellIOD1_count; ++j)
		{
			dEpithelialCellIOD1[a11++] += g_p_calc_data[i]._p_dEpithelialCellIOD1[j];
		}
		for (int j = 0; j < g_p_calc_data[i]._p_dLymphocyteCellIOD1_count; ++j)
		{
			dLymphocyteCellIOD1[a12++] += g_p_calc_data[i]._p_dLymphocyteCellIOD1[j];
		}
		for (int j = 0; j < g_p_calc_data[i]._p_dAOD1_count; ++j)
		{
			dAOD1[a13++] += g_p_calc_data[i]._p_dAOD1[j];
		}

		aLymphocyteC += g_p_calc_data[i]._p_dLymphocyteCellIOD1_count;//验证数量

		nCountLymphocyte += g_p_calc_data[i]._nCountLymphocyte;
		dLymphocyteIODSumValue += g_p_calc_data[i]._dLymphocyteIODSumValue;

		nCountCells += g_p_calc_data[i]._nCountCells;
		nCountCellsAll += g_p_calc_data[i]._nCountCellsAll;
	}
	if (aLymphocyteC != nCountLymphocyte)
	{
		//::MessageBox(NULL, _T("Lymphocyte error."), _T(""), MB_OK);
	}

	double dIODMeanValue = 0;
	if (nCountLymphocyte == 0){ nCountLymphocyte = 1; dLymphocyteIODSumValue = 180; }
	dIODMeanValue = dLymphocyteIODSumValue / nCountLymphocyte; //所有淋巴细胞的平均IOD

	CString shangpi;
	CString DI_Value;
	int nmax = 0;
	int *a = new int[400](); 
	int *b = new int[400]();
	int ciod = 0;
	int countof600=0;
	for (int i = 0, j=0; i < nCountCells; i++)//面积600以下 将0~0.3范围内的aod排序并直方图化 countof600是面积600以下的总和
	{
		if (nCellsPixelArea[i]<600)
		{
			double dTemp = dAOD1[i];
			countof600++;
			int n = 0;
			for (double m = 0; m <0.3; m += 0.02)
			{
				if (dTemp > m && dTemp < m + 0.02)
				{
					b[n]++;
				}
				n++;
			}
		}
	}
	int cellcount = 0;
	for (int i = 14; i>=0; i--)//cAod是可以将Aod大小排序较小的75%的细胞除掉的参数
// 		int i = 0.02; i<0.2; i+=0.02)
	{
		cellcount = cellcount + b[i];
		if (0.75*countof600 < cellcount)
		{
			cAod = i*0.02;
			break;
		}
	}
	int c[2000] = { 0 };
	for (int i = 0; i < nCountCells; i++)
	{
		//if (nCellsPixelArea[i] > 600 && dAOD1[i]>0.05&&dAOD1[i]<0.22 || dAOD1[i]>cAod&&nCellsPixelArea[i] < 600)//面积小于800或AOD小于0.05不参与坐标ciod的计算（杂质删不干净无法跟兰丁一模一样 就把大部分是杂质的区域删掉）
		{
			int iod = (int)(dEpithelialCellIOD1[i] + 0.5);//四舍五入取整
			dEpithelialCellIOD2[i] = iod;
			if (iod>0 && iod<2000)
			{
				c[iod] ++;
			}
// 			for (int m = 50; m < 400; m++)//二倍体峰的iod一般在50以上 400以下
// 			{
// 				if (dEpithelialCellIOD2[i] == m)
// 				{
// 					a[m]++;
// 				}
// 			}
		}
	}
// 	for (int i = 50; i<400; i++)
// 	{
// 		if (a[i] > nmax)
// 		{
// 			nmax = a[i];
// 			ciod = i;
// 		}
// 	}
	for (int i = 0; i < 2000; ++i)
	{
		if (c[i] > nmax)
		{
			nmax = c[i];
			ciod = i;
		}
	}

	delete[] a;
	a = NULL;
	delete[] b;
	b = NULL;

	int nCountNUMArrayEx[81] = { 0 };
	for (int i = 0; i < nCountCells; ++i)
	{
		double dVal = 0.0, dVal1 = 0.1;
		for (int j = 0; j < 81; ++j)
		{
			dVal = 0.1 * j;
			dVal1 = 0.1*(j + 1);
			double dtemp = DI_X * dEpithelialCellIOD1[i] / ciod;
			if (j == 80)
			{
				if (dtemp > dVal)
				{
					nCountNUMArrayEx[j]++;
				}
			}
			else
			{
				if (dtemp > dVal && dtemp <= dVal1)
				{
					nCountNUMArrayEx[j]++;
				}
			}
		}
	}
	int amax = -1, aidx = -1;
	for (int i = 0; i < 81; ++i)
	{
		if (nCountNUMArrayEx[i]>nmax)
		{
			nmax = nCountNUMArrayEx[i];
			aidx = i;
		}
	}
#define cval_dis 0.2
	double _ciod = 0, _meangray = 0, _meanaod = 0, graycount=0, grayc=0, aodcount=0, aodc=0;
	double _meanarea = 0, areacount = 0, areac = 0;
	int idx1 = -1, idx2 = -1;
	if (nmax>0.6)
	{
		double dMidPos = aidx * 0.1+0.05;
		double d = dMidPos - cval_dis;
		double d1 = dMidPos + cval_dis;
		if ( d > 0 )
		{
			int iod1 = 0, iod2 = 0;
			iod1 = d / 2 * ciod;
			iod2 = d1 / 2 * ciod;
			double d2 = 0;
			int c = 0;
			for (int i = 0; i < nCountCells; ++i)
			{
				bool bOk = false;
				double dou = dEpithelialCellIOD1[i];
				if (dou >= iod1 && dou <= iod2)
				{
					d2 += dou;
					c++;

					bOk = true;
				}
				//int igrayvalue = dGrayValue1[i];
				//int ipixelarea = nCellsPixelArea[i];
				//double dmeangrayvalue = 0.0;
				//if (ipixelarea>0)
				//{
				//	dmeangrayvalue = igrayvalue*1.0 / ipixelarea;
				//}
				//double dou1 = 0, dou2 = dAOD1[i];
				//if (ipixelarea>0)
				//{
				//	dou1 = dou / ipixelarea;
				//	if (dou1>0 && dou2>0)
				//	{
				//		if (!(dou1>=dou2-0.000001 && dou1<=dou2+0.000001))
				//		{
				//			MessageBox(NULL, _T("gggggg"), _T(""), MB_OK);
				//		}
				//	}
				//}
				//if (bOk)
				//{
				//	if (dmeangrayvalue>0)
				//	{
				//		grayc++;
				//		graycount += dmeangrayvalue;
				//	}
				//	if (dou1>0)
				//	{
				//		aodc++;
				//		aodcount += dou1;
				//	}
				//	if (ipixelarea > 0)
				//	{
				//		areac++;
				//		areacount += ipixelarea;
				//	}
				//}

				//if (dou>=iod1)
				//{
				//	if (idx1 == -1)
				//		idx1 = i;
				//}
				//else if (dou>=iod2)
				//{
				//	if (idx2 == -1)
				//	{
				//		if (i > 1)
				//			idx2 = i - 1;
				//		else
				//			idx2 = i;
				//	}
				//}
			}
			if (idx2==-1)
			{
				idx2 = nCountCells - 1;
			}
			if (c>0)
			{
			    _ciod = d2 / c;
				//if (grayc>0)
				//{
				//	_meangray = graycount / grayc;
				//}
				//if (aodc>0)
				//{
				//	_meanaod = aodcount / aodc;
				//}
				//if (areac>0)
				//{
				//	_meanarea = areacount / areac;
				//}
			}
		}
	}

	string sPathFile, sPath, sDataFileName, sDataFileResultName;
	USES_CONVERSION;
	sPathFile = W2A(m_FileDir);
	int nLocation = sPathFile.rfind("\\");
	sPath = sPathFile.substr(0, nLocation) + "\\";
	sDataFileName = sPathFile.substr(nLocation + 1, sPathFile.length());
	sDataFileResultName = sDataFileName + "Result\\";
	string sResultTxtName0 = sPath + sDataFileResultName + "Result\\CellsName.txt";
	string sResultTxtName1 = sPath + sDataFileResultName + "Result\\CellsLambda.txt";
	string sResultTxtName2 = sPath + sDataFileResultName + "Result\\CellsPixelArea.txt";
	string sResultTxtName3 = sPath + sDataFileResultName + "Result\\SortIndex.txt";
	string sResultTxtName4 = sPath + sDataFileResultName + "Result\\CIOD.txt";
	string sResultTxtName5 = sPath + sDataFileResultName + "Result\\Aod.txt";

	string s1(sPath + sDataFileResultName + "Result\\shangpi_iod.txt");
	string s2(sPath + sDataFileResultName + "Result\\linba_iod.txt");
	string s3(sPath + sDataFileResultName + "Result\\shangpi_DI.txt");
	CString sfileshangpi_iod(s1.c_str())/*, sfilelinba_iod(s2.c_str())*/, sfilesahngpi_di(s3.c_str());

	CStdioFile file_shangpi_IOD;
	file_shangpi_IOD.Open(sfileshangpi_iod.GetString(), CFile::modeCreate | CFile::modeReadWrite);

// 	CStdioFile file_linba_IOD;
// 	file_linba_IOD.Open(sfilelinba_iod.GetString(), CFile::modeCreate | CFile::modeReadWrite);

	CStdioFile file_shangpi_DI;
	file_shangpi_DI.Open(sfilesahngpi_di.GetString(), CFile::modeCreate | CFile::modeReadWrite);

	double dEpithelialMax = -100, dEpithelialMin = 10000, dMeanEpithelial = 0, dMeanEpithelial1 = 0;
	for (int i = 0; i < nCountCells; i++)
	{
		double d = dEpithelialCellIOD1[i];
		if (_ciod>0)
			DI[i] = (d / _ciod);//ciod为细胞数目最高峰
		else
			DI[i] = (d / ciod);//ciod为细胞数目最高峰
		//DI[i] = (dEpithelialCellIOD1[i] / dIODMeanValue) * (1.1618* dAOD1[i] + 0.2149);//民航
		//DI[i] = dEpithelialCellIOD1[i] / dIODMeanValue;//原公式（20190617）
		shangpi.Format(_T("%.4f\n"), d);//输出上皮细胞iod到d盘根目录
		file_shangpi_IOD.WriteString(shangpi);
		//double t = DI[i];
		DI_Value.Format(_T("%.4f\n"), DI[i]);//输出上皮细胞DI到d盘根目录 
		file_shangpi_DI.WriteString(DI_Value);

		if (d > dEpithelialMax)
		{
			dEpithelialMax = d;
		}
		if (d < dEpithelialMin)
		{
			dEpithelialMin = d;
		}
		dMeanEpithelial += d;
	}
	file_shangpi_IOD.Close();
	file_shangpi_DI.Close();

	if (nCountCells>0)
	{
		dMeanEpithelial1 = dMeanEpithelial / nCountCells;
	}

// 	CString linbaIODMeanValue;
// 	linbaIODMeanValue.Format(_T("%.4f\n"), dIODMeanValue);
// 	file_linba_IOD.WriteString(linbaIODMeanValue);//在d盘根目录写入淋巴平均IOD值
// 	file_linba_IOD.Close();

	double dLymphocyteMax = -100, dLymphocyteMin = 10000, dMeanLymphocyte = 0, dMeanLymphocyte1 = 0;
	string s4(sPath + sDataFileResultName + "Result\\linba_iod.txt");
	CString sfilelinba_iod(s4.c_str());
	CStdioFile file_linba_iod;
	file_linba_iod.Open(sfilelinba_iod.GetString(), CFile::modeCreate | CFile::modeReadWrite);
	for (int i = 0; i < nCountLymphocyte; ++i)
	{
		double d = dLymphocyteCellIOD1[i];
		CString str;
		str.Format(_T("%.4f\n"), d);
		file_linba_iod.WriteString(str);
		if (d > dLymphocyteMax)
		{
			dLymphocyteMax = d;
		}
		if (d < dLymphocyteMin)
		{
			dLymphocyteMin = d;
		}
		dMeanLymphocyte += d;
	}
	file_linba_iod.Close();
	if (nCountLymphocyte>0)
	{
		dMeanLymphocyte1 = dMeanLymphocyte / nCountLymphocyte;
	}

	string s5(sPath + sDataFileResultName + "Result\\iod_summary.txt");
	CString sfileiod_summary(s5.c_str());
// 	CStdioFile file_iod_summary;
// 	file_iod_summary.Open(sfileiod_summary.GetString(), CFile::modeCreate | CFile::modeReadWrite);
	FILE *fp_file6 = NULL;
	fp_file6 = fopen(s5.c_str(), "w+");
	fprintf(fp_file6, "上皮max:%.4f, min:%.4f \n", dEpithelialMax, dEpithelialMin);
	if (dEpithelialMax>0)
	{
		fprintf(fp_file6, "上皮min/max:%.4f \n", dEpithelialMin / dEpithelialMax);
	}
	fprintf(fp_file6, "上皮平均:%.4f \n", dMeanEpithelial1);

	fprintf(fp_file6, "淋巴max:%.4f, min:%.4f \n", dLymphocyteMax, dLymphocyteMin);
	if (dLymphocyteMax>0)
	{
		fprintf(fp_file6, "淋巴min/max:%.4f \n", dLymphocyteMin / dLymphocyteMax);
	}
	fprintf(fp_file6, "淋巴平均:%.4f \n", dMeanLymphocyte1);

	fclose(fp_file6);

	//{
	//	string s5(sPath + sDataFileResultName + "iod_summarybbb.txt");
	//	CString sfileiod_summary(s5.c_str());
	//	CStdioFile file_iod_summary;
	//	file_iod_summary.Open(sfileiod_summary.GetString(), CFile::modeCreate | CFile::modeReadWrite);
	//	CString t1;
	//	//t1.Format(_T("bbbbbbbbbbbbb"));
	//	t1.Format(_T("上皮max:%.4f, min:%.4f \n"), dEpithelialMax, dEpithelialMin);
	//	file_iod_summary.WriteString(t1);

	//	file_iod_summary.Close();
	//}

	//基于Lambda进行上皮细胞排序
	for (int i = 0; i < nCountCells; i++)
		SortIndex[i] = i;

	for (int j = 0; j < nCountCells; j++)
	{
		for (int i = 0; i < nCountCells - j - 1; i++)
		{
			//DI排序
			if (DI[i] < DI[i + 1])
			{
				double temp = DI[i];
				DI[i] = DI[i + 1];
				DI[i + 1] = temp;

				int idx = SortIndex[i];
				SortIndex[i] = SortIndex[i + 1];
				SortIndex[i + 1] = idx;
			}

			//灰度值排序
			//double dMeanGrayValue = dGrayValue1[i] / nCellsPixelArea[i];
			//double dMeanGrayValue1 = dGrayValue1[i + 1] / nCellsPixelArea[i + 1];
			//if (dMeanGrayValue < dMeanGrayValue1)
			//{
			//	double d = dGrayValue1[i];
			//	dGrayValue1[i] = dGrayValue1[i + 1];
			//	dGrayValue1[i + 1] = d;

			//	double d1 = nCellsPixelArea[i];
			//	nCellsPixelArea[i] = nCellsPixelArea[i + 1];
			//	nCellsPixelArea[i + 1] = d1;

			//	int idx = SortIndex[i];
			//	SortIndex[i] = SortIndex[i + 1];
			//	SortIndex[i + 1] = idx;
			//}

			//面积排序
// 			if (nCellsPixelArea[i] < nCellsPixelArea[i + 1])
// 			{
// 				int a = nCellsPixelArea[i];
// 				nCellsPixelArea[i] = nCellsPixelArea[i + 1];
// 				nCellsPixelArea[i + 1] = a;
// 
// 				int idx = SortIndex[i];
// 				SortIndex[i] = SortIndex[i + 1];
// 				SortIndex[i + 1] = idx;
// 			}
		}
	}

	int nCountNUMArray[4] = { 0 };
	for (int i = 0; i < nCountCells; ++i)
	{
		double dou = DI[i] * DI_X;
		if (dou>=1.5 && dou<=2.5)
		{
			nCountNUMArray[1] ++;
		}
		else if (dou > 2.5 && dou <= 3.5)
		{
			nCountNUMArray[2] ++;
		}
		else if (dou > 3.5)
		{
			nCountNUMArray[3] ++;
		}
	}
	TCHAR shangpi_buf[64], linba_buf[64];
	_stprintf(shangpi_buf, _T("%d"), nCountCells);
	_stprintf(linba_buf, _T("%d"), (int)(nCountLymphocyte*0.04));
	TCHAR lpFName[128];
	wsprintf(lpFName, _T("%s\\%s"), m_FileDir.GetString(), _T("cell_result.ini"));
	WritePrivateProfileString(_T("result"), _T("shangpi"), shangpi_buf, lpFName);
	WritePrivateProfileString(_T("result"), _T("linba"), linba_buf, lpFName);
	TCHAR cValueTemp[100];
	_stprintf(cValueTemp, _T("%d"), nCountNUMArray[1]);
	WritePrivateProfileString(_T("result"), _T("erbeiti"), cValueTemp, lpFName);
	_stprintf(cValueTemp, _T("%d"), nCountNUMArray[2]);
	WritePrivateProfileString(_T("result"), _T("sanbeiti"), cValueTemp, lpFName);
	_stprintf(cValueTemp, _T("%d"), nCountNUMArray[3]);
	WritePrivateProfileString(_T("result"), _T("duobeiti"), cValueTemp, lpFName);

	//保存需要统计的信息到Result文件夹中
	FILE *fp_file0 = NULL;
	FILE *fp_file1 = NULL;
	FILE *fp_file2 = NULL;
	FILE *fp_file3 = NULL;
	FILE *fp_file4 = NULL;
	FILE *fp_file5 = NULL;
	fp_file0 = fopen(sResultTxtName0.c_str(), "w+");
	fp_file1 = fopen(sResultTxtName1.c_str(), "w+");
	fp_file2 = fopen(sResultTxtName2.c_str(), "w+");
	fp_file3 = fopen(sResultTxtName3.c_str(), "w+");
	fp_file4 = fopen(sResultTxtName4.c_str(), "w+");
	fp_file5 = fopen(sResultTxtName5.c_str(), "w+");

	for (int i = 0; i < nCountCells; i++)
	{
		fprintf(fp_file0, "%s\n", vCellsfullfilenames[i].c_str());
		fprintf(fp_file1, "%f\n", DI[i]);
		fprintf(fp_file2, "%d\n", nCellsPixelArea[i]);
		fprintf(fp_file3, "%d\n", SortIndex[i]);
		fprintf(fp_file4, "ciod %d _ciod %0.2f cAod %f\n", ciod, _ciod, cAod);
		fprintf(fp_file5, "%f\n", dAOD1[i]);
	}
	fclose(fp_file0);
	fclose(fp_file1);
	fclose(fp_file2);
	fclose(fp_file3);
	fclose(fp_file4);
	fclose(fp_file5);

	//移动文件
	TCHAR lpBin[MAX_PATH];
	InitPath(lpBin, NULL, NULL);
	TCHAR lpini[MAX_PATH];
	wsprintf(lpini, _T("%s%s"), lpBin, _T("number.ini"));
	TCHAR buf[128] = { 0 };
	GetPrivateProfileString(_T("select_path_out"), _T("select_box"), _T("1"), buf, 128, lpini);
	int iSel = _ttoi(buf);
	if (1==iSel)
	{
		TCHAR buf1[256] = { 0 };
		GetPrivateProfileString(_T("select_path_save"), _T("select_path"), _T(""), buf1, 256, lpini);
		CString sArchi(buf1);
		if (!sArchi.IsEmpty())
		{
			CString sOriginal = m_pLabelArchivePath->GetText();
			CString sOriginalName;
			sOriginalName = sOriginal.Mid(sOriginal.ReverseFind('\\') + 1);

			DWORD dw = GetFileAttributes(sOriginal.GetString());
			dw |= FILE_ATTRIBUTE_HIDDEN;
			dw &= ~FILE_ATTRIBUTE_READONLY;
			SetFileAttributes(sOriginal.GetString(), dw);

			nyCopyFile(sOriginal.GetString(), sArchi.GetString());

			nyDeleteFile(sOriginal.GetString());
			//DeleteDirectory(sOriginal, TRUE);

			CString copyFileName;
			copyFileName.Append(sArchi);
			copyFileName.Append(_T("\\"));
			copyFileName.Append(sOriginalName);
			dw &= ~FILE_ATTRIBUTE_HIDDEN;
			dw &= ~FILE_ATTRIBUTE_READONLY;
			SetFileAttributes(copyFileName, dw);

			CString sOriginalEx, sOriginalNameEx;
			sOriginalEx.Append(sOriginal);
			sOriginalEx.Append(_T("Result"));
			sOriginalNameEx.Append(sOriginalName);
			sOriginalNameEx.Append(_T("Result"));
			dw = GetFileAttributes(sOriginalEx.GetString());
			dw |= FILE_ATTRIBUTE_HIDDEN;
			dw &= ~FILE_ATTRIBUTE_READONLY;
			SetFileAttributes(sOriginalEx.GetString(), dw);

			nyCopyFile(sOriginalEx.GetString(), sArchi.GetString());

			nyDeleteFile(sOriginalEx.GetString());
// 			CStringA sOriginalExAA;
// 			sOriginalExAA = sOriginalEx;
// 			nyDeleteFileA(sOriginalExAA.GetString());
			//DeleteDirectory(sOriginalEx, TRUE);

			dw &= ~FILE_ATTRIBUTE_HIDDEN;
			dw &= ~FILE_ATTRIBUTE_READONLY;
			CString copyFileNameEx;
			copyFileNameEx.Append(sArchi);
			copyFileNameEx.Append(_T("\\"));
			copyFileNameEx.Append(sOriginalNameEx);
			SetFileAttributes(copyFileNameEx.GetString(), dw);

			m_pLabelArchivePath->SetText(copyFileName.GetString());
			m_FileDir = copyFileName;
			_tcscpy(gszFolder, copyFileName.GetString());

			TCHAR lpInfo[MAX_PATH] = { 0 };
			TCHAR lpBin[MAX_PATH];
			InitPath(lpBin, NULL, NULL);
			TCHAR lpINI[128];
			wsprintf(lpINI, _T("%s%s"), lpBin, _T("publish.ini"));
			WritePrivateProfileString(_T("archipath"), _T("path"), m_FileDir.GetString(), lpINI);
		}
	}

	m_bIsDisposeCalc = false;

	m_bSuc = true;

	m_aProgress = 100;
	PostMessage(WM_USER + 456, 0, 0);

//	m_pGifAnim->SetVisible(false);

	CString temp_value = _T("");   //temp_value用来处理int值
	temp_value.Format(_T("%d"), ciod);//固定格式
	CString str;
	str.Format(_T("%d"), ciod);
	if (check_count == g_thread_count)
	{
		SOUND_SUCCEED
		MessageBox(GetHWND(), _T("处理完成"), _T("提示"), MB_OK | MB_SYSTEMMODAL);
		SwitchTab(2);
		StatisticsInfo();
	}
	else
	{
		SOUND_WARNING
		MessageBox(GetHWND(), _T("未知错误"), _T("提示"), MB_OK | MB_SYSTEMMODAL);
	}
	
	return 0;
}

int CMainDlgWnd:: _ReadSocket(SOCKET nFile, void * pData, int Size)
{
	int nLeft, nRead, nReadCount;
	char *pcData = (char*)pData;
	ASSERT(pData != NULL && pSize != NULL);
	nLeft = Size;
	nReadCount = -1;
	int count = 0;
	while (nReadCount < count)
	{
		if ((nRead = recv(nFile, pcData, nLeft, 0)) <= 0)
		{
			nRead = 0;
			break;
		}
		if (count==0)
		{
			char buf[9] = { 0 };
			memcpy_s(buf, 8, pData, 8);
			count = atoi(buf);
		}
		nLeft -= nRead;
		pcData += nRead;
		nReadCount += nRead;
		if (nReadCount+1>=count)
		{
			return 1;
		}
	}
	//*pSize = *pSize - nLeft;
	return 0;

}

void CMainDlgWnd::SplitString(vector<string>& vstr, char* src, const char* step)
{
	if (strlen(src)<=0)
	{
		return;
	}
	char *token = strtok(src, step);
	if (token)
	{
		vstr.push_back(token);
	}
	while (token != NULL)
	{
		//printf("%s\n", token);
		token = strtok(NULL, step);
		if (token)
		{
			vstr.push_back(token);
		}
	}
}

void CMainDlgWnd::ShowOneCellSelectionData(int index, CString str[13])
{
	if (index >= 6)
		return;
	for (int i = 0; i < 13; i++)
	{
		if (! str[i].IsEmpty())
		{
			m_pSelectEdit[index][i]->SetText(str[i].GetString());
		}
	}
	UpdateWindow(m_hWnd);
}

void CMainDlgWnd::ShowOneCellSelectionImg(int index, CString strImg)
{
	if (index >= 6)
		return;
	if (strImg.IsEmpty())
		return;
	m_pSelectImg[index]->SetBkImage(strImg.GetString());
	UpdateWindow(m_hWnd);
}

void CMainDlgWnd::ShowPregress(CString str, int progress)
{
	m_pProgress->SetText(str.GetString());
	if (progress>=0 && progress<=100)
	{
		//m_pclock_begin_end->SetVisible(false);
		m_pProgressBar->SetValue(progress);
		SetAniProgress();
	}
	//UpdateWindow(m_hWnd);
}


void CMainDlgWnd::ShowProgressEx(int progress)
{
	if (progress >= 0 && progress <= 100)
	{
		m_pProgressBar->SetValue(progress);
		SetAniProgress();
	}
}

void CMainDlgWnd::SetAniProgress()
{
	if (m_pAniProgress && m_aProgress>0 && m_aProgress <= 100)
	{
		TCHAR buf[256] = { 0 };
		_stprintf(buf, _T("animate/ani/Z%d.png"), m_aProgress);
		m_pAniProgress->SetBkImage(buf);
		_stprintf(buf, _T("%d%%"), m_aProgress);
		m_pProgressPercent->SetText(buf);
	}
}

void CMainDlgWnd::SetShowCell(int row, int column)
{
	g_FLOW = FLOW_CELLLOOK;

	if (row>=_row_cnt || column>=_column_cnt )
		return;

	int curUiIndex = row*_column_cnt + column;
	int index = m_wSelPage*_cnt() + curUiIndex;

	if (g_PatientInfoCur.sName.IsEmpty() || g_PatientInfoCur.sBLH.IsEmpty())
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("请新建一个用户或者病历管理选择一个用户样本"), _T("警告"), MB_OKCANCEL);
		return;
	}

	string sPathFile;
	USES_CONVERSION;
	sPathFile = W2A(m_FileDir);
	if (sPathFile.length() == 0)
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("请选择制片或者打开档案路径"), _T("警告"), MB_OKCANCEL);
		return;
	}
	if (vCellsfullfilenames.size() == 0)
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("请先进行实时筛选"), _T("警告"), MB_OK);
		return;
	}
// 	if (!m_bTatisticsInfo)
// 	{
// 		::MessageBox(m_hWnd, _T("请查看统计信息"), _T("警告"), MB_OK);
// 		return;
// 	}
// 	if (!m_bStatisticsShow)
// 	{
// 		::MessageBox(m_hWnd, _T("请查看统计结果"), _T("警告"), MB_OK);
// 		return;
// 	}

	{
		int i = 0;
		int j = 0;

		//文件夹相关路径
		string sPath;
		string sPathFile;
		string sFilePathName;
		string sDataFileName;
		string sImgFileName;
		string sROIName;
		string sImgPathName;
		string sCellResultPath;

		USES_CONVERSION;
		sPathFile = W2A(m_FileDir);
		int nLocation = sPathFile.rfind("\\");
		sPath = sPathFile.substr(0, nLocation) + "\\";
		sDataFileName = sPathFile.substr(nLocation + 1, sPathFile.length());

		nLocation = SortIndex[index];
		sFilePathName = vCellsfullfilenames[nLocation];

		int loc_temp;
		loc_temp = sFilePathName.rfind("Cells") + 6;
		sImgFileName = sFilePathName.substr(loc_temp, sFilePathName.length() - loc_temp);
		loc_temp = sImgFileName.find("_");
		sROIName = sImgFileName.substr(loc_temp + 1, sImgFileName.find(".") - loc_temp - 1);
		sImgFileName = sImgFileName.substr(0, loc_temp);

		sImgPathName = sPathFile + "\\" + sImgFileName + ".jpg";
		Mat I = imread(sImgPathName.c_str(), 1);

		//读入Cell在原图中的位置信息；并画在原图中
		sCellResultPath = sPathFile + "Result\\Result\\" + sImgFileName + "Cells.csv";;

		Rect BoundingBox(0, 0, 1, 1);

		// 读文件  
		ifstream inFile(sCellResultPath.c_str(), ios::in);
		string lineStr;
		vector<vector<string>> strArray;
		while (getline(inFile, lineStr))
		{
			//// 打印整行字符串  
			//cout << lineStr << endl;
			//// 存成二维表结构  
			//stringstream ss(lineStr);
			//string str;
			//vector<string> lineArray;
			//// 按照逗号分隔  
			//while (getline(ss, str, ','))
			//	lineArray.push_back(str);
			//strArray.push_back(lineArray);

			stringstream ss(lineStr);
			string str;
			getline(ss, str, ',');
			if (strcmp(str.c_str(), sROIName.c_str()) == 0)
			{
				//找到对应的Cells序号，随后直接读取对应Rectangular信息，读到第19次后才是Rectangular信息
				for (i = 0; i < 22-1; i++)
				{
					getline(ss, str, ',');
				}
				//以下为对应的Rectangular.x,Rectangular.y,Rectangular.width,Rectangular.height信息
				getline(ss, str, ',');
				BoundingBox.x = atoi(str.c_str());
				getline(ss, str, ',');
				BoundingBox.y = atoi(str.c_str());
				getline(ss, str, ',');
				BoundingBox.width = atoi(str.c_str());
				getline(ss, str, ',');
				BoundingBox.height = atoi(str.c_str());
				break;
			}
			else
				continue;

		}

		int page, uiIndex;
		bool b = FindIndex(0, page, uiIndex);
		if (m_pBlockPage[0].wSelCount == _cnt_sel_cell && b && page == m_wSelPage && curUiIndex == uiIndex)
		{
			//讲画图的结果进行resize并show出来
			rectangle(I, BoundingBox, Scalar(255, 0, 0), 1);
			putText(I, sROIName, Point(BoundingBox.x, BoundingBox.y), FONT_HERSHEY_COMPLEX, 1, Scalar(255, 0, 0), 1);

			Mat I_resize;
			resize(I, I_resize, Size(I.cols / 2, I.rows / 2), (0, 0), (0, 0), INTER_LINEAR);
			string temp;
			string __filepath1;
			string __filepath2;
			temp = sImgFileName + "_" + sROIName;
			imshow(temp.c_str(), I_resize);
			CStringA aPath;
			aPath = m_FileDir;
			__filepath1 = aPath + "Result\\1.jpg";
			__filepath2 = aPath + "Result\\2.jpg";
			const char* p7 = __filepath1.data();
			int length = sizeof(TCHAR)*(strlen(p7) + 1);
			LPTSTR tcBuffer = new TCHAR[length];
			memset(tcBuffer, 0, length);
			MultiByteToWideChar(CP_ACP, 0, p7, strlen(p7), tcBuffer, length);

			imwrite(__filepath2, I_resize);

			m_bSelFirstCell = true;

// 			MONITORINFO oMonitor = {};
// 			oMonitor.cbSize = sizeof(oMonitor);
// 			::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);
// 			CDuiRect rcWork = oMonitor.rcWork;
// 			RECT rt = { 0 };
// 			::GetWindowRect(m_hWnd, &rt);  //得到的既是相对于屏幕左上角的窗口矩形区域
// 			POINT pt = { rt.left, rt.top };
// 			long lWidth = rt.right - rt.left;
// 			long lHeight = rt.bottom - rt.top;
// 			RECT rcDest = m_pPageTab3->GetPos();
// 			long lDestWidth = rcDest.right - rcDest.left;
// 			long lDestHeight = rcDest.bottom - rcDest.top;
// 			
// 			HWND hwnd = ::GetDesktopWindow();
// 			HDC hDC = ::GetDC(hwnd);//??
// 			RECT rect;
// 			::GetClientRect(hwnd, &rect);//获取屏幕大小??
// 			HDC hDCMem = ::CreateCompatibleDC(hDC);//创建兼容DC??
// 
// 			lWidth = 1000;
// 			lHeight = 600;
// 			HBITMAP hBitMap = ::CreateCompatibleBitmap(hDC, lDestWidth, lDestHeight);//创建兼容位图??
// 			HBITMAP hOldMap = (HBITMAP)::SelectObject(hDCMem, hBitMap);//将位图选入DC，并保存返回值??
// 
// 			::BitBlt(hDCMem, 0, 0, lDestWidth, lDestHeight, hDC, rt.left+rcDest.left, rt.top+rcDest.top, SRCCOPY);//将屏幕DC的图象复制到内存DC中??
// 			//StretchBlt(hDCMem, 0, 0, lWidth, lHeight, hDC, rt.left, rt.top, lWidth, lHeight, SRCCOPY);
// 
// 			CImage image;
// 			image.Attach(hBitMap);
// 			image.Save(tcBuffer);//如果文件后缀为.bmp，则保存为为bmp格式??
// 			image.Detach();
// 
// 			::SelectObject(hDCMem, hOldMap);//选入上次的返回值??
// 
// 			//释放??
// 			::DeleteObject(hBitMap);
// 			::DeleteDC(hDCMem);
// 			::DeleteDC(hDC);


// 			HDC hdc = ::GetWindowDC(m_hWnd);
// 			CDC *pDC = GetDC(m_hWnd);
// 			CDC  memDC;
// 			memDC.CreateCompatibleDC(pDC);
// 			CRect rt;
// 			GetWindowRect(&rt);
// 			CBitmap Bmp;
// 			Bmp.CreateCompatibleBitmap(pDC, rt.Width(), rt.Height());
// 			CBitmap *pBmpPrev = (CBitmap*)memDC.SelectObject(&Bmp);
// 			memDC.BitBlt(0, 0, rt.Width(), rt.Height(), pDC, 0, 0, SRCCOPY);
// 			CImage image;
// 			image.Attach((HBITMAP)Bmp.m_hObject);
// 			image.Save(tcBuffer);

			waitKey(10);
		}
		else
		{
			//讲画图的结果进行resize并show出来
			rectangle(I, BoundingBox, Scalar(255, 255, 0), 1);
			putText(I, sROIName, Point(BoundingBox.x, BoundingBox.y), FONT_HERSHEY_COMPLEX, 1, Scalar(255, 0, 0), 1);

			Mat I_resize;
			resize(I, I_resize, Size(I.cols / 2, I.rows / 2), (0, 0), (0, 0), INTER_LINEAR);
			string temp;
			temp = sImgFileName + "_" + sROIName;
			imshow(temp.c_str(), I_resize);
			waitKey(10);
		}
	}
}

void CMainDlgWnd::DelShowCell(int index)
{
	if (index >= 32 || index < 0)
		return;

	if (g_PatientInfoCur.sName.IsEmpty() || g_PatientInfoCur.sBLH.IsEmpty())
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("请新建一个用户或者病历管理选择一个用户样本"), _T("警告"), MB_OKCANCEL);
		return;
	}

	string sPathFile11;
	USES_CONVERSION;
	sPathFile11 = W2A(m_FileDir);
	if (sPathFile11.length() == 0)
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("请选择制片或者打开档案路径"), _T("警告"), MB_OKCANCEL);
		return;
	}
	if (vCellsfullfilenames.size() == 0)
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("请先进行实时筛选"), _T("警告"), MB_OK);
		return;
	}
// 	if (!m_bTatisticsInfo)
// 	{
// 		::MessageBox(m_hWnd, _T("请查看统计信息"), _T("警告"), MB_OK);
// 		return;
// 	}
// 	if (!m_bStatisticsShow)
// 	{
// 		::MessageBox(m_hWnd, _T("请查看统计结果"), _T("警告"), MB_OK);
// 		return;
// 	}

	string sDataFileResultName;
	string sPath;
	string sPathFile;
	string sFilePathName;
	string sDataFileName;
	string sResultDraw1;
	string sResultDraw2;

	delete_data(index);

	sPathFile = W2A(m_FileDir);

	int nLocation = sPathFile.rfind("\\");
	sPath = sPathFile.substr(0, nLocation) + "\\";
	sDataFileName = sPathFile.substr(nLocation + 1, sPathFile.length());

	//// TODO:  在此添加控件通知处理程序代码
	//int nLocation = sPathFile.rfind("\\");
	//sPath = sPathFile.substr(0, nLocation) + "\\";
	//sDataFileName = sPathFile.substr(nLocation + 1, sPathFile.length());
	sPathFile = sPathFile + "\\*.jpg";

	int pImageLocX[8] = { 12, 155, 295, 430, 558, 690, 820, 930 };
	int pImageLocY[4] = { 10, 150, 305, 460 };
	//显示排序后的前N个上皮细胞及DNA含量
	for (int i = 0; i < 32; i++)
	{
		int nLocation = SortIndex[i];
		double dLambda = ((DI[i]) * DI_X);
		sFilePathName = vCellsfullfilenames[nLocation];
		char cValueTemp[100];
		string sLambda;
		sprintf_s(cValueTemp, "%f", dLambda);
		sLambda = cValueTemp;
		CString str1(sLambda.c_str());
		//OutputDebugString(_T("1233\n"));
		//显示图片
// 		CString sFileName(sFilePathName.c_str());
// 		m_pStatisticsImg[i]->SetBkImage(sFileName.GetString());
// 		//edit
// 		m_pStatisticsEdit[i]->SetText(str1.GetString());
	}
}

void CMainDlgWnd::delete_data(int index)
{
	int i = 0;
	for (i = index; i < 100; i++)
	{
		SortIndex[i] = SortIndex[1 + i];
		DI[i] = DI[1 + i];
		dCircleDegree[i] = dCircleDegree[1 + i];
		nCellsPixelArea[i] = nCellsPixelArea[1 + i];
	}
}

void CMainDlgWnd::SelPage(WORD wSel)
{
	if (wSel >= _cnt_show_page)
		return;

	for (int i = 0; i < _cnt_show_page; ++i)
	{
		double d = (DI[i*_cnt() + 0]) * DI_X;
		if (d<0.000001)
		{
			m_wCurShowPageCount = i;
			break;
		}
	}
	if (m_wCurShowPageCount==0)
	{
		return;
	}

	CString str;
	str.Format(_T("第 %d 页 共 %d 页"), wSel+1, m_wCurShowPageCount);
	if (wSel + 1 == _cnt_show_page)
		str.Format(_T("第 %d 页 共 %d 页"), wSel + 1, m_wCurShowPageCount);
	m_pLabelPage->SetText(str.GetString());

	OutputDebugString(_T("SetPageItem begin.\n"));
	m_iValidIndex = -1;
	SetPageItem();
	OutputDebugString(_T("SetPageItem end.\n"));
}


void CMainDlgWnd::SetPageItem()
{
	int result[_cnt_sel_cell];
	int count;
	bool b = GetPageIndex(m_wSelPage, result, count);
	for (int i = 0; i < _cnt(); ++i)
	{
		int row = i / _column_cnt;
		int column = i % _column_cnt;

		double dLambda = ((DI[m_wSelPage*_cnt() + i]) * DI_X);
		//if (dLambda < 0.000001 || dAOD1[i]>cAod &&nCellsPixelArea[i] < 600)
		if (dLambda < 0.000001)
		{
			SetPageItemImg(i, _T(""));
			SetPageItemDI(i, _T(""));

			m_pBlockPage[0].image[row][column]->SetBorderSize(1);
			m_pBlockPage[0].image[row][column]->SetBorderColor(GetColorDword(_T("ffcccccc")));
			m_pBlockPage[0].image[row][column]->SetUserData(_T(""));
		}
		else
		{
			m_iValidIndex = i;

			int nLocation = SortIndex[m_wSelPage*_cnt() + i];
			string sImg = vCellsfullfilenames[nLocation];
			CString sImg1(sImg.c_str());
			SetPageItemImg(i, sImg1);

			CString sEdit;
			sEdit.Format(_T("%.6f"), dLambda);
			SetPageItemDI(i, sEdit);
		}

		bool bFind = false;
		if (b)
		{
			for (int j = 0; j < count; ++j)
			{
				if (result[j]==i)
				{
					bFind = true;
					break;
				}
			}
		}
		if (b && bFind)
		{
			m_pBlockPage[0].image[row][column]->SetBorderSize(4);
			m_pBlockPage[0].image[row][column]->SetBorderColor(GetColorDword(_T("ff3543DE")));
			m_pBlockPage[0].image[row][column]->SetUserData(_T("1"));
		}
		else
		{
			m_pBlockPage[0].image[row][column]->SetBorderSize(1);
			m_pBlockPage[0].image[row][column]->SetBorderColor(GetColorDword(_T("ffcccccc")));
			m_pBlockPage[0].image[row][column]->SetUserData(_T(""));
		}
	}
}

void CMainDlgWnd::SetPageItemImg(int index, CString path)
{
	if (!(index >= 0 && index < _cnt()))
		return;
	int row = index / _column_cnt;
	int column = index % _column_cnt;
	m_pBlockPage[0].image[row][column]->SetBkImage(path.GetString());
}

void CMainDlgWnd::SetPageItemDI(int index, CString di)
{
	if (!(index >= 0 && index < _cnt()))
		return;
	int row = index / _column_cnt;
	int column = index % _column_cnt;
	m_pBlockPage[0].edit[row][column]->SetText(di.GetString());
}


bool CMainDlgWnd::FindIndex(int desIndex, int& page, int& uiIndex)
{
	if (desIndex < 0 || desIndex >= _cnt_sel_cell)
		return false;
	page = -1;
	uiIndex = -1;
	if (desIndex < m_pBlockPage[0].wSelCount)
	{
		int idx[_cnt_sel_cell] = { -1 }, idx1[_cnt_sel_cell] = { -1 };
		int count = 0, count1 = 0;
		int minindex = _cnt_sel_cell;
		int maxindex = -1;
		for (int i = 0; i < m_pBlockPage[0].wSelCount; ++i)
		{
			if (m_pBlockPage[0].curselindex[i][1] < minindex)
			{
				minindex = m_pBlockPage[0].curselindex[i][1];
			}
			if (m_pBlockPage[0].curselindex[i][1] > maxindex)
			{
				maxindex = m_pBlockPage[0].curselindex[i][1];
			}
		}
		for (int i = 0; i < m_pBlockPage[0].wSelCount; ++i)
		{
			if (m_pBlockPage[0].curselindex[i][1] == minindex)
			{
				idx[count++] = m_pBlockPage[0].curselindex[i][2];
			}
			if (m_pBlockPage[0].curselindex[i][1] == maxindex)
			{
				idx1[count1++] = m_pBlockPage[0].curselindex[i][2];
			}
		}
		int minindex2 = 100000;
		for (int i = 0; i < count; ++i)
		{
			if (idx[i] < minindex2)
				minindex2 = idx[i];
		}
		int maxindex2 = -1;
		for (int i = 0; i < count1; ++i)
		{
			if (idx[i] > maxindex2)
				maxindex2 = idx1[i];
		}
		int findex = -1;
		for (int i = 0; i < m_pBlockPage[0].wSelCount; ++i)
		{
			if (desIndex == 0)
			{
				if (m_pBlockPage[0].curselindex[i][1] == minindex &&
					m_pBlockPage[0].curselindex[i][2] == minindex2)
				{
					findex = i;
					break;
				}
			}
			else
			{
				if (m_pBlockPage[0].curselindex[i][1] == maxindex &&
					m_pBlockPage[0].curselindex[i][2] == maxindex2)
				{
					findex = i;
					break;
				}
			}
		}
		page = m_pBlockPage[0].curselindex[findex][1];
		uiIndex = m_pBlockPage[0].curselindex[findex][2];
		if (uiIndex >= 0 && uiIndex<_cnt() && page >= 0 && page<_cnt_show_page)
		{
			return true;
		}
		assert(false);
		return false;
	}
	return false;
}


bool CMainDlgWnd::IsDiIndex(int index)
{
	if (index < 0)
		return false;

	int idx[_cnt_sel_cell] = { -1 };
	int count = 0;
	for (int i = 0; i < m_pBlockPage[0].wSelCount; ++i)
	{
		idx[count++] = m_pBlockPage[0].curselindex[i][1] * _cnt() + m_pBlockPage[0].curselindex[i][2];
	}
	sort(idx, idx + m_pBlockPage[0].wSelCount);
	for (int i = 0; i < count; ++i)
	{
		if (index == idx[i])
		{
			return true;
		}
	}
	if (index>=idx[count-1])
	{
		return true;
	}
	return false;
}

bool CMainDlgWnd::GetPageIndex(int page, int res[_cnt_sel_cell], int&count)
{
	if (page < 0 || page >= _cnt_show_page)
		return false;
	memset(res, -1, sizeof(int)*_cnt_sel_cell);
	count = 0;
	for (int i = 0; i < m_pBlockPage[0].wSelCount; ++i)
	{
		if (page == m_pBlockPage[0].curselindex[i][1])
		{
			res[count++] = m_pBlockPage[0].curselindex[i][2];
		}
	}
	return count>0;
}


bool CMainDlgWnd::GetSelCell(CString allpath[_cnt_sel_cell], CString alledit[_cnt_sel_cell])
{
	bool b = false;
	int cnt = 0;
	struct TailData
	{
		CString path;
		CString edit;
	};
	map<float, TailData> mSort;
	for (int i = 0; i < m_pBlockPage[0].wSelCount; ++i)
	{
		int page = m_pBlockPage[0].curselindex[i][1];
		int index = m_pBlockPage[0].curselindex[i][2];
		int sortindex = page*_cnt() + index;
		int nLocation = SortIndex[sortindex];
		string sImg = vCellsfullfilenames[nLocation];
		CString sImg1(sImg.c_str());
		allpath[i] = sImg1;
		if (!PathFileExists(sImg1.GetString()))
			cnt++;
		CString sEdit;
		double dLambda = ((DI[sortindex]) * DI_X);
		sEdit.Format(_T("%.6f"), dLambda);
		alledit[i] = sEdit;
		TailData data;
		data.path = sImg1;
		data.edit = sEdit;
		mSort.insert(pair<float, TailData>(dLambda, data));
	}
	if (cnt == 0)
	{
		int index = 0;
		map<float, TailData>::reverse_iterator iter;
		for (iter = mSort.rbegin(); iter != mSort.rend(); ++iter)
		{
			allpath[index] = iter->second.path;
			alledit[index] = iter->second.edit;
			index++;
		}
	}
	
	return cnt == 0;
}

void CMainDlgWnd::SelImg(int row, int column)
{
	if (row >= _row_cnt || column >= _column_cnt)
		return;

	//文件夹相关路径
	string sPath;
	string sPathFile;
	string sFilePathName;
	string sDataFileName;
	OutputDebugString(_T("111111111111111111111111\n"));
	if (g_PatientInfoCur.sName.IsEmpty() || g_PatientInfoCur.sBLH.IsEmpty())
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("请新建一个用户或者病历管理选择一个用户样本"), _T("警告"), MB_OKCANCEL);
		return;
	}
	USES_CONVERSION;
	sPathFile = W2A(m_FileDir);
	if (sPathFile.length() == 0)
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("请选择制片或者打开档案路径"), _T("警告"), MB_OKCANCEL);
		return;
	}
	if (vCellsfullfilenames.size() == 0)
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("请先进行实时筛选"), _T("警告"), MB_OK);
		return;
	}
	if (!m_bTatisticsInfo)
	{
		if (IsDisposeCalc())
		{
			SOUND_WARNING
			::MessageBox(m_hWnd, _T("当前样本检测中，请等待操作完成."), _T("警告"), MB_OKCANCEL);
			return;
		}
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("请先进行实时筛选"), _T("警告"), MB_OK);
		return;
	}

	CDuiString data = m_pBlockPage[0].image[row][column]->GetUserData();
	m_pBlockPage[0].image[row][column]->SetBorderSize(4);
	if (data.Compare(_T("1")) ==0)
	{
		if (m_pBlockPage[0].wSelCount>0)
		{
			m_pBlockPage[0].image[row][column]->SetBorderSize(1);
			m_pBlockPage[0].image[row][column]->SetBorderColor(GetColorDword(_T("ffcccccc")));
			m_pBlockPage[0].image[row][column]->SetUserData(_T(""));

			int idx = -1;
			for (int i = 0; i < m_pBlockPage[0].wSelCount; ++i)
			{
				int page = m_pBlockPage[0].curselindex[i][1];
				int uiidx = m_pBlockPage[0].curselindex[i][2];
				if (page==m_wSelPage)
				{
					int index1 = row*_column_cnt + column;
					if ( index1 ==uiidx )
					{
						idx = i;
						break;
					}
				}
			}
			if (idx!=-1)
			{
				int	temp[_cnt_sel_cell][3] = { -1 };
				int icount = 0;
				for (int i = 0; i < idx; ++i)
				{
					temp[i][0] = i;
					temp[i][1] = m_pBlockPage[0].curselindex[i][1];
					temp[i][2] = m_pBlockPage[0].curselindex[i][2];
					icount++;
				}
				for (int i = idx + 1; i < m_pBlockPage[0].wSelCount; ++i)
				{
					temp[icount][0] = icount;
					temp[icount][1] = m_pBlockPage[0].curselindex[i][1];
					temp[icount][2] = m_pBlockPage[0].curselindex[i][2];
					icount++;
				}
				assert(icount == m_pBlockPage[0].wSelCount - 1);
				m_pBlockPage[0].wSelCount--;

				memcpy_s(m_pBlockPage[0].curselindex, sizeof(m_pBlockPage[0].curselindex), temp, sizeof(temp));
			}
		}
	}
	else
	{
		if (m_pBlockPage[0].wSelCount<_cnt_sel_cell)
		{
			m_pBlockPage[0].image[row][column]->SetBorderSize(4);
			m_pBlockPage[0].image[row][column]->SetBorderColor(GetColorDword(_T("ff3543DE")));
			m_pBlockPage[0].image[row][column]->SetUserData(_T("1"));
			m_pBlockPage[0].curselindex[m_pBlockPage[0].wSelCount][0] = m_pBlockPage[0].wSelCount;
			m_pBlockPage[0].curselindex[m_pBlockPage[0].wSelCount][1] = m_wSelPage;
			m_pBlockPage[0].curselindex[m_pBlockPage[0].wSelCount][2] = row*_column_cnt + column;
			m_pBlockPage[0].wSelCount++;
		}
		else
		{
			SOUND_WARNING
			::MessageBox(m_hWnd, _T("当前已经选择合适的细胞个数(20)."), _T("提示"), MB_OKCANCEL);
		}
	}

	TCHAR buf[64] = { 0 };
	_stprintf(buf, _T("当前选择的总数%d\r\n"), m_pBlockPage[0].wSelCount);
	OutputDebugString(buf);

	TCHAR buf1[128] = { 0 };
	_stprintf(buf1, _T("已选细胞个数为:%d"), m_pBlockPage[0].wSelCount);
	m_pLabelSelCellCount->SetText(buf1);
}

void CMainDlgWnd::StatisticsInfo()
{
	g_FLOW = FLOW_STATISTICS;
	nCountCells;
	//循环计数器
	int i = 0;
	int j = 0;

	//文件夹相关路径
	string sPath;
	string sPathFile;
	string sFilePathName;
	string sDataFileName;
	string sResultDraw1;
	string sResultDraw2;

	if (g_PatientInfoCur.sName.IsEmpty() || g_PatientInfoCur.sBLH.IsEmpty())
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("请新建一个用户或者病历管理选择一个用户样本"), _T("警告"), MB_OKCANCEL);
		return;
	}
	USES_CONVERSION;
	sPathFile = W2A(m_FileDir);
	if (sPathFile.length() == 0)
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("请选择制片或者打开档案路径"), _T("警告"), MB_OKCANCEL);
		return;
	}
	if (vCellsfullfilenames.size() == 0)
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("请先进行实时筛选"), _T("警告"), MB_OK);
		return;
	}
	{
		int nLocation = sPathFile.rfind("\\");
		sPath = sPathFile.substr(0, nLocation) + "\\";
		sDataFileName = sPathFile.substr(nLocation + 1, sPathFile.length());

		if (nCountCells == 0)
		{
			string CellsPixelAreaPath = sPathFile + "Result\\Result\\CellsPixelArea.txt";
			string SortIndexPath = sPathFile + "Result\\Result\\SortIndex.txt";
			FILE *fp3;
			FILE *fp4;
			fp3 = fopen(CellsPixelAreaPath.c_str(), "r+");
			fp4 = fopen(SortIndexPath.c_str(), "r+");
			while (!feof(fp3))
			{
				string vCellsfullfilename_temp = "";
				double DI_temp = 0;
				int nCellsPixelArea_temp = 0;
				int SortIndex_temp = 0;
				fscanf(fp3, "%d\n", &nCellsPixelArea_temp);
				fscanf(fp4, "%d\n", &SortIndex_temp);

				nCellsPixelArea[nCountCells] = nCellsPixelArea_temp;
				SortIndex[nCountCells] = SortIndex_temp;
				nCountCells++;
			}
			fclose(fp3);
			fclose(fp4);
			for (i = 0; i < nCountCells; i++)
			{
				DI[i] = DI_nextuse[i];
				vCellsfullfilenames.push_back(vCellsfullfilenames_nextuse[i]);
			}
		}
		OutputDebugString(_T("abc\n"));
		{

			sPathFile = sPathFile + "\\*.jpg";

			
			int pImageLocX[8] = { 12, 155, 295, 430, 558, 690, 820, 930 };
			int pImageLocY[4] = { 10, 150, 305, 460 };
			//显示排序后的前N个上皮细胞及DNA含量
			for (i = 0; i < 32; i++)
			{
				int nLocation = SortIndex[i];
				double dLambda = ((DI[i]) * DI_X);
				sFilePathName = vCellsfullfilenames[nLocation];
				char cValueTemp[100];
				string sLambda;
				sprintf_s(cValueTemp, "%f", dLambda);
				sLambda = cValueTemp;
				CString str1(sLambda.c_str());
				OutputDebugString(_T("1233\n"));

				//显示图片
				CString sFileName(sFilePathName.c_str());
				//m_pStatisticsImg[i]->SetBkImage(sFileName.GetString());
				//edit
				//m_pStatisticsEdit[i]->SetText(str1.GetString());
			}
			OutputDebugString(_T("22222\n"));

			SelPage(m_wSelPage);

			m_bTatisticsInfo = true;

			return;
		}

	}OutputDebugString(_T("9999\n"));
}

void CMainDlgWnd::ShowStatisticResult()
{
	g_FLOW = FLOW_STATISTICSSHOW;


// 	CString s[_cnt_sel_cell];
// 	for (int i = 0; i < _cnt_sel_cell; ++i)
// 	{
// 		s[i] = _T("E:\\work11\\TestResult\\Cells\\000 - 022_75.bmp");
// 	}
// 	SaveCellImage( s, _T("E:\\work11\\TestResult\\1.jpg"));
//   return;

	if (gMainDlgWnd->IsDisposeCalc())
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("当前样本检测中，请等待操作完成."), _T("警告"), MB_OKCANCEL);
		return;
	}

	int i = 0;
	int j = 0;

	//文件夹相关路径
	string sPath;
	string sPathFile;
	string sFilePathName;
	string sDataFileName;
	OutputDebugString(_T("111111111111111111111111\n"));
	if (g_PatientInfoCur.sName.IsEmpty() || g_PatientInfoCur.sBLH.IsEmpty())
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("请新建一个用户或者病历管理选择一个用户样本"), _T("警告"), MB_OKCANCEL);
		return;
	}
	USES_CONVERSION;
	sPathFile = W2A(m_FileDir);
	if (sPathFile.length() == 0)
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("请选择制片或者打开档案路径"), _T("警告"), MB_OKCANCEL);
		return;
	}
	if (vCellsfullfilenames.size() == 0)
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("请先进行实时筛选"), _T("警告"), MB_OK);
		return;
	}
	if (m_pBlockPage[0].wSelCount < _cnt_sel_cell)
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("请选择20个合适的细胞."), _T("警告"), MB_OKCANCEL);
		return;
	}
// 	if (!m_bTatisticsInfo)
// 	{
// 		::MessageBox(m_hWnd, _T("请查看统计信息"), _T("警告"), MB_OK);
// 		return;
// 	}
	{
		int nLocation = sPathFile.rfind("\\");
		sPath = sPathFile.substr(0, nLocation) + "\\";
		sDataFileName = sPathFile.substr(nLocation + 1, sPathFile.length());
		OutputDebugString(_T("22222222222222222\n"));
		if (nCountCells == 0)
		{
			string CellsPixelAreaPath = sPathFile + "Result\\Result\\CellsPixelArea.txt";
			string SortIndexPath = sPathFile + "Result\\Result\\SortIndex.txt";
			FILE *fp3;
			FILE *fp4;
			OutputDebugString(_T("3333333333333-1\n"));
			fp3 = fopen(CellsPixelAreaPath.c_str(), "r+");
			fp4 = fopen(SortIndexPath.c_str(), "r+");
			while (!feof(fp3))
			{
				string vCellsfullfilename_temp = "";
				double DI_temp = 0;
				int nCellsPixelArea_temp = 0;
				int SortIndex_temp = 0;
				fscanf(fp3, "%d\n", &nCellsPixelArea_temp);
				fscanf(fp4, "%d\n", &SortIndex_temp);

				nCellsPixelArea[nCountCells] = nCellsPixelArea_temp;
				SortIndex[nCountCells] = SortIndex_temp;
				nCountCells++;
			}
			fclose(fp3);
			fclose(fp4);
			for (i = 0; i < nCountCells; i++)
			{
				DI[i] = DI_nextuse[i];
				vCellsfullfilenames.push_back(vCellsfullfilenames_nextuse[i]);
			}
		}
		OutputDebugString(_T("3333333333333333\n"));
		{

			sPathFile = sPathFile + "\\*.jpg";

			//对DNA含量分布进行统计
			double dMaxDNA = 0.0;
			double dMinDNA = 1000.0;
			dMaxDNA = 0.0;
			for (i = 0; i < nCountCells; i++)
			{
				if (dMaxDNA < DI[i])
				{
					dMaxDNA = DI[i];
				}
				if (dMinDNA > DI[i])
				{
					dMinDNA = DI[i];
				}
			}
			dMinDNA = dMinDNA - dMinDNA / 10;
			OutputDebugString(_T("5555555555555\n"));

			memset(nCountNUMArray, 0, sizeof(nCountNUMArray));
			int uipage = -1, uiIndex = -1, firstIndex = -1;
			bool bFindIndex = FindIndex(0, uipage, uiIndex);
			if (bFindIndex)
			{
				firstIndex = uipage * _cnt() + uiIndex;
			}
			for (int i = firstIndex; i < nCountCells; ++i)
			{
				if (! IsDiIndex(i))
				    continue;
				double dVal = 0.0, dVal1 = 0.1;
				for (int j = 0; j < 81; ++j)
				{
					dVal = 0.1 * j;
					dVal1 = 0.1*(j + 1);
					double dtemp = DI[i] * DI_X;
					if (j == 80)
					{
						if (dtemp > dVal)
						{
							nCountNUMArray[j]++;
						}
					}
					else
					{
						if (dtemp > dVal && dtemp <= dVal1)
						{
							nCountNUMArray[j]++;
						}
					}
				}
			}
		
			int nMaxCount = 0;
			for (i = 0; i < 81; i++)
			{
				if (nMaxCount < nCountNUMArray[i])
				{
					nMaxCount = nCountNUMArray[i];
				}
			}

#define pox_left	234
#define pox_delta	16
#define pox_height  734
#define pox_bottom  66
#define pox_h		(pox_height+pox_bottom)

			double nRatio = double(nMaxCount) / (801 - 140);
			OutputDebugString(_T("777777777777\n"));
			Mat Background1 = imread("background2.png", 1);


			//int Pos_X[22] = { 234, 273, 312, 351, 389, 428, 467, 505, 544, 583, 621, 660, 699, 738, 776, 815, 854, 892, 931, 970, 1008,1046};
			int Pos_X[83] = { 234, 250, 266, 282, 298, 314, 330, 346, 362, 378,
							  394, 410, 426, 442, 458, 474, 490, 506, 522, 538,
							  554, 570, 586, 602, 618, 634, 650, 666, 682, 698, 
							  714, 730, 746, 762, 778, 794, 810, 826, 842, 858,
							  874, 890, 906, 922, 938, 954, 970, 986, 1002, 1018,
							  1034, 1050, 1066, 1082, 1098, 1114, 1130, 1146, 1162, 1178, 
							  1194, 1210, 1226, 1242, 1258, 1274, 1290, 1306, 1322, 1338, 
							  1354, 1370, 1386, 1402, 1418, 1434, 1450, 1466, 1482, 1498,
							  1514, 1530 };
			//117 * 2 = pox_left   464 * 2 = 928
			//width 347 * 2 = 694  694 / 9 = 77.11  8 --4.995
			//height = 400 * 2 = 800
			memset(Pos_X, 0, sizeof(Pos_X));
			int iBeginX = pox_left/*, iWidthX = 694*/, iItemWidth = 7, iScale = 77, iScaleRemainder = 7;
			int iRectWidth = 20, iRectWidth1 = 7;
			int iCritical = 5, iCriticalIndex = -1;//di值
			for (int i = 0; i < 81; ++i)
			{
				int iScaleCount = i / 10;
				int iScaleRelativeIdx = i % 10;
				int iScaleBeginX = iScaleRemainder, iScaleEndX = iScaleRemainder - iScaleBeginX;
				int x = iBeginX + iScaleCount*iScale + iScaleBeginX + iItemWidth*iScaleRelativeIdx;
				Pos_X[i] = x;
				if (iBeginX + iCritical*iScale < x)
				{
					if (iCriticalIndex==-1)
						iCriticalIndex = i;
				}
			}
			Pos_X[81] += iRectWidth;

			//对21个统计数据画图；慢速筛选画图
			int iRedFirstCnt = 0;
			for (int i = 0; i < 81; ++i)
			{
				Rect BoundingBox(0, 0, 1, 1);
				BoundingBox.x = Pos_X[i];
				BoundingBox.y = 801 - nCountNUMArray[i] / nRatio;
				BoundingBox.width = Pos_X[i + 1] - Pos_X[i]; //iRectWidth;
				BoundingBox.height = 801 - BoundingBox.y;
				if (i >= iCriticalIndex)
				{
					rectangle(Background1, BoundingBox, Scalar(0, 0, 255), -1);
				}
				else
				{
					rectangle(Background1, BoundingBox, Scalar(255, 130, 0), -1);
				}
			}

			if (nCountNUMArray[80] > 0)
			{
				TCHAR lpBin[MAX_PATH];
				InitPath(lpBin, NULL, NULL);
				CString strpath = lpBin;
				CString name = _T("警报.wav");
				strpath += name;
				//ShellExecute(NULL, _T("open"), name, NULL, lpBin, SW_SHOWNORMAL);
				SOUND_WARNING
			}
			OutputDebugString(_T("88888888888\n"));
			//纵坐标标记；
			string label_value;
			char cValueTemp[100];
			sprintf_s(cValueTemp, "%d", 0);
			label_value = cValueTemp;
			double iThickness = 1.5;
			int iFontFace = FONT_HERSHEY_COMPLEX_SMALL;
			putText(Background1, label_value, Point(150, 801 + 5), iFontFace, iThickness, Scalar(0, 0, 0), 1);
			sprintf_s(cValueTemp, "%d", nMaxCount / 9 * 2);
			label_value = cValueTemp;
			putText(Background1, label_value, Point(150, 654 + 5), iFontFace, iThickness, Scalar(0, 0, 0), 1);
			sprintf_s(cValueTemp, "%d", nMaxCount / 9 * 4);
			label_value = cValueTemp;
			putText(Background1, label_value, Point(150, 507 + 5), iFontFace, iThickness, Scalar(0, 0, 0), 1);
			sprintf_s(cValueTemp, "%d", nMaxCount / 9 * 6);
			label_value = cValueTemp;
			putText(Background1, label_value, Point(150, 361 + 5), iFontFace, iThickness, Scalar(0, 0, 0), 1);
			sprintf_s(cValueTemp, "%d", nMaxCount / 9 * 8);
			label_value = cValueTemp;
			putText(Background1, label_value, Point(150, 214 + 5), iFontFace, iThickness, Scalar(0, 0, 0), 1);
			//横坐标标记
			sprintf_s(cValueTemp, "%d", 0);
			label_value = cValueTemp;
			putText(Background1, label_value, Point(228, 851), iFontFace, iThickness, Scalar(0, 0, 0), 1);
			sprintf_s(cValueTemp, "%d", 1);
			label_value = cValueTemp;
			putText(Background1, label_value, Point(303, 851), iFontFace, iThickness, Scalar(0, 0, 0), 1);
			sprintf_s(cValueTemp, "%d", 2);
			label_value = cValueTemp;
			putText(Background1, label_value, Point(380, 851), iFontFace, iThickness, Scalar(0, 0, 0), 1);
			sprintf_s(cValueTemp, "%d", 3);
			label_value = cValueTemp;
			putText(Background1, label_value, Point(458, 851), iFontFace, iThickness, Scalar(0, 0, 0), 1);
			sprintf_s(cValueTemp, "%d", 4);
			label_value = cValueTemp;
			putText(Background1, label_value, Point(535, 851), iFontFace, iThickness, Scalar(0, 0, 0), 1);
			sprintf_s(cValueTemp, "%d", 5);
			label_value = cValueTemp;
			putText(Background1, label_value, Point(613, 851), iFontFace, iThickness, Scalar(0, 0, 0), 1);
			sprintf_s(cValueTemp, "%d", 6);
			label_value = cValueTemp;
			putText(Background1, label_value, Point(690, 851), iFontFace, iThickness, Scalar(0, 0, 0), 1);
			sprintf_s(cValueTemp, "%d", 7);
			label_value = cValueTemp;
			putText(Background1, label_value, Point(767, 851), iFontFace, iThickness, Scalar(0, 0, 0), 1);
			sprintf_s(cValueTemp, "%d", 8);
			label_value = cValueTemp;
			putText(Background1, label_value, Point(845, 851), iFontFace, iThickness, Scalar(0, 0, 0), 1);
			sprintf_s(cValueTemp, "%d", 9);
			label_value = cValueTemp;
			putText(Background1, label_value, Point(922, 851), iFontFace, iThickness, Scalar(0, 0, 0), 1);
			sprintf_s(cValueTemp, "%d", 10);
			label_value = cValueTemp;
			putText(Background1, label_value, Point(999, 851), iFontFace, iThickness, Scalar(0, 0, 0), 1);

			//Title
//			putText(Background1, "DNA Content_Cell Number_Histogram", Point(300, 45), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0), 1);
			OutputDebugString(_T("9999999999\n"));
			resize(Background1, Background1, Size(Background1.cols / 2, Background1.rows / 2), (0, 0), (0, 0), CV_INTER_AREA);
			imshow("DNA_Content_Cell_Number_Histogram", Background1);
			waitKey(10);
			string sResultDraw1 = sPath + sDataFileName + "Result\\DNA_Content_Cell_Number_Histogram.jpg";
			imwrite(sResultDraw1, Background1);
			//20190812zcw更新

			//DNA含量与上皮细胞面积散点图分布
			int nMaxArea = 0;
			for (int i = firstIndex; i < nCountCells; i++)
			{
				if (!IsDiIndex(i))
					continue;
				if (nMaxArea < nCellsPixelArea[i])
				{
					nMaxArea = nCellsPixelArea[i];
				}
			}
			double nRatioY = double(nMaxArea) / (801 - 140);
			double nRatioX = 8.0 / (855 - pox_left);
			OutputDebugString(_T("99999-2\n"));
			Mat Background2 = imread("background1.png", IMREAD_COLOR);
			for (int i = firstIndex; i < nCountCells; i++)
			{ 
				if (!IsDiIndex(i))
					continue;
// 				if (dAOD1[i]>cAod &&nCellsPixelArea[i] < 600)
// 				{
// 					continue;
// 				}
				double loc_y = 801 - nCellsPixelArea[SortIndex[i]] / nRatioY;
				if (loc_y>= 801-2)
				{
					loc_y = 801 - 4 - i % 15;
				}
				double loc_x = pox_left + DI[i]*2 / nRatioX;
				if (loc_x<=pox_left)
				{
					loc_x = pox_left+i%15+4;
				}
				int w = 10, h = 20;
				if (loc_x > 621)
				{
					//circle(Background2, Point(loc_x, loc_y), 3, Scalar(0, 0, 255), -1);
					//画十字
					line(Background2, cvPoint(loc_x - w / 2, loc_y), cvPoint(loc_x + w / 2, loc_y), CV_RGB(255, 0, 0), 2, CV_AA, 0);
					line(Background2, cvPoint(loc_x, loc_y - h / 2), cvPoint(loc_x, loc_y + h / 2), CV_RGB(255, 0, 0), 2, CV_AA, 0);
				}
				else
				{
					//circle(Background2, Point(loc_x, loc_y), 3, Scalar(0, 255, 0), -1);
					//画十字
					line(Background2, cvPoint(loc_x - w / 2, loc_y), cvPoint(loc_x + w / 2, loc_y), CV_RGB(0, 130, 255), 2, CV_AA, 0);
					line(Background2, cvPoint(loc_x, loc_y - h / 2), cvPoint(loc_x, loc_y + h / 2), CV_RGB(0, 130, 255), 2, CV_AA, 0);
				}
			}
//#define		a_x 4
//#define		a_2 2.0
//			for (double d = 0.0; d < a_x; d += 0.1)
//			{
//				double x = pox_left + d / nRatioX;
//				double dm = a_2 * a_x * a_x / (801 - 320);
//				double y = 801 - a_2*d*d / dm;
//				circle(Background2, Point(x, y), 3, Scalar(0, 215, 255), -1);
//			}
			//y label
			sprintf_s(cValueTemp, "%d", 0);
			label_value = cValueTemp;
			putText(Background2, label_value, Point(150, 801 + 5), iFontFace, iThickness, Scalar(0, 0, 0), 1);
			sprintf_s(cValueTemp, "%d", nMaxArea / 9 * 2);
			label_value = cValueTemp;
			putText(Background2, label_value, Point(150, 654 + 5), iFontFace, iThickness, Scalar(0, 0, 0), 1);
			sprintf_s(cValueTemp, "%d", nMaxArea / 9 * 4);
			label_value = cValueTemp;
			putText(Background2, label_value, Point(150, 507 + 5), iFontFace, iThickness, Scalar(0, 0, 0), 1);
			sprintf_s(cValueTemp, "%d", nMaxArea / 9 * 6);
			label_value = cValueTemp;
			putText(Background2, label_value, Point(150, 361 + 5), iFontFace, iThickness, Scalar(0, 0, 0), 1);
			sprintf_s(cValueTemp, "%d", nMaxArea / 9 * 8);
			label_value = cValueTemp;
			putText(Background2, label_value, Point(150, 214 + 5), iFontFace, iThickness, Scalar(0, 0, 0), 1);

			//x label
			sprintf_s(cValueTemp, "%d", 0);
			label_value = cValueTemp;
			putText(Background2, label_value, Point(228, 851), iFontFace, iThickness, Scalar(0, 0, 0), 1);
			sprintf_s(cValueTemp, "%d", 1);
			label_value = cValueTemp;
			putText(Background2, label_value, Point(303, 851), iFontFace, iThickness, Scalar(0, 0, 0), 1);
			sprintf_s(cValueTemp, "%d", 2);
			label_value = cValueTemp;
			putText(Background2, label_value, Point(380, 851), iFontFace, iThickness, Scalar(0, 0, 0), 1);
			sprintf_s(cValueTemp, "%d", 3);
			label_value = cValueTemp;
			putText(Background2, label_value, Point(458, 851), iFontFace, iThickness, Scalar(0, 0, 0), 1);
			sprintf_s(cValueTemp, "%d", 4);
			label_value = cValueTemp;
			putText(Background2, label_value, Point(535, 851), iFontFace, iThickness, Scalar(0, 0, 0), 1);
			sprintf_s(cValueTemp, "%d", 5);
			label_value = cValueTemp;
			putText(Background2, label_value, Point(613, 851), iFontFace, iThickness, Scalar(0, 0, 0), 1);
			sprintf_s(cValueTemp, "%d", 6);
			label_value = cValueTemp;
			putText(Background2, label_value, Point(690, 851), iFontFace, iThickness, Scalar(0, 0, 0), 1);
			sprintf_s(cValueTemp, "%d", 7);
			label_value = cValueTemp;
			putText(Background2, label_value, Point(767, 851), iFontFace, iThickness, Scalar(0, 0, 0), 1);
			sprintf_s(cValueTemp, "%d", 8);
			label_value = cValueTemp;
			putText(Background2, label_value, Point(845, 851), iFontFace, iThickness, Scalar(0, 0, 0), 1);
			sprintf_s(cValueTemp, "%d", 9);
			label_value = cValueTemp;
			putText(Background2, label_value, Point(922, 851), iFontFace, iThickness, Scalar(0, 0, 0), 1);
			sprintf_s(cValueTemp, "%d", 10);
			label_value = cValueTemp;
			putText(Background2, label_value, Point(999, 851), iFontFace, iThickness, Scalar(0, 0, 0), 1);
			//Title
//			putText(Background2, "DNA Content_Cell Area_Scatter Diagram", Point(250, 45), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 0), 1);
			resize(Background2, Background2, Size(Background2.cols / 2, Background2.rows / 2), (0, 0), (0, 0), CV_INTER_AREA);
			imshow("DNA_Content_Cell_Area_Scatter_Diagram", Background2);
			waitKey(10);

			string sResultDraw2 = sPath + sDataFileName + "Result\\DNA_Content_Cell_Area_Scatter_Diagram.jpg";
			imwrite(sResultDraw2, Background2);
			OutputDebugString(_T("9999999-4444\n"));

		}

		m_bStatisticsShow = true;
	}
}

bool CMainDlgWnd::ResetDataParam()
{
	m_bTatisticsInfo = false;
	m_bStatisticsShow = false;

	//memset(nCountNUMArray, 0, sizeof(int)* 20);

	vCellsfullfilenames_nextuse.clear();
	memset(DI_nextuse, 0, sizeof(DI_nextuse));

	memset(nCountNUMArray, 0, sizeof(nCountNUMArray));

	string sPath;
	string sPathFile;
	string sFilePathName;
	string sFileName;
	string sPathOtus;
	string sPathImageDrawFirst;
	string sPathImageDrawFinal;
	string sPathROICells;
	string sPathROILymphocyte;
	string sPathROIOthers;
	string sDataFileName;
	string sDataFileResultName;
	USES_CONVERSION;
	sPathFile = W2A(m_FileDir);
	int nLocation = sPathFile.rfind("\\");
	sPath = sPathFile.substr(0, nLocation) + "\\";
	sDataFileName = sPathFile.substr(nLocation + 1, sPathFile.length());
	sDataFileResultName = sDataFileName + "Result\\";
	string sss = sPathFile;
	sss = sss + "Result\\";
	//m_lpResultP = c_str(sss);
	sss = sss + "Result\\";
	sprintf(m_lpResultPath, ("%s"), sss.data());
	nImageNumAll = 0;
	long fHandle;
	string stemp = W2A(m_FileDir);
	sPathFile = stemp + "\\*.jpg";
	struct _finddata_t fileinfo;
	if ((fHandle = _findfirst(sPathFile.c_str(), &fileinfo)) == -1L)
	{
		printf("当前目录下没有bmp文件\n");
	}
	else
	{
		//建立数据结果及过程中的文件及数据
		string StrTemp0 = sPath + sDataFileName + "Result";
		CString sDel(StrTemp0.c_str());
		if (PathFileExists(sDel.GetString()))
		{
			try
			{
				//DeleteDirectory(sDel, true);
			}
			catch (...)
			{
			}
		}
		_mkdir(StrTemp0.c_str());
		string StrTemp = sPath + sDataFileResultName + "Lymphocyte";
		_mkdir(StrTemp.c_str());
		StrTemp = sPath + sDataFileResultName + "Cells";
		_mkdir(StrTemp.c_str());
		StrTemp = sPath + sDataFileResultName + "ImageDrawFinal";
		_mkdir(StrTemp.c_str());
		StrTemp = sPath + sDataFileResultName + "Result";
		_mkdir(StrTemp.c_str());
		StrTemp = sPath + sDataFileResultName + "Others";
		_mkdir(StrTemp.c_str()); 

		g_vec_file_name.swap(vector<string>());
		vector<string> vsfilename;
		int imgCount = 0;

		do
		{
			vsfilename.push_back(fileinfo.name);
			imgCount++;
		} while (_findnext(fHandle, &fileinfo) == 0);

		bool _bFilter = true;
		map<string, string> maptemp;
		{
			map<string, string> mapresult;
			string articulation;
			articulation = stemp + "\\articulation.txt";
			if (PathFileExistsA(articulation.c_str()) && _bFilter)
			{
				FILE *file = NULL;
				file = fopen(articulation.c_str(), "r+");
				char c[1024] = { 0 };
				while ((fgets(c, 1024, file)) != NULL)
				{
					string str(c), s1, s2;
					int pos = str.find(" ");
					if (pos != string::npos)
					{
						s1 = str.substr(0, pos);
					}
					int pos1 = str.rfind(" ");
					int pos2 = str.rfind("\n");
					int len = pos2 - pos1 - 1;
					if (pos1 != string::npos)
					{
						s2 = str.substr(pos1 + 1, len);
					}
					maptemp[s2] = s1; //s2文件名 s1清晰度
				}
				fclose(file);

				//0.8385
				//map<string, string>::iterator iter = maptemp.begin();
				//for (; iter != maptemp.end(); ++iter)
				//{
				//	string s1, s2;
				//	s1 = iter->first;
				//	s2 = iter->second;
				//	double d = atof(s1.c_str());
				//	if (d >= 0.8385)
				//	{
				//		mapresult[s1] = s2;
				//	}
				//}
				//if (mapresult.size()<1)
				//{
				//	return false;
				//}
			}

			//if (mapresult.size()>0 && _bFilter)
			//{
			//	map<string, string>::iterator iter = mapresult.begin();
			//	for (iter; iter != mapresult.end(); ++iter)
			//	{
			//		g_vec_file_name.push_back(iter->second);
			//	}
			//	nImageNumAll = mapresult.size();
			//}
			//else
			//{
			//	g_vec_file_name = vsfilename;
			//	nImageNumAll = imgCount;
			//}
		}

		bool bFilter = _bFilter;
		vector<string> vecresult;
		string articulation1;
		articulation1 = stemp + "\\articulation1.txt";
		vector<string> vectemp;
		if (PathFileExistsA(articulation1.c_str()) && bFilter)
		{
			FILE *fp;
			if ((fp = fopen(articulation1.c_str(), "r")) == NULL) {
				cout << "文件打开失败！" << endl;
			}

			fseek(fp, 0L, SEEK_END);
			int len = ftell(fp)+1;
			rewind(fp);

			char* pData = new char[len];
			memset(pData, 0, sizeof(char)*len);

			if (fread(pData, 1, len, fp) != len) {
				cout << "读取失败" << endl;
			}
			fclose(fp);

			string aaa(pData);

			SplitString(vectemp, pData, "|");
			delete[] pData;
			pData = NULL;

			if (vectemp.size()<=0)
			{
				return false;
			}
		}

		if (vectemp.size() > 0 && bFilter)
		{
			int c = 0;
			vector<string>::iterator iter = vectemp.begin();
			for (iter; iter != vectemp.end(); ++iter)
			{
				string name(*iter);
				string s = maptemp[name];
				if (!s.empty())
				{
					double dou = atof(s.c_str());
					if (dou>0.8385)
					{
						g_vec_file_name.push_back(*iter);
						c++;
					}
				}
				
			}
			nImageNumAll = c;
			if (c<=0)
			{
				return false;
			}
		}
		else
		{
			g_vec_file_name = vsfilename;
			nImageNumAll = imgCount;
		}
	}

// 	{
// 		string _filepath;
// 		_filepath = g_filepath + "Result\\";
// 		string c_s5 = "1.jpg";
// 		string c_s7 = _filepath + c_s5;
// 		CString sFile(c_s7.c_str());
// 		if (PathFileExists(sFile.GetString()))
// 		{
// 			DeleteFile(sFile.GetBuffer(0));
// 		}
// 	}

	//user_admin;
	hdib = NULL;
	nCountCells = 0;
	nCountCellsAll = 0;
	vCellsfullfilenames.swap(vector<string>());

	memset(nCellsPixelArea, 0, sizeof(int)*100000);
	memset(DI, 0, sizeof(double)* 100000);
	memset(dCircleDegree, 0, sizeof(double)* 100000);
	memset(dCellgrayvar, 0, sizeof(double)* 100000);
	memset(dGrayValue1, 0, sizeof(int)* 100000);
	memset(dODVar, 0, sizeof(int)* 100000);
	memset(dGrayMax1, 0, sizeof(int)* 100000);
	memset(dgrayvar, 0, sizeof(int)* 100000);
	memset(SortIndex, 0, sizeof(int)* 100000);
	memset(dBackgroundGrayMeanValue1, 0, sizeof(double)* 100000);
	memset(dEpithelialCellIOD1, 0, sizeof(double)* 100000);
	memset(dEpithelialCellIOD2, 0, sizeof(double)* 100000);
	memset(dLymphocyteCellIOD1, 0, sizeof(double)* 100000);
	memset(dAOD1, 0, sizeof(double)* 100000);

	MeanLymphocyteGrayValue = 0;
	SumLymphocyteGrayValue = 0;
	nCountLymphocyte = 0;
	SumLymphocyteAreaValue = 0;
	MeanLymphocyteIOD = 0;
	dLymphocyteIODSumValue = 0;

	if (g_p_calc_data != NULL)
	{
		delete[] g_p_calc_data;
		g_p_calc_data = NULL;
	}

	//statistics
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 13; j++)
		{
			m_pSelectEdit[i][j]->SetText(_T(""));
		}
		m_pSelectImg[i]->SetBkImage(_T(""));
	}
	m_pProgress->SetText(_T("")); 
	m_pProgressPercent->SetText(_T("0%"));
	m_pGifAnim->SetVisible(false);
	m_pGifAnim1->SetVisible(false);
	m_pAniProgress->SetVisible(false);
	//m_pclock_begin_end->SetVisible(false);
	m_pProgressBar->SetValue(0);

	m_bSelFirstCell = false;

	//statistics
// 	for (int i = 0; i < 32; ++i)
// 	{
// 		m_pStatisticsImg[i]->SetBkImage(_T(""));
// 		m_pStatisticsEdit[i]->SetText(_T(""));
// 	}

	if (m_pBlockPage)
	{
		m_pBlockPage[0].reset();
	}
	m_wSelPage = 0;
	CString str;
	m_wCurShowPageCount = _cnt_show_page;
	str.Format(_T("第 1 页 共 %d 页"), m_wCurShowPageCount);
	m_pLabelPage->SetText(str.GetString());
	m_pLabelSelCellCount->SetText(_T("已选细胞个数为:0"));
	for (int i = 0; i < _row_cnt; ++i)
	{
		for (int j = 0; j < _column_cnt; ++j)
		{
			m_pBlockPage[0].image[i][j]->SetBkImage(_T(""));
			m_pBlockPage[0].edit[i][j]->SetText(_T(""));
			m_pBlockPage[0].image[i][j]->SetBorderSize(1);
			m_pBlockPage[0].image[i][j]->SetBorderColor(GetColorDword(_T("ffcccccc")));
			m_pBlockPage[0].image[i][j]->SetUserData(_T(""));
		}
	}

	g_record.reset();

	return true;
}

CString CMainDlgWnd::GetExePath()
{
	TCHAR lpPath[MAX_PATH];
	GetModuleFileName(NULL, lpPath, sizeof(lpPath)-1);
	PathRemoveFileSpec(lpPath);
	PathRemoveBackslash(lpPath);
	PathAddBackslash(lpPath);
	return lpPath;
}

BOOL CMainDlgWnd::SaveBmp(HDC hPaintDC, HBITMAP  hBitmap, CString  FileName)
{
	HDC     hDC;
	//当前分辨率下每象素所占字节数         
	    int     iBits;
	//位图中每象素所占字节数         
	    WORD     wBitCount;
	//定义调色板大小，     位图中像素字节大小     ，位图文件大小     ，     写入文件字节数             
	    DWORD     dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
	//位图属性结构             
	    BITMAP     Bitmap;
	//位图文件头结构         
	    BITMAPFILEHEADER     bmfHdr;
	//位图信息头结构             
	BITMAPINFOHEADER     bi;
	//指向位图信息头结构                 
	LPBITMAPINFOHEADER     lpbi;
	//定义文件，分配内存句柄，调色板句柄             
	HANDLE     fh, hDib, hPal, hOldPal = NULL;
	
	int aa = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+288 * 352 * 24 / 8;
	
	//计算位图文件每个像素所占字节数             
	//hDC = CreateDC(L"DISPLAY", NULL, NULL, NULL);
	hDC = ::CreateCompatibleDC(hPaintDC);
	iBits = GetDeviceCaps(hDC, BITSPIXEL)     *     GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if (iBits <= 1)
	        wBitCount = 1;
	else  if (iBits <= 4)
	        wBitCount = 4;
	else if (iBits <= 8)
	        wBitCount = 8;
	else
	        wBitCount = 24;
	
	GetObject(hBitmap, sizeof(Bitmap), (LPSTR)&Bitmap);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap.bmWidth;
	bi.biHeight = Bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = Bitmap.bmWidth * Bitmap.bmHeight * 3;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrImportant = 0;
	bi.biClrUsed = 0;
	
	dwBmBitsSize = ((Bitmap.bmWidth *wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;
	
	//为位图内容分配内存             
	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;
	
	//     处理调色板                 
	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
	    hDC = ::GetDC(NULL);
	    hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
	    RealizePalette(hDC);
	}
	
	//     获取该调色板下新的像素值             
	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight,
	    (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)+dwPaletteSize,
	    (BITMAPINFO *)lpbi, DIB_RGB_COLORS);
	
	//恢复调色板                 
	if (hOldPal)
	{
	    ::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
	    RealizePalette(hDC);
	    ::ReleaseDC(NULL, hDC);
	}
	
	//创建位图文件                 
	fh = CreateFile(FileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
	    FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	
	if (fh == INVALID_HANDLE_VALUE)         return     FALSE;
	
	//     设置位图文件头             
	bmfHdr.bfType = 0x4D42;     //     "BM"             
	dwDIBSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER)+(DWORD)sizeof(BITMAPINFOHEADER)+dwPaletteSize;
	//     写入位图文件头             
	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
	//     写入位图文件其余内容             
	WriteFile(fh, (LPSTR)lpbi, dwDIBSize - 14, &dwWritten, NULL);
	//清除                 
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);
	
	return     TRUE;
}

BOOL CMainDlgWnd::DeleteDirectory(CString DirName, BOOL bRemoveRootDir)
{

	CFileFind tempFind;
	CString csZRootDir;

	csZRootDir.Format(L"%s\\*.*", DirName.GetBuffer(0));

	BOOL IsFinded = (BOOL)tempFind.FindFile(csZRootDir.GetBuffer(0));

	while (IsFinded)

	{
		IsFinded = (BOOL)tempFind.FindNextFile();

		if (!tempFind.IsDots())

		{
			CString csFileName;


			csFileName.Format(L"%s", tempFind.GetFileName().GetBuffer(0));
			if (tempFind.IsDirectory())

			{
				CString tmp;
				tmp.Format(L"%s\\%s", DirName.GetBuffer(0), csFileName.GetBuffer(0));

				DeleteDirectory(tmp, TRUE);

			}

			else

			{
				CString tmp;
				tmp.Format(L"%s\\%s", DirName, csFileName.GetBuffer(0));

				DeleteFile(tmp.GetBuffer(0));

			}

		}

	}

	tempFind.Close();
	if (bRemoveRootDir)
	{
		if (!RemoveDirectory(DirName))

		{
			return FALSE;
		}
	}

	return TRUE;
}

DWORD CMainDlgWnd::GetColorDword(LPCTSTR clr)
{
	if (clr == NULL) return 0;
	LPCTSTR colVal = clr;
	if (*colVal == _T('#')) colVal = ::CharNext(colVal);
	LPTSTR pstr = NULL;
	DWORD clrColor = _tcstoul(colVal, &pstr, 16);
	return clrColor;
}


bool CMainDlgWnd::IsAdmin(bool bTip)
{
	CString name = gLoginParam.sName.GetData();
	if (name.Compare(_T("test")) == 0 ||
		name.Compare(_T("admin")) == 0
		)
	{
		return true;
	}
	if (bTip)
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("只有管理员权限才能打开"), _T("警告"), MB_OK);
	}
	return false;
}

/////////////////////////////////////////////////////////////////////

void CMainDlgWnd::montage(cv::Mat &output, cv::Mat &input, cv::Rect rect, cv::Scalar color /*= cv::Scalar(0)*/)
{
	if (rect.x + rect.width > output.cols || rect.y + rect.height > output.rows) {
		//拼图超界限了，用color填充
		int cols = std::max(rect.x + rect.width, output.cols);
		int rows = std::max(rect.y + rect.height, output.rows);
		cv::Mat Bigger(rows, cols, output.type());
		Bigger = color;
		montage(Bigger, output, cv::Rect(0, 0, output.cols, output.rows), color);
		output = Bigger.clone();
	}

	if (rect.width != input.cols || rect.height != input.rows) {
		cv::Mat temp;
		resize(input, temp, cv::Size(rect.width, rect.height));
		temp.copyTo(output(rect));
	}
	else input.copyTo(output(rect));
}

void CMainDlgWnd::montage(cv::Mat &output, cv::Mat &input, cv::Point location, double zoom_size /*= 1.0*/, cv::Scalar color /*= cv::Scalar(0)*/)
{
	cv::Rect rect(location.x, location.y, input.cols*zoom_size, input.rows*zoom_size);
	montage(output, input, rect, color);
}

void CMainDlgWnd::montage(cv::Mat &output, cv::Mat &input, int x, int y, double zoom_size /*= 1.0*/, cv::Scalar color /*= cv::Scalar(0)*/)
{
	cv::Rect rect(x, y, input.cols*zoom_size, input.rows*zoom_size);
	montage(output, input, rect, color);
}

void CMainDlgWnd::SaveCellImage(CString str[_cnt_sel_cell], CString str1[_cnt_sel_cell], CString path)
{
	cv::Mat BK = cv::imread("background1.png");
	CvFont font;
	/*自己新建一个画布当然也是可以的
	cv::Mat frame(800,800,CV_8UC3);
	frame = cv::Scalar(0);//让画布变成黑色
	*/
	//注意：拼接图片类型必须一样,
	CStringA strA = (CStringA)str[0];
	cv::Mat img1 = cv::imread(strA.GetString());
	strA = (CStringA)str[1];
	cv::Mat img2 = cv::imread(strA.GetString());
	strA = (CStringA)str[2];
	cv::Mat img3 = cv::imread(strA.GetString());
	strA = (CStringA)str[3];
	cv::Mat img4 = cv::imread(strA.GetString());
	strA = (CStringA)str[4];
	cv::Mat img5 = cv::imread(strA.GetString());
	strA = (CStringA)str[5];
	cv::Mat img6 = cv::imread(strA.GetString());
	strA = (CStringA)str[6];
	cv::Mat img7 = cv::imread(strA.GetString());
	strA = (CStringA)str[7];
	cv::Mat img8 = cv::imread(strA.GetString());
	strA = (CStringA)str[8];
	cv::Mat img9 = cv::imread(strA.GetString());
	strA = (CStringA)str[9];
	cv::Mat img10 = cv::imread(strA.GetString());
	strA = (CStringA)str[10];
	cv::Mat img11 = cv::imread(strA.GetString());
	strA = (CStringA)str[11];
	cv::Mat img12 = cv::imread(strA.GetString());
	strA = (CStringA)str[12];
	cv::Mat img13 = cv::imread(strA.GetString());
	strA = (CStringA)str[13];
	cv::Mat img14 = cv::imread(strA.GetString());
	strA = (CStringA)str[14];
	cv::Mat img15 = cv::imread(strA.GetString());
	strA = (CStringA)str[15];
	cv::Mat img16 = cv::imread(strA.GetString());
	strA = (CStringA)str[16];
	cv::Mat img17 = cv::imread(strA.GetString());
	strA = (CStringA)str[17];
	cv::Mat img18 = cv::imread(strA.GetString());
	strA = (CStringA)str[18];
	cv::Mat img19 = cv::imread(strA.GetString());
	strA = (CStringA)str[19];
	cv::Mat img20 = cv::imread(strA.GetString());

#define delta_edit_x (-12)
	strA = (CStringA)str1[0];
	IplImage* BK_TEXT1 = &IplImage(BK); // Mat -> IplImage 类型转换
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.03, 1.0, 0, 1, 1);
	cvPutText(BK_TEXT1, strA.GetString(), cvPoint(325 + delta_edit_x, 185), &font, cvScalar(0, 0, 0, 1));

	strA = (CStringA)str1[1];
	IplImage* BK_TEXT2 = &IplImage(BK); // Mat -> IplImage 类型转换
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.03, 1.0, 0, 1, 1);
	cvPutText(BK_TEXT2, strA.GetString(), cvPoint(430 + delta_edit_x, 185), &font, cvScalar(0, 0, 0, 1));

	strA = (CStringA)str1[2];
	IplImage* BK_TEXT3 = &IplImage(BK); // Mat -> IplImage 类型转换
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.03, 1.0, 0, 1, 1);
	cvPutText(BK_TEXT3, strA.GetString(), cvPoint(530 + delta_edit_x, 185), &font, cvScalar(0, 0, 0, 1));

	strA = (CStringA)str1[3];
	IplImage* BK_TEXT4 = &IplImage(BK); // Mat -> IplImage 类型转换
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.03, 1.0, 0, 1, 1);
	cvPutText(BK_TEXT4, strA.GetString(), cvPoint(630 + delta_edit_x, 185), &font, cvScalar(0, 0, 0, 1));

	strA = (CStringA)str1[4];
	IplImage* BK_TEXT5 = &IplImage(BK); // Mat -> IplImage 类型转换
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.03, 1.0, 0, 1, 1);
	cvPutText(BK_TEXT5, strA.GetString(), cvPoint(730 + delta_edit_x, 185), &font, cvScalar(0, 0, 0, 1));

	strA = (CStringA)str1[5];
	IplImage* BK_TEXT6 = &IplImage(BK); // Mat -> IplImage 类型转换
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.03, 1.0, 0, 1, 1);
	cvPutText(BK_TEXT6, strA.GetString(), cvPoint(325 + delta_edit_x, 285), &font, cvScalar(0, 0, 0, 1));

	strA = (CStringA)str1[6];
	IplImage* BK_TEXT10 = &IplImage(BK); // Mat -> IplImage 类型转换
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.03, 1.0, 0, 1, 1);
	cvPutText(BK_TEXT10, strA.GetString(), cvPoint(430 + delta_edit_x, 285), &font, cvScalar(0, 0, 0, 1));

	strA = (CStringA)str1[7];
	IplImage* BK_TEXT7 = &IplImage(BK); // Mat -> IplImage 类型转换
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.03, 1.0, 0, 1, 1);
	cvPutText(BK_TEXT7, strA.GetString(), cvPoint(530 + delta_edit_x, 285), &font, cvScalar(0, 0, 0, 1));

	strA = (CStringA)str1[8];
	IplImage* BK_TEXT8 = &IplImage(BK); // Mat -> IplImage 类型转换
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.03, 1.0, 0, 1, 1);
	cvPutText(BK_TEXT8, strA.GetString(), cvPoint(630 + delta_edit_x, 285), &font, cvScalar(0, 0, 0, 1));

	strA = (CStringA)str1[9];
	IplImage* BK_TEXT9 = &IplImage(BK); // Mat -> IplImage 类型转换
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.03, 1.0, 0, 1, 1);
	cvPutText(BK_TEXT9, strA.GetString(), cvPoint(730 + delta_edit_x, 285), &font, cvScalar(0, 0, 0, 1));

	strA = (CStringA)str1[10];
	IplImage* BK_TEXT11 = &IplImage(BK); // Mat -> IplImage 类型转换
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.03, 1.0, 0, 1, 1);
	cvPutText(BK_TEXT11, strA.GetString(), cvPoint(325 + delta_edit_x, 385), &font, cvScalar(0, 0, 0, 1));

	strA = (CStringA)str1[11];
	IplImage* BK_TEXT15 = &IplImage(BK); // Mat -> IplImage 类型转换
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.03, 1.0, 0, 1, 1);
	cvPutText(BK_TEXT15, strA.GetString(), cvPoint(430 + delta_edit_x, 385), &font, cvScalar(0, 0, 0, 1));

	strA = (CStringA)str1[12];
	IplImage* BK_TEXT12 = &IplImage(BK); // Mat -> IplImage 类型转换
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.03, 1.0, 0, 1, 1);
	cvPutText(BK_TEXT12, strA.GetString(), cvPoint(530 + delta_edit_x, 385), &font, cvScalar(0, 0, 0, 1));

	strA = (CStringA)str1[13];
	IplImage* BK_TEXT13 = &IplImage(BK); // Mat -> IplImage 类型转换
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.03, 1.0, 0, 1, 1);
	cvPutText(BK_TEXT13, strA.GetString(), cvPoint(630 + delta_edit_x, 385), &font, cvScalar(0, 0, 0, 1));

	strA = (CStringA)str1[14];
	IplImage* BK_TEXT14 = &IplImage(BK); // Mat -> IplImage 类型转换
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.03, 1.0, 0, 1, 1);
	cvPutText(BK_TEXT14, strA.GetString(), cvPoint(730 + delta_edit_x, 385), &font, cvScalar(0, 0, 0, 1));

	strA = (CStringA)str1[15];
	IplImage* BK_TEXT16 = &IplImage(BK); // Mat -> IplImage 类型转换
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.03, 1.0, 0, 1, 1);
	cvPutText(BK_TEXT16, strA.GetString(), cvPoint(325 + delta_edit_x, 485), &font, cvScalar(0, 0, 0, 1));

	strA = (CStringA)str1[16];
	IplImage* BK_TEXT20 = &IplImage(BK); // Mat -> IplImage 类型转换
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.03, 1.0, 0, 1, 1);
	cvPutText(BK_TEXT20, strA.GetString(), cvPoint(430 + delta_edit_x, 485), &font, cvScalar(0, 0, 0, 1));

	strA = (CStringA)str1[17];
	IplImage* BK_TEXT17 = &IplImage(BK); // Mat -> IplImage 类型转换
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.03, 1.0, 0, 1, 1);
	cvPutText(BK_TEXT17, strA.GetString(), cvPoint(530 + delta_edit_x, 485), &font, cvScalar(0, 0, 0, 1));

	strA = (CStringA)str1[18];
	IplImage* BK_TEXT18 = &IplImage(BK); // Mat -> IplImage 类型转换
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.03, 1.0, 0, 1, 1);
	cvPutText(BK_TEXT18, strA.GetString(), cvPoint(630 + delta_edit_x, 485), &font, cvScalar(0, 0, 0, 1));

	strA = (CStringA)str1[19];
	IplImage* BK_TEXT19 = &IplImage(BK); // Mat -> IplImage 类型转换
	cvInitFont(&font, CV_FONT_HERSHEY_DUPLEX, 0.03, 1.0, 0, 1, 1);
	cvPutText(BK_TEXT19, strA.GetString(), cvPoint(730 + delta_edit_x, 485), &font, cvScalar(0, 0, 0, 1));

#define delta_x (0)
	double dC = 5.0;
	int thickness = 4;
	int _y = -1;
	float f0 = _ttof(str1[0].GetString());
	if (f0 >= dC)
	{
		cv::Rect rect(319+delta_x, 99 + _y, 72, 72);//左上坐标（x,y）和矩形的长(x)宽(y)
		cv::rectangle(BK, rect, cv::Scalar(0, 0, 255), thickness, 0);
	}
	float f1 = _ttof(str1[1].GetString());
	if (f1 >= dC)
	{
		cv::Rect rect(419 + delta_x, 99 + _y, 72, 72);//左上坐标（x,y）和矩形的长(x)宽(y)
		cv::rectangle(BK, rect, cv::Scalar(0, 0, 255), thickness, 0);
	}
	float f2 = _ttof(str1[2].GetString());
	if (f2 >= dC)
	{
		cv::Rect rect(519 + delta_x, 99 + _y, 72, 72);//左上坐标（x,y）和矩形的长(x)宽(y)
		cv::rectangle(BK, rect, cv::Scalar(0, 0, 255), thickness, 0);
	}
	float f3 = _ttof(str1[3].GetString());
	if (f3 >= dC)
	{
		cv::Rect rect(619 + delta_x, 99 + _y, 72, 72);//左上坐标（x,y）和矩形的长(x)宽(y)
		cv::rectangle(BK, rect, cv::Scalar(0, 0, 255), thickness, 0);
	}
	float f4 = _ttof(str1[4].GetString());
	if (f4 >= dC)
	{
		cv::Rect rect(719 + delta_x, 99 + _y, 72, 72);//左上坐标（x,y）和矩形的长(x)宽(y)
		cv::rectangle(BK, rect, cv::Scalar(0, 0, 255), thickness, 0);
	}
	////////////////////
	float f5 = _ttof(str1[5].GetString());
	if (f5 >= dC)
	{
		cv::Rect rect(319 + delta_x, 199 + _y, 72, 72);//左上坐标（x,y）和矩形的长(x)宽(y)
		cv::rectangle(BK, rect, cv::Scalar(0, 0, 255), thickness, 0);
	}
	float f6 = _ttof(str1[6].GetString());
	if (f6 >= dC)
	{
		cv::Rect rect(419 + delta_x, 199 + _y, 72, 72);//左上坐标（x,y）和矩形的长(x)宽(y)
		cv::rectangle(BK, rect, cv::Scalar(0, 0, 255), thickness, 0);
	}
	float f7 = _ttof(str1[7].GetString());
	if (f7 >= dC)
	{
		cv::Rect rect(519 + delta_x, 199 + _y, 72, 72);//左上坐标（x,y）和矩形的长(x)宽(y)
		cv::rectangle(BK, rect, cv::Scalar(0, 0, 255), thickness, 0);
	}
	float f8 = _ttof(str1[8].GetString());
	if (f8 >= dC)
	{
		cv::Rect rect(619 + delta_x, 199 + _y, 72, 72);//左上坐标（x,y）和矩形的长(x)宽(y)
		cv::rectangle(BK, rect, cv::Scalar(0, 0, 255), thickness, 0);
	}
	float f9 = _ttof(str1[9].GetString());
	if (f9 >= dC)
	{
		cv::Rect rect(719 + delta_x, 199 + _y, 72, 72);//左上坐标（x,y）和矩形的长(x)宽(y)
		cv::rectangle(BK, rect, cv::Scalar(0, 0, 255), thickness, 0);
	}
	/////////////
	float f10 = _ttof(str1[10].GetString());
	if (f10 >= dC)
	{
		cv::Rect rect(319 + delta_x, 299 + _y, 72, 72);//左上坐标（x,y）和矩形的长(x)宽(y)
		cv::rectangle(BK, rect, cv::Scalar(0, 0, 255), thickness, 0);
	}
	float f11 = _ttof(str1[11].GetString());
	if (f11 >= dC)
	{
		cv::Rect rect(419 + delta_x, 299 + _y, 72, 72);//左上坐标（x,y）和矩形的长(x)宽(y)
		cv::rectangle(BK, rect, cv::Scalar(0, 0, 255), thickness, 0);
	}
	float f12 = _ttof(str1[12].GetString());
	if (f12 >= dC)
	{
		cv::Rect rect(519 + delta_x, 299 + _y, 72, 72);//左上坐标（x,y）和矩形的长(x)宽(y)
		cv::rectangle(BK, rect, cv::Scalar(0, 0, 255), thickness, 0);
	}
	float f13 = _ttof(str1[13].GetString());
	if (f13 >= dC)
	{
		cv::Rect rect(619 + delta_x, 299 + _y, 72, 72);//左上坐标（x,y）和矩形的长(x)宽(y)
		cv::rectangle(BK, rect, cv::Scalar(0, 0, 255), thickness, 0);
	}
	float f14 = _ttof(str1[14].GetString());
	if (f14 >= dC)
	{
		cv::Rect rect(719 + delta_x, 299 + _y, 72, 72);//左上坐标（x,y）和矩形的长(x)宽(y)
		cv::rectangle(BK, rect, cv::Scalar(0, 0, 255), thickness, 0);
	}
	////////////////////////
	float f15 = _ttof(str1[15].GetString());
	if (f15 >= dC)
	{
		cv::Rect rect(319 + delta_x, 399 + _y, 72, 72);//左上坐标（x,y）和矩形的长(x)宽(y)
		cv::rectangle(BK, rect, cv::Scalar(0, 0, 255), thickness, 0);
	}
	float f16 = _ttof(str1[16].GetString());
	if (f16 >= dC)
	{
		cv::Rect rect(419 + delta_x, 399 + _y, 72, 72);//左上坐标（x,y）和矩形的长(x)宽(y)
		cv::rectangle(BK, rect, cv::Scalar(0, 0, 255), thickness, 0);
	}
	float f17 = _ttof(str1[17].GetString());
	if (f17 >= dC)
	{
		cv::Rect rect(519 + delta_x, 399 + _y, 72, 72);//左上坐标（x,y）和矩形的长(x)宽(y)
		cv::rectangle(BK, rect, cv::Scalar(0, 0, 255), thickness, 0);
	}
	float f18 = _ttof(str1[18].GetString());
	if (f18 >= dC)
	{
		cv::Rect rect(619 + delta_x, 399 + _y, 72, 72);//左上坐标（x,y）和矩形的长(x)宽(y)
		cv::rectangle(BK, rect, cv::Scalar(0, 0, 255), thickness, 0);
	}
	float f19 = _ttof(str1[19].GetString());
	if (f18 >= dC)
	{
		cv::Rect rect(719 + delta_x, 399 + _y, 72, 72);//左上坐标（x,y）和矩形的长(x)宽(y)
		cv::rectangle(BK, rect, cv::Scalar(0, 0, 255), thickness, 0);
	}

	cv::Rect rect(300, 80, 510, 420);//左上坐标（x,y）和矩形的长(x)宽(y)
	cv::rectangle(BK, rect, cv::Scalar(168, 168, 168), 1, 0);

	montage(BK, img1, cv::Point(320, 100), 1, cv::Scalar(0, 255, 0));
	montage(BK, img2, cv::Point(420, 100), 1, cv::Scalar(0, 255, 0));
	montage(BK, img3, cv::Point(520, 100), 1, cv::Scalar(0, 255, 0));
	montage(BK, img4, cv::Point(620, 100), 1, cv::Scalar(0, 255, 0));
	montage(BK, img5, cv::Point(720, 100), 1, cv::Scalar(0, 255, 0));

	montage(BK, img6, cv::Point(320, 200), 1, cv::Scalar(0, 255, 0));
	montage(BK, img7, cv::Point(420, 200), 1, cv::Scalar(0, 255, 0));
	montage(BK, img8, cv::Point(520, 200), 1, cv::Scalar(0, 255, 0));
	montage(BK, img9, cv::Point(620, 200), 1, cv::Scalar(0, 255, 0));
	montage(BK, img10, cv::Point(720, 200), 1, cv::Scalar(0, 255, 0));

	montage(BK, img11, cv::Point(320, 300), 1, cv::Scalar(0, 255, 0));
	montage(BK, img12, cv::Point(420, 300), 1, cv::Scalar(0, 255, 0));
	montage(BK, img13, cv::Point(520, 300), 1, cv::Scalar(0, 255, 0));
	montage(BK, img14, cv::Point(620, 300), 1, cv::Scalar(0, 255, 0));
	montage(BK, img15, cv::Point(720, 300), 1, cv::Scalar(0, 255, 0));

	montage(BK, img16, cv::Point(320, 400), 1, cv::Scalar(0, 255, 0));
	montage(BK, img17, cv::Point(420, 400), 1, cv::Scalar(0, 255, 0));
	montage(BK, img18, cv::Point(520, 400), 1, cv::Scalar(0, 255, 0));
	montage(BK, img19, cv::Point(620, 400), 1, cv::Scalar(0, 255, 0));
	montage(BK, img20, cv::Point(720, 400), 1, cv::Scalar(0, 255, 0));

	cv::Rect rectL(290, 80, 530, 430);
	cv::Mat img_now = BK(rectL);
//	cv::imshow("正常地拼图", img_now);
	//cv::imshow("正常地拼图", BK);
	cv::waitKey(25);
	CStringA a = (CStringA)path;
	cv::imwrite(a.GetString(), img_now);
}


bool CMainDlgWnd::FindProcess(CString strProcessName, DWORD& nPid)
{

	TCHAR tszProcess[64] = { 0 };
	lstrcpy(tszProcess, strProcessName.GetString());
	//查找进程
	STARTUPINFO st;
	PROCESS_INFORMATION pi;
	PROCESSENTRY32 ps;
	HANDLE hSnapshot;
	memset(&st, 0, sizeof(STARTUPINFO));
	st.cb = sizeof(STARTUPINFO);
	memset(&ps, 0, sizeof(PROCESSENTRY32));
	ps.dwSize = sizeof(PROCESSENTRY32);
	memset(&pi, 0, sizeof(PROCESS_INFORMATION));
	// 遍历进程 
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE)
		return false;
	if (!Process32First(hSnapshot, &ps))
		return false;
	do {
		if (lstrcmp(ps.szExeFile, tszProcess) == 0)
		{
			//找到制定的程序
			nPid = ps.th32ProcessID;
			CloseHandle(hSnapshot);
			printf("找到进程: %s\n", tszProcess);
			return true;
			//getchar();
			//return dwPid;
		}
	} while (Process32Next(hSnapshot, &ps));
	CloseHandle(hSnapshot);
	return false;
}

bool CMainDlgWnd::KillProcess(DWORD dwPid)
{
	printf("Kill进程Pid = %d\n", dwPid);
	//关闭进程
	HANDLE killHandle = OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION |   // Required by Alpha
		PROCESS_CREATE_THREAD |   // For CreateRemoteThread
		PROCESS_VM_OPERATION |   // For VirtualAllocEx/VirtualFreeEx
		PROCESS_VM_WRITE,             // For WriteProcessMemory);
		FALSE, dwPid);
	if (killHandle == NULL)
		return false;
	TerminateProcess(killHandle, 0);
	return true;
}


int CMainDlgWnd::IsInvalidCell(Mat& srcImage)
{
	Mat copymat = srcImage.clone();

	TCHAR lpPath[MAX_PATH];
	GetModuleFileName(NULL, lpPath, sizeof(lpPath) - 1);
	PathRemoveFileSpec(lpPath);
	PathRemoveBackslash(lpPath);
	PathAddBackslash(lpPath);
	CStringA sBin(lpPath);
	CStringA sBinTemp(sBin + "Temp\\");
	if (_access(sBinTemp.GetString(), 0) != 0)
	{
		_mkdir(sBinTemp.GetString());
	}

	CStringA sPre(sBin + "srcpre.png");
	Mat imgContour;
	{
		Mat mROIOtsu;// ROI Otsu分割进行黑白反色；
		threshold(srcImage, mROIOtsu, 255, 255, CV_THRESH_OTSU | CV_THRESH_BINARY);
		mROIOtsu = 255 - mROIOtsu;

		// 边缘检测，采用findContours寻找对应ROI区域外轮廓；
		Mat mROIEdge;
		Canny(mROIOtsu, mROIEdge, 0, 50, 5);

		vector<Vec4i> vROIHierarchy;
		vector<vector<Point>> vROIContour;
		findContours(mROIEdge, vROIContour, vROIHierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point());

		//绘制轮廓和凸包
		Mat drawing = Mat::zeros(mROIEdge.size(), CV_8UC3);
		for (int i = 0; i < vROIContour.size(); i++)
		{
			//Scalar color = Scalar(255, 255, 255);
			drawContours(drawing, vROIContour, i, cvScalar(255, 255, 255), CV_FILLED, 8, vector<Vec4i>(), 0, Point());
		}
		imwrite(sPre.GetString(), drawing);
		Mat matTemp = imread(sPre.GetString(), 0);
		//imgContour = matTemp.clone();

		Mat gray_2, threshout_2, threshout_3;
		blur(matTemp, threshout_2, Size(3, 3));
		threshold(threshout_2, threshout_3, 254, 255, THRESH_BINARY);

		vector<Vec4i> vROIHierarchy_2;
		vector<vector<Point>> vROIContour_2;
		findContours(threshout_3, vROIContour_2, vROIHierarchy_2, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

		//https://blog.csdn.net/kksc1099054857/article/details/90342648
		int size = 200;

		Mat ImageContours = Mat::zeros(threshout_3.size(), CV_8UC1);  //绘制
		Mat ImgContours = Mat::zeros(threshout_3.size(), CV_8UC1);

		vector<vector<Point>>::iterator k;                    //迭代器，访问容器数据

		for (k = vROIContour_2.begin(); k != vROIContour_2.end();)      //遍历容器,设置面积因子
		{
			int a = contourArea(*k, false);
			if (a < size)
			{//删除指定元素，返回指向删除元素下一个元素位置的迭代器
				k = vROIContour_2.erase(k);
			}
			else
				++k;
		}

		////contours[i]代表第i个轮廓，contours[i].size()代表第i个轮廓上所有的像素点
		//for (int i = 0; i < vROIContour_2.size(); i++)
		//{
		//	for (int j = 0; j < vROIContour_2[i].size(); j++)
		//	{
		//		//获取轮廓上点的坐标
		//		Point P = Point(vROIContour_2[i][j].x, vROIContour_2[i][j].y);
		//		ImgContours.at<uchar>(P) = 255;
		//	}
		//	drawContours(ImageContours, vROIContour_2, i, Scalar(255), -1, 8);
		//}

		if (vROIContour_2.size() != 1)
		{
			CStringA sTemp1(sBin + "Temp\\1\\");
			if (_access(sTemp1.GetString(), 0) != 0)
			{
				_mkdir(sTemp1.GetString());
			}
			static int fileindex = 0;
			char buf[256] = { 0 };
			sprintf_s(buf, "%s\\%d.png", sTemp1.GetString(), fileindex++);
			imwrite(buf, copymat);
			return 0;
		}
	}
	imgContour = imread(sPre.GetString());
	if (!imgContour.data){
		cout << "falied to read" << endl;
		return 0;
	}

	//图像灰度图转化并平滑滤波
	Mat BinaryImage;
	Mat GrayImage;
	GrayImage.create(imgContour.size(), imgContour.type());//原图转化为灰度图
	cvtColor(imgContour, GrayImage, CV_BGR2GRAY);

	Mat srcImage1 = GrayImage.clone();
	blur(srcImage1, srcImage1, Size(3, 3));

	Mat threshold_output;
	vector<Vec4i>hierarchy;
	//使用Threshold检测图像边缘
	Mat srcImage2 = srcImage1.clone();
	threshold(srcImage2, threshold_output, 0, 255, THRESH_BINARY);

	Mat min = threshold_output.clone();

	//寻找max凸包图片
	//寻找图像轮廓
	vector<vector<Point>>contours;
	findContours(threshold_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
	//寻找图像凸包
	vector<vector<Point>>hull(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		convexHull(Mat(contours[i]), hull[i], false);
	}
	//绘制轮廓和凸包
	Mat drawing = Mat::zeros(threshold_output.size(), CV_8UC3);
	for (int i = 0; i < contours.size(); i++)
	{
		drawContours(drawing, hull, i, cvScalar(255, 255, 255), CV_FILLED, 8, vector<Vec4i>(), 0, Point());
	}
	Mat max = drawing.clone();

	//图像相减
	CStringA sMax(sBin + "max.png");
	CStringA sMin(sBin + "min.png");
	CStringA sDest(sBin + "dest.png");
	Mat dst;
	cv::imwrite(sMax.GetString(), max);
	cv::imwrite(sMin.GetString(), min);
	Mat img1 = imread(sMax.GetString());
	Mat img2 = imread(sMin.GetString());
	absdiff(img1, img2, dst);
	Mat saveDest = dst.clone();
// 	cv::imwrite(sDest.GetString(), saveDest);
// 	Mat srcImage3 = imread("dest.png");

	//寻找图像轮廓
	Mat gray1, threshout;
	cvtColor(saveDest, gray1, COLOR_BGR2GRAY);
	blur(gray1, gray1, Size(3, 3));
	int thresh = 125;
	threshold(gray1, threshout, thresh, 255, THRESH_BINARY);

	vector<vector<Point>>contours1;
	vector<Vec4i>hierarchy1;
	Mat matout;
	findContours(threshout, contours1, hierarchy1, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

	//寻找图像凸包
	vector<vector<Point>>hull1(contours1.size());
	for (int i = 0; i < contours1.size(); i++)
	{
		convexHull(Mat(contours1[i]), hull1[i], false);
	}

	//绘制轮廓和凸包
	Mat drawing1 = Mat::zeros(threshout.size(), CV_8UC3);
	for (int i = 0; i < contours1.size(); i++)
	{
		Scalar color = Scalar(255, 255, 255);
		drawContours(drawing1, contours1, i, color, CV_FILLED, 8, vector<Vec4i>(), 0, Point());
	}

	{
		//寻找图像轮廓
		//Mat src1 = imread("dest.png");
		Mat src1 = drawing1.clone();
		Mat gray1, threshout;
		cvtColor(src1, gray1, COLOR_BGR2GRAY);
		blur(gray1, gray1, Size(3, 3));
		threshold(gray1, threshout, thresh, 255, THRESH_BINARY);

		vector<vector<Point>>contours1;
		vector<Vec4i>hierarchy1;
		Mat matout;
		findContours(threshout, contours1, hierarchy1, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
		if ( contours1.size() <= 1)
		{
			return 1;
		}
		else
		{
			CStringA sTemp1(sBin + "Temp\\2\\");
			if (_access(sTemp1.GetString(), 0) != 0)
			{
				_mkdir(sTemp1.GetString());
			}
			static int fileindex1 = 0;
			char buf[256] = { 0 };
			sprintf_s(buf, "%s\\%d.png", sTemp1.GetString(), fileindex1++);
			imwrite(buf, copymat);
		}
	}

	return 0;
}

BOOL CMainDlgWnd::CopyToClipboard(const char* pszData, int nDataLen)
{
	if (::OpenClipboard(NULL))
	{
		::EmptyClipboard();
		HGLOBAL clipbuffer;
		char *buffer;
		clipbuffer = ::GlobalAlloc(GMEM_DDESHARE, nDataLen + 1);
		buffer = (char *)::GlobalLock(clipbuffer);
		strcpy(buffer, pszData);
		::GlobalUnlock(clipbuffer);
		::SetClipboardData(CF_TEXT, clipbuffer);
		::CloseClipboard();
		return TRUE;
	}
	return FALSE;
}


// #include <windows.h>
// #include <stdio.h>
// #include <iostream>
// #include <string>
// #include <vector>
// #include <istream>
// #include <stdlib.h>
// #include <fstream>
// #include <sstream>
// #include <Shlwapi.h> 

using namespace std;
#pragma comment(lib, "Shlwapi.lib")

bool CMainDlgWnd::nyCopyFile(const std::wstring& srcpath, const std::wstring despath)
{
	bool bRet = false;
	if (srcpath.empty() || despath.empty())
	{
		return bRet;
	}

	//需要拷贝到数组，直接赋值有时候执行失败
	//**确保目录的路径以2个\0结尾**/ 
	TCHAR szFrom[MAX_PATH * 2] = { 0 };
	TCHAR szTo[MAX_PATH * 2] = { 0 };
	memset(szFrom, 0, sizeof(szFrom));
	memset(szTo, 0, sizeof(szTo));
	_tcscpy_s(szFrom, MAX_PATH * 2, srcpath.c_str());
	_tcscpy_s(szTo, MAX_PATH * 2, despath.c_str());
// 	szFrom[_tcslen(srcpath.c_str())] = _T('\0');
// 	szFrom[_tcslen(srcpath.c_str()) + 1] = _T('\0');
// 	szTo[_tcslen(despath.c_str())] = _T('\0');
// 	szTo[_tcslen(despath.c_str()) + 1] = _T('\0');

	SHFILEOPSTRUCT fop;
	ZeroMemory(&fop, sizeof(fop));
	fop.fFlags = FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR /*| FOF_SILENT*/;
	fop.wFunc = FO_COPY;
	fop.pFrom = szFrom;		/*_T("E:\\mywww\\server\\mysql\\5.5\\*.*");*/
	fop.pTo = szTo;			/*_T("E:\\mywww\\server\\test\\5.5");*/
	bRet = (SHFileOperation(&fop) == 0 ? true : false);

	return bRet;
}

void CMainDlgWnd::nyDeleteFile(/*LPCTSTR lpszPath*/ const std::wstring& srcpath)
{
	//SHFILEOPSTRUCT FileOp;
	//ZeroMemory((void*)&FileOp, sizeof(SHFILEOPSTRUCT));

	//FileOp.fFlags = FOF_NOCONFIRMATION;
	//FileOp.hNameMappings = NULL;
	//FileOp.hwnd = NULL;
	//FileOp.lpszProgressTitle = NULL;
	//FileOp.pFrom = lpszPath;
	//FileOp.pTo = NULL;
	//FileOp.wFunc = FO_DELETE;

	//int ret = SHFileOperation(&FileOp) == 0;

	//return ;

	int nOk;
	TCHAR strSrc[MAX_PATH] = { 0 };//源文件路径; 
	TCHAR strDst[MAX_PATH] = { 0 };//目标文件路径; 
	TCHAR strTitle[] = _T("文件拷贝"); //文件删除进度对话框标题 
	_tcscpy_s(strSrc, MAX_PATH, srcpath.c_str());
	_tcscpy_s(strDst, MAX_PATH, srcpath.c_str());
	SHFILEOPSTRUCT FileOp;//定义SHFILEOPSTRUCT结构对象; 
	FileOp.hwnd = this->m_hWnd;
	FileOp.wFunc = FO_DELETE; //执行文件删除操作; 
	FileOp.pFrom = strSrc;
	FileOp.pTo = NULL;
	FileOp.fFlags = FOF_SILENT | FOF_NOCONFIRMATION;// FOF_ALLOWUNDO;//此标志使删除文件备份到Windows回收站 
	FileOp.hNameMappings = NULL;
	FileOp.lpszProgressTitle = strTitle;

	//开始删除文件 
	try
	{
		nOk = SHFileOperation(&FileOp);
 		int a = -1;
		if (nOk)
			TRACE("There is an error: %d/n", nOk);
		else
			TRACE("SHFileOperation finished successfully/n");
	}
	catch (...)
	{
		int b = -1;
	}
}

void CMainDlgWnd::nyDeleteFileA(/*LPCTSTR lpszPath*/ const std::string& srcpath)
{
	int nOk;
	char strSrc[MAX_PATH] = { 0 };//源文件路径; 
	char strDst[MAX_PATH] = { 0 };//目标文件路径; 
	char strTitle[] = "文件拷贝"; //文件删除进度对话框标题 
	strcpy(strSrc, srcpath.c_str());
	strcpy(strDst, srcpath.c_str());
	SHFILEOPSTRUCTA FileOp;//定义SHFILEOPSTRUCT结构对象; 
	FileOp.hwnd = this->m_hWnd;
	FileOp.wFunc = FO_DELETE; //执行文件删除操作; 
	FileOp.pFrom = strSrc;
	FileOp.pTo = NULL;
	FileOp.fFlags = FOF_SILENT | FOF_NOCONFIRMATION;// FOF_ALLOWUNDO;//此标志使删除文件备份到Windows回收站 
	FileOp.hNameMappings = NULL;
	FileOp.lpszProgressTitle = strTitle;

	//开始删除文件 
	try
	{
		nOk = SHFileOperationA(&FileOp);
		int a = -1;
		if (nOk)
			TRACE("There is an error: %d/n", nOk);
		else
			TRACE("SHFileOperation finished successfully/n");
	}
	catch (...)
	{
		int b = -1;
	}
}

DWORD CMainDlgWnd::EnumerateFileInDrectory(LPSTR szPath, LPSTR szToPath)
{
	WIN32_FIND_DATAA FindFileData;
	HANDLE hListFile;
	CHAR szFilePath[MAX_PATH];
	CHAR myFilePath[MAX_PATH];
	CHAR copy_to_FilePath[MAX_PATH] = "";
	lstrcpyA(copy_to_FilePath, szToPath);
	CHAR new_FileName[MAX_PATH];
	//构造代表子目录和文件夹路径的字符串，使用通配符“*”
	lstrcpyA(szFilePath, szPath);
	lstrcpyA(myFilePath, szPath);
	//注释的代码可以用于查找所有以“.txt结尾”的文件。
	//lstrcat(szFilePath, "\\*.txt");
	lstrcatA(szFilePath, "\\*");

	//查找第一个文件/目录，获得查找句柄
	hListFile = FindFirstFileA(szFilePath, &FindFileData);
	//判断句柄
	if (hListFile == INVALID_HANDLE_VALUE)
	{
		printf("错误：%d", GetLastError());
		return 1;
	}
	else
	{
		do
		{
			/*    如果不想显示代表本级目录和上级目录的“.”和“..”，
			可以使用注释部分的代码过滤。 */
			if (lstrcmpA(FindFileData.cFileName, ".") == 0 ||
				lstrcmpA(FindFileData.cFileName, "..") == 0)
			{
				continue;
			}

			if (FindFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
			{
				printf("%s\t\t", FindFileData.cFileName);
				printf("<DIR> \n ");
				lstrcatA(myFilePath, "\\");
				lstrcatA(myFilePath, FindFileData.cFileName);
				cout << "file path" << myFilePath << endl;
				EnumerateFileInDrectory(myFilePath, szToPath);
				memset(myFilePath, 0, MAX_PATH);
				lstrcpyA(myFilePath, szPath);
			}
			else
			{
				cout << "file name" << FindFileData.cFileName << endl;
				lstrcatA(myFilePath, "\\");
				lstrcatA(myFilePath, FindFileData.cFileName);

				memset(new_FileName, 0, MAX_PATH);
				lstrcpyA(new_FileName, copy_to_FilePath);
				lstrcatA(new_FileName, FindFileData.cFileName);
				CopyFileA(myFilePath, new_FileName, FALSE);
				memset(myFilePath, 0, MAX_PATH);
				lstrcpyA(myFilePath, szPath);
			}
			printf("\n");
		} while (FindNextFileA(hListFile, &FindFileData));
	}
	return 0;
}




