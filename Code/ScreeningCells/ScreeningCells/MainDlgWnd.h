#pragma once
#include "PopWnd.h"
#include "ShlObj.h"
#include "MsgWnd.h"
#include "ControlEx.h"
#include "SkinManager.h"

///////////////////////////
#include "patientinfo_t.h"

#include<opencv2/opencv.hpp>
#include<algorithm>
using namespace std;

#define buf_send_recv_len 8196 //通信最大字节数
//消息定义
#define msg_len									strlen("00")
#define msg_c_s_init							"00"			//初始化完成
#define msg_c_s_calcresult						"01"			//计算结果通知
#define msg_s_c_calcfilename					"02"			//计算信息
#define msg_c_s_calcprocess						"03"			//进度
#define msg_s_c_close							"99"

///////////////////////////////////////////////////////////////////////////
class ComputerExamineUI : public CContainerUI
{
public:
	ComputerExamineUI(const TCHAR* xml)
	{
		CDialogBuilder builder;
		CContainerUI* pPage = static_cast<CContainerUI*>(builder.Create(xml, (UINT)0));
		if (pPage) {
			this->Add(pPage);
		}
		else {
			this->RemoveAll();
			return;
		}
	}
};

class CDialogBuilderCallbackEx : public IDialogBuilderCallback
{
public:
	CDialogBuilderCallbackEx(const TCHAR* xml)
	{
		memset((char*)m_aName, 0, sizeof(m_aName));
		int len = _tcslen(xml);
		wcscpy_s(m_aName,len+1, xml);
	}
	CControlUI* CreateControl(LPCTSTR pstrClass)
	{
		if (_tcscmp(pstrClass, m_aName) == 0) return new ComputerExamineUI(m_aName);
		return NULL;
	}
private:
	TCHAR m_aName[256];
};

//////////////////////////////////////////////////////////////////////////
///

class CMainPage : public CNotifyPump
{
public:
	CMainPage();

public:
	void SetPaintMagager(CPaintManagerUI* pPaintMgr);

	DUI_DECLARE_MESSAGE_MAP()
		virtual void OnClick(TNotifyUI& msg);
	virtual void OnSelectChanged(TNotifyUI &msg);
	virtual void OnItemClick(TNotifyUI &msg);

private:
	CPaintManagerUI* m_pPaintManager;
};

//////////////////////////////////////////////////////////////////////////
///

#define MAX_RADIUS	4.5		//最大圆半径6.5mm

#define cd_image_index	 3000
struct calc_data 
{
	int image_index[cd_image_index];
	int image_count;
	int pid;
	char buf[buf_send_recv_len];
	bool bend;
	int index;

	int*		_p_nCellsPixelArea;	//
	double *	_p_DI;
	double *	_p_dCircleDegree;	//
	double *	_p_dCellgrayvar;	//
	int *		_p_dGrayValue1;	//
	int *		_p_dODVar;
	int *		_p_dGrayMax1;	//
	int *		_p_dgrayvar;
	int *		_p_SortIndex;
	double *	_p_dBackgroundGrayMeanValue1;
	double *	_p_dEpithelialCellIOD1;	//
	double *	_p_dLymphocyteCellIOD1;
	double *	_p_dAOD1;   //

	int			_p_nCellsPixelArea_count;
	int		   	_p_DI_count;
	int		   	_p_dCircleDegree_count;
	int		   	_p_dCellgrayvar_count;
	int 		_p_dGrayValue1_count;
	int 		_p_dODVar_count;
	int 		_p_dGrayMax1_count;
	int 		_p_dgrayvar_count;
	int 		_p_SortIndex_count;
	int		   	_p_dBackgroundGrayMeanValue1_count;
	int		   	_p_dEpithelialCellIOD1_count;
	int		   	_p_dEpithelialCellIOD2_count;
	int			_p_dLymphocyteCellIOD1_count;
	int		   	_p_dAOD1_count;

