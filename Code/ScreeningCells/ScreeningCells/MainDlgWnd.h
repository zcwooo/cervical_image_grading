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

#define buf_send_recv_len 8196 //ͨ������ֽ���
//��Ϣ����
#define msg_len									strlen("00")
#define msg_c_s_init							"00"			//��ʼ�����
#define msg_c_s_calcresult						"01"			//������֪ͨ
#define msg_s_c_calcfilename					"02"			//������Ϣ
#define msg_c_s_calcprocess						"03"			//����
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

#define MAX_RADIUS	4.5		//���Բ�뾶6.5mm

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


	//������������ɾ���ٽ�������
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
	//���ù��캯������������ȥ�ٽ���
	CCriticalSectionAutoLock(CMyCriticalSection *mySection)
		:pCMySection(mySection)
	{
		pCMySection->Lock();
	}

	//���������������������뿪�ٽ���
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

public:// UI��ʼ��
	DuiLib::CDuiString GetSkinFile();
	LPCTSTR GetWindowClassName() const;
	UINT GetClassStyle() const;
	void InitWindow();
	void OnFinalMessage(HWND hWnd);

	void _SetFocus(){ SetFocus(m_hWnd); }

public:// �ӿڻص�
	CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual BOOL Receive(SkinChangedParam param);
	LPCTSTR QueryControlText(LPCTSTR lpstrId, LPCTSTR lpstrType);

public:// UI֪ͨ��Ϣ
	void Notify(TNotifyUI& msg);
	void OnLClick(CControlUI *pControl);

	void ClickMenuHelp(LPCTSTR name);

	DUI_DECLARE_MESSAGE_MAP()


public:// ϵͳ��Ϣ
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
private:// UI����
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
	CString m_strName; //����
	//BOOL m_Radio_sex; no
	//int m_iAgeLow; //no
	//int m_iAgeHigh; //no
	CString m_strBLNo; //������

	//COleDateTime m_TimeLow; no
	//COleDateTime m_TimeHigh; no
	//BOOL m_bChecked; //no

	_ConnectionPtr m_pConnection;
	_RecordsetPtr m_pRecordset;
	CString			  m_strSort;	//�������
	CString           m_strFilter;  //ɸѡ���
	CString m_strName_filter;
	CString m_strSex_filter;
	CString m_strAgeLow_filter;
	CString m_strAgeHigh_filter;
	CString m_strTimeLow_filter;
	CString m_strTimeHigh_filter;
	CString m_strCheck_filter;
	CString m_strBLH_filter;

