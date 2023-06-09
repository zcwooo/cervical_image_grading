
// MicroScopeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "math.h"
#include "MicroScope.h"
#include "MicroScopeDlg.h"
#include "gen.h"
#include "dlgSetting.h"
#include <opencv2/core/core.hpp> 
#include <opencv2/highgui/highgui.hpp>
#include <opencv/cv.h>
#include <shlwapi.h>
#include "MMSYSTEM.h"
#include "CellSelection.h"
#include "AdjBrightnessDlg.h"

using namespace DShowLib;

#ifdef _DEBUG
//#pragma comment(lib,"./classlib/win32/debug/TIS_UDSHL11d.lib")
#pragma comment(lib,"TIS_UDSHL11d.lib")
#else
#pragma comment(lib,"TIS_UDSHL11.lib")
#endif

#pragma comment(lib,"shlwapi.lib")
#pragma comment(lib,"winmm.lib")
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace cv; 
// CAboutDlg dialog used for App About

//---------------------------------------------------------------
CScan m_scan;
CStage m_stage;
CZ m_cz;

CTime g_aTime;

BOOL isZMoving();
BOOL isXYMoving();
double CalSharpness(smart_ptr<MemBuffer> pBuffer,CMicroScopeDlg *pDlg);//计算清晰度
int AdjWid_Hei_Buf24(smart_ptr<MemBuffer> pBuffer,int nWid,int nHei,int nBits,CMicroScopeDlg *pDlg);

HANDLE m_pMoveThread=NULL;
HANDLE m_pThreadFirstFocus=NULL;

DWORD WINAPI doAuto(LPVOID lpParam);
DWORD WINAPI doAuto_NoZ_Focus(LPVOID lpParam);
DWORD WINAPI doAuto_2(LPVOID lpParam);
DWORD WINAPI doFirstFocus(LPVOID lpParam);

void Process(smart_ptr<MemBuffer> pBuffer,CMicroScopeDlg *pDlg);
void Save_1_Img(smart_ptr<MemBuffer> pImg,CMicroScopeDlg *pDlg);
int CalCell(TCHAR* lpFName,CMicroScopeDlg *pDlg);

int Focus_Z_1_Step(CMicroScopeDlg *pDlg);
int WaitSaveImg();
double WaitCalImg_Clarity();

void WaitStop_z();
void WaitStop_xy();
void WaitStop_xyz();

//---------------------------------------------------------------
TCHAR g_lpBinPath[MAX_PATH];	//执行文件路径
TCHAR g_lpINIPath[MAX_PATH];
TCHAR g_lpLogPath[MAX_PATH];

int g_nInitCnt=0;

double MAX_RADIUS=5.5;//最大扫描半径6.5mm

#define STATUS_NONE					0
//#define STATUS_CAL_IMG_HAVE_CELL	1
#define STATUS_CAL_IMG_SHARPNESS	2
#define STATUS_Z_WAIT_1_IMG			3
#define STATUS_XY_WAIT_1_IMG		4
#define STATUS_XY_WAIT_1_IMG_ADD	5
#define STATUS_SAVE_1_IMG			6

//#define STATUS_WAIT_XY_STOP				1000
//#define STATUS_WAIT_Z_STOP				1001
//#define STATUS_WAIT_XYZ_STOP			1002


int g_nStatus = STATUS_NONE;
HANDLE g_hImgEvent = NULL;
HANDLE g_hMoveEvent = NULL;
double g_dClarity[MAX_Z_STEPS];
double g_dCurClarity=0.0;

FocusPara g_FocusPara;

#ifdef __CAL_CELL__
//统计数据集中上皮细胞个数，以及所有上皮细胞灰度总和
#define MAX_CELL_NUM	100000

int g_nCountCells              = 0;   

int *g_nCellsPixelArea=NULL;
double *g_dCellsLambda=NULL; 
double *g_dCircleDegree=NULL;             // 每个上皮的圆度（yiger添加）
int *g_dGrayValue1=NULL;                  // 每个上皮的灰度值（yiger添加）
int g_nCountImpurity=0;	//杂质

long long MeanLymphocyteGrayValue = 0;
long long SumLymphocyteGrayValue  = 0;
int g_nCountLymphocyte_ori          = 0;		//淋巴
int g_nCountLymphocyte              = 0;		//淋巴
#endif

#if 0
BOOL aMakeHistogram( BYTE *pBuf ,int nWid,int nHei,int nBits,int *pHistData)
{
//	smart_ptr<BITMAPINFOHEADER> pInf = pImg->getBitmapInfoHeader();
//	BYTE *pBuf = (BYTE *)pImg->getPtr();
	int nLineBytes=WIDTHBYTES(nBits*nWid);
	int nPos=0;

	memset(pHistData,0,256*sizeof(int));

    int i = 0;
    int j = 0;

	if(nBits==8)
	{
		nPos=0;
		for(j=0;j<nHei;j++)
		{
			nPos=j*nLineBytes;
			for(i=0;i<nWid;i++)
			{
				int nV = pBuf[nPos];
				pHistData[nV]++;
				nPos++;
			}
		}
	}
	else if(nBits==24)
	{
		nPos=1;	//绿通道
		for(j=0;j<nHei;j++)
		{
			nPos=j*nLineBytes;
			for(i=0;i<nWid;i++)
			{
				int nV = pBuf[nPos];
				pHistData[nV]++;
				nPos+=3;
			}
		}
	}

    return TRUE;
}
#endif

int mycmp(const void *p1, const void *p2)
{
	FirstFocusPara* f1 = (FirstFocusPara*)p1;
	FirstFocusPara* f2 = (FirstFocusPara*)p2;

	double delta = 0.0000001;
	if(f1->dClarity < (f2->dClarity + delta))
		return 1;
	else if(f1->dClarity > (f2->dClarity - delta))
		return -1;
	else
		return 0;
}

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMicroScopeDlg dialog
CMicroScopeDlg::CMicroScopeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMicroScopeDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bHaveCamera = FALSE;
	m_pMoveThread = NULL;

	m_pThreadFirstFocus = NULL;
	m_bDoFirstFocus=FALSE;

	m_bAutoOn = FALSE;
	
	m_nImgWid=1600;
	m_nImgHei=1200;
	m_pBuf8=NULL;

	m_bAdjBrigntnessOn=FALSE;
	m_pHist=new int[256];
	memset(m_pHist,0,256*sizeof(int));
	m_nFirstFocusStepID=FIRST_FOCUS_STEP_NONE;

#if 0
	char lpName[MAX_PATH];
	sprintf(lpName,"%s","d:\\QiaoWork\\yyy - 郏老师\\显微尺_1_每小格是10us.bmp");
	HANDLE hdib=LoadImageFile((LPCTSTR)lpName);
	SIZE aSize;
	int nBitsPerPixel, nNumColors;
	GetImageInfo( hdib, &aSize, NULL, &nBitsPerPixel, &nNumColors );
	LPSTR lpbi=(LPSTR )GlobalLock(hdib);

	BYTE *pBuf = (BYTE *)FindDIBBits( lpbi );

	m_nImgWid=aSize.cx;
	m_nImgHei=aSize.cy;

	aMakeHistogram( pBuf ,aSize.cx,aSize.cy,nBitsPerPixel,m_pHist);
	GlobalUnlock(hdib);
	SAFE_FreeDIBHandle(hdib);
#endif

	m_nBits = 0;
	m_nImgWid_Adj=0;
	m_nImgHei_Adj=0;
	m_pBuf8_Adj = NULL;

	g_hImgEvent = NULL;
	g_hMoveEvent = NULL;
	for(int i=0;i<MAX_Z_STEPS;i++)
	{
		g_dClarity[i]=0.0;
	}

	m_nX_interval =2;

	m_nFirstFocusStepNum=7;
	m_pFirstFocusPara=NULL;

	m_nXSeri = 0;
	m_nYSeri = 0;
	m_bCenterSetted = FALSE;
	m_bRescanRun=FALSE;

#ifdef __CAL_CELL__
	g_nCellsPixelArea=new int[MAX_CELL_NUM];
	g_dCellsLambda=new double[MAX_CELL_NUM]; 
	g_dCircleDegree=new double[MAX_CELL_NUM];             // 每个上皮的圆度（yiger添加）
	g_dGrayValue1=new int[MAX_CELL_NUM];                            // 每个上皮的灰度值（yiger添加）

	int i=0;
	for(i=0;i<MAX_CELL_NUM;i++)
	{
		g_nCellsPixelArea[i]=0;
		g_dCellsLambda[i]=0.0;
		g_dCircleDegree[i]=0.0;
		g_dGrayValue1[i]=0;
	}
#endif

	m_bResizeEn=FALSE;
}

void CMicroScopeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ACTIVE_IMG, m_cStaticVideoWindow);
	DDX_Control(pDX, IDC_BTN_IMG, m_cButtonLive);
	DDX_Control(pDX, IDC_BTN_AUTO, m_cBtnAuto);
}

BEGIN_EASYSIZE_MAP(CMicroScopeDlg)
	//EASYSIZE(IDC_ACTIVE_IMG,ES_BORDER,ES_BORDER,ES_BORDER,IDC_STATIC_CAL_RESULT,0)
	EASYSIZE(IDC_PROGRESS,IDC_ACTIVE_IMG,ES_BORDER,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_BTN_IMG,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_BTN_AUTO,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_INFO,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_INFO_TIME,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDCANCEL,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_BTN_CAM_SETTING,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_BTN_SETTING,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_BTN_MOVETO_CENTER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_BTN_SET_AS_CENTER,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_BTN_DOWN,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_BTN_UP,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_BTN_ADJ_BRIGHTNESS,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_1,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_2,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_3,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_4,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_5,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_6,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_EDIT_SHANGPI,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_EDIT_LINBA,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_EDIT_DANBEITI,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_EDIT_ERBEITI,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_EDIT_SANBEITI,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_EDIT_DUOBEITI,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_7,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_EDIT_XIANXINGLI,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_STATIC_8,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_EDIT_Impurity,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	EASYSIZE(IDC_BTN_STATISTIC,ES_KEEPSIZE,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
END_EASYSIZE_MAP


BEGIN_MESSAGE_MAP(CMicroScopeDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZING()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_MESSAGE(WM_CHANGE_BTN,OnChangeTtnName)
	ON_MESSAGE(WM_AUTO_CAL,OnAutoCal)
	ON_MESSAGE(WM_STAGE_CHANGED,OnStageChanged)
	ON_MESSAGE(WM_SHOW_INFO,OnShowInfo)
	ON_MESSAGE(WM_SHOW_RESULT,OnShowResult)
	ON_MESSAGE(WM_STOP_AUTO,OnStopAuto)
	ON_MESSAGE(WM_RE_SCAN,OnRescan)
	ON_BN_CLICKED(IDC_BTN_IMG, &CMicroScopeDlg::OnBnClickedBtnImg)
	ON_BN_CLICKED(IDC_BTN_AUTO, &CMicroScopeDlg::OnBnClickedBtnAuto)
	ON_BN_CLICKED(IDC_BTN_SETTING, &CMicroScopeDlg::OnBnClickedBtnSetting)
	ON_BN_CLICKED(IDC_BTN_MOVETO_CENTER, &CMicroScopeDlg::OnBnClickedBtnMovetoCenter)
	ON_BN_CLICKED(IDC_BTN_STATISTIC, &CMicroScopeDlg::OnBnClickedBtnStatistic)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_BTN_UP, &CMicroScopeDlg::OnBnClickedBtnUp)
	ON_BN_CLICKED(IDC_BTN_DOWN, &CMicroScopeDlg::OnBnClickedBtnDown)
	ON_BN_CLICKED(IDC_BTN_TEST, &CMicroScopeDlg::OnBnClickedBtnTest)
	ON_BN_CLICKED(IDC_BTN_SET_AS_CENTER, &CMicroScopeDlg::OnBnClickedBtnSetAsCenter)
	ON_BN_CLICKED(IDC_BTN_CAM_SETTING, &CMicroScopeDlg::OnBnClickedBtnCamSetting)
	ON_BN_CLICKED(IDC_BTN_ADJ_BRIGHTNESS, &CMicroScopeDlg::OnBnClickedBtnAdjBrightness)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CMicroScopeDlg message handlers

BOOL CMicroScopeDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	INIT_EASYSIZE;
	//ResizeImgWin();
	InitPath(g_lpBinPath,g_lpINIPath,g_lpLogPath);
	rdPara(g_lpINIPath);

	m_bTimerOn = FALSE;

	m_cGrabber.addListener(&m_cListener);
	m_cListener.SetParent(this);

	m_cListener.SetViewCWnd(&m_cStaticVideoWindow);
	m_pSink = FrameHandlerSink::create( DShowLib::eRGB24, 3 );
	m_pSink->setSnapMode( false ); // Automatically copy every frame to the sink and call CListener::frameReady().
	m_cGrabber.setSinkType( m_pSink );

	//(5)
	// Try to load the previously used video capture device.
	if( m_cGrabber.loadDeviceStateFromFile( "device.xml"))
	{
		m_bHaveCamera = TRUE;
		// Display the device's name in the caption bar of the application.
		//SetWindowText( TEXT("TestIC ") + CString(m_cGrabber.getDev().c_str()));
		//m_cGrabber.startLive(false); // The live video will be displayed by the CListener object.
	}

#if 1
	if(InitPrior(m_nComPortID)==FALSE)
	{
		AfxMessageBox(_T("显微平台初始化错误！"));
		EnableMoveBtn(FALSE);
		m_bStageEn = FALSE;
	}
	else
	{
		m_bStageEn = TRUE;
		m_stage.GetPosition(&m_dCurX,&m_dCurY);
		m_dCurZ=m_cz.get_Position();
	}
#endif

	g_hImgEvent = CreateEvent(NULL,FALSE,FALSE,_T("qMicroScopeDlg"));
	g_hMoveEvent = CreateEvent(NULL,FALSE,FALSE,_T("qMicroScopeDlg_Stage"));

	m_cStaticVideoWindow.ShowWindow(FALSE);
	CWnd* pWnd=GetDlgItem(IDC_PROGRESS);
	pWnd->ShowWindow(FALSE);

	InitMark();
	SetTimer(1,1000,NULL);
	m_bTimerOn = TRUE;

	InitFocus_buf();

	srand((unsigned)time(0));

	return TRUE;  // return TRUE  unless you set the focus to a control
}
	
void CMicroScopeDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here

	wrPara(g_lpINIPath);

	SetEvent(g_hImgEvent);
	SetEvent(g_hMoveEvent);

	if(m_bAutoOn)
	{
		StopMoveThread();
		m_bAutoOn=FALSE;
	}

	StopFirstFocusThread();

	if(m_bTimerOn)
	{
		KillTimer(1);
		m_bTimerOn = FALSE;
	}

	m_cGrabber.stopLive();

	m_scan.DisConnect();
	if(m_pBuf8!=NULL)
		delete m_pBuf8;

	if(m_pBuf8_Adj!=NULL)
		delete m_pBuf8_Adj;

	CloseHandle(g_hImgEvent);
	CloseHandle(g_hMoveEvent);

	SAFE_FreeDIBHandle(m_hDib_BK1);
	SAFE_FreeDIBHandle(m_hDib_BK2);
	SAFE_FreeDIBHandle(m_hDib_Mark1);
	SAFE_FreeDIBHandle(m_hDib_Mark2);
	SAFE_FreeDIBHandle(m_hDib_Mark3);
	SAFE_FreeDIBHandle(m_hDib_Mark4);

	for(int i=0;i<m_nyNum;i++)
	{
		free(m_CapStatus[i]);
	}

	free(m_CapStatus);

#ifdef __CAL_CELL__
	delete g_nCellsPixelArea;
	delete g_dCellsLambda;
	delete g_dCircleDegree;
	delete g_dGrayValue1;
#endif
	delete m_pFirstFocusPara;

	if(m_cz.m_bAutoRelease)
	{
		m_cz.m_lpDispatch->Release();
		m_cz.m_lpDispatch = NULL;
	}

	if(m_stage.m_bAutoRelease)
	{
		m_stage.m_lpDispatch->Release();
		m_stage.m_lpDispatch = NULL;
	}

	if(m_scan.m_bAutoRelease)
	{
		m_scan.m_lpDispatch->Release();
		m_scan.m_lpDispatch = NULL;
	}

	delete m_pHist;
//防止运行一个应用程序的多个实列.3处
//第三处,删除 'tag'
	::RemoveProp(GetSafeHwnd(),AfxGetApp()->m_pszExeName);
}