	long long	_MeanLymphocyteGrayValue = 0;
	long long	_SumLymphocyteGrayValue = 0;
	int			_nCountLymphocyte = 0;       //
	long long	_SumLymphocyteAreaValue = 0;
	long long	_MeanLymphocyteIOD = 0;
	long long   _dLymphocyteIODSumValue = 0;   //

	int			_nCountCells;  //
	int			_nCountCellsAll;


	double _dIODMeanValue_temp;

#define ncount 10000
	calc_data()
	{
	}
	void newdata()
	{
		_p_nCellsPixelArea = new int[ncount];
		_p_DI = new double[ncount];
		_p_dCircleDegree = new double[ncount];
		_p_dCellgrayvar = new double[ncount];
		_p_dGrayValue1 = new int[ncount];
		_p_dODVar = new int[ncount];
		_p_dGrayMax1 = new int[ncount];
		_p_dgrayvar = new int[ncount];
		_p_SortIndex = new int[ncount];
		_p_dBackgroundGrayMeanValue1 = new double[ncount];
		_p_dEpithelialCellIOD1 = new double[ncount];
		_p_dLymphocyteCellIOD1 = new double[ncount];
		_p_dAOD1 = new double[ncount];
	}
	void deldata()
	{
		delete[] _p_nCellsPixelArea;
		delete[] _p_DI;
		delete[] _p_dCircleDegree;
		delete[] _p_dCellgrayvar;
		delete[] _p_dGrayValue1;
		delete[] _p_dODVar;
		delete[] _p_dGrayMax1;
		delete[] _p_dgrayvar;
		delete[] _p_SortIndex;
		delete[] _p_dBackgroundGrayMeanValue1;
		delete[] _p_dEpithelialCellIOD1;
		delete[] _p_dLymphocyteCellIOD1;
		delete[] _p_dAOD1;
	}
	void reset()
	{
		memset(image_index, -1, sizeof(image_index));
		image_count = 0;

		pid = -1;
		memset(buf, 0, sizeof(buf));
		bend = false;
		index = -1;

		memset(_p_nCellsPixelArea, 0, sizeof(int) * ncount);
		memset(_p_DI, 0, sizeof(double) * ncount);
		memset(_p_dCircleDegree, 0, sizeof(double) * ncount);
		memset(_p_dCellgrayvar, 0, sizeof(double) * ncount);
		memset(_p_dGrayValue1, 0, sizeof(int) * ncount);
		memset(_p_dODVar, 0, sizeof(int) * ncount);
		memset(_p_dGrayMax1, 0, sizeof(int) * ncount);
		memset(_p_dgrayvar, 0, sizeof(int) * ncount);
		memset(_p_SortIndex, 0, sizeof(int) * ncount);
		memset(_p_dBackgroundGrayMeanValue1, 0, sizeof(double) * ncount);
		memset(_p_dEpithelialCellIOD1, 0, sizeof(double) * ncount);
		memset(_p_dLymphocyteCellIOD1, 0, sizeof(double)* ncount);
		memset(_p_dAOD1, 0, sizeof(double) * ncount);

		_p_nCellsPixelArea_count = 0;
		_p_DI_count = 0;
		_p_dCircleDegree_count = 0;
		_p_dCellgrayvar_count = 0;
		_p_dGrayValue1_count = 0;
		_p_dODVar_count = 0;
		_p_dGrayMax1_count = 0;
		_p_dgrayvar_count = 0;
		_p_SortIndex_count = 0;
		_p_dBackgroundGrayMeanValue1_count = 0;
		_p_dEpithelialCellIOD1_count = 0;
		_p_dEpithelialCellIOD2_count = 0;
		_p_dLymphocyteCellIOD1_count = 0;
		_p_dAOD1_count = 0;

		_MeanLymphocyteGrayValue = 0;
		_SumLymphocyteGrayValue = 0;
		_nCountLymphocyte = 0;
		_SumLymphocyteAreaValue = 0;
		_MeanLymphocyteIOD = 0;
		_dLymphocyteIODSumValue = 0;

		_nCountLymphocyte = 0;
		_dLymphocyteIODSumValue = 0;
		_nCountCells = 0;
		_nCountCellsAll = 0;

		_dIODMeanValue_temp = 0;
	}
};