public:

	void OpenFolder(CString& szFolder); // ��OnBnClickedButton1����
	BOOL OpenDB();
	void ExitADOConn();
	void CloseDB();
	void MkFilter(int nID);
	void Save_info_todb(void);
	bool CMainDlgWnd::find_info_toid(CString sBLHID, CString& sID);

	int GetLastID();
	int m_nLastID;
	void ModifyDBByID(int nModifyID, PatiengInfo* pInfo);

	//������
	void OnBnClickedButton1();			//ѡ�񱾵�ɨ����ļ� -->OpenFolder()							[3.����]
	void OnBnClickedBtnMicroscope();	//����ɨ�����													[2.��Ƭ]
	void OnBnClickedBtnInputinfo();		//��δʹ�ã��½�(�û���Ϣ����) -->ʹ�� On32814(); //������Ϣ    [1.�½�]
	void OnBnClickedButton19();			//���ɱ���														[4.����]
	void OnBnClickedButton4();			//���������ɸѡ
	void OnBnClickedButton3();			//�л���ʵʱɸѡҳ��
	void OnBnClickedBtnBl();			//��������


	//����
	void On32813();	//��΢��
	void On32825(); //�����뾶��mm��
	void On32840(); //x�򲽳���um��
	void On32841(); //y�򲽳���um��
	void On32842(); //ֹͣ��ֵ

	//��Ϣ �Ƴ�
	void On32814(); //������Ϣ 
	void On32830(); //�ܰ�ϸ��
	void On32831(); //��Ƥϸ��
	void On32836(); //������
	void On32837(); //������
	void On32838();	//������
	void On32839(); //�౶��
	void On32843(); //CellsLambda
	void On32844(); //CellsName
	void On32845(); //CellsPixelArea
	void On32846(); //SortIndex

	//����
	bool On32815(bool bModify = false, CString sBLH = _T(""), CString sPropose = _T("")); //���ɱ���
	void Oncheckreport();	//��˱���
	void On32826(); //��ӡ����

	//ɸѡ
	void On32817(); //ʵʱɸѡ

	//����
	void On32818();	//����֧��
	void On32819();	//��ϵ����
	void OnRegister(); //ע���˺�
	void OnModifyPwd(); //�޸�����
	void OnLeftCount(); //ʣ�����

	///////////////////////////////////////////////////////
	//page1

	CLabelUIEx* m_pLabelArchivePath; //����

	void SwitchTab(int ipage); // ipage 0 1 2

	CEditUI* m_pEditCurName; //��ǰѡ�л���
	CEditUI* m_pEditCurBLH;  //������
	void ShowCurUser();
	void ClearCurUser();

	/////////////////////////////////////////////////////////////
	//selection page2
	bool m_bIsDisposeBegin;
	bool m_bIsDisposeEnd;
	void OnDisposeProgressEx();//��ʼ���� �������
	void SetThreadItemFileName(int index, char name[buf_send_recv_len], char* head);
	void SetThreadItemPID(int index, int pid);
	void SetThreadItemResult(int index, const char result[buf_send_recv_len]);
	void SetThreadItemEnd(int index);
	bool CheckThreadItemsEnd();
	bool GetThreadItemDataIndex(int pid, int &index);
	bool GetThreadItemFileName(const char srcbuf[buf_send_recv_len], vector<string> &vname, string& path, int& index);
	bool GetThreadItemFileNameIndex(int &index);
	int	 DisposeProgressEx(); //���ռ��������
	int _ReadSocket(SOCKET nFile, void * pData, int Size);
	void SplitString(vector<string>& vstr, char* src, const char* step);
	bool IsDisposeProgress() { return m_bIsDisposeBegin && !m_bIsDisposeEnd; }
	bool m_bIsDisposeCalc;
	bool IsDisposeCalc() { return m_bIsDisposeCalc; }

	CEditUI*	m_pSelectEdit[6][13];
	CControlUI* m_pSelectImg[6];
	//str0-12 ��� �ܳ� ���᳤ ���᳤ ���ն� ƫ���� Բ�� ���ζ� ���Ķ� ��Բ�� ��ɢ�� ���Ծ�M02 ƽ���Ҷ�ֵ
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
#define _cnt() ((_row_cnt)*(_column_cnt)) //ÿҳ�ĸ���
#define _page 1
#define _cnt_show_page 100 //��ǰ��ʾҳ��
#define _cnt_sel_cell  20 //��ʾѡ��ϸ���ĸ���

	struct BlockPage
	{
		CControlUI*	 image[_row_cnt][_column_cnt];
		CButtonUI*	 btndot[_row_cnt][_column_cnt];
		CButtonUI*	 btnsel[_row_cnt][_column_cnt];
		CEditUI*	 edit[_row_cnt][_column_cnt];
// 		CString		 strpath[_cnt_page][_cnt()];
// 		float		 fvalue[_cnt_page][_cnt()];
		int			 selsortindex[_cnt_sel_cell]; //ÿ��ֵ��Ӧ����������
		WORD		 wSelCount;
		//ÿ��ֵ����Ӧ��ҳ������
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

	BlockPage* m_pBlockPage;		//һҳ�Ŀؼ������ݽṹ
	CVerticalLayoutUI* m_pTab3Page;	//һҳ�������С
	CHorizontalLayoutUI* m_pTab3Hori[_row_cnt]; //һҳ��ÿ�е�layout
	WORD m_wCurShowPageCount; //_cnt_show_page
	WORD m_wSelPage; //[0-_cnt_show_page)
	int  m_iValidIndex; //��Ч��ϸ����ť

	CButtonUI* m_pBtnLastPage;
	CButtonUI* m_pBtnNextPage;
	CLabelUI* m_pLabelPage;
	CLabelUI* m_pLabelSelCellCount;
	CEditUIEx1* m_pLabelPageSkip;
	CButtonUI* m_pBtnPageSkip;

	bool m_bSelFirstCell;	//ѡ���һ��ϸ��
	void SetShowCell(int row, int column);	//��ʾϸ��
	void DelShowCell(int index);	//ɾ��ϸ�� index [0-31] ����
	void delete_data(int index); //--����

	void SelPage(WORD wSel);
	void SetPageItem();
	void SetPageItemImg(int index, CString path);
	void SetPageItemDI(int index, CString di);
	bool FindIndex(int desIndex, int& page, int& uiIndex); //desIndex ָselsortindex������ ���ҵ�һ��ϸ�����һ�����ڵ�ҳ��
	bool IsDiIndex(int index);
	bool GetPageIndex(int page, int res[_cnt_sel_cell], int&count);
	bool GetSelCell(CString allpath[_cnt_sel_cell], CString alledit[_cnt_sel_cell]);
	void SelImg(int row, int column);

	bool m_bTatisticsInfo;
	bool m_bStatisticsShow;
	void StatisticsInfo(); //ͳ����Ϣ
	void ShowStatisticResult(); //��ʾͳ�ƽ��

	bool ResetDataParam();

	////////////////////////////////////////////////////////
	CString GetExePath();
	BOOL SaveBmp(HDC hPaintDC, HBITMAP  hBitmap, CString  FileName);
	BOOL DeleteDirectory(CString DirName, BOOL bRemoveRootDir);
	DWORD GetColorDword(LPCTSTR clr);

	bool IsAdmin(bool bTip=true);

	////////////////////
	/////////////////////
	//����20��ϸ��ƴ��ͼ
	//�����ֳ����
	void montage(cv::Mat &output, cv::Mat &input, cv::Rect rect,
		cv::Scalar color = cv::Scalar(0));//���ƴͼ��Χ����ouput��ô�Զ���color��ɫ���

	void montage(cv::Mat &output, cv::Mat &input, int x, int y, int width, int height, cv::Scalar color = cv::Scalar(0)) {
		montage(output, input, cv::Rect(x, y, width, height));
	}
	//���ֳ���ȵ����
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