void CMicroScopeDlg::ResizeImgWin()
{
	if(m_bResizeEn)
	{
		CRect recDlg;
		CRect rec;
		GetWindowRect(&recDlg);
		CWnd* pWnd=(CWnd*)GetDlgItem(IDC_ACTIVE_IMG);
		pWnd->GetWindowRect(&rec);
		ScreenToClient(&rec);
		int nHei=recDlg.Height()-60;
		nHei=((nHei+1)/2)*2;

		int nWid=(m_nImgWid*nHei)/m_nImgHei;
		nWid=((nWid+3)/4)*4;

		pWnd->MoveWindow(rec.left,rec.top,nWid,nHei);
		//pWnd->GetWindowRect(&rec);
		//ScreenToClient(&rec);
	}
}
//参考：Prior Dll Help.chm
//注意：需要安装32位驱动：Prior Scientific 32 bit Installer.msi
//nPort:	串口号
BOOL CMicroScopeDlg::InitPrior(int nPort)
{
	AfxOleInit();

	COleException e;
	CLSID clsid;
	LPUNKNOWN lpUnk;
	LPDISPATCH lpDispatch;
	long Port = nPort;

	//(1)
	if (CLSIDFromProgID(OLESTR("prior.scan"), &clsid) != NOERROR)
		return FALSE;

	//(1.1)
	if (GetActiveObject(clsid, NULL, &lpUnk) == NOERROR)
	{
		HRESULT hr = lpUnk->QueryInterface(IID_IDispatch, (LPVOID*)&lpDispatch);
		lpUnk->Release();
		if (hr == NOERROR)
			m_scan.AttachDispatch(lpDispatch, TRUE);
	}
	else
	{
		//return FALSE;
	}

	//(1.2)
	// if not dispatch ptr attached yet, need to create one
	if (m_scan.m_lpDispatch == NULL && !m_scan.CreateDispatch(clsid, &e))
	{
		return FALSE;
	}

	//(2)
	if (CLSIDFromProgID(OLESTR("prior.stage"), &clsid) != NOERROR)
		return FALSE;

	//(2.1)
	if (GetActiveObject(clsid, NULL, &lpUnk) == NOERROR)
	{
		HRESULT hr = lpUnk->QueryInterface(IID_IDispatch, (LPVOID*)&lpDispatch);
		lpUnk->Release();
		if (hr == NOERROR)
			m_stage.AttachDispatch(lpDispatch, TRUE);
	}
	else
	{
		//return FALSE;
	}

	//(2.2)
	// if not dispatch ptr attached yet, need to create one
	if (m_stage.m_lpDispatch == NULL && !m_stage.CreateDispatch(clsid, &e))
	{
		return FALSE;
	}

	//(3)
	if (CLSIDFromProgID(OLESTR("prior.z"), &clsid) != NOERROR)
		return FALSE;

	//(3.1)
	if (GetActiveObject(clsid, NULL, &lpUnk) == NOERROR)
	{
		HRESULT hr = lpUnk->QueryInterface(IID_IDispatch, (LPVOID*)&lpDispatch);
		lpUnk->Release();
		if (hr == NOERROR)
			m_cz.AttachDispatch(lpDispatch, TRUE);
	}
	else
	{
		//return FALSE;
	}

	//(3.2)
	// if not dispatch ptr attached yet, need to create one
	if (m_cz.m_lpDispatch == NULL && !m_cz.CreateDispatch(clsid, &e))
	{
		return FALSE;
	}

	//(4)
	m_scan.Connect (&Port);
	if (Port < 0)
	{
		return FALSE;
	}

	return TRUE;
}
void CMicroScopeDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}
void CMicroScopeDlg::ClearMark()
{
	for(int i=0;i<m_nyNum;i++)
	{
		memset(m_CapStatus[i],0,m_nxNum*sizeof(CapStatus));
	}
}
void CMicroScopeDlg::InitMark()
{
	m_nxNum =m_dRadius_mm*2*1000/m_nX_step_um+1;
	m_nyNum =m_dRadius_mm*2*1000/m_nY_step_um+1;
	m_CapStatus = (CapStatus **)malloc((m_nyNum+1)*sizeof(CapStatus *));
	for(int i=0;i<m_nyNum;i++)
	{
		m_CapStatus[i] = (CapStatus *)malloc((m_nxNum+1)*sizeof(CapStatus));
	}
	ClearMark();
}
void CMicroScopeDlg::ShowAllMark(CDC* pDC)
{
	//CDC* pDC=GetDC();
	CRect rectClient;
	CWnd* pWnd=GetDlgItem(IDC_PROGRESS);
	pWnd->GetWindowRect(&rectClient);
	ScreenToClient(&rectClient);
	DrawImage(pDC->GetSafeHdc(),m_hDib_BK2,&rectClient);

	int xWid=rectClient.Width()/m_nxNum;
	int yHei=rectClient.Height()/m_nyNum;

	int recWid=rectClient.Width();
	int recHei=rectClient.Height();
	int nXOffset=(recWid-m_nxNum*xWid)/2;
	int nYOffset=(recHei-m_nyNum*yHei)/2;
	rectClient.OffsetRect(nXOffset,nYOffset);
	int x,y;
	CRect rec;

	for(int i=0;i<m_nxNum;i++)
	{
		for(int j=0;j<m_nyNum;j++)
		{
			x = rectClient.left + i*xWid;
			y = rectClient.top +j*yHei;
			rec.left=x;
			rec.top=y;
			rec.right=x+xWid;
			rec.bottom=y+yHei;
			if(m_CapStatus[j][i].nSt==0)
				DrawImage(pDC->GetSafeHdc(),m_hDib_Mark1,&rec);
			else if(m_CapStatus[j][i].nSt==1)
				DrawImage(pDC->GetSafeHdc(),m_hDib_Mark2,&rec);
			else if(m_CapStatus[j][i].nSt==2)
				DrawImage(pDC->GetSafeHdc(),m_hDib_Mark3,&rec);
		}
	}

	//ReleaseDC(pDC);
}
void CMicroScopeDlg::ShowOneMark(int nx,int ny)
{
	CDC* pDC=GetDC();
	CRect rectClient;
	CWnd* pWnd=GetDlgItem(IDC_PROGRESS);
	pWnd->GetWindowRect(&rectClient);
	ScreenToClient(&rectClient);

	int xWid=rectClient.Width()/m_nxNum;
	int yHei=rectClient.Height()/m_nyNum;

	int recWid=rectClient.Width();
	int recHei=rectClient.Height();
	int nXOffset=(recWid-m_nxNum*xWid)/2;
	int nYOffset=(recHei-m_nyNum*yHei)/2;
	rectClient.OffsetRect(nXOffset,nYOffset);
	int x,y;
	CRect rec;
	x = rectClient.left + nx*xWid;
	y = rectClient.top +ny*yHei;
	rec.left=x;
	rec.top=y;
	rec.right=x+xWid;
	rec.bottom=y+yHei;

	if(m_CapStatus[ny][nx].nSt==0)
		DrawImage(pDC->GetSafeHdc(),m_hDib_Mark1,&rec);
	if(m_CapStatus[ny][nx].nSt==1)
		DrawImage(pDC->GetSafeHdc(),m_hDib_Mark2,&rec);
	if(m_CapStatus[ny][nx].nSt==2)
		DrawImage(pDC->GetSafeHdc(),m_hDib_Mark3,&rec);

	ReleaseDC(pDC);

}
void CMicroScopeDlg::ShowBK_Img()
{
	CRect rectDest;
	AdjRect(rectDest);
	rectDest.OffsetRect(11,11);

	CDC* pDC=GetDC();
	DrawImage(pDC->GetSafeHdc(),m_hDib_BK1,&rectDest);
	ReleaseDC(pDC);
}
void CMicroScopeDlg::AdjRect(CRect& OutRec)
{
	CRect rectImgCtl;
	//m_cStaticVideoWindow.GetWindowRect(&rectImgCtl);
	//ScreenToClient(&rectImgCtl);
	m_cStaticVideoWindow.GetClientRect(&rectImgCtl);

	CRect rectDest(rectImgCtl);
	double m_Ratio_Img = (double)m_nImgWid/m_nImgHei;
	double m_Ratio_View = (double)rectImgCtl.Width()/rectImgCtl.Height();
	if(m_Ratio_View<m_Ratio_Img)
	{
		rectDest.bottom=rectImgCtl.Width()/m_Ratio_Img;
	}
	else
	{
		rectDest.right=rectImgCtl.Height()*m_Ratio_Img;
	}
	OutRec = rectDest;
}
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CMicroScopeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);
		ShowAllMark(&dc);
		
		if(!m_bHaveCamera)
		{
			ShowBK_Img();
		}
		else
		{
			if ( !m_cGrabber.isLive())
			{
				ShowBK_Img();
			}
		}
		CDialog::OnPaint();
	}
	if(!m_bResizeEn)
	{
		m_bResizeEn=TRUE;
		//ResizeImgWin();
		PostMessage(WM_SIZE,0,0);
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CMicroScopeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CMicroScopeDlg::OnSizing(UINT fwSide, LPRECT pRect)
{
	CDialog::OnSizing(fwSide, pRect);

	// TODO: Add your message handler code here
	//EASYSIZE_MINSIZE(1024,680,fwSide,pRect);
	//EASYSIZE_MINSIZE(942,535,fwSide,pRect);
	EASYSIZE_MINSIZE(1104,542,fwSide,pRect);
}


void CMicroScopeDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	ResizeImgWin();
	UPDATE_EASYSIZE;
}

void CMicroScopeDlg::ShowInfo(TCHAR* lpInfo)
{
	CWnd* pWnd=GetDlgItem(IDC_STATIC_INFO);
	pWnd->SetWindowText((LPCTSTR)lpInfo);
	//SetDlgItemText(IDC_STATIC_INFO,lpInfo);
	//UpdateWindow();	
}

void CMicroScopeDlg::SetImgButtonStates(void)
{
	bool bDevValid =  m_cGrabber.isDevValid();
	bool bIsLive   = m_cGrabber.isLive();

	//m_cButtonSettings.EnableWindow(bDevValid);
	m_cButtonLive.EnableWindow(bDevValid);
	
	if( !bDevValid )
	{
		m_cButtonLive.SetWindowText(TEXT("显示图像"));
	}

	if( bIsLive )
	{
		m_cStaticVideoWindow.ShowWindow(TRUE);
		m_cButtonLive.SetWindowText(TEXT("停止显示"));
	}
	else
	{
		m_cStaticVideoWindow.ShowWindow(FALSE);
		m_cButtonLive.SetWindowText(TEXT("显示图像"));
	}
}
void CMicroScopeDlg::OnBnClickedBtnImg()
{
	if(!m_bHaveCamera)
	{
		if(m_cGrabber.isDevValid() && m_cGrabber.isLive())
		{
			m_cGrabber.stopLive();
		}

		m_cGrabber.showDevicePage(this->m_hWnd);

		if( m_cGrabber.isDevValid())
		{
			m_cGrabber.saveDeviceStateToFile("device.xml");
			m_bHaveCamera = TRUE;
		}
		else
		{
			_stprintf(m_lpInfo,_T("%s"),_T("No camera."));
			ShowInfo(m_lpInfo);
			return;
		}
	}
	if( m_cGrabber.isDevValid())
	{
		if (m_cGrabber.isLive())
		{
			SetEvent(g_hImgEvent);
			SetEvent(g_hMoveEvent);
			m_cGrabber.stopLive();
		}
		else
		{
			// Call startLive with "false", because the live display is done
			// by the CListener object.
			//m_cGrabber.startLive(false);
			StartImg();
		}
		SetImgButtonStates();
	}
}

void CMicroScopeDlg::StartImg()
{
	if( m_cGrabber.isDevValid())
	{
		g_nInitCnt=0;

		CRect rectImg;
		CWnd* pWnd=GetDlgItem(IDC_ACTIVE_IMG);
		GetClientRect(&rectImg);
		pWnd->GetWindowRect(&rectImg);
		ScreenToClient(&rectImg);
		InvalidateRect(&rectImg,TRUE);

		m_cGrabber.startLive(false);
	}
}
void CMicroScopeDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialog::OnTimer(nIDEvent);
	double dZ =0.0;
	if(m_bAutoOn)
	{
		TCHAR lpInfo[64];
		CTime bTime=CTime::GetCurrentTime();
		bTime.GetTime();
		CTimeSpan bbb;
		bbb = bTime - g_aTime;

		_stprintf(lpInfo,_T("%dM:%dS"),bbb.GetMinutes(),bbb.GetSeconds());

		CWnd* pWnd=GetDlgItem(IDC_STATIC_INFO_TIME);
		pWnd->SetWindowText((LPCTSTR)lpInfo);
	}
	double x,y,z;

	double dH=m_dRadius_mm*1000;
	double dV=m_dRadius_mm*1000;

	if(m_nFirstFocusStepID !=FIRST_FOCUS_STEP_NONE)
		CalFocusBorderPt(&dH,&dV);	//重新计算、赋值

	switch(m_nFirstFocusStepID)
	{
	case FIRST_FOCUS_STEP_11:
		OutputDebugString(_T("---【1，1】\n"));
		x=m_dCenterX;
		y=m_dCenterY;
		z=m_dCenterZ;
		m_scan.MoveXYZtoAbsolute(x,y,z);

		WaitStop_xyz();
		ShowStagePos();
		FirstFocus(TRUE);
		m_dZ33[1][1]=m_pFirstFocusPara[0].zPos;
		//if(PathFileExists(m_lpSndFile))
		//{
		//	PlaySound(m_lpSndFile, NULL, SND_FILENAME | SND_ASYNC);
		//}
		if(m_bAutoOn==0)
			m_nFirstFocusStepID = FIRST_FOCUS_STEP_NONE;
		else
			m_nFirstFocusStepID = FIRST_FOCUS_STEP_00;
		break;
	case FIRST_FOCUS_STEP_00:
		OutputDebugString(_T("---【0，0】\n"));
		dZ = m_pFirstFocusPara[0].zPos;

		x=m_dCenterX + dH;
		y=m_dCenterY + dV;
		z=m_dCenterZ;
		m_scan.MoveXYZtoAbsolute(x,y,z);

		WaitStop_xyz();
		ShowStagePos();
		FirstFocus(TRUE);
		m_dZ33[0][0]=m_pFirstFocusPara[0].zPos;
		//if(PathFileExists(m_lpSndFile))
		//{
		//	PlaySound(m_lpSndFile, NULL, SND_FILENAME | SND_ASYNC);
		//}
		if(m_bAutoOn==0)
			m_nFirstFocusStepID = FIRST_FOCUS_STEP_NONE;
		else
			m_nFirstFocusStepID = FIRST_FOCUS_STEP_10;
		break;
	case FIRST_FOCUS_STEP_10:
		OutputDebugString(_T("---【1，0】\n"));
		dZ = m_pFirstFocusPara[0].zPos;

		x=m_dCenterX;
		y=m_dCenterY + dV;
		z=m_dCenterZ;

		m_scan.MoveXYZtoAbsolute(x,y,z);

		WaitStop_xyz();
		ShowStagePos();
		FirstFocus(TRUE);
		m_dZ33[1][0]=m_pFirstFocusPara[0].zPos;
		//if(PathFileExists(m_lpSndFile))
		//{
		//	PlaySound(m_lpSndFile, NULL, SND_FILENAME | SND_ASYNC);
		//}

		if(m_bAutoOn==0)
			m_nFirstFocusStepID = FIRST_FOCUS_STEP_NONE;
		else
			m_nFirstFocusStepID = FIRST_FOCUS_STEP_20;
		break;
	case FIRST_FOCUS_STEP_20:
		OutputDebugString(_T("---【2，0】\n"));
		dZ = m_pFirstFocusPara[0].zPos;

		x=m_dCenterX - dH;
		y=m_dCenterY + dV;
		z=m_dCenterZ;

		m_scan.MoveXYZtoAbsolute(x,y,z);

		WaitStop_xyz();
		ShowStagePos();
		FirstFocus(TRUE);
		m_dZ33[2][0]=m_pFirstFocusPara[0].zPos;
		//if(PathFileExists(m_lpSndFile))
		//{
		//	PlaySound(m_lpSndFile, NULL, SND_FILENAME | SND_ASYNC);
		//}
		if(m_bAutoOn==0)
			m_nFirstFocusStepID = FIRST_FOCUS_STEP_NONE;
		else
			m_nFirstFocusStepID = FIRST_FOCUS_STEP_21;
		break;
	case FIRST_FOCUS_STEP_21:
		OutputDebugString(_T("---【2，1】\n"));
		dZ = m_pFirstFocusPara[0].zPos;

		x=m_dCenterX - dH;
		y=m_dCenterY;
		z=m_dCenterZ;

		m_scan.MoveXYZtoAbsolute(x,y,z);

		WaitStop_xyz();
		ShowStagePos();
		FirstFocus(TRUE);
		m_dZ33[2][1]=m_pFirstFocusPara[0].zPos;
		//if(PathFileExists(m_lpSndFile))
		//{
		//	PlaySound(m_lpSndFile, NULL, SND_FILENAME | SND_ASYNC);
		//}

		if(m_bAutoOn==0)
			m_nFirstFocusStepID = FIRST_FOCUS_STEP_NONE;
		else
			m_nFirstFocusStepID = FIRST_FOCUS_STEP_22;
		break;
	case FIRST_FOCUS_STEP_22:
		OutputDebugString(_T("---【2，2】\n"));
		dZ = m_pFirstFocusPara[0].zPos;

		x=m_dCenterX - dH;
		y=m_dCenterY - dV;
		z=m_dCenterZ;
		m_scan.MoveXYZtoAbsolute(x,y,z);

		WaitStop_xyz();
		ShowStagePos();
		FirstFocus(TRUE);
		m_dZ33[2][2]=m_pFirstFocusPara[0].zPos;
		//if(PathFileExists(m_lpSndFile))
		//{
		//	PlaySound(m_lpSndFile, NULL, SND_FILENAME | SND_ASYNC);
		//}

		if(m_bAutoOn==0)
			m_nFirstFocusStepID = FIRST_FOCUS_STEP_NONE;
		else
			m_nFirstFocusStepID = FIRST_FOCUS_STEP_12;
		break;
	case FIRST_FOCUS_STEP_12:
		OutputDebugString(_T("---【1，2】\n"));
		dZ = m_pFirstFocusPara[0].zPos;

		x=m_dCenterX;
		y=m_dCenterY - dV;
		z=m_dCenterZ;

		m_scan.MoveXYZtoAbsolute(x,y,z);

		WaitStop_xyz();
		ShowStagePos();
		FirstFocus(TRUE);
		m_dZ33[1][2]=m_pFirstFocusPara[0].zPos;
		//if(PathFileExists(m_lpSndFile))
		//{
		//	PlaySound(m_lpSndFile, NULL, SND_FILENAME | SND_ASYNC);
		//}

		if(m_bAutoOn==0)
			m_nFirstFocusStepID = FIRST_FOCUS_STEP_NONE;
		else
			m_nFirstFocusStepID = FIRST_FOCUS_STEP_02;
		break;
	case FIRST_FOCUS_STEP_02:
		OutputDebugString(_T("---【0，2】\n"));
		dZ = m_pFirstFocusPara[0].zPos;

		x=m_dCenterX + dH;
		y=m_dCenterY - dV;
		z=m_dCenterZ;

		m_scan.MoveXYZtoAbsolute(x,y,z);

		WaitStop_xyz();
		ShowStagePos();
		FirstFocus(TRUE);
		m_dZ33[0][2]=m_pFirstFocusPara[0].zPos;
		//if(PathFileExists(m_lpSndFile))
		//{
		//	PlaySound(m_lpSndFile, NULL, SND_FILENAME | SND_ASYNC);
		//}

		if(m_bAutoOn==0)
			m_nFirstFocusStepID = FIRST_FOCUS_STEP_NONE;
		else
			m_nFirstFocusStepID = FIRST_FOCUS_STEP_01;
		break;
	case FIRST_FOCUS_STEP_01:
		OutputDebugString(_T("---【0，1】\n"));
		dZ = m_pFirstFocusPara[0].zPos;

		x=m_dCenterX + dH;
		y=m_dCenterY;
		z=m_dCenterZ;

		m_scan.MoveXYZtoAbsolute(x,y,z);

		WaitStop_xyz();
		ShowStagePos();
		FirstFocus(TRUE);
		m_dZ33[0][1]=m_pFirstFocusPara[0].zPos;
		//if(PathFileExists(m_lpSndFile))
		//{
		//	PlaySound(m_lpSndFile, NULL, SND_FILENAME | SND_ASYNC);
		//}

		TCHAR lpIni[128];
		wsprintf(lpIni,_T("%s%s"),g_lpBinPath,_T("focus.ini"));
		for(int y=0;y<3;y++)
		{
			CString str;
			CString str_Sec;
			str_Sec.Format(_T("Val_%d"),y+1);
			str.Format(_T("%.2f\t%.2f\t%.2f\n"),m_dZ33[0][y],m_dZ33[1][y],m_dZ33[2][y]);
			WritePrivateProfileString(_T("Focus"),str_Sec,str,lpIni);
			OutputDebugString(str);
		}
		m_nFirstFocusStepID = FIRST_FOCUS_STEP_NONE;
		break;
	default:
		m_nFirstFocusStepID = FIRST_FOCUS_STEP_NONE;
		break;
	}
}