class CMyCriticalSection
{
public:
	CMyCriticalSection()
	{
		InitializeCriticalSection(&m_cSection);
	}

	void Lock()
	{
		EnterCriticalSection(&m_cSection);
	}

	void UnLock()
	{
		LeaveCriticalSection(&m_cSection);
	}


	//利用析构函数删除临界区对象
	virtual ~CMyCriticalSection()
	{
		DeleteCriticalSection(&m_cSection);
	}
private:
	CRITICAL_SECTION                        m_cSection;
};
class CCriticalSectionAutoLock
{
public:
	//利用构造函数上锁，即进去临界区
	CCriticalSectionAutoLock(CMyCriticalSection *mySection)
		:pCMySection(mySection)
	{
		pCMySection->Lock();
	}

	//利用析构函数解锁，即离开临界区
	virtual ~CCriticalSectionAutoLock()
	{
		pCMySection->UnLock();
	}
private:
	CMyCriticalSection                      *pCMySection;
};

class CMainDlgWnd : public WindowImplBase, public SkinChangedReceiver
{
public:
	CMainDlgWnd();
	~CMainDlgWnd();

public:// UI初始化
	DuiLib::CDuiString GetSkinFile();
	LPCTSTR GetWindowClassName() const;
	UINT GetClassStyle() const;
	void InitWindow();
	void OnFinalMessage(HWND hWnd);

	void _SetFocus(){ SetFocus(m_hWnd); }

public:// 接口回调
	CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual BOOL Receive(SkinChangedParam param);
	LPCTSTR QueryControlText(LPCTSTR lpstrId, LPCTSTR lpstrType);

public:// UI通知消息
	void Notify(TNotifyUI& msg);
	void OnLClick(CControlUI *pControl);

	void ClickMenuHelp(LPCTSTR name);

