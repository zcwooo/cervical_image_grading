
// MicroScopeDlg.h : header file
//

#pragma once


// Namespace for using cout.
using namespace std;

#ifndef _DEBUG
#define  __CAL_CELL__	//采集的同时，计算细胞
#endif

#include "afxwin.h"
#include "resource.h"
#include "Listener.h"

#include "qImgLib2User/qImgLib.h"
#pragma comment(lib,"qImgLib2User/qImgLib.lib")

#define Z_DIR_UP	0
#define Z_DIR_DOWN	1

#define MAX_Z_STEPS		20		//z方向移动几步来判断清晰度
#define BTN_HEIGHT					32	//button的高度，显示图像时候减去该高度。
#define MAX_WAIT_SAVE_IMG_TIME		3000	//3000ms
//#define WaitCircle 30				//30ms	//使用变量，从ini文件读取


#define WM_STAGE_CHANGED			(WM_USER + 1)
#define WM_SHOW_INFO				(WM_USER + 2)
#define WM_SHOW_RESULT				(WM_USER + 3)
#define WM_CHANGE_BTN				(WM_USER + 4)
#define WM_STOP_AUTO				(WM_USER + 5)
#define WM_RE_SCAN					(WM_USER + 6)
#define WM_AUTO_CAL					(WM_USER + 7)
#define WM_USER_CAL					(WM_USER + 100)

#define FIRST_FOCUS_STEP_NONE	0
#define FIRST_FOCUS_STEP_11	1
#define FIRST_FOCUS_STEP_00	2
#define FIRST_FOCUS_STEP_10	3
#define FIRST_FOCUS_STEP_20	4
#define FIRST_FOCUS_STEP_21	5
#define FIRST_FOCUS_STEP_22	6
#define FIRST_FOCUS_STEP_12	7
#define FIRST_FOCUS_STEP_02	8
#define FIRST_FOCUS_STEP_01	9

typedef struct __FocusPara
{
	void* pWnd;
	int nFocusStep;
	int nFocusStepNum;
}FocusPara;

typedef struct __CapStatus
{
	int nSt;
	char lpFName[MAX_PATH];
}CapStatus;

typedef struct __FirstFocusPara
{
	double dClarity;
	double zPos;
}FirstFocusPara;

typedef struct _tagdPoint
{
	double dx;
	double dy;
}dPoint;

// CMicroScopeDlg dialog
class CMicroScopeDlg : public CDialog
{
DECLARE_EASYSIZE
// Construction
public:
	CMicroScopeDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_MICROSCOPE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

public:

public:
// Implementation
protected:
	HICON m_hIcon;
public:
	DShowLib::Grabber		m_cGrabber; // The instance of the Grabber class.
    DShowLib::FrameHandlerSink::tFHSPtr           m_pSink;
	CListener m_cListener;	


	BOOL m_bResizeEn;

	BOOL m_bStageEn;
	BOOL m_bHaveCamera;
	BOOL m_bStart_OpenCam;

	void ClearResult();

	TCHAR m_lpResult_ShangPi[64];
	TCHAR m_lpResult_LinBa[64];
	TCHAR m_lpResult_XianXingLi[64];
	TCHAR m_lpResult_Impurity[64];	//杂质

	int m_nxNum;	//水平方向有多少个图像
	int m_nyNum;	//垂直方向有多少个图像
	CapStatus **m_CapStatus;
	void InitMark();
	void ClearMark();

	BOOL m_bAdjBrigntnessOn;
	int* m_pHist;

	dPoint m_dPt33[3][3];
	double m_dZ33[3][3];

//private:
	void ResizeImgWin();
	int m_nComPortID;
	double m_dRadius_mm;		//按照圆形扫描，圆半径
	double m_dRadius_mm_bak;
	BOOL m_bRescanRun;			//是否自动重扫过
	double m_dMaxRadius_mm;		//最大按照圆形扫描，圆半径
	int m_nX_step_um;
	int m_nY_step_um;
	int m_zFocus_Step_Num;
	int m_zFocus_Step_Num_1;
	double m_zFineFocus_Step_um;
	int m_zRoughFocusStep_um;