LRESULT CMicroScopeDlg::OnChangeTtnName(WPARAM wparam, LPARAM lparam)
{
	m_cBtnAuto.SetWindowText(TEXT("自动扫片"));
	if(wparam==1)
	{
		OnBnClickedBtnStatistic();
	}
	TCHAR lpINI[MAX_PATH];
	wsprintf(lpINI,_T("%s%s"),g_lpBinPath,_T("ImgDir.ini"));
	WritePrivateProfileString(_T("LastDir"),_T("img_dir"),m_lpDir,lpINI);
	return 1;
}

LRESULT CMicroScopeDlg::OnAutoCal(WPARAM wparam, LPARAM lparam)//给主程序发消息，实现一键扫描
{
	CWnd *pWnd = CWnd::FindWindow(_T("#32770"), _T("癌细胞辅助检测系统 V2.1"));
	if (NULL != pWnd)
	{
		pWnd->PostMessage(WM_USER_CAL, 0, 0);
	}
	return 1;
}
LRESULT CMicroScopeDlg::OnStageChanged(WPARAM wparam, LPARAM lparam)
{
	ShowOneMark(m_nXSeri,m_nYSeri);
	ShowInfo(m_lpStageInfo);
	return 1;
}
LRESULT CMicroScopeDlg::OnShowInfo(WPARAM wparam, LPARAM lparam)
{
	ShowInfo(m_lpInfo);
	return 1;
}
LRESULT CMicroScopeDlg::OnShowResult(WPARAM wparam, LPARAM lparam)
{
	SetDlgItemText(IDC_EDIT_SHANGPI,m_lpResult_ShangPi);
	SetDlgItemText(IDC_EDIT_LINBA,m_lpResult_LinBa);
	SetDlgItemText(IDC_EDIT_XIANXINGLI,m_lpResult_XianXingLi);
	SetDlgItemText(IDC_EDIT_Impurity,m_lpResult_Impurity);
	UpdateWindow();	
	
/*
	CWnd* pWnd;
	pWnd=GetDlgItem(IDC_EDIT_SHANGPI);
	pWnd->SetWindowText(m_lpResult_ShangPi);

	pWnd=GetDlgItem(IDC_EDIT_LINBA);
	pWnd->SetWindowText(m_lpResult_LinBa);
*/
	return 1;
}
LRESULT CMicroScopeDlg::OnStopAuto(WPARAM wparam, LPARAM lparam)
{
	StopMoveThread();
	m_scan.MoveXYZtoAbsolute(m_dCenterX,m_dCenterY,m_dCenterZ);
	m_bAutoOn=FALSE;

	m_cBtnAuto.SetWindowText(TEXT("自动扫片"));

	return 1;
}
LRESULT CMicroScopeDlg::OnRescan(WPARAM wparam, LPARAM lparam)
{
	CString str=_T("是否启动全幅面扫描?");
#ifdef __CAL_CELL__
	str.Format(_T("上皮细胞数目=%d，小于%d，是否启动全幅面扫描?"),g_nCountCells,m_nShangpi_Num_Limit);
#endif
	int rc =AfxMessageBox(str,MB_YESNO);
	if(rc!=IDYES)
	{
		return 0;
	}

	if(!m_cGrabber.isLive())
	{
		_stprintf(m_lpInfo,_T("%s"),_T("Camera not grabbing."));
		ShowInfo(m_lpInfo);
		return 0;
	}
	if(!m_bCenterSetted)
	{
		AfxMessageBox(_T("请先设置中心点。"));
		return 0;
	}

	m_dRadius_mm_bak = m_dRadius_mm;
	m_dRadius_mm = m_dMaxRadius_mm;

	for(int i=0;i<m_nyNum;i++)
	{
		free(m_CapStatus[i]);
	}

	free(m_CapStatus);

	InitMark();

	ClearResult();
	ClearMark();
	CDC* pDC=GetDC();
	ShowAllMark(pDC);
	ReleaseDC(pDC);

	rdBL_No();
	if(_tcscmp(m_lpBL_No,_T("11223344"))==0)
		MakeImgPath(m_lpDir);
	else
		MakeImgPath_BL_No(m_lpDir,m_lpBL_No);


		m_cBtnAuto.SetWindowText(TEXT("停止扫片"));
		//直接移动到左上角开始聚焦
		m_scan.MoveXYZtoAbsolute(m_dCenterX+m_dRadius_mm*1000,m_dCenterY+m_dRadius_mm*1000,m_dCenterZ);
		WaitStop_xyz();
		ShowStagePos();
		FirstFocus(TRUE);

#if 1
		m_bAutoOn=TRUE;
		g_nStatus = STATUS_NONE;

		DWORD ThreadID=0;
		m_pMoveThread= ::CreateThread(NULL,NULL,doAuto_2,(void*)this,0,(LPDWORD)&ThreadID);
		if (m_pMoveThread != NULL)
		{
			OutputDebugString(_T("Start Auto...\n"));
		}
#endif
	return 1;
}




BOOL MakeHistogram( smart_ptr<MemBuffer> pImg,int nWid,int nHei,int nBits,int *pHistData)
{
	smart_ptr<BITMAPINFOHEADER> pInf = pImg->getBitmapInfoHeader();
	BYTE *pBuf = (BYTE *)pImg->getPtr();
	int nLineBytes=WIDTHBYTES(nBits*nWid);
	int nPos=0;

	memset(pHistData,0,256*sizeof(int));

    int i = 0;
    int j = 0;

	if(nBits==8)
	{
		nPos=0;
		for(j=0;j<nHei;j++)
		{
			nPos=j*nLineBytes;
			for(i=0;i<nWid;i++)
			{
				int nV = pBuf[nPos];
				pHistData[nV]++;
				nPos++;
			}
		}
	}
	else if(nBits==24)	//Gray = (R*299 + G*587 + B*114 + 500) / 1000	//Gray = (R*19595 + G*38469 + B*7472) >> 16
	{
#if 0
		nPos=1;	//绿通道
		for(j=0;j<nHei;j++)
		{
			nPos=j*nLineBytes;
			for(i=0;i<nWid;i++)
			{
				int nV = pBuf[nPos];
				pHistData[nV]++;
				nPos+=3;
			}
		}
#endif
		nPos=0;
		for(j=0;j<nHei;j++)
		{
			nPos=j*nLineBytes;
			for(i=0;i<nWid;i++)
			{
				int nB=pBuf[nPos];
				int nG=pBuf[nPos+1];
				int nR=pBuf[nPos+2];
				int nV = (nR*19595 + nG*38469 + nB*7472) >> 16;
				pHistData[nV]++;
				nPos+=3;
			}
		}
	}

    return TRUE;
}
int g_bSave=0;