	DUI_DECLARE_MESSAGE_MAP()


public:// 系统消息
	LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseHover(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	virtual BOOL OnMainWndMaxMinEx(bool bMax);

public:// WebBrowser
	virtual HRESULT STDMETHODCALLTYPE UpdateUI(void);
	virtual HRESULT STDMETHODCALLTYPE GetHostInfo(CWebBrowserUI* pWeb, DOCHOSTUIINFO __RPC_FAR *pInfo);
	virtual HRESULT STDMETHODCALLTYPE ShowContextMenu(CWebBrowserUI* pWeb, DWORD dwID, POINT __RPC_FAR *ppt, IUnknown __RPC_FAR *pcmdtReserved, IDispatch __RPC_FAR *pdispReserved);

	CPaintManagerUI* GetManager(){ return &m_pm; }

	CControlUI*   m_pTab1;
private:// UI变量
	CButtonUI* m_pCloseBtn;
	CButtonUI* m_pMaxBtn;
	CButtonUI* m_pRestoreBtn;
	CButtonUI* m_pMinBtn;
	CButtonUI* m_pSkinBtn;

	CTrayIcon m_trayIcon;

	COptionUI*    m_pOptionTabUI[3];
	CTabLayoutUI* m_pTabRight;
	CTabLayoutUI* m_pTabLeft;

	CMenuWnd* m_pMenu;
	CStdStringPtrMap m_MenuInfos;
	CMenuWnd* m_pMenuFile;
	CStdStringPtrMap m_MenuFileMap;
	CMenuWndEx* m_pMenuSetting;
	CStdStringPtrMap m_MenuSettingMap;
	CMenuWnd* m_pMenuInfo;
	CStdStringPtrMap m_MenuInfoMap;
	CMenuWnd* m_pMenuReport;
	CStdStringPtrMap m_MenuReportMap;
	CMenuWnd* m_pMenuScreening;
	CStdStringPtrMap m_MenuScreeningMap;
	CMenuWnd* m_pMenuHelp;
	CStdStringPtrMap m_MenuHelpMap;
public:
	void ClearMenu();

public:
	CMainPage m_MainPage;

	BOOL _bMouseTrack = TRUE;

	int _iBingLiSel;

	int _iMenuSel;

	/////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
public:
	////int nCountNUMArray[21];
	char m_lpResultPath[MAX_PATH];
	//char m_lpResultP[MAX_PATH]; //no

	//CMyListCtrl m_List; //no
	CString m_strName; //姓名
	//BOOL m_Radio_sex; no
	//int m_iAgeLow; //no
	//int m_iAgeHigh; //no
	CString m_strBLNo; //病历号

	//COleDateTime m_TimeLow; no
	//COleDateTime m_TimeHigh; no
	//BOOL m_bChecked; //no

	_ConnectionPtr m_pConnection;
	_RecordsetPtr m_pRecordset;
	CString			  m_strSort;	//排序语句
	CString           m_strFilter;  //筛选语句
	CString m_strName_filter;
	CString m_strSex_filter;
	CString m_strAgeLow_filter;
	CString m_strAgeHigh_filter;
	CString m_strTimeLow_filter;
	CString m_strTimeHigh_filter;
	CString m_strCheck_filter;
	CString m_strBLH_filter;

public:

	void OpenFolder(CString& szFolder); // 被OnBnClickedButton1调用
	BOOL OpenDB();
	void ExitADOConn();
	void CloseDB();
	void MkFilter(int nID);
	void Save_info_todb(void);
	bool CMainDlgWnd::find_info_toid(CString sBLHID, CString& sID);

	int GetLastID();
	int m_nLastID;
	void ModifyDBByID(int nModifyID, PatiengInfo* pInfo);

	//左侧操作
	void OnBnClickedButton1();			//选择本地扫描的文件 -->OpenFolder()							[3.档案]
	void OnBnClickedBtnMicroscope();	//调用扫描软件													[2.制片]
	void OnBnClickedBtnInputinfo();		//（未使用）新建(用户信息输入) -->使用 On32814(); //病人信息    [1.新建]
	void OnBnClickedButton19();			//生成报告														[4.报告]
	void OnBnClickedButton4();			//主界面快速筛选
	void OnBnClickedButton3();			//切换到实时筛选页面
	void OnBnClickedBtnBl();			//病厉管理


	//设置
	void On32813();	//显微镜
	void On32825(); //测量半径（mm）
	void On32840(); //x向步长（um）
	void On32841(); //y向步长（um）
	void On32842(); //停止阈值

	//信息 移除
	void On32814(); //病人信息 
	void On32830(); //淋巴细胞
	void On32831(); //上皮细胞
	void On32836(); //单倍体
	void On32837(); //二倍体
	void On32838();	//三倍体
	void On32839(); //多倍体
	void On32843(); //CellsLambda
	void On32844(); //CellsName
	void On32845(); //CellsPixelArea
	void On32846(); //SortIndex

	//报告
	bool On32815(bool bModify = false, CString sBLH = _T(""), CString sPropose = _T("")); //生成报告
	void Oncheckreport();	//审核报告
	void On32826(); //打印报告

	//筛选
	void On32817(); //实时筛选

	//帮助
	void On32818();	//技术支持
	void On32819();	//联系我们
	void OnRegister(); //注册账号
	void OnModifyPwd(); //修改密码
	void OnLeftCount(); //剩余次数

	///////////////////////////////////////////////////////
	//page1

	CLabelUIEx* m_pLabelArchivePath; //档案

	void SwitchTab(int ipage); // ipage 0 1 2

	CEditUI* m_pEditCurName; //当前选中患者
	CEditUI* m_pEditCurBLH;  //病历号
	void ShowCurUser();
	void ClearCurUser();

	/////////////////////////////////////////////////////////////
	//selection page2
	bool m_bIsDisposeBegin;
	bool m_bIsDisposeEnd;
	void OnDisposeProgressEx();//开始处理 处理进度
	void SetThreadItemFileName(int index, char name[buf_send_recv_len], char* head);
	void SetThreadItemPID(int index, int pid);
	void SetThreadItemResult(int index, const char result[buf_send_recv_len]);
	void SetThreadItemEnd(int index);
	bool CheckThreadItemsEnd();
	bool GetThreadItemDataIndex(int pid, int &index);
	bool GetThreadItemFileName(const char srcbuf[buf_send_recv_len], vector<string> &vname, string& path, int& index);
	bool GetThreadItemFileNameIndex(int &index);
	int	 DisposeProgressEx(); //接收计算程序结果
	int _ReadSocket(SOCKET nFile, void * pData, int Size);
	void SplitString(vector<string>& vstr, char* src, const char* step);
	bool IsDisposeProgress() { return m_bIsDisposeBegin && !m_bIsDisposeEnd; }
	bool m_bIsDisposeCalc;
	bool IsDisposeCalc() { return m_bIsDisposeCalc; }

	CEditUI*	m_pSelectEdit[6][13];
	CControlUI* m_pSelectImg[6];
	//str0-12 面积 周长 主轴长 次轴长 紧凑度 偏心率 圆度 矩形度 离心度 椭圆度 离散度 惯性矩M02 平均灰度值
	void ShowOneCellSelectionData(int index, CString str[13]);
	void ShowOneCellSelectionImg(int index, CString strImg);
	CLabelUI* m_pProgress;
	CLabelUI* m_pProgressPercent;
	CProgressUI* m_pProgressBar;
	CGifAnimUI* m_pGifAnim;
	CGifAnimUI* m_pGifAnim1;
	CControlUI* m_pclock_begin_end;
	CControlUI* m_pAniProgress;
	void ShowPregress(CString str, int progress);
	void ShowProgressEx(int progress);
	void SetAniProgress();
	CTime m_aTime;
	int	  m_aProgress;
	bool  m_bSuc;

	int m_iExamine;
	//////////////////////////////////////////////////////////////
	//statistics page3
	CControlUI* m_pPageTab3;
	CControlUI* m_pStatisticsImg[32];
	CEditUI*	m_pStatisticsEdit[32];
	CButtonUI*  m_pStatisticsBtnShow[32];
	CButtonUI*  m_pStatisticsBtnDelete[32];
#define _row_cnt 5
#define _column_cnt 8
#define _cnt() ((_row_cnt)*(_column_cnt)) //每页的个数
#define _page 1
#define _cnt_show_page 100 //当前显示页数
#define _cnt_sel_cell  20 //显示选择细胞的个数

	struct BlockPage
	{
		CControlUI*	 image[_row_cnt][_column_cnt];
		CButtonUI*	 btndot[_row_cnt][_column_cnt];
		CButtonUI*	 btnsel[_row_cnt][_column_cnt];
		CEditUI*	 edit[_row_cnt][_column_cnt];
// 		CString		 strpath[_cnt_page][_cnt()];
// 		float		 fvalue[_cnt_page][_cnt()];
		int			 selsortindex[_cnt_sel_cell]; //每项值对应排序后的索引
		WORD		 wSelCount;
		//每项值所对应的页面索引
		int			 curselindex[_cnt_sel_cell][3]; //0 [0-_cnt_sel_cell) 1 page index 2 ui index
		BlockPage(){
			memset(image, 0, sizeof(image));
			memset(btndot, 0, sizeof(btndot));
			memset(btnsel, 0, sizeof(btnsel));
			memset(edit, 0, sizeof(edit));
			reset();
		}
		void reset()
		{
			memset(selsortindex, -1, sizeof(selsortindex));
			wSelCount = 0;
			memset(curselindex, -1, sizeof(curselindex));
		}
	};

	BlockPage* m_pBlockPage;		//一页的控件和数据结构
	CVerticalLayoutUI* m_pTab3Page;	//一页的整体大小
	CHorizontalLayoutUI* m_pTab3Hori[_row_cnt]; //一页中每行的layout
	WORD m_wCurShowPageCount; //_cnt_show_page
	WORD m_wSelPage; //[0-_cnt_show_page)
	int  m_iValidIndex; //有效的细胞按钮

	CButtonUI* m_pBtnLastPage;
	CButtonUI* m_pBtnNextPage;
	CLabelUI* m_pLabelPage;
	CLabelUI* m_pLabelSelCellCount;
	CEditUIEx1* m_pLabelPageSkip;
	CButtonUI* m_pBtnPageSkip;

	bool m_bSelFirstCell;	//选择第一个细胞
	void SetShowCell(int row, int column);	//显示细胞
	void DelShowCell(int index);	//删除细胞 index [0-31] 无用
	void delete_data(int index); //--无用

	void SelPage(WORD wSel);
	void SetPageItem();
	void SetPageItemImg(int index, CString path);
	void SetPageItemDI(int index, CString di);
	bool FindIndex(int desIndex, int& page, int& uiIndex); //desIndex 指selsortindex的索引 查找第一个细胞最后一个所在的页数
	bool IsDiIndex(int index);
	bool GetPageIndex(int page, int res[_cnt_sel_cell], int&count);
	bool GetSelCell(CString allpath[_cnt_sel_cell], CString alledit[_cnt_sel_cell]);
	void SelImg(int row, int column);

	bool m_bTatisticsInfo;
	bool m_bStatisticsShow;
	void StatisticsInfo(); //统计信息
	void ShowStatisticResult(); //显示统计结果

	bool ResetDataParam();

	////////////////////////////////////////////////////////
	CString GetExePath();
	BOOL SaveBmp(HDC hPaintDC, HBITMAP  hBitmap, CString  FileName);
	BOOL DeleteDirectory(CString DirName, BOOL bRemoveRootDir);
	DWORD GetColorDword(LPCTSTR clr);

	bool IsAdmin(bool bTip=true);

	////////////////////
	/////////////////////
	//保存20个细胞拼接图
	//不保持长宽比
	void montage(cv::Mat &output, cv::Mat &input, cv::Rect rect,
		cv::Scalar color = cv::Scalar(0));//如果拼图范围超过ouput那么自动用color颜色填充

	void montage(cv::Mat &output, cv::Mat &input, int x, int y, int width, int height, cv::Scalar color = cv::Scalar(0)) {
		montage(output, input, cv::Rect(x, y, width, height));
	}
	//保持长宽比的情况
	void montage(cv::Mat &output, cv::Mat &input, cv::Point location, double zoom_size = 1.0, cv::Scalar color = cv::Scalar(0));
	void montage(cv::Mat &output, cv::Mat &input, int x, int y, double zoom_size = 1.0, cv::Scalar color = cv::Scalar(0)); 

	void SaveCellImage(CString str[_cnt_sel_cell], CString str1[_cnt_sel_cell], CString path);


	bool FindProcess(CString strProcessName, DWORD& nPid);
	bool KillProcess(DWORD dwPid);

	int IsInvalidCell(cv::Mat& srcImage);

	BOOL CopyToClipboard(const char* pszData, int nDataLen);

	bool nyCopyFile(const std::wstring& srcpath, const std::wstring despath);
	void nyDeleteFile(/*LPCTSTR lpszPath*/ const std::wstring& srcpath);
	void nyDeleteFileA(/*LPCTSTR lpszPath*/ const std::string& srcpath);
	DWORD EnumerateFileInDrectory(LPSTR szPath, LPSTR szToPath);


};