	BOOL m_bCenterSetted;
	double m_dCenterX;
	double m_dCenterY;
	double m_dCenterZ;
	void wrCenterXYZ(TCHAR *lpINI);
	void rdCenterXYZ(TCHAR *lpINI);

	void CalFocusBorderPt(double* pH,double* pV);

	int m_nFirstFocusStepID;
	double m_dCurX;
	double m_dCurY;
	double m_dCurZ;
	int m_nX_interval;		//水平方向每间隔几步、Z方向自动聚焦一次

	int m_nXSeri;
	int m_nYSeri;

	BOOL m_bAutoCal;
	int m_nShangpi_Num_Limit;
	int m_nImgWid;
	int m_nImgHei;
	BYTE* m_pBuf8;

	int m_nBits;
	int m_nImgWid_Adj;
	int m_nImgHei_Adj;
	BYTE* m_pBuf8_Adj;

	TCHAR m_lpStageInfo[128];
	TCHAR m_lpDir[MAX_PATH];
	TCHAR m_lpSndFile[MAX_PATH];

	HANDLE m_hDib_BK1;
	HANDLE m_hDib_BK2;
	HANDLE m_hDib_Mark1;
	HANDLE m_hDib_Mark2;
	HANDLE m_hDib_Mark3;
	HANDLE m_hDib_Mark4;

	BOOL m_bTimerOn;

	void rdBL_No();
	TCHAR m_lpBL_No[MAX_PATH];

	int m_nCellNum_th;

	int m_nFirstFocusStepNum;
	FirstFocusPara* m_pFirstFocusPara;
	
	void ShowAllMark(CDC* pDC);
	void ShowOneMark(int nx,int ny);
	void ShowBK_Img();
	void ClearShowResult();
	void InitFocus_Z(int nStepNum,double nStep_um,double dCurZ);
	void InitFocus_buf();

	BOOL m_bAutoOn;
	void CheckErr(long err);
	void StopMoveThread();

	TCHAR m_lpInfo[128];
	void ShowInfo(TCHAR* lpInfo);

	BOOL InitPrior(int nPort);
	void rdPara(TCHAR *lpINI);
	void rdFirstFocusPara(TCHAR *lpINI);
	void wrPara(TCHAR *lpINI);
	void EnableMoveBtn(BOOL bEn);

	void FirstFocus(BOOL bWithFineFocus);
	void ShowStagePos();
	void GetStageXYZPos(double *px,double *py,double *pz);
	void AdjRect(CRect& pOutRec);

	BOOL m_bDoFirstFocus;
	void StopFirstFocusThread();

	CStatic m_cStaticVideoWindow;
	CButton m_cButtonLive;
	CButton m_cBtnAuto;
	void SetImgButtonStates(void);
	void StartImg();

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnImg();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg LRESULT OnChangeTtnName(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnAutoCal(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnStageChanged(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnShowInfo(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnShowResult(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnStopAuto(WPARAM wparam, LPARAM lparam);
	afx_msg LRESULT OnRescan(WPARAM wparam, LPARAM lparam);
	afx_msg void OnBnClickedBtnAuto();
	afx_msg void OnBnClickedBtnSetting();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedBtnMovetoCenter();
	afx_msg void OnBnClickedBtnStatistic();
	afx_msg void OnBnClickedBtnUp();
	afx_msg void OnBnClickedBtnDown();
	afx_msg void OnBnClickedBtnTest();
	afx_msg void OnBnClickedBtnSetAsCenter();
	afx_msg void OnBnClickedBtnCamSetting();
	afx_msg void OnBnClickedBtnAdjBrightness();
};