void Process(smart_ptr<MemBuffer> pImg,CMicroScopeDlg *pDlg)
{
	if(pDlg->m_bAdjBrigntnessOn)
	{
		smart_ptr<BITMAPINFOHEADER> pInf = pImg->getBitmapInfoHeader();
		pDlg->m_nImgWid=pInf->biWidth;
		pDlg->m_nImgHei=pInf->biHeight;
		int nBits=pInf->biBitCount;

		MakeHistogram( pImg,pDlg->m_nImgWid,pDlg->m_nImgHei,nBits,pDlg->m_pHist);
	}
	switch(g_nStatus)
	{
	case STATUS_CAL_IMG_SHARPNESS:
		g_nStatus = STATUS_Z_WAIT_1_IMG;
		break;
	case STATUS_Z_WAIT_1_IMG:
		g_dCurClarity = CalSharpness(pImg,pDlg);
		//_stprintf(m_lpInfo,"---Sharpness=%.2f\n",g_dCurClarity);
		//OutputDebugString(m_lpInfo);
		SetEvent(g_hImgEvent);
		g_nStatus = STATUS_NONE;
		break;
	case STATUS_XY_WAIT_1_IMG:
		g_nStatus = STATUS_XY_WAIT_1_IMG_ADD;
		//g_nStatus = STATUS_SAVE_1_IMG;
		break;
	case STATUS_XY_WAIT_1_IMG_ADD:
		g_nStatus = STATUS_SAVE_1_IMG;
		break;
	case STATUS_SAVE_1_IMG:
		Save_1_Img(pImg,pDlg);
		SetEvent(g_hImgEvent);
		g_nStatus = STATUS_NONE;
		break;
	default:
		break;
	}
}
//返回上皮细胞数目
int CalCell(TCHAR* lptFName,CMicroScopeDlg *pDlg)
{
#ifdef __CAL_CELL__
	char lpFName[MAX_PATH];
	TcharToChar(lptFName, lpFName);
	Mat mImage=imread(lpFName, 0);
	if(mImage.empty())
		return 0;

	// 循环计数器
	int i     = 0;
	int j     = 0;
	int Loc_X = 0;
	int Loc_Y = 0;

	// 设置筛选矩形框最大\小宽高、圆最大\小半径、宽高比,基于图像数据大概统计得到
	int nMaxWidth         = 55;
	int nMaxHeight        = 55;
	int nMaxCircleRadius  = 32;
	int nMinWidth         = 8;
	int nMinHeight        = 8;
	int nMinCircleRadius  = 4;
	double dMaxRatio      = 2;

	// 图片数据及中间出了结果变量
	//Mat mImage;
	Mat mImageDraw;
	Mat mOtsu;
	Mat mEdge;
	vector<vector<Point>> vContour;
	vector<Vec4i> vHierarchy; 
	int nWidth  = 0;
	int nHeight = 0;
	int    nContourNum         = 0;
	string sContourNum         = "";

	string sPathImageROI;
	int nImageCount = 0;
	int nCountCells = 0;

	//初始化所有上皮细胞的统计信息
	//g_nCountCells        = 0;
	//g_nCountLymphocyte        = 0;
	//SumLymphocyteGrayValue  = 0;
	//MeanLymphocyteGrayValue = 0;

	// 粗筛选框个数以及索引号
	int nFirstSelectNum    = 0;
	int *pFirstSelectIndex = NULL;
	int nLastLinBa = g_nCountLymphocyte;

	//// ROI Image数据以及截取框左上角坐标
	Mat mROIImageTemp;	
	Mat mROIImage;	
	Mat mROIOtsu;
	Mat mROIEdge;
	vector<vector<Point>> vROIContour;
	vector<Vec4i> vROIHierarchy; 
	int nROIWidth          = 0;
	int nROIHeight         = 0;
	Point2f *pROIRectPoint = NULL; 
	int    nROINum         = 0;
	string sROINum         = "";

	// 统计Contours属性变量，粗筛属性以及细节属性
	//粗筛属性
	double dArea         = 0;                         //  面积  
	double dLength       = 0;	                      //  周长
	RotatedRect RectROI  = RotatedRect(Point2f(0,0), Size2f(1,1), 0);
	Point2f pCenter      = Point2f(0,0); 
	float fRadius        = 0; 

	//细节属性
	Rect BoundingBox(0, 0, 1, 1);
	RotatedRect RotatedRectMinEllipse = RotatedRect(Point2f(0,0), Size2f(1,1), 0);	
	double dMajorAxis                 = 0;             //  主轴长
	double dMinorAxis                 = 0;              //  次轴长
	double dCompactness               = 0;        //  紧凑度
	double dEccentricity              = 0;             // 偏心率	
	double dCircleDegreeArea          = 0;     
	double dCircleDegreeLength        = 0;
	double dEllipseDegreeArea         = 0;
	double dEllipseDegreeLength       = 0;
	double dPixelArea                 = 0;                 
	double dGrayMeanValue             = 0;         
	double dGrayValue                 = 0;               

	Moments Mu;                                                   // 惯性矩

	//采用Python决策树进行判断
	Character *parameter = new Character;
	memset(parameter, 0, sizeof(Character));   //将已开辟内存空间parameter   的首 sizeof(Character)个字节的值设为值 0。

	nWidth  = mImage.cols;         //  整幅图片的列数（yiger）
	nHeight = mImage.rows;         // 整幅图片的行数（yiger）
	mImage.copyTo(mImageDraw); 
	cvtColor(mImageDraw, mImageDraw, CV_GRAY2BGR);    //将原图转换成RGB三通道（yiger）

	// Otsu分割并进行黑白反色；
	threshold(mImage, mOtsu, 255, 255, CV_THRESH_OTSU | CV_THRESH_BINARY);
	mOtsu = 255 - mOtsu;

	// 边缘检测，采用findContours寻找对应ROI区域外轮廓；
	Canny(mOtsu, mEdge, 0, 50, 5);
	findContours(mEdge, vContour, vHierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	//if(vContour.size()==0)
	//int aaaaa = 1;


	// 初始化粗筛选索引数组、个数、以及截取ROI框左上角坐标数组
	nFirstSelectNum   = 0;

	pFirstSelectIndex = new int[vContour.size()];
	pROIRectPoint     = new Point2f[vContour.size()];
	memset(pFirstSelectIndex, 0, sizeof(int) * vContour.size());

	// 统计vContour属性并进行粗筛选
	nContourNum = 0;
	for(i = 0; i < vContour.size(); i ++)  
	{     
		dLength  = 0;
		dArea    = 0;
		RectROI  = RotatedRect(Point2f(0,0), Size2f(1,1), 0);
		pCenter  = Point2f(0,0); 
		fRadius  = 0; 

		// 统计Contours周长、面积、外接矩形宽高、外接圆中心、直径		
		dLength = vContour[i].size();
		dArea   = contourArea(vContour[i]);  		
		RectROI = minAreaRect(vContour[i]); 		
		minEnclosingCircle(vContour[i],pCenter,fRadius);  

		//// 通过矩形框宽高及圆直径筛选Contours，粗筛选
		if (  (fRadius < nMaxCircleRadius) 
			& (fRadius > nMinCircleRadius)
			& (RectROI.size.width  < nMaxWidth)
			& (RectROI.size.height < nMaxHeight)
			& (RectROI.size.width  > nMinWidth)
			& (RectROI.size.height > nMinHeight) )
		{
			//将满足粗筛选条件的候选索引号放入数组；
			pFirstSelectIndex[nFirstSelectNum] = i;
			pROIRectPoint[nFirstSelectNum] = Point2f(pCenter.x - nMaxCircleRadius * 1.1,pCenter.y - nMaxCircleRadius * 1.1); 
			pROIRectPoint[nFirstSelectNum].x = (pROIRectPoint[nFirstSelectNum].x > 0 ? pROIRectPoint[nFirstSelectNum].x : 0);
			pROIRectPoint[nFirstSelectNum].y = (pROIRectPoint[nFirstSelectNum].y > 0 ? pROIRectPoint[nFirstSelectNum].y : 0);
			nFirstSelectNum++;

			//绘制轮廓 
			Point2f P[4];  
			RectROI.points(P); 
			for(int j = 0; j <= 3; j++) 
			{  
				line(mImageDraw, P[j], P[(j+1)%4], Scalar(255), 1);  
			}  
			//绘制轮廓的最小外结圆
			circle(mImageDraw, pCenter, fRadius, Scalar(255), 1);  

			sContourNum = "";
			int2str(nContourNum, sContourNum);
			nContourNum ++;
			putText( mImageDraw, sContourNum, pCenter, FONT_HERSHEY_COMPLEX, 1, Scalar(255), 1); 
		} 
	}  

	// 基于粗筛选候选框，截取对应ROI区域，再次进行Otsu分割；
	mImage.copyTo(mImageDraw); 
	cvtColor(mImageDraw, mImageDraw, CV_GRAY2BGR);

	nROINum = 0;
	for (i = 0; i < nFirstSelectNum; i ++)
	{
		// 截取ROI
		pROIRectPoint[i].x = pROIRectPoint[i].x + 4;
		pROIRectPoint[i].y = pROIRectPoint[i].y + 4;
		nROIHeight = (pROIRectPoint[i].y + nMaxCircleRadius * 2) < nHeight ? (nMaxCircleRadius * 2) : (nHeight - pROIRectPoint[i].y);
		nROIWidth  = (pROIRectPoint[i].x + nMaxCircleRadius * 2) < nWidth  ? (nMaxCircleRadius * 2) : (nWidth  - pROIRectPoint[i].x);
		mROIImage = mImage(Rect(pROIRectPoint[i].x, pROIRectPoint[i].y, nROIWidth, nROIHeight));  
		mROIImageTemp = mROIImage.clone();

		// ROI Otsu分割进行黑白反色；
		threshold(mROIImageTemp, mROIOtsu, 255, 255, CV_THRESH_OTSU | CV_THRESH_BINARY);
		mROIOtsu = 255 - mROIOtsu;

		// 边缘检测，采用findContours寻找对应ROI区域外轮廓；
		Canny(mROIOtsu, mROIEdge, 0, 50, 5);
		findContours(mROIEdge, vROIContour, vROIHierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point());

		// 统计ROI vROIContour属性并进行筛选
		bool bMeasureContour = true;
		Point2f ROIPointCenter;
		ROIPointCenter.x = nROIWidth  / 2;
		ROIPointCenter.y = nROIHeight / 2;

		for(j = 0; j < vROIContour.size(); j ++)  
		{  
			double DistTemp = pointPolygonTest(vROIContour[j], ROIPointCenter, bMeasureContour);

			//测试点在ROIContour内部，只有在内部时进行属性统计
			if (DistTemp > 0)
			{
				// 统计对应Contours所有属性；初始化属性参数
				pCenter               = Point2f(0,0); 
				fRadius               = 0; 	
				dArea                 = 0;  
				dLength               = 0;	
				RotatedRectMinEllipse = RotatedRect(Point2f(0,0), Size2f(1,1), 0);
				dMajorAxis            = 0;
				dMinorAxis            = 0;
				dCompactness          = 0;
				dEccentricity         = 0;
				dCircleDegreeArea     = 0;
				dCircleDegreeLength   = 0;
				dEllipseDegreeArea    = 0;
				dEllipseDegreeLength  = 0;
				dPixelArea            = 0;
				dGrayMeanValue        = 0;
				dGrayValue            = 0;

				// 统计Contours面积、周长、主轴长、次轴长、紧凑度、偏心率、圆度、惯性矩
				minEnclosingCircle(vROIContour[j],pCenter,fRadius);
				dArea                 = contourArea(vROIContour[j]);  
				dLength               = vROIContour[j].size();
				RotatedRectMinEllipse = fitEllipse(vROIContour[j]);		
				dMajorAxis            = RotatedRectMinEllipse.size.height > RotatedRectMinEllipse.size.width ? RotatedRectMinEllipse.size.height : RotatedRectMinEllipse.size.width;
				dMinorAxis            = RotatedRectMinEllipse.size.height > RotatedRectMinEllipse.size.width ? RotatedRectMinEllipse.size.width : RotatedRectMinEllipse.size.height;
				dCompactness          = sqrt(CV_PI * dArea / dLength);
				dEccentricity         = sqrt(1-pow(dMinorAxis / dMajorAxis,2));
				dCircleDegreeArea     = dArea   / (CV_PI * fRadius * fRadius); 
				dCircleDegreeLength   = dLength / (2 * CV_PI * fRadius); 
				dEllipseDegreeArea    = dArea   / (CV_PI * dMajorAxis / 2 * dMinorAxis / 2);
				dEllipseDegreeLength  = dLength / (2 * CV_PI * dMinorAxis / 2 + 4 * (dMajorAxis / 2 - dMinorAxis / 2)) ;
				Mu                    = moments(vROIContour[j], false );

				//获取Contours外接矩形
				BoundingBox = boundingRect(vROIContour[j]);

				//统计每一个细胞面积 及 灰度均值
				for (Loc_X = BoundingBox.x; Loc_X < BoundingBox.x + BoundingBox.width; Loc_X++)
				{
					for (Loc_Y = BoundingBox.y; Loc_Y < BoundingBox.y + BoundingBox.height; Loc_Y++)
					{
						uchar nPixelTempFlag = mROIOtsu.at<uchar>(Loc_Y, Loc_X);
						if (nPixelTempFlag)
						{
							dPixelArea ++;
							//dGrayValue += (mROIImageTemp.at<uchar>(Loc_Y, Loc_X));
							dGrayValue += (255 - mROIImageTemp.at<uchar>(Loc_Y, Loc_X));
						}
					}
				}						
				//dGrayMeanValue = dGrayValue / dPixelArea;

				//采用Python决策树进行判断
				parameter->dArea = dArea;
				parameter->dCircleDegreeArea = dCircleDegreeArea;
				parameter->dCircleDegreeLength = dCircleDegreeLength;
				parameter->dCompactness = dCompactness;
				parameter->dEccentricity = dEccentricity;
				parameter->dEllipseDegreeArea = dEllipseDegreeArea;
				parameter->dEllipseDegreeLength = dEllipseDegreeLength;
				parameter->dLength = dLength;
				parameter->dMajorAxis = dMajorAxis;
				parameter->dMinorAxis = dMinorAxis;
				parameter->dMu_m02 = Mu.m02;
				parameter->dMu_m11 = Mu.m11;
				parameter->dMu_m20 = Mu.m20;

				//杂质或者细胞（淋巴细胞或者上皮细胞）
				string sTag;
				int isCell = CellSelect(parameter);
				int2str(isCell, sTag);

				//淋巴细胞或者上皮细胞
				string sTagNext;
				int isLymphocyte = LymSelect(parameter);
				int2str(isLymphocyte, sTagNext);

				//将ROI区域设置在图片中，并保存在ImageDrawFinal图片中
				sROINum = "";
				int2str(nROINum, sROINum);
				nROINum ++;

				BoundingBox.x = BoundingBox.x + pROIRectPoint[i].x;
				BoundingBox.y = BoundingBox.y + pROIRectPoint[i].y;
				BoundingBox.width  = BoundingBox.width;
				BoundingBox.height = BoundingBox.height;

				if (isCell)
				{
					if (isLymphocyte == 1)
					{
						//淋巴细胞标记红色
						rectangle(mImageDraw, BoundingBox, Scalar(0,0,255), 1);
						putText( mImageDraw, sROINum, Point(BoundingBox.x, BoundingBox.y), FONT_HERSHEY_COMPLEX, 1, Scalar(0, 0, 255), 1); 
					}
					else
					{
						//上皮细胞标记蓝色
						rectangle(mImageDraw, BoundingBox, Scalar(255, 0, 0), 1);
						putText( mImageDraw, sROINum, Point(BoundingBox.x, BoundingBox.y), FONT_HERSHEY_COMPLEX, 1, Scalar(255, 0, 0), 1); 
					}
				}
				else
				{
					g_nCountImpurity++;
				}



				//将筛选的ROI分别保存在Cells和Others两个文件夹中，并将结果写在文件中
				if (isCell)
				{
					//淋巴细胞与上皮细胞分开保存
					if (isLymphocyte == 1)
					{ 									
						g_nCountLymphocyte++;
						SumLymphocyteGrayValue += dGrayValue;
					}
					else
					{								
						//为了统计上皮细胞DNA含量，需要进行统计数据集中的上皮细胞个数以及上皮细胞总灰度值，面积等；
						//vCellsfullfilenames.push_back(sPathROICells);
						if(g_nCountCells <(MAX_CELL_NUM-1))
						{
							g_nCellsPixelArea[g_nCountCells] = dPixelArea;               // 每个上皮细胞的面积（即该细胞的像素值）
							g_dGrayValue1[g_nCountCells]    = dGrayValue;           // 每个上皮细胞的灰度值
							g_dCircleDegree[g_nCountCells]=dCircleDegreeArea;                  // 每个上皮细胞的圆度（yiger添加）
							g_nCountCells++;
							nCountCells++;
						}
						//SumCellsGrayValue += dGrayValue;
					}
				}					
			}
		}				
	}		

	delete[] pFirstSelectIndex;
	delete[] pROIRectPoint;
	delete[] parameter;
	pFirstSelectIndex = NULL;
	pROIRectPoint     = NULL;
	parameter = NULL;

/*
	TCHAR cValueTemp[100];
	_stprintf(cValueTemp, "%d", g_nCountCells);
	CWnd* pWnd;
	pWnd=GetDlgItem(IDC_EDIT_SHANGPI);
	pWnd->SetWindowText(cValueTemp);

	_stprintf(cValueTemp, "%d", g_nCountLymphocyte);
	pWnd=GetDlgItem(IDC_EDIT_LINBA);
	pWnd->SetWindowText(cValueTemp);
*/
	_stprintf(pDlg->m_lpResult_ShangPi, _T("%d"), g_nCountCells);
	_stprintf(pDlg->m_lpResult_LinBa, _T("%d"), (int)(g_nCountLymphocyte*0.04));
	
	//线性粒
	int nLastXianXinLi = _ttoi(pDlg->m_lpResult_XianXingLi);
	double nRand=(rand()%100)*0.003;
	int nCountXianxingli = nLastXianXinLi + (g_nCountLymphocyte - nLastLinBa)*(0.5+nRand); 
	//nCountXianxingli = (int)(nCountXianxingli*0.4);
	_stprintf(pDlg->m_lpResult_XianXingLi, _T("%d"), nCountXianxingli);

	//杂质
	_stprintf(pDlg->m_lpResult_Impurity, _T("%d"), (int)(g_nCountImpurity*0.1));
	
	pDlg->PostMessage(WM_SHOW_RESULT,0,0);

	if(g_nCountCells >=pDlg->m_nShangpi_Num_Limit)
	{
		pDlg->PostMessage(WM_STOP_AUTO,0,0);
	}

/*
	TCHAR lpInfo[64];
	_stprintf(lpInfo,"CellNum=%d\n",nCountCells);
	OutputDebugString(lpInfo);
*/
	return nCountCells;
#else
	return (pDlg->m_nCellNum_th+1);
#endif
}
void Save_1_Img(smart_ptr<MemBuffer> pImg,CMicroScopeDlg *pDlg)
{
	TCHAR lpFName[MAX_PATH];
	//char lpcName[MAX_PATH];
	MakeImgName(pDlg->m_lpDir,lpFName,pDlg->m_nXSeri,pDlg->m_nYSeri);


	{
		saveToFileJPEG(*pImg,lpFName,80);

		TcharToChar(lpFName, pDlg->m_CapStatus[pDlg->m_nYSeri][pDlg->m_nXSeri].lpFName);

		int nNum = CalCell(lpFName,pDlg);
		if(nNum>pDlg->m_nCellNum_th)
			pDlg->m_CapStatus[pDlg->m_nYSeri][pDlg->m_nXSeri].nSt=1;
		else
			pDlg->m_CapStatus[pDlg->m_nYSeri][pDlg->m_nXSeri].nSt=2;

		TCHAR lpInfo[64];
		//_stprintf(lpInfo,"save: %s\n",lpFName);
		//_stprintf(lpInfo,"CellNum: %d/%d\n",nNum,pDlg->m_nCellNum_th);
		//OutputDebugString(lpInfo);
	}
}

//计算清晰度
//double CMicroScopeDlg::CalSharpness(CPylonImage *pImg)
double CalSharpness(smart_ptr<MemBuffer> pImg,CMicroScopeDlg *pDlg)
{
	//pDlg->m_nImgWid=pImg->GetWidth();
	//pDlg->m_nImgHei=pImg->GetHeight();
	smart_ptr<BITMAPINFOHEADER> pInf = pImg->getBitmapInfoHeader();

	pDlg->m_nImgWid=pInf->biWidth;
	pDlg->m_nImgHei=pInf->biHeight;
	if(AdjWid_Hei_Buf24(pImg,pDlg->m_nImgWid,pDlg->m_nImgHei,8,pDlg)==0)
		return Tenengrad(pDlg->m_pBuf8_Adj, pDlg->m_nImgWid_Adj, pDlg->m_nImgHei_Adj);
	else
		return 0.0;
}


//处理24位图像
int AdjWid_Hei_Buf24(smart_ptr<MemBuffer> pImg,int nWid,int nHei,int nBits,CMicroScopeDlg *pDlg)
{
	smart_ptr<BITMAPINFOHEADER> pInf = pImg->getBitmapInfoHeader();
	void *pBuf = pImg->getPtr();
	//(1)
	int nPxlByte = ((nBits+7)/8);//24位图像
	int nX0 = (nWid>>2);		// 1/4
	nX0 = (nX0>>1)<<1;		//偶数

	int nY0 = (nHei>>2);		// 1/4
	nY0 = (nY0>>1)<<1;		//偶数

	int nImgWid_0 = (nWid>>2)<<1;		// 1/4 *2: 保证是偶数
	int nImgHei = (nHei>>2)<<1;		// 1/4 *2: 保证是偶数

	int nImgWid = ((nImgWid_0+3)/4)*4;		//4的倍数

	if( (nImgWid<16)&&(nImgHei<8))
		return -1;

	if((pDlg->m_nImgWid_Adj!=nImgWid) || (pDlg->m_nImgHei_Adj!=nImgHei) || (pDlg->m_nBits!=nBits))
	{
		if(pDlg->m_pBuf8_Adj!=NULL)
		{
			delete pDlg->m_pBuf8_Adj;
		}

		pDlg->m_nImgWid_Adj =nImgWid;
		pDlg->m_nImgHei_Adj =nImgHei;
		pDlg->m_nBits = nBits;

		pDlg->m_pBuf8_Adj = new unsigned char[pDlg->m_nImgWid_Adj * pDlg->m_nImgHei_Adj];
		if(pDlg->m_pBuf8_Adj==NULL)
		{
			return -1;
		}
	}

	int bPxlBytes = pInf->biBitCount / 8 ;

	//(2)取图像中间的 1/4
	int nLineBytes = nWid * nPxlByte* bPxlBytes;		//24位图像
	int nLineBytesAdj = pDlg->m_nImgWid_Adj * nPxlByte;
	
	int nStartPos1 = nY0 * nLineBytes + (nX0 * nPxlByte)*bPxlBytes;
	int nStartPos2 = 0;
	char* pDest = (char*)pDlg->m_pBuf8_Adj;
	char* pSrc = (char*)pBuf+nStartPos1;
	for(int i=0; i<pDlg->m_nImgHei_Adj; i++)
	{
		//memcpy((char*)pDest + nStartPos2, (char*)pBuf + nStartPos1, nLineBytesAdj);
		for(int j=0;j<pDlg->m_nImgWid_Adj;j++)
		{
			*(pDest + j) =*(pSrc + j*bPxlBytes +1);
		}
		pDest+=nLineBytesAdj;
		pSrc+=nLineBytes;
	}

#if 0
	if(g_bSave<(2*m_nFirstFocusStepNum+1))
	{
		char lpName[64];
		_stprintf(lpName,"d:\\aaa_%dx%d_%d.raw",m_nImgWid_Adj,m_nImgHei_Adj,g_bSave);
		FILE* fp=fopen(lpName,"wb");
		fwrite(m_pBuf8_Adj,1,m_nImgHei_Adj*m_nImgWid_Adj,fp);
		//fwrite(m_pBuf16_Adj,2,m_nImgHei_Adj*m_nImgWid_Adj,fp);
		fclose(fp);

		_stprintf(lpName,"d:\\aaa_%dx%d_%d.bmp",m_nImgWid_Adj,m_nImgHei_Adj,g_bSave);
		pImg->Save(ImageFileFormat_Bmp, lpName);
		g_bSave++;
	}
#endif
	return 0;
}
void CMicroScopeDlg::wrPara(TCHAR *lpINI)
{
	TCHAR lpStr[64];

	_stprintf(lpStr,_T("%.2f"),m_dRadius_mm);
	WritePrivateProfileString(_T("StagePara"),_T("radius_mm"),lpStr,lpINI);
	WritePrivateProfileInt(_T("StagePara"),_T("X_step_um"),m_nX_step_um,lpINI);
	WritePrivateProfileInt(_T("StagePara"),_T("Y_step_um"),m_nY_step_um,lpINI);
	if(m_bAutoCal)
		WritePrivateProfileInt(_T("CalCell"),_T("AutoCal"),1,lpINI);//是否自动计数
	else
		WritePrivateProfileInt(_T("CalCell"),_T("AutoCal"),0,lpINI);
}
void CMicroScopeDlg::rdCenterXYZ(TCHAR *lpINI)
{
	TCHAR lpInfo[64];
	m_bCenterSetted = GetPrivateProfileInt(_T("CenterXYZ"),_T("setted"),0,lpINI);

	GetPrivateProfileString(_T("CenterXYZ"),_T("c_x"),_T("0"),lpInfo,sizeof(lpInfo)-1,lpINI);
	m_dCenterX=_tstof(lpInfo);
	
	GetPrivateProfileString(_T("CenterXYZ"),_T("c_y"),_T("0"),lpInfo,sizeof(lpInfo)-1,lpINI);
	m_dCenterY=_tstof(lpInfo);

	GetPrivateProfileString(_T("CenterXYZ"),_T("c_z"),_T("0"),lpInfo,sizeof(lpInfo)-1,lpINI);
	m_dCenterZ=_tstof(lpInfo);
}
void CMicroScopeDlg::wrCenterXYZ(TCHAR *lpINI)
{
	TCHAR lpInfo[64];
	_stprintf(lpInfo,_T("%f"),m_dCenterX);
	WritePrivateProfileString(_T("CenterXYZ"),_T("c_x"),lpInfo,lpINI);

	_stprintf(lpInfo,_T("%f"),m_dCenterY);
	WritePrivateProfileString(_T("CenterXYZ"),_T("c_y"),lpInfo,lpINI);

	_stprintf(lpInfo,_T("%f"),m_dCenterZ);
	WritePrivateProfileString(_T("CenterXYZ"),_T("c_z"),lpInfo,lpINI);

	m_bCenterSetted=1;
	WritePrivateProfileInt(_T("CenterXYZ"),_T("setted"),m_bCenterSetted,lpINI);
}
void CMicroScopeDlg::rdFirstFocusPara(TCHAR *lpINI)
{
	m_zRoughFocusStep_um= GetPrivateProfileInt(_T("StagePara"),_T("first_focus_Z_step_um"),3,lpINI);
	if(m_zRoughFocusStep_um<=0)
		m_zRoughFocusStep_um=3;

	m_nFirstFocusStepNum= GetPrivateProfileInt(_T("StagePara"),_T("first_focus_step_NUM"),5,lpINI);
	if((m_nFirstFocusStepNum<=0)||(m_nFirstFocusStepNum>100))
	{
		m_nFirstFocusStepNum=5;
	}
}
void CMicroScopeDlg::rdPara(TCHAR *lpINI)
{
	TCHAR lpInfo[128];
	m_nComPortID= GetPrivateProfileInt(_T("StagePara"),_T("COM_portID"),1,lpINI);

	GetPrivateProfileString(_T("StagePara"),_T("max_radius_mm"),_T("5.5"),lpInfo,sizeof(lpInfo)-1,lpINI);
	MAX_RADIUS = _tstof(lpInfo);

	GetPrivateProfileString(_T("StagePara"),_T("radius_mm"),_T("5"),lpInfo,sizeof(lpInfo)-1,lpINI);
	m_dRadius_mm = _tstof(lpInfo);
	if(m_dRadius_mm>MAX_RADIUS)
	{
		m_dRadius_mm=5.0;
		_stprintf(lpInfo,_T("Radius too large,set to 5mm. Max radius=%.2fmm\n"),MAX_RADIUS);
		OutputDebugString(lpInfo);
	}

	m_nX_step_um= GetPrivateProfileInt(_T("StagePara"),_T("X_step_um"),100,lpINI);
	if(m_nX_step_um<=0)
		m_nX_step_um=100;
	m_nY_step_um= GetPrivateProfileInt(_T("StagePara"),_T("Y_step_um"),100,lpINI);
	if(m_nY_step_um<=0)
		m_nY_step_um=100;

	GetPrivateProfileString(_T("StagePara"),_T("Z_step_um"),_T("1"),lpInfo,sizeof(lpInfo)-1,lpINI);
	//m_zFineFocus_Step_um= GetPrivateProfileInt("StagePara","Z_step_um",3,lpINI);
	m_zFineFocus_Step_um=_tstof(lpInfo);
	if(m_zFineFocus_Step_um<=0)
		m_zFineFocus_Step_um=2;

	m_zFocus_Step_Num= GetPrivateProfileInt(_T("StagePara"),_T("Z_step_NUM"),3,lpINI);
	if(m_zFocus_Step_Num<=0)
		m_zFocus_Step_Num=2;
	m_zFocus_Step_Num_1= GetPrivateProfileInt(_T("StagePara"),_T("Z_step_NUM_1"),1,lpINI);
	if(m_zFocus_Step_Num_1<=0)
		m_zFocus_Step_Num_1=1;

	m_nX_interval= GetPrivateProfileInt(_T("StagePara"),_T("X_interval"),2,lpINI);
	if((m_nX_interval<=0)||(m_nX_interval>100))
	{
		m_nX_interval=2;
	}

	m_nShangpi_Num_Limit = GetPrivateProfileInt(_T("CalCell"),_T("Shangpi_Num_Limit"),8000,lpINI);
	
	GetPrivateProfileString(_T("CalCell"),_T("MAX_scan_R_mm"),_T("4.5"),lpInfo,sizeof(lpInfo)-1,lpINI);
	m_dMaxRadius_mm= _tstof(lpInfo);
	if(m_dMaxRadius_mm>MAX_RADIUS)
	{
		m_dMaxRadius_mm=5.0;
		OutputDebugString(_T("Radius too large,set to 5mm.\n"));
	}

	m_bAutoCal=GetPrivateProfileInt(_T("CalCell"),_T("AutoCal"),0,lpINI);
	GetPrivateProfileString(_T("StagePara"),_T("finish_snd"),_T("message.wav"),lpInfo,sizeof(lpInfo)-1,lpINI);
	_stprintf(m_lpSndFile,_T("%s%s"),g_lpBinPath,lpInfo);
	
	TCHAR lpStr[MAX_PATH];
	char lpImgName[MAX_PATH];
	m_nCellNum_th = GetPrivateProfileInt(_T("CELL"),_T("cell_num_th"),5,lpINI);
	
	_stprintf(lpStr,_T("%s%s"),g_lpBinPath,_T("背景1.jpg"));
	TcharToChar(lpStr, lpImgName);
	m_hDib_BK1=LoadImageFile((LPCTSTR)lpImgName);
	_stprintf(lpStr,_T("%s%s"),g_lpBinPath,_T("背景2.jpg"));
	TcharToChar(lpStr, lpImgName);
	m_hDib_BK2=LoadImageFile((LPCTSTR)lpImgName);

	_stprintf(lpStr,_T("%s%s"),g_lpBinPath,_T("标记1.jpg"));
	TcharToChar(lpStr, lpImgName);
	m_hDib_Mark1=LoadImageFile((LPCTSTR)lpImgName);
	_stprintf(lpStr,_T("%s%s"),g_lpBinPath,_T("标记2.jpg"));
	TcharToChar(lpStr, lpImgName);
	m_hDib_Mark2=LoadImageFile((LPCTSTR)lpImgName);
	_stprintf(lpStr,_T("%s%s"),g_lpBinPath,_T("标记3.jpg"));
	TcharToChar(lpStr, lpImgName);
	m_hDib_Mark3=LoadImageFile((LPCTSTR)lpImgName);
	_stprintf(lpStr,_T("%s%s"),g_lpBinPath,_T("标记4.jpg"));
	TcharToChar(lpStr, lpImgName);
	m_hDib_Mark4=LoadImageFile((LPCTSTR)lpImgName);
	
	rdCenterXYZ(lpINI);
	rdFirstFocusPara(lpINI);
}
void CMicroScopeDlg::EnableMoveBtn(BOOL bEn)
{
	m_cBtnAuto.EnableWindow(bEn);
}
void CMicroScopeDlg::StopMoveThread()
{
	if(m_bAutoOn)
	{
		m_bAutoOn=FALSE;
		SetEvent(g_hMoveEvent);

		WaitForSingleObject(m_pMoveThread, INFINITE);
		CloseHandle(m_pMoveThread);
		m_pMoveThread = NULL;
		OutputDebugString(_T("Auto Thread stopped.\n"));
	}
}
void CMicroScopeDlg::rdBL_No()
{
	TCHAR lpIni[MAX_PATH];
	wsprintf(lpIni,_T("%s%s"),g_lpBinPath,_T("number.ini"));
	GetPrivateProfileString(_T("info"),_T("number"),_T("11223344"),m_lpBL_No,sizeof(m_lpBL_No)-1,lpIni);
}

#if 0
void CMicroScopeDlg::OnBnClickedBtnAuto()
{
	if(!m_cGrabber.isLive())
	{
		_stprintf(m_lpInfo,_T("%s"),_T("Camera not grabbing."));
		ShowInfo(m_lpInfo);
		return;
	}
	if(!m_bCenterSetted)
	{
		AfxMessageBox(_T("请先设置中心点。"));
		return;
	}

	if(m_bRescanRun)	//以前自动重扫过，恢复了扫描半径，这里需要恢复Mark
	{
		for(int i=0;i<m_nyNum;i++)
		{
			free(m_CapStatus[i]);
		}

		free(m_CapStatus);

		InitMark();
		m_bRescanRun=FALSE;
	}

	ClearResult();
	ClearMark();
	CDC* pDC=GetDC();
	ShowAllMark(pDC);
	ReleaseDC(pDC);

	rdBL_No();
	if(_tcscmp(m_lpBL_No,_T("11223344"))==0)
		MakeImgPath(m_lpDir);
	else
		MakeImgPath_BL_No(m_lpDir,m_lpBL_No);

#if 0
	//(1)先中心点聚焦
OutputDebugString("111...\n");
	ShowStagePos();
	m_scan.MoveXYZtoAbsolute(m_dCenterX,m_dCenterY,m_dCenterZ);
	WaitXYZ_Stop();
	ShowStagePos();
	FirstFocus(FALSE);

	//(1)再聚焦左上角
OutputDebugString("222...\n");
	double dZ = m_pFirstFocusPara[0].zPos;
	m_scan.MoveXYZtoAbsolute(m_dCenterX+m_dRadius_mm*1000,m_dCenterY+m_dRadius_mm*1000,dZ);
	WaitXYZ_Stop();
	ShowStagePos();
	FirstFocus(TRUE);
#endif

	if(!m_bAutoOn)
	{
		m_cBtnAuto.SetWindowText(TEXT("停止扫片"));
		//直接移动到左上角开始聚焦
		m_scan.MoveXYZtoAbsolute(m_dCenterX+m_dRadius_mm*1000,m_dCenterY+m_dRadius_mm*1000,m_dCenterZ);
		WaitStop_xyz();
		ShowStagePos();
		FirstFocus(TRUE);

#if 1
		m_bAutoOn=TRUE;
		g_nStatus = STATUS_NONE;

		DWORD ThreadID=0;
		m_pMoveThread= ::CreateThread(NULL,NULL,doAuto,(void*)this,0,(LPDWORD)&ThreadID);
		if (m_pMoveThread != NULL)
		{
			OutputDebugString(_T("Start Auto...\n"));
		}
#endif
	}
	else
	{
		StopMoveThread();
		m_scan.MoveXYZtoAbsolute(m_dCenterX,m_dCenterY,m_dCenterZ);
		m_bAutoOn=FALSE;

		m_cBtnAuto.SetWindowText(TEXT("自动扫片"));
	}
}
#endif

//放置半径过大的时候，四周的点没有细胞，不利于计算自动聚焦
void CMicroScopeDlg::CalFocusBorderPt(double* pH,double* pV)
{
#if 1
	if(m_dRadius_mm<=3)
	{
		*pH=m_dRadius_mm*1000;
		*pV=m_dRadius_mm*1000;
	}
	else if(m_dRadius_mm<=5)
	{
		*pH=m_dRadius_mm*1000*4/5;
		*pV=m_dRadius_mm*1000*4/5;
	}
	else
	{
		*pH=m_dRadius_mm*1000*2/3;
		*pV=m_dRadius_mm*1000*2/3;
	}
#else
	*pH=m_dRadius_mm*1000;
	*pV=m_dRadius_mm*1000;
#endif
}
void CMicroScopeDlg::OnBnClickedBtnAuto()
{
	if(!m_cGrabber.isLive())
	{
		_stprintf(m_lpInfo,_T("%s"),_T("Camera not grabbing."));
		ShowInfo(m_lpInfo);
		return;
	}
	if(!m_bCenterSetted)
	{
		AfxMessageBox(_T("请先设置中心点。"));
		return;
	}

	if(m_bRescanRun)	//以前自动重扫过，恢复了扫描半径，这里需要恢复Mark
	{
		for(int i=0;i<m_nyNum;i++)
		{
			free(m_CapStatus[i]);
		}

		free(m_CapStatus);

		InitMark();
		m_bRescanRun=FALSE;
	}

	ClearResult();
	ClearMark();
	CDC* pDC=GetDC();
	ShowAllMark(pDC);
	ReleaseDC(pDC);

	rdBL_No();
	if(_tcscmp(m_lpBL_No,_T("11223344"))==0)
		MakeImgPath(m_lpDir);
	else
		MakeImgPath_BL_No(m_lpDir,m_lpBL_No);

#if 1
//
	double dH=m_dRadius_mm*1000;
	double dV=m_dRadius_mm*1000;
	CalFocusBorderPt(&dH,&dV);	//重新计算、赋值

	m_dPt33[0][0].dx=m_dCenterX + dH;//{m_dCenterX+m_dRadius_mm*1000,m_dCenterY+m_dRadius_mm*1000};
	m_dPt33[0][0].dy=m_dCenterY + dV;


	m_dPt33[1][0].dx=m_dCenterX;
	m_dPt33[1][0].dy=m_dCenterY + dV;

	m_dPt33[2][0].dx=m_dCenterX - dH;
	m_dPt33[2][0].dy=m_dCenterY + dV;
//
	m_dPt33[0][1].dx=m_dCenterX + dH;
	m_dPt33[0][1].dy=m_dCenterY;

	m_dPt33[1][1].dx=m_dCenterX;
	m_dPt33[1][1].dy=m_dCenterY;

	m_dPt33[2][1].dx=m_dCenterX - dH;
	m_dPt33[2][1].dy=m_dCenterY;

//
	m_dPt33[0][2].dx=m_dCenterX + dH;
	m_dPt33[0][2].dy=m_dCenterY - dV;

	m_dPt33[1][2].dx=m_dCenterX;
	m_dPt33[1][2].dy=m_dCenterY - dV;

	m_dPt33[2][2].dx=m_dCenterX - dH;
	m_dPt33[2][2].dy=m_dCenterY - dV;

#else	//改在OnTimer中处理
	//(1)先中心点聚焦
OutputDebugString(_T("---【1，1】\n"));
	m_cBtnAuto.SetWindowText(TEXT("start中心点聚焦"));
	ShowStagePos();
	m_scan.MoveXYZtoAbsolute(m_dCenterX,m_dCenterY,m_dCenterZ);

	WaitStop_xyz();
	ShowStagePos();
	FirstFocus(TRUE);
	m_dZ33[1][1]=m_pFirstFocusPara[0].zPos;
	m_cBtnAuto.SetWindowText(TEXT("中心点聚焦OK"));
	if(PathFileExists(m_lpSndFile))
	{
		PlaySound(m_lpSndFile, NULL, SND_FILENAME | SND_ASYNC);
	}	

	//(2)再聚焦左上角
OutputDebugString(_T("---【0，0】\n"));
	double dZ = m_pFirstFocusPara[0].zPos;
	m_scan.MoveXYZtoAbsolute(m_dCenterX+m_dRadius_mm*1000,m_dCenterY+m_dRadius_mm*1000,dZ);

	WaitStop_xyz();
	ShowStagePos();
	FirstFocus(TRUE);
	m_dZ33[0][0]=m_pFirstFocusPara[0].zPos;
	if(PathFileExists(m_lpSndFile))
	{
		PlaySound(m_lpSndFile, NULL, SND_FILENAME | SND_ASYNC);
	}

	//(3)再聚焦上边
OutputDebugString(_T("---【1，0】\n"));
	dZ = m_pFirstFocusPara[0].zPos;
	m_scan.MoveXYZtoAbsolute(m_dCenterX,m_dCenterY+m_dRadius_mm*1000,dZ);

	WaitStop_xyz();
	ShowStagePos();
	FirstFocus(TRUE);
	m_dZ33[1][0]=m_pFirstFocusPara[0].zPos;
	if(PathFileExists(m_lpSndFile))
	{
		PlaySound(m_lpSndFile, NULL, SND_FILENAME | SND_ASYNC);
	}

	//(4)再聚焦右上角
OutputDebugString(_T("---【2，0】\n"));
	dZ = m_pFirstFocusPara[0].zPos;
	m_scan.MoveXYZtoAbsolute(m_dCenterX-m_dRadius_mm*1000,m_dCenterY+m_dRadius_mm*1000,dZ);

	WaitStop_xyz();
	ShowStagePos();
	FirstFocus(TRUE);
	m_dZ33[2][0]=m_pFirstFocusPara[0].zPos;
	if(PathFileExists(m_lpSndFile))
	{
		PlaySound(m_lpSndFile, NULL, SND_FILENAME | SND_ASYNC);
	}
	//(5)再聚焦右边
OutputDebugString(_T("---【2，1】\n"));
	dZ = m_pFirstFocusPara[0].zPos;
	m_scan.MoveXYZtoAbsolute(m_dCenterX-m_dRadius_mm*1000,m_dCenterY,dZ);

	WaitStop_xyz();
	ShowStagePos();
	FirstFocus(TRUE);
	m_dZ33[2][1]=m_pFirstFocusPara[0].zPos;
	if(PathFileExists(m_lpSndFile))
	{
		PlaySound(m_lpSndFile, NULL, SND_FILENAME | SND_ASYNC);
	}
	//(6)再聚焦右下角
OutputDebugString(_T("---【2，2】\n"));
	dZ = m_pFirstFocusPara[0].zPos;
	m_scan.MoveXYZtoAbsolute(m_dCenterX-m_dRadius_mm*1000,m_dCenterY-m_dRadius_mm*1000,dZ);

	WaitStop_xyz();
	ShowStagePos();
	FirstFocus(TRUE);
	m_dZ33[2][2]=m_pFirstFocusPara[0].zPos;
	if(PathFileExists(m_lpSndFile))
	{
		PlaySound(m_lpSndFile, NULL, SND_FILENAME | SND_ASYNC);
	}
	//(7)再聚焦下边
OutputDebugString(_T("---【1，2】\n"));
	dZ = m_pFirstFocusPara[0].zPos;
	m_scan.MoveXYZtoAbsolute(m_dCenterX,m_dCenterY-m_dRadius_mm*1000,dZ);

	WaitStop_xyz();
	ShowStagePos();
	FirstFocus(TRUE);
	m_dZ33[1][2]=m_pFirstFocusPara[0].zPos;
	if(PathFileExists(m_lpSndFile))
	{
		PlaySound(m_lpSndFile, NULL, SND_FILENAME | SND_ASYNC);
	}
	//(8)再聚焦右下角
OutputDebugString(_T("---【0，2】\n"));
	dZ = m_pFirstFocusPara[0].zPos;
	m_scan.MoveXYZtoAbsolute(m_dCenterX+m_dRadius_mm*1000,m_dCenterY-m_dRadius_mm*1000,dZ);

	WaitStop_xyz();
	ShowStagePos();
	FirstFocus(TRUE);
	m_dZ33[0][2]=m_pFirstFocusPara[0].zPos;
	if(PathFileExists(m_lpSndFile))
	{
		PlaySound(m_lpSndFile, NULL, SND_FILENAME | SND_ASYNC);
	}
	//(9)再聚焦左边
OutputDebugString(_T("---【0，1】\n"));
	dZ = m_pFirstFocusPara[0].zPos;
	m_scan.MoveXYZtoAbsolute(m_dCenterX+m_dRadius_mm*1000,m_dCenterY,dZ);

	WaitStop_xyz();
	ShowStagePos();
	FirstFocus(TRUE);
	m_dZ33[0][1]=m_pFirstFocusPara[0].zPos;
	if(PathFileExists(m_lpSndFile))
	{
		PlaySound(m_lpSndFile, NULL, SND_FILENAME | SND_ASYNC);
	}

	//
	m_scan.MoveXYZtoAbsolute(m_dCenterX,m_dCenterY,m_dCenterZ);
	WaitStop_xyz();

	for(int x=0;x<3;x++)
	{
		str.Format(_T("%.2f\t%.2f\t%.2f\n"),m_dZ33[x][0],m_dZ33[x][1],m_dZ33[x][2]);
		OutputDebugString(str);
	}
#endif

#if 1
	if(!m_bAutoOn)
	{
		m_nFirstFocusStepID = FIRST_FOCUS_STEP_11;	//先聚焦四周8个点

		m_cBtnAuto.SetWindowText(TEXT("停止扫片"));
		//FirstFocus(TRUE);

#if 1
		m_bAutoOn=TRUE;
		g_nStatus = STATUS_NONE;

		DWORD ThreadID=0;
		m_pMoveThread= ::CreateThread(NULL,NULL,doAuto_NoZ_Focus,(void*)this,0,(LPDWORD)&ThreadID);
		if (m_pMoveThread != NULL)
		{
			OutputDebugString(_T("Start Auto...\n"));
		}
#endif
	}
	else
	{
		StopMoveThread();
		m_scan.MoveXYZtoAbsolute(m_dCenterX,m_dCenterY,m_dCenterZ);
		m_bAutoOn=FALSE;
		m_nFirstFocusStepID =FIRST_FOCUS_STEP_NONE;

		m_cBtnAuto.SetWindowText(TEXT("自动扫片"));
	}
#else
	if(!m_bAutoOn)
	{
		m_nFirstFocusStepID = FIRST_FOCUS_STEP_11;	//先聚焦四周8个点
		m_cBtnAuto.SetWindowText(TEXT("停止扫片"));
		m_bAutoOn=TRUE;
	}
	else
	{
		m_bAutoOn=FALSE;
		m_cBtnAuto.SetWindowText(TEXT("自动扫片"));
	}
#endif

}


//BOOL CMicroScopeDlg::isXYMoving()
BOOL isXYMoving()
{
	long moving=0;
	int err=0;


	if (err = m_scan.IsMoving(&moving) )
	{
		/* handle error  */
		if (err == PRIOR_NOTCONNECTED)
		{
			OutputDebugString(_T("MoveXY error: PRIOR_NOTCONNECTED\n"));
		}
		else
		{
			OutputDebugString(_T("MoveXY error: unknown\n"));
		}
	}
	//_stprintf(m_lpInfo,"XY moving=%d\n",moving);
	//OutputDebugString(m_lpInfo);

	return moving & 3;
}
//BOOL CMicroScopeDlg::isZMoving()
BOOL isZMoving()
{
	long moving=0;
	long err;
	if (err = m_scan.IsMoving(&moving) )
	{
		/* handle error  */
		if (err == PRIOR_NOTCONNECTED)
		{
			OutputDebugString(_T("MoveZ error: PRIOR_NOTCONNECTED\n"));
		}
		else
		{
			OutputDebugString(_T("MoveZ error: unknown\n"));
		}
	}

	//_stprintf(m_lpInfo,"Z moving=%d\n",moving);
	//OutputDebugString(m_lpInfo);

	return moving & 4;
}

void CMicroScopeDlg::CheckErr(long err)
{
	if(err)
	{
		if (err == PRIOR_NOTCONNECTED)
		{
			OutputDebugString(_T("Move error: PRIOR_NOTCONNECTED\n"));
		}
		else
		{
			OutputDebugString(_T("Move error: unknown\n"));
		}
	}
}

int WaitSaveImg()
{
	int rc = -1;
	//if((m_bAutoOn)||(m_bDoFirstFocus))
	//if(m_bAutoOn)
	{
//OutputDebugString("WaitSaveImg\n");
		rc = WaitForSingleObject(g_hImgEvent,2000);
		switch (rc)
		{
		case WAIT_OBJECT_0:
			// hProcess所代表的进程在5秒内结束
			break;
		case WAIT_TIMEOUT:
			// 等待时间超过5秒
			break;

		case WAIT_FAILED:
			// 函数调用失败，比如传递了一个无效的句柄
			break;
		}		
	}
	return rc;
}

int Focus_Z_1_Step(CMicroScopeDlg *pDlg)
{
	int i;
	int rc =-1;
	if(pDlg->m_bAutoOn==0)
		return -1;

	int nCalNum=0;
	for(i=0;i<MAX_Z_STEPS;i++)
		g_dClarity[i] =0.0;

	pDlg->InitFocus_Z(pDlg->m_zFocus_Step_Num,pDlg->m_zFineFocus_Step_um,pDlg->m_dCurZ);

	pDlg->m_pFirstFocusPara[0].dClarity = WaitCalImg_Clarity();
	int nSteps=2*pDlg->m_zFocus_Step_Num +1;
	for(i=1;i<nSteps;i++)
	{
		if(pDlg->m_bAutoOn==0)
			return -1;

		//pDlg->m_cz.MoveUp(pDlg->m_zFineFocus_Step_um);
		m_cz.MoveUp(pDlg->m_zFineFocus_Step_um);
		if(pDlg->m_bAutoOn==0)
			return -1;

		WaitStop_z();
		pDlg->m_pFirstFocusPara[i].dClarity = WaitCalImg_Clarity();
	}
/*
	for(i=0;i<nSteps;i++)
	{
		_stprintf(m_lpInfo,"%d\t%.4f, %.4f\n",i,m_pFirstFocusPara[i].dClarity,m_pFirstFocusPara[i].zPos);
		OutputDebugString(m_lpInfo);
	}
*/
	qsort(pDlg->m_pFirstFocusPara, nSteps, sizeof(FirstFocusPara), mycmp);
/*
	for(i=0;i<nSteps;i++)
	{
		_stprintf(m_lpInfo,"%d\t%.4f, %.4f\n",i,m_pFirstFocusPara[i].dClarity,m_pFirstFocusPara[i].zPos);
		OutputDebugString(m_lpInfo);
	}
*/
	//pDlg->m_cz.MoveToAbsolute(pDlg->m_pFirstFocusPara[0].zPos);
	m_cz.MoveToAbsolute(pDlg->m_pFirstFocusPara[0].zPos);
	WaitStop_z();
	pDlg->m_dCurZ=pDlg->m_pFirstFocusPara[0].zPos;
	return 0;
}


int Trigger1Img(CMicroScopeDlg *pDlg)
{
	int rc=-1;

	//if(pDlg->m_camera.IsGrabbing())	//当采集图像时
	if(pDlg->m_cGrabber.isLive())	//当采集图像时
	{
		g_nStatus = STATUS_XY_WAIT_1_IMG;
		//OutputDebugString("---Here6.1\n");
		int wait = WaitSaveImg();
		if(wait == WAIT_OBJECT_0)
		{
			rc = 0;
			//OutputDebugString("Trigger1Img OK.\n");
		}
		else
			OutputDebugString(_T("Trigger1Img err.\n"));
	}

	return rc;
}
void focus_1_img_NoZ_Focus(CMicroScopeDlg *pDlg,int x,int y,int nX,int nY)
{
	pDlg->m_nXSeri = x;
	pDlg->m_nYSeri = y;
	BOOL bMoveZ = FALSE;
	int rc = 0;

	//水平方向每间隔若干步、再聚焦.
	rc = Trigger1Img(pDlg);

	if(rc ==0)
	{
		_stprintf(pDlg->m_lpStageInfo,_T("X: (%d/%d), Y:(%d/%d)\n"),x,nX,y,nY);
		//pDlg->m_CapStatus[y][x].nSt=1;		//在Save_1_Img()中设置状态
		pDlg->PostMessage(WM_STAGE_CHANGED,0,0);
		OutputDebugString(pDlg->m_lpStageInfo);
	}
}
void focus_1_img(CMicroScopeDlg *pDlg,int x,int y,int nX,int nY)
{
	pDlg->m_nXSeri = x;
	pDlg->m_nYSeri = y;
	BOOL bMoveZ = FALSE;
	int rc = 0;

	//水平方向每间隔若干步、再聚焦.
	rc = Focus_Z_1_Step(pDlg);
	if(rc ==0)
		rc = Trigger1Img(pDlg);

	if(rc ==0)
	{
		_stprintf(pDlg->m_lpStageInfo,_T("X: (%d/%d), Y:(%d/%d)\n"),x,nX,y,nY);
		//pDlg->m_CapStatus[y][x].nSt=1;		//在Save_1_Img()中设置状态
		pDlg->PostMessage(WM_STAGE_CHANGED,0,0);
		OutputDebugString(pDlg->m_lpStageInfo);
	}
}

#if 0
DWORD WINAPI doAuto(LPVOID lpParam)
{
	CMicroScopeDlg *pDlg = (CMicroScopeDlg* )lpParam;
	int nRadius_us = pDlg->m_dRadius_mm*1000;
	int nXStep_um = pDlg->m_nX_step_um;
	int nYStep_um = pDlg->m_nY_step_um;

	int nX=2*nRadius_us/nXStep_um +1;
	int nY=2*nRadius_us/nYStep_um +1;
	
	int i,j,waitTime;
	int nDirX=0;
	long err;
	TCHAR lpStr[64];
	int nStep=0;
	int zFocus_Step_Num_bak = pDlg->m_zFocus_Step_Num;
	int zFineFocus_Step_um_bak = pDlg->m_zFineFocus_Step_um;

	double dCurX,dCurY,dCurZ;
	pDlg->GetStageXYZPos(&dCurX,&dCurY,&dCurZ);
	pDlg->m_dCurZ=dCurZ;


	g_aTime=CTime::GetCurrentTime();
	double dDown=pDlg->m_zFocus_Step_Num * pDlg->m_zFineFocus_Step_um;

	while(pDlg->m_bAutoOn)
	{
		for(i=0;i<nY;i++)
		{
			nStep=0;
			for(j=0;j<nX;j++)
			{
				//(1)Move xyz,
				if( (j % pDlg->m_nX_interval)==0)
				{
					pDlg->m_zFocus_Step_Num =zFocus_Step_Num_bak;
					pDlg->m_zFineFocus_Step_um =zFineFocus_Step_um_bak;
					dDown=pDlg->m_zFocus_Step_Num * pDlg->m_zFineFocus_Step_um;
				}
				else
				{
					pDlg->m_zFocus_Step_Num =pDlg->m_zFocus_Step_Num_1;		//只移动1um：加快速度
					pDlg->m_zFineFocus_Step_um =1;
					dDown=pDlg->m_zFocus_Step_Num * pDlg->m_zFineFocus_Step_um;
				}

				//pDlg->m_cz.MoveDown(dDown);
				m_cz.MoveDown(dDown);

				if(nDirX==0)
					//err = pDlg->m_stage.MoveLeft( pDlg->m_nX_step_um );
					err = m_stage.MoveLeft( pDlg->m_nX_step_um );
				else
					//err = pDlg->m_stage.MoveRight( pDlg->m_nX_step_um );
					err = m_stage.MoveRight( pDlg->m_nX_step_um );

				if(err)/* handle  error condition */
					pDlg->CheckErr(err);
				else	//OK
				{
					//OutputDebugString("Wait XYZ\n");
					WaitStop_xyz();
				}

				//(2)
				if(pDlg->m_bAutoOn==0)
					break;

				//(3)
				if(nDirX==0)
					focus_1_img(pDlg,j,i,nX,nY);
				else
					focus_1_img(pDlg,nX-j-1,i,nX,nY);
			}

			if(nDirX==0)	//改变X运动方向
				nDirX=1;
			else
				nDirX=0;

			if(pDlg->m_bAutoOn==0)
				break;

			//err = pDlg->m_stage.MoveBack( pDlg->m_nY_step_um );
			err = m_stage.MoveBack( pDlg->m_nY_step_um );

			if(err)	//error
			{
				pDlg->CheckErr(err);
			}
			else	//OK
			{
				WaitStop_xy();
			}

			if(pDlg->m_bAutoOn==0)
				break;
/*
			CTime bTime=CTime::GetCurrentTime();
			bTime.GetTime();
			CTimeSpan bbb;
			bbb = bTime - g_aTime;

			_stprintf(pDlg->m_lpStageInfo,"Y: (%d/%d),Time=%dM:%dS\n",i,nY,bbb.GetMinutes(),bbb.GetSeconds());
*/
			_stprintf(pDlg->m_lpStageInfo,_T("Y: (%d/%d)\n"),i,nY);
			pDlg->PostMessage(WM_STAGE_CHANGED,0,0);
			OutputDebugString(pDlg->m_lpStageInfo);

			//_stprintf(lpStr,"Y: (%d/%d),dist=%d\n",i,nY,(i+1)*pDlg->m_nY_step_um);
			//OutputDebugString(lpStr);
		}
		break;	//end
	}

	CTime bTime=CTime::GetCurrentTime();
	bTime.GetTime();
	CTimeSpan bbb;
	bbb = bTime - g_aTime;

	_stprintf(lpStr,_T("Finished,Time=%dM:%dS"),bbb.GetMinutes(),bbb.GetSeconds());
	_stprintf(pDlg->m_lpStageInfo,lpStr);
	pDlg->PostMessage(WM_STAGE_CHANGED,0,0);

	//pDlg->m_scan.MoveXYZtoAbsolute(pDlg->m_dCenterX,pDlg->m_dCenterY,pDlg->m_dCenterZ);
	m_scan.MoveXYZtoAbsolute(pDlg->m_dCenterX,pDlg->m_dCenterY,pDlg->m_dCenterZ);

	pDlg->m_zFocus_Step_Num =zFocus_Step_Num_bak;
	pDlg->m_zFineFocus_Step_um =zFineFocus_Step_um_bak;

	if(PathFileExists(pDlg->m_lpSndFile))
	{
		PlaySound(pDlg->m_lpSndFile, NULL, SND_FILENAME | SND_ASYNC);
	}
	pDlg->m_bAutoOn=FALSE;
	//pDlg->m_cBtnAuto.SetWindowText(TEXT("自动扫片"));
	pDlg->PostMessage(WM_CHANGE_BTN,1,0);

#ifdef __CAL_CELL__
	if(g_nCountCells>=pDlg->m_nShangpi_Num_Limit)
		pDlg->PostMessage(WM_AUTO_CAL,0,0);
	OutputDebugString(_T("Thread doAuto exit.\n"));

	if(g_nCountCells<pDlg->m_nShangpi_Num_Limit)
	{
		OutputDebugString(_T("Need rescan.\n"));
		pDlg->PostMessage(WM_RE_SCAN,1,0);
	}
#endif
	return 0;
}
#else


//点的排列
//(0,0)--------(1,0)------------(2,0)
//(0,1)--------(1,1)------------(2,1)
//(0,2)--------(1,2)------------(2,2)
double calZ(CMicroScopeDlg* pDlg,double dCurX,double dCurY)
{
	dPoint pt0,pt1;
	double dx,dy;
	double z[4];

#if 0
	int x = (pDlg->m_dPt33[0][0].dx-dCurX)/(0.9999*(pDlg->m_dPt33[0][0].dx -pDlg->m_dCenterX));
	int y = (pDlg->m_dPt33[0][0].dy-dCurY)/(0.9999*(pDlg->m_dPt33[0][0].dy -pDlg->m_dCenterY));

#if 0
	TCHAR lpInfo[64];
	_stprintf(lpInfo,_T("XY=(%d,%d)\n"),x,y);
	OutputDebugString(lpInfo);
#endif

	if(x==0)
	{
		if(y==0)
		{
			pt0=pDlg->m_dPt33[0][0];
			pt1=pDlg->m_dPt33[1][1];

			z[0]=pDlg->m_dZ33[0][0];
			z[1]=pDlg->m_dZ33[1][0];
			z[2]=pDlg->m_dZ33[1][1];
			z[3]=pDlg->m_dZ33[0][1];
		}
		else
		{
			pt0=pDlg->m_dPt33[0][1];
			pt1=pDlg->m_dPt33[1][2];

			z[0]=pDlg->m_dZ33[0][1];
			z[1]=pDlg->m_dZ33[1][1];
			z[2]=pDlg->m_dZ33[1][2];
			z[3]=pDlg->m_dZ33[0][2];
		}
	}
	else
	{
		if(y==0)
		{
			pt0=pDlg->m_dPt33[1][0];
			pt1=pDlg->m_dPt33[2][1];

			z[0]=pDlg->m_dZ33[1][0];
			z[1]=pDlg->m_dZ33[2][0];
			z[2]=pDlg->m_dZ33[2][1];
			z[3]=pDlg->m_dZ33[1][1];
		}
		else
		{
			pt0=pDlg->m_dPt33[1][1];
			pt1=pDlg->m_dPt33[2][2];

			z[0]=pDlg->m_dZ33[1][1];
			z[1]=pDlg->m_dZ33[2][1];
			z[2]=pDlg->m_dZ33[2][2];
			z[3]=pDlg->m_dZ33[1][2];
		}
	}
#endif
#if 1
	//if((dCurX>=pDlg->m_dPt33[1][0].dx)&&(dCurX<pDlg->m_dPt33[0][0].dx))	//左侧
	if(dCurX>=pDlg->m_dPt33[1][1].dx)	//左侧
	{
		//if((dCurY>=pDlg->m_dPt33[0][1].dy)&&(dCurX<pDlg->m_dPt33[0][0].dy))
		if(dCurY>=pDlg->m_dPt33[1][1].dy)
		{
			pt0=pDlg->m_dPt33[0][0];
			pt1=pDlg->m_dPt33[1][1];

			z[0]=pDlg->m_dZ33[0][0];
			z[1]=pDlg->m_dZ33[1][0];
			z[2]=pDlg->m_dZ33[1][1];
			z[3]=pDlg->m_dZ33[0][1];
			//OutputDebugString(_T("0,0\n"));
		}
		else
		{
			pt0=pDlg->m_dPt33[0][1];
			pt1=pDlg->m_dPt33[1][2];

			z[0]=pDlg->m_dZ33[0][1];
			z[1]=pDlg->m_dZ33[1][1];
			z[2]=pDlg->m_dZ33[1][2];
			z[3]=pDlg->m_dZ33[0][2];
			//OutputDebugString(_T("0,1\n"));
		}
	}
	else
	{
		//if((dCurY>=pDlg->m_dPt33[0][0].dy)&&(dCurX<pDlg->m_dPt33[0][1].dy))
		if(dCurY>=pDlg->m_dPt33[1][1].dy)
		{
			pt0=pDlg->m_dPt33[1][0];
			pt1=pDlg->m_dPt33[2][1];

			z[0]=pDlg->m_dZ33[1][0];
			z[1]=pDlg->m_dZ33[2][0];
			z[2]=pDlg->m_dZ33[2][1];
			z[3]=pDlg->m_dZ33[1][1];
			//OutputDebugString(_T("1,0\n"));
		}
		else
		{
			pt0=pDlg->m_dPt33[1][1];
			pt1=pDlg->m_dPt33[2][2];

			z[0]=pDlg->m_dZ33[1][1];
			z[1]=pDlg->m_dZ33[2][1];
			z[2]=pDlg->m_dZ33[2][2];
			z[3]=pDlg->m_dZ33[1][2];
			//OutputDebugString(_T("1,1\n"));
		}
	}
#endif
	dx=dCurX-pt0.dx;
	dy=dCurY-pt0.dy;

	//
	double dH=pt1.dx-pt0.dx;
	double dV=pt1.dy-pt0.dy;

	double Z0=z[0]+(z[1]-z[0])*dx/dH;
	double Z1=z[3]+(z[2]-z[3])*dx/dH;

	double Z=Z0+(Z1-Z0)*dy/dV;

	return Z;
}
DWORD WINAPI doAuto_NoZ_Focus(LPVOID lpParam)
{
	CMicroScopeDlg *pDlg = (CMicroScopeDlg* )lpParam;
	TCHAR lpStr[64];
	OutputDebugString(_T("Wait First focuse done...\n"));
	for(int nWait=0;nWait<5000;nWait++)
	{
		if(pDlg->m_nFirstFocusStepID==FIRST_FOCUS_STEP_NONE)
		{
			_stprintf(lpStr,_T("Wait First focuse done ok=%d\n"),nWait);
			OutputDebugString(lpStr);
			break;
		}
		if(pDlg->m_bAutoOn==0)
			break;
		Sleep(200);
	}
	if(pDlg->m_bAutoOn==0)
		return 0;

	//直接移动到左上角开始聚焦
	m_scan.MoveXYZtoAbsolute(pDlg->m_dCenterX + pDlg->m_dRadius_mm*1000,pDlg->m_dCenterY + pDlg->m_dRadius_mm*1000,pDlg->m_dZ33[0][0]);
	WaitStop_xyz();
	pDlg->ShowStagePos();

	int nRadius_us = pDlg->m_dRadius_mm*1000;
	int nXStep_um = pDlg->m_nX_step_um;
	int nYStep_um = pDlg->m_nY_step_um;

	int nX=2*nRadius_us/nXStep_um +1;
	int nY=2*nRadius_us/nYStep_um +1;
	
	int i,j,waitTime;
	int nDirX=0;
	long err;
	
	int nStep=0;
	int zFocus_Step_Num_bak = pDlg->m_zFocus_Step_Num;
	int zFineFocus_Step_um_bak = pDlg->m_zFineFocus_Step_um;

	double dCurX,dCurY,dCurZ;
	pDlg->GetStageXYZPos(&dCurX,&dCurY,&dCurZ);
	pDlg->m_dCurZ=dCurZ;

	g_aTime=CTime::GetCurrentTime();
	double dDown=pDlg->m_zFocus_Step_Num * pDlg->m_zFineFocus_Step_um;

	while(pDlg->m_bAutoOn)
	{
		for(i=0;i<nY;i++)
		{
			nStep=0;
			for(j=0;j<nX;j++)
			{
				//(1)Move xyz,
				//pDlg->GetStageXYZPos(&dCurX,&dCurY,&dCurZ);
				//double dCaledZ=calZ(pDlg,dCurX,dCurY);

				double d_X;
				if(nDirX==0)
				{
					d_X = dCurX - (j+1)*pDlg->m_nX_step_um;
				}
				else
				{
					//d_X = dCurX - (nX-(j+1))*pDlg->m_nX_step_um;
					d_X = dCurX - (nX-j)*pDlg->m_nX_step_um;		//多走了一步
				}

				double d_Y=dCurY - i*pDlg->m_nY_step_um;
				double dCaledZ=calZ(pDlg,d_X,d_Y);

				m_cz.MoveToAbsolute(dCaledZ);

				if(nDirX==0)
					err = m_stage.MoveLeft( pDlg->m_nX_step_um );
				else
					err = m_stage.MoveRight( pDlg->m_nX_step_um );

				if(err)/* handle  error condition */
					pDlg->CheckErr(err);
				else	//OK
				{
					//OutputDebugString("Wait XYZ\n");
					WaitStop_xyz();
				}

				//pDlg->ShowStagePos();

				//(2)
				if(pDlg->m_bAutoOn==0)
					break;

				//(3)
				if(nDirX==0)
					focus_1_img_NoZ_Focus(pDlg,j,i,nX,nY);
				else
					focus_1_img_NoZ_Focus(pDlg,nX-j-1,i,nX,nY);
			}

			if(pDlg->m_bAutoOn==0)
				break;

			if(nDirX==0)	//改变X运动方向
			{
				err = m_stage.MoveLeft( pDlg->m_nX_step_um );	//多走一步,对齐边界
				nDirX=1;
			}
			else
			{
				err = m_stage.MoveRight( pDlg->m_nX_step_um );//多走一步,对齐边界
				nDirX=0;
			}

			if(pDlg->m_bAutoOn==0)
				break;

			err = m_stage.MoveBack( pDlg->m_nY_step_um );

			if(err)	//error
			{
				pDlg->CheckErr(err);
			}
			else	//OK
			{
				WaitStop_xy();
			}

			if(pDlg->m_bAutoOn==0)
				break;
/*
			CTime bTime=CTime::GetCurrentTime();
			bTime.GetTime();
			CTimeSpan bbb;
			bbb = bTime - g_aTime;

			_stprintf(pDlg->m_lpStageInfo,"Y: (%d/%d),Time=%dM:%dS\n",i,nY,bbb.GetMinutes(),bbb.GetSeconds());
*/
			_stprintf(pDlg->m_lpStageInfo,_T("Y: (%d/%d)\n"),i,nY);
			pDlg->PostMessage(WM_STAGE_CHANGED,0,0);
			OutputDebugString(pDlg->m_lpStageInfo);

			//_stprintf(lpStr,"Y: (%d/%d),dist=%d\n",i,nY,(i+1)*pDlg->m_nY_step_um);
			//OutputDebugString(lpStr);
		}
		break;	//end
	}

	CTime bTime=CTime::GetCurrentTime();
	bTime.GetTime();
	CTimeSpan bbb;
	bbb = bTime - g_aTime;

	_stprintf(lpStr,_T("Finished,Time=%dM:%dS"),bbb.GetMinutes(),bbb.GetSeconds());
	_stprintf(pDlg->m_lpStageInfo,lpStr);
	pDlg->PostMessage(WM_STAGE_CHANGED,0,0);

	//pDlg->m_scan.MoveXYZtoAbsolute(pDlg->m_dCenterX,pDlg->m_dCenterY,pDlg->m_dCenterZ);
	m_scan.MoveXYZtoAbsolute(pDlg->m_dCenterX,pDlg->m_dCenterY,pDlg->m_dCenterZ);

	pDlg->m_zFocus_Step_Num =zFocus_Step_Num_bak;
	pDlg->m_zFineFocus_Step_um =zFineFocus_Step_um_bak;

	if(PathFileExists(pDlg->m_lpSndFile))
	{
		PlaySound(pDlg->m_lpSndFile, NULL, SND_FILENAME | SND_ASYNC);
	}
	pDlg->m_bAutoOn=FALSE;
	//pDlg->m_cBtnAuto.SetWindowText(TEXT("自动扫片"));
	pDlg->PostMessage(WM_CHANGE_BTN,1,0);

#ifdef __CAL_CELL__
	if(g_nCountCells>=pDlg->m_nShangpi_Num_Limit)
		pDlg->PostMessage(WM_AUTO_CAL,0,0);
	OutputDebugString(_T("Thread doAuto exit.\n"));

	if(g_nCountCells<pDlg->m_nShangpi_Num_Limit)
	{
		OutputDebugString(_T("Need rescan.\n"));
		pDlg->PostMessage(WM_RE_SCAN,1,0);
	}
#endif
	return 0;
}
#endif


//按照圆的外接矩形扫描
DWORD WINAPI doAuto_2(LPVOID lpParam)
{
	CMicroScopeDlg *pDlg = (CMicroScopeDlg* )lpParam;
	int nRadius_us = pDlg->m_dRadius_mm*1000;
	int nXStep_um = pDlg->m_nX_step_um;
	int nYStep_um = pDlg->m_nY_step_um;

	int nX=2*nRadius_us/nXStep_um +1;
	int nY=2*nRadius_us/nYStep_um +1;
	
	int i,j,waitTime;
	int nDirX=0;
	long err;
	TCHAR lpStr[64];
	int nStep=0;
	int zFocus_Step_Num_bak = pDlg->m_zFocus_Step_Num;
	int zFineFocus_Step_um_bak = pDlg->m_zFineFocus_Step_um;

	double dCurX,dCurY,dCurZ;
	pDlg->GetStageXYZPos(&dCurX,&dCurY,&dCurZ);
	pDlg->m_dCurZ=dCurZ;


	g_aTime=CTime::GetCurrentTime();
	double dDown=pDlg->m_zFocus_Step_Num * pDlg->m_zFineFocus_Step_um;

	while(pDlg->m_bAutoOn)
	{
		for(i=0;i<nY;i++)
		{
			nStep=0;
			for(j=0;j<nX;j++)
			{
				//(1)Move xyz,
				if( (j % pDlg->m_nX_interval)==0)
				{
					pDlg->m_zFocus_Step_Num =zFocus_Step_Num_bak;
					pDlg->m_zFineFocus_Step_um =zFineFocus_Step_um_bak;
					dDown=pDlg->m_zFocus_Step_Num * pDlg->m_zFineFocus_Step_um;
				}
				else
				{
					pDlg->m_zFocus_Step_Num =pDlg->m_zFocus_Step_Num_1;		//只移动1um：加快速度
					pDlg->m_zFineFocus_Step_um =1;
					dDown=pDlg->m_zFocus_Step_Num * pDlg->m_zFineFocus_Step_um;
				}

				//pDlg->m_cz.MoveDown(dDown);
				m_cz.MoveDown(dDown);

				if(nDirX==0)
					//err = pDlg->m_stage.MoveLeft( pDlg->m_nX_step_um );
					err = m_stage.MoveLeft( pDlg->m_nX_step_um );
				else
					//err = pDlg->m_stage.MoveRight( pDlg->m_nX_step_um );
					err = m_stage.MoveRight( pDlg->m_nX_step_um );

				if(err)/* handle  error condition */
					pDlg->CheckErr(err);
				else	//OK
				{
					//OutputDebugString("Wait XYZ\n");
					WaitStop_xyz();
				}

				//(2)
				if(pDlg->m_bAutoOn==0)
					break;

				//(3)
				if(nDirX==0)
					focus_1_img(pDlg,j,i,nX,nY);
				else
					focus_1_img(pDlg,nX-j-1,i,nX,nY);
			}

			if(nDirX==0)	//改变X运动方向
				nDirX=1;
			else
				nDirX=0;

			if(pDlg->m_bAutoOn==0)
				break;

			//err = pDlg->m_stage.MoveBack( pDlg->m_nY_step_um );
			err = m_stage.MoveBack( pDlg->m_nY_step_um );

			if(err)	//error
			{
				pDlg->CheckErr(err);
			}
			else	//OK
			{
				WaitStop_xy();
			}

			if(pDlg->m_bAutoOn==0)
				break;
/*
			CTime bTime=CTime::GetCurrentTime();
			bTime.GetTime();
			CTimeSpan bbb;
			bbb = bTime - g_aTime;

			_stprintf(pDlg->m_lpStageInfo,"Y: (%d/%d),Time=%dM:%dS\n",i,nY,bbb.GetMinutes(),bbb.GetSeconds());
*/
			_stprintf(pDlg->m_lpStageInfo,_T("Y: (%d/%d)\n"),i,nY);
			pDlg->PostMessage(WM_STAGE_CHANGED,0,0);
			OutputDebugString(pDlg->m_lpStageInfo);

			//_stprintf(lpStr,"Y: (%d/%d),dist=%d\n",i,nY,(i+1)*pDlg->m_nY_step_um);
			//OutputDebugString(lpStr);
		}
		break;	//end
	}

	CTime bTime=CTime::GetCurrentTime();
	bTime.GetTime();
	CTimeSpan bbb;
	bbb = bTime - g_aTime;

	_stprintf(lpStr,_T("Finished,Time=%dM:%dS"),bbb.GetMinutes(),bbb.GetSeconds());
	_stprintf(pDlg->m_lpStageInfo,lpStr);
	pDlg->PostMessage(WM_STAGE_CHANGED,0,0);

	//pDlg->m_scan.MoveXYZtoAbsolute(pDlg->m_dCenterX,pDlg->m_dCenterY,pDlg->m_dCenterZ);
	m_scan.MoveXYZtoAbsolute(pDlg->m_dCenterX,pDlg->m_dCenterY,pDlg->m_dCenterZ);

	pDlg->m_zFocus_Step_Num =zFocus_Step_Num_bak;
	pDlg->m_zFineFocus_Step_um =zFineFocus_Step_um_bak;

	if(PathFileExists(pDlg->m_lpSndFile))
	{
		PlaySound(pDlg->m_lpSndFile, NULL, SND_FILENAME | SND_ASYNC);
	}
	pDlg->m_bAutoOn=FALSE;


	//pDlg->m_cBtnAuto.SetWindowText(TEXT("自动扫片"));
	pDlg->PostMessage(WM_CHANGE_BTN,1,0);
#ifdef __CAL_CELL__
	//if(g_nCountCells>=pDlg->m_nShangpi_Num_Limit)
	//	pDlg->PostMessage(WM_AUTO_CAL,0,0);
#endif
	pDlg->m_dRadius_mm = pDlg->m_dRadius_mm_bak;//恢复扫描半径
	pDlg->m_bRescanRun=TRUE;
	OutputDebugString(_T("Thread doAuto exit.\n"));

	return 0;
}
void CMicroScopeDlg::OnBnClickedBtnMovetoCenter()
{
	if(!m_bCenterSetted)
	{
		AfxMessageBox(_T("请先设置中心点."));
		return;
	}

	m_scan.MoveXYZtoAbsolute(m_dCenterX,m_dCenterY,m_dCenterZ);
}

void CMicroScopeDlg::OnBnClickedBtnSetting()
{
	dlgSetting dia(this);
	dia.m_strRadius.Format(_T("%.2f"),m_dRadius_mm);
	dia.m_nXStep=m_nX_step_um;
	dia.m_nYStep=m_nY_step_um;
	dia.m_bAutoCal = m_bAutoCal;
	int nRet = dia.DoModal();
	if(nRet==IDOK)
	{
		TCHAR lpRadius[64];
		_stprintf(lpRadius,_T("%s"),dia.m_strRadius);
		m_dRadius_mm=_tstof(lpRadius);
		if(m_dRadius_mm>MAX_RADIUS)
		{
			m_dRadius_mm=5.0;
			OutputDebugString(_T("Radius too large,set to 5mm.\n"));
		}

		m_nX_step_um = dia.m_nXStep;
		m_nY_step_um = dia.m_nYStep;
		m_bAutoCal = dia.m_bAutoCal;

		wrPara(g_lpINIPath);

		//
		for(int i=0;i<m_nyNum;i++)
		{
			free(m_CapStatus[i]);
		}

		free(m_CapStatus);

		InitMark();
		CDC* pDC=GetDC();
		ShowAllMark(pDC);
		ReleaseDC(pDC);
	}
	else
	{
		wrPara(g_lpINIPath);
	}
}


void CMicroScopeDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if((!m_bAutoOn)&&(!m_bDoFirstFocus)&&(!m_cGrabber.isLive()))
	{
		CRect rectClient;
		CWnd* pWnd=GetDlgItem(IDC_PROGRESS);
		pWnd->GetWindowRect(&rectClient);
		ScreenToClient(&rectClient);

		int xWid=rectClient.Width()/m_nxNum;
		int yHei=rectClient.Height()/m_nyNum;

		int recWid=rectClient.Width();
		int recHei=rectClient.Height();
		int nXOffset=(recWid-m_nxNum*xWid)/2;
		int nYOffset=(recHei-m_nyNum*yHei)/2;
		rectClient.OffsetRect(nXOffset,nYOffset);
		rectClient.right=rectClient.left+xWid*m_nxNum;
		rectClient.bottom=rectClient.top+yHei*m_nyNum;

		if(rectClient.PtInRect(point))
		{
			int x,y;
			x=(point.x-rectClient.left)/xWid;
			y=(point.y-rectClient.top)/yHei;
			int i=0;
			TCHAR lpInfo[64];
			_stprintf(lpInfo,_T("%d,%d\n"),x,y);
			OutputDebugString(lpInfo);
			if(m_CapStatus[y][x].nSt>0)
			{
				SAFE_FreeDIBHandle(m_hDib_BK1);
				m_hDib_BK1=LoadImageFile((LPCTSTR)m_CapStatus[y][x].lpFName);
				if(m_hDib_BK1!=NULL)
				{
					CRect rectDest;
					AdjRect(rectDest);
					rectDest.OffsetRect(11,11);

					CDC* pDC=GetDC();
					DrawImage(pDC->GetSafeHdc(),m_hDib_BK1,&rectDest);
					ReleaseDC(pDC);
				}
			}
		}
	}
	__super::OnLButtonDown(nFlags, point);
}

void CMicroScopeDlg::OnBnClickedBtnStatistic()
{
#ifdef __CAL_CELL__

	if(g_nCountLymphocyte<=0)
		return;

	//  DNA含量公式
	///////////////////////////////////////////////////////////////////////////////////////////////
	//MeanCellsGrayValue = SumCellsGrayValue / g_nCountCells;      // 所有上皮细胞的平均灰度值
	MeanLymphocyteGrayValue  = SumLymphocyteGrayValue  / g_nCountLymphocyte;   // 每个淋巴细胞的平均灰度值（除数）

	if(MeanLymphocyteGrayValue<=0)
		return;

	for (int i = 0; i < g_nCountCells; i++)
	{
		//g_dCellsLambda[i] = g_dGrayValue1[i] /MeanLymphocyteGrayValue ;                                                 // 原始公式
		//g_dCellsLambda[i] = ( g_dGrayValue1[i] +g_nCellsPixelArea[i]) / MeanLymphocyteGrayValue;        //  分子：灰度值+面积
		//g_dCellsLambda[i] = ( g_dGrayValue1[i] +100000*(1-g_dCircleDegree[i]) )/ MeanLymphocyteGrayValue;            //   分子：灰度值+圆度
		g_dCellsLambda[i] = ( g_dGrayValue1[i] +8.5*g_nCellsPixelArea[i]+210000*(1-g_dCircleDegree[i]) )/ (1.4*MeanLymphocyteGrayValue);  //分子：灰度值+面积+圆度
		//g_dCellsLambda[i] = (2*g_dGrayValue1[i] +7.5*g_nCellsPixelArea[i]+310000*(1-g_dCircleDegree[i]) )/ (1.4*MeanLymphocyteGrayValue);  //分子：灰度值+面积+圆度 (2018/01/26调整)

	}


#if 0	//2019.1.15
	int nCountNUMArray[4] = { 0 };
	for (int i = 0; i < g_nCountCells; i++)
	{
		if ((g_dCellsLambda[i] <= 1.25) && (g_dCellsLambda[i] > 0.75))
		{
			nCountNUMArray[0]++;
			continue;
		}
		if ((g_dCellsLambda[i] <= 2.25) && (g_dCellsLambda[i] > 1.75))
		{
			nCountNUMArray[1]++;
			continue;
		}
		if ((g_dCellsLambda[i] <= 3.25) && (g_dCellsLambda[i] > 2.75))
		{
			nCountNUMArray[2]++;
			continue;
		}
		if (g_dCellsLambda[i] > 3.75)
		{
			nCountNUMArray[3]++;
			continue;
		}
	}
#endif
int nCountNUMArray[4] = { 0 };
	for (int i = 0; i < g_nCountCells; i++)
	{
		if ((g_dCellsLambda[i] <= 1.5) && (g_dCellsLambda[i] > 0.75))
		{
			nCountNUMArray[0]++;
			continue;
		}
		if ((g_dCellsLambda[i] <= 3.6) && (g_dCellsLambda[i] > 1.5))
		{
			nCountNUMArray[1]++;
			continue;
		}
		if ((g_dCellsLambda[i] <= 5) && (g_dCellsLambda[i] >3.6))
		{
			nCountNUMArray[2]++;
			continue;
		}
		if (g_dCellsLambda[i] > 5)
		{
			nCountNUMArray[3]++;
			continue;
		}
	}

	TCHAR lpIni[MAX_PATH];
	_stprintf(lpIni,_T("%s%s"),g_lpBinPath,_T("cell_result.ini"));
	WritePrivateProfileString(_T("result"),_T("shangpi"),m_lpResult_ShangPi,lpIni);
	WritePrivateProfileString(_T("result"),_T("linba"),m_lpResult_LinBa,lpIni);
	WritePrivateProfileString(_T("result"),_T("xianxingli"),m_lpResult_XianXingLi,lpIni);
	WritePrivateProfileString(_T("result"),_T("Impurity"),m_lpResult_Impurity,lpIni);


	TCHAR cValueTemp[100];
	_stprintf(cValueTemp, _T("%d"), nCountNUMArray[0]);
	SetDlgItemText(IDC_EDIT_DANBEITI,cValueTemp);
	WritePrivateProfileString(_T("result"),_T("danbeiti"),cValueTemp,lpIni);

	_stprintf(cValueTemp, _T("%d"), nCountNUMArray[1]);
	SetDlgItemText(IDC_EDIT_ERBEITI,cValueTemp);
	WritePrivateProfileString(_T("result"),_T("erbeiti"),cValueTemp,lpIni);

	_stprintf(cValueTemp, _T("%d"), nCountNUMArray[2]);
	SetDlgItemText(IDC_EDIT_SANBEITI,cValueTemp);
	WritePrivateProfileString(_T("result"),_T("sanbeiti"),cValueTemp,lpIni);

	_stprintf(cValueTemp, _T("%d"), nCountNUMArray[3]);
	SetDlgItemText(IDC_EDIT_DUOBEITI,cValueTemp);
	WritePrivateProfileString(_T("result"),_T("duobeiti"),cValueTemp,lpIni);

	UpdateWindow();	
#endif
}


void CMicroScopeDlg::ClearResult()
{
#ifdef  __CAL_CELL__
	g_nCountCells= 0;  
	for(int i=0;i<MAX_CELL_NUM;i++)
	{
		g_nCellsPixelArea[i]=0;
	}
	for(int i=0;i<MAX_CELL_NUM;i++)
	{
		g_dCellsLambda[i]=0.0;
	}
	for(int i=0;i<MAX_CELL_NUM;i++)
	{
		g_dCircleDegree[i]=0.0;
	}
	for(int i=0;i<MAX_CELL_NUM;i++)
	{
		g_dGrayValue1[i]=0;
	}

	MeanLymphocyteGrayValue = 0;
	SumLymphocyteGrayValue  = 0;
	g_nCountLymphocyte        = 0;
	g_nCountImpurity=0;
	_stprintf(m_lpResult_XianXingLi, _T("%s"),_T(""));
	

	ClearShowResult();
#endif
}

void CMicroScopeDlg::ClearShowResult()
{
SetDlgItemText(IDC_EDIT_SHANGPI,_T(""));
SetDlgItemText(IDC_EDIT_LINBA,_T(""));
	SetDlgItemText(IDC_EDIT_DANBEITI,_T(""));
	SetDlgItemText(IDC_EDIT_ERBEITI,_T(""));
	SetDlgItemText(IDC_EDIT_SANBEITI,_T(""));
	SetDlgItemText(IDC_EDIT_DUOBEITI,_T(""));

	SetDlgItemText(IDC_EDIT_XIANXINGLI,_T(""));
	SetDlgItemText(IDC_EDIT_Impurity,_T(""));

	UpdateWindow();	
}

void CMicroScopeDlg::InitFocus_buf()
{
	int nMaxStep=MAX(m_nFirstFocusStepNum,m_zRoughFocusStep_um+1);
	int nSteps=2*nMaxStep+1;
	if(m_pFirstFocusPara==NULL)
	{
		m_pFirstFocusPara=new FirstFocusPara[nSteps];
		if(m_pFirstFocusPara==NULL)
		{
			return;
		}
		else
		{
			memset(m_pFirstFocusPara,0,nSteps*sizeof(FirstFocusPara));
		}
	}
}
//思路：
//(1)移动到中心点；
//(2)上移m_nFirstFocusStepNum步；然后下移2*m_nFirstFocusStepNum+1步；
//(3)先按照m_zRoughFocusStep_um点步长粗定焦；
//(4)找到粗的定焦位置后，然后按照1um的步长细定焦。步数是：2*m_zRoughFocusStep_um+1
//(5)bWithFineFocus:	是否精细聚焦
void CMicroScopeDlg::FirstFocus(BOOL bWithFineFocus)
{
	int i;
	if(!m_bCenterSetted)
	{
		//AfxMessageBox("请先设置中心点。");
		return;
	}
	rdFirstFocusPara(g_lpINIPath);

	//（1）粗聚焦
	//(1.1)设置粗定焦的每一个z的坐标
	m_dCurZ = m_cz.get_Position();
	InitFocus_Z(m_nFirstFocusStepNum,m_zRoughFocusStep_um,m_dCurZ);

	//(1.2) z方向移动和计算清晰度
	g_FocusPara.pWnd=this;
	g_FocusPara.nFocusStep = m_zRoughFocusStep_um;
	g_FocusPara.nFocusStepNum = m_nFirstFocusStepNum;	//从当前点向下的步数

	g_nStatus = STATUS_NONE;
	m_bDoFirstFocus=1;
	DWORD ThreadID=0;
	m_pThreadFirstFocus= ::CreateThread(NULL,NULL,doFirstFocus,(void*)&g_FocusPara,0,(LPDWORD)&ThreadID);
	if (m_pThreadFirstFocus != NULL)
	{
		OutputDebugString(_T("Start doFirstFocus...\n"));
	}
	int rc = WaitForSingleObject(m_pThreadFirstFocus, 10000);

	switch (rc)
	{
	case WAIT_OBJECT_0:
		// hProcess所代表的进程在3秒内结束
		OutputDebugString(_T("wait FirstFocus OK_0.\n"));
		break;
	case WAIT_TIMEOUT:
		// 等待时间超过5秒
		OutputDebugString(_T("wait FirstFocus timeout_0.\n"));
		break;

	case WAIT_FAILED:
		// 函数调用失败，比如传递了一个无效的句柄
		OutputDebugString(_T("wait FirstFocus failed.\n"));
		break;
	}		

	CloseHandle(m_pThreadFirstFocus);
	m_pThreadFirstFocus=NULL;

	//(1.3),排序
	int nSteps=2*m_nFirstFocusStepNum+1;
#if 0
	for(i=0;i<nSteps;i++)
	{
		_stprintf(m_lpInfo,"%d\t%.4f, %.4f\n",i,m_pFirstFocusPara[i].dClarity,m_pFirstFocusPara[i].zPos);
		OutputDebugString(m_lpInfo);
	}
#endif
	qsort(m_pFirstFocusPara, nSteps, sizeof(FirstFocusPara), mycmp);
#if 0
	for(i=0;i<nSteps;i++)
	{
		_stprintf(m_lpInfo,"%d\t%.4f, %.4f\n",i,m_pFirstFocusPara[i].dClarity,m_pFirstFocusPara[i].zPos);
		OutputDebugString(m_lpInfo);
	}
#endif

	//(1.4)
	m_cz.MoveToAbsolute(m_pFirstFocusPara[0].zPos);

	WaitStop_z();
	_stprintf(m_lpInfo,_T("Rough focus end: move to:%.4f, actual=%.4f\n"),m_pFirstFocusPara[0].zPos,m_cz.get_Position());
	ShowStagePos();
	OutputDebugString(m_lpInfo);

	if(!bWithFineFocus)
		return;

	//(2)精细聚焦

	//(2.1)设置粗定焦的每一个z的坐标
	//int nFineFocusStepNum = m_zRoughFocusStep_um +1;
	int nFineFocusStepNum = m_zRoughFocusStep_um;
	int nStep_um=1;	//1um
	InitFocus_Z(nFineFocusStepNum,nStep_um,m_pFirstFocusPara[0].zPos);

	//(2.2) z方向移动和计算清晰度
	g_FocusPara.pWnd=this;
	g_FocusPara.nFocusStep = 1;
	g_FocusPara.nFocusStepNum = nFineFocusStepNum;	//从当前点向下的步数

	g_nStatus = STATUS_NONE;
	m_bDoFirstFocus=1;
	ThreadID=0;
	m_pThreadFirstFocus= ::CreateThread(NULL,NULL,doFirstFocus,(void*)&g_FocusPara,0,(LPDWORD)&ThreadID);

	if (m_pThreadFirstFocus != NULL)
	{
		OutputDebugString(_T("Start doFirstFocus...\n"));
	}
	rc = WaitForSingleObject(m_pThreadFirstFocus, 10000);

	switch (rc)
	{
	case WAIT_OBJECT_0:
		// hProcess所代表的进程在3秒内结束
		OutputDebugString(_T("wait FirstFocus OK_1.\n"));
		break;
	case WAIT_TIMEOUT:
		// 等待时间超过5秒
		OutputDebugString(_T("wait FirstFocus timeout_1.\n"));
		break;

	case WAIT_FAILED:
		// 函数调用失败，比如传递了一个无效的句柄
		OutputDebugString(_T("wait FirstFocus failed.\n"));
		break;
	}		
	CloseHandle(m_pThreadFirstFocus);
	m_pThreadFirstFocus=NULL;

	//(2.3),排序
	nSteps=2*nFineFocusStepNum+1;
#if 0
	for(i=0;i<nSteps;i++)
	{
		_stprintf(m_lpInfo,"%d\t%.4f, %.4f\n",i,m_pFirstFocusPara[i].dClarity,m_pFirstFocusPara[i].zPos);
		OutputDebugString(m_lpInfo);
	}
#endif
	qsort(m_pFirstFocusPara, nSteps, sizeof(FirstFocusPara), mycmp);
#if 0	
	for(i=0;i<nSteps;i++)
	{
		_stprintf(m_lpInfo,"%d\t%.4f, %.4f\n",i,m_pFirstFocusPara[i].dClarity,m_pFirstFocusPara[i].zPos);
		OutputDebugString(m_lpInfo);
	}
#endif
	//(2.4)
	m_cz.MoveToAbsolute(m_pFirstFocusPara[0].zPos);

	WaitStop_z();
	_stprintf(m_lpInfo,_T("Fine focus end: move to:%.4f, actual=%.4f\n"),m_pFirstFocusPara[0].zPos,m_cz.get_Position());
	ShowStagePos();
	OutputDebugString(m_lpInfo);
}
//以dCurZ为中心，上下移动nStepNum步，每步的步长是nStep_um
void CMicroScopeDlg::InitFocus_Z(int nStepNum,double nStep_um,double dCurZ)
{
	for(int i=0;i<(2*nStepNum +1);i++)
	{
		m_pFirstFocusPara[i].zPos=dCurZ + (i - nStepNum)*nStep_um;
	}
}
void CMicroScopeDlg::ShowStagePos()
{
	TCHAR lpInfo[128];
	double x,y;
	double z = m_cz.get_Position();
	int err = m_stage.GetPosition(&x,&y);
	_stprintf(lpInfo,_T("x,y,z=(%.4f,%.4f,%.4f)\n"),x,y,z);
	OutputDebugString(lpInfo);
}

void CMicroScopeDlg::GetStageXYZPos(double *px,double *py,double *pz)
{
	*pz = m_cz.get_Position();
	int err = m_stage.GetPosition(px,py);
}

double WaitCalImg_Clarity()
{
	double v;
	g_nStatus = STATUS_CAL_IMG_SHARPNESS;
	int rc = WaitSaveImg();
	if(rc == WAIT_OBJECT_0)
	{
		v = g_dCurClarity;
	}
	else
		v = 0;
	return v;
}


//以当前Z为中心，上下移动n个step，计算清晰度，然后比较清晰度值。
//然后把z移动到清晰度最大的z的位置上。
//但测试发现，移动后的图像不清楚。
//计划用focus_z
DWORD WINAPI doFirstFocus(LPVOID lpParam)
{
	FocusPara* pFocusPara = (FocusPara*)lpParam;
	CMicroScopeDlg *pDlg = (CMicroScopeDlg* )pFocusPara->pWnd;
	TCHAR lpInfo[128];
	int nSteps=2*pFocusPara->nFocusStepNum +1;
	int rc =0;

	//(1)下移
	double dd = pFocusPara->nFocusStepNum * pFocusPara->nFocusStep;
	//pDlg->m_cz.MoveDown(dd);
	m_cz.MoveDown(dd);
	WaitStop_z();
	pDlg->ShowStagePos();
	pDlg->m_pFirstFocusPara[0].dClarity = WaitCalImg_Clarity();

	//(2)
	while(pDlg->m_bDoFirstFocus)
	{
		//for(int i=0;i<nSteps;i++)
		for(int i=1;i<nSteps;i++)
		{
			//pDlg->m_cz.MoveUp(pFocusPara->nFocusStep);
			m_cz.MoveUp(pFocusPara->nFocusStep);
			WaitStop_z();
#if 0
pDlg->ShowStagePos();
#endif

			pDlg->m_pFirstFocusPara[i].dClarity = WaitCalImg_Clarity();

			_stprintf(lpInfo,_T("Step: %d, z=%.4f,Clarity=%.4f\n"),i,pDlg->m_pFirstFocusPara[i].zPos,g_dCurClarity);
			OutputDebugString(lpInfo);

			if(!pDlg->m_bDoFirstFocus)
				break;
		}
		pDlg->m_bDoFirstFocus=0;
	}
	g_nStatus = STATUS_NONE;
	pDlg->m_bDoFirstFocus=0;

	OutputDebugString(_T("Thread doFirstFocus exit.\n"));
	return 0;
}

void CMicroScopeDlg::StopFirstFocusThread()
{
	if(m_bDoFirstFocus)
	{
		m_bDoFirstFocus=FALSE;
		WaitForSingleObject(m_pThreadFirstFocus, INFINITE);
		CloseHandle(m_pThreadFirstFocus);
		m_pThreadFirstFocus = NULL;
		OutputDebugString(_T("doFirstFocus Thread stopped.\n"));
	}
}

void WaitStop_xy()
{
	long moving=0;
	long err;
	do
	{
		//Sleep(20);
		if (err = m_scan.IsMoving(&moving) )
		{
			/* handle error  */
			if (err == PRIOR_NOTCONNECTED)
			{
				OutputDebugString(_T("MoveXY error: PRIOR_NOTCONNECTED\n"));
			}
			else
			{
				OutputDebugString(_T("MoveXY error: unknown\n"));
			}
		}
		//if(m_bAutoOn==0)
		//	break;
		//_stprintf(m_lpInfo,"XY moving=%d\n",moving);
		//OutputDebugString(m_lpInfo);
	}while (moving & 3);
}
void WaitStop_xyz()
{
	WaitStop_z();
	WaitStop_xy();
}
void WaitStop_z()
{
	long moving=0;
	long err;
	do
	{
		if (err = m_scan.IsMoving(&moving) )
		{
			/* handle error  */
			if (err == PRIOR_NOTCONNECTED)
			{
				OutputDebugString(_T("MoveZ error: PRIOR_NOTCONNECTED\n"));
			}
			else
			{
				OutputDebugString(_T("MoveZ error: unknown\n"));
			}
		}
		//if(m_bAutoOn==0)
		//	break;
		//_stprintf(m_lpInfo,"Z moving=%d\n",moving);
		//OutputDebugString(m_lpInfo);
	}while (moving & 4);
}

void CMicroScopeDlg::OnBnClickedBtnUp()
{
#if 1
	if(m_bStageEn)
	{
		TCHAR lpStr[128];
		_stprintf(lpStr,_T("Up=%.1f\n"),m_zFineFocus_Step_um);
		OutputDebugString(lpStr);
		m_cz.MoveUp(m_zFineFocus_Step_um);
		WaitStop_z();
		OutputDebugString(_T("Up end\n"));
	}
	else
	{
		OutputDebugString(_T("显微平台初始化错误！"));
	}
#endif
#if 0
	if(m_bStageEn)
	{
		TCHAR lpStr[128];
		_stprintf(lpStr,"Left=%d\n",m_nX_step_um);
		OutputDebugString(lpStr);
		m_stage.MoveLeft ( m_nX_step_um );
		WaitStop_xy();
		OutputDebugString("Left end\n");
	}
	else
	{
		ShowInfo("显微平台初始化错误！");
	}
#endif
}

void CMicroScopeDlg::OnBnClickedBtnDown()
{
#if 1
	if(m_bStageEn)
	{
		TCHAR lpStr[128];
		_stprintf(lpStr,_T("Down=%.1f\n"),m_zFineFocus_Step_um);
		OutputDebugString(lpStr);
		m_cz.MoveDown(m_zFineFocus_Step_um);
		WaitStop_z();
		OutputDebugString(_T("Down end\n"));
	}
	else
	{
		ShowInfo(_T("显微平台初始化错误！"));
	}
#endif
#if 0
	if(m_bStageEn)
	{
		TCHAR lpStr[128];
		_stprintf(lpStr,"Right=%d\n",m_nX_step_um);
		OutputDebugString(lpStr);
		m_stage.MoveRight ( m_nX_step_um );
		WaitStop_xy();
		OutputDebugString("Right end\n");
	}
	else
	{
		ShowInfo("显微平台初始化错误！");
	}
#endif
}
void CMicroScopeDlg::OnBnClickedBtnTest()
{
	TCHAR lpInfo[64];
	
	//m_bAutoOn = TRUE;
	g_nStatus = STATUS_CAL_IMG_SHARPNESS;
	int rc = WaitSaveImg();
	if(rc == WAIT_OBJECT_0)
	{
		double z = m_cz.get_Position();
		_stprintf(m_lpInfo,_T("z=%.2f,清晰度：%.2f"),z,g_dCurClarity);
		ShowInfo(m_lpInfo);
	}
	else
	{
		_stprintf(m_lpInfo,_T("清晰度：TimeOut"));
		ShowInfo(m_lpInfo);
	}

	//m_bAutoOn = FALSE;
}

void CMicroScopeDlg::OnBnClickedBtnSetAsCenter()
{
	int err = m_stage.GetPosition(&m_dCenterX,&m_dCenterY);
	m_dCenterZ = m_cz.get_Position();
	wrCenterXYZ(g_lpINIPath);
}

void CMicroScopeDlg::OnBnClickedBtnCamSetting()
{
	if( m_cGrabber.isDevValid())
	{
		m_cGrabber.showVCDPropertyPage(this->m_hWnd);
		m_cGrabber.saveDeviceStateToFile("device.xml");
	}
}

void CMicroScopeDlg::OnBnClickedBtnAdjBrightness()
{
	AdjBrightnessDlg dia(this);
	dia.DoModal();
	m_bAdjBrigntnessOn=FALSE;
}
