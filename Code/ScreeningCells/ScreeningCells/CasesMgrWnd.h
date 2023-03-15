#pragma once
#include <vector>
using namespace std;
#include "MainDlgWnd.h"

struct PatientInfo
{
	CDuiString id;
	CDuiString name;
	CDuiString sex; //0 ƒ– 1 male
	CDuiString age;
	CDuiString bingLiHao;
	CDuiString mobileNumber;
	CDuiString date;
	CDuiString examine;//…Û∫À
	CDuiString diagnose;//’Ô∂œ
	CDuiString path;
};

struct ModifyReportInfo 
{
	CString sOpinion;
	CString name;
	CString sex;
	CString age;
	CString bingLiHao;
	CString sMobile;
	bool	bValid;
	ModifyReportInfo()
	{
		bValid = false;
	}
};

extern vector<PatientInfo> gVecPatientInfos;
extern CMainDlgWnd* gMainDlgWnd;
extern struct LoginParam gLoginParam;

class CCasesMgrWnd : public WindowImplBase, public IListCallbackUI
{
public:
	static CCasesMgrWnd* pThis;
	static int MessageBox(HWND hParent = NULL)
	{
		pThis = new CCasesMgrWnd();
		pThis->Create(hParent, _T("casemgrwnd"), WS_POPUP | WS_CLIPCHILDREN, WS_EX_TOOLWINDOW);
		pThis->CenterWindow();
		return pThis->ShowModal();
	}
	static CCasesMgrWnd* GetThis()
	{
		return pThis;
	}

public:
	CCasesMgrWnd();
	~CCasesMgrWnd();

public:
	CListUI* m_pList;
	int m_iRowCount;

public:
	virtual void OnFinalMessage(HWND);
	virtual CDuiString GetSkinFile();
	virtual LPCTSTR GetWindowClassName(void) const;
	virtual void InitWindow();

	CControlUI* CreateControl(LPCTSTR pstrClass);

	DUI_DECLARE_MESSAGE_MAP()
	void Notify(TNotifyUI& msg);
	virtual void OnClick(TNotifyUI& msg);

	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void AddListData(int count, int iSel=-1);
	void AddListDataItem(PatientInfo info);
	virtual LPCTSTR GetItemText(CControlUI* pControl, int iIndex, int iSubItem);

	virtual void OnItemSelect(TNotifyUI &msg);

	////////////////////////////////////////////////////


	void LoadData(); //

	int m_curIndex;

	////////////////////////////////
	CButtonUI* m_pBtnFindName;
	CEditUI* m_pFindName;
	CButtonUI* m_pBtnSex;
	COptionUI* m_pMale;
	COptionUI* m_pFemale;
	CButtonUI* m_pBtnAge;
	CEditUI* m_pAge1;
	CEditUI* m_pAge2;
	CButtonUI* m_pBtnIsShenHe;
	COptionUI* m_pNoShenhe; //Œ¥…Û∫À
	COptionUI* m_pShenhe;
	CButtonUI* m_pBtnBingLiHao;
	CEditUI* m_pBingLiHao;
	CButtonUI* m_pBtnDate;
	CDateTimeUI* m_pDate1;
	CDateTimeUI* m_pDate2;

	CButtonUI* m_pBtnReport;
	CButtonUI* m_pBtnExamine;
	CButtonUI* m_pBtnRecalc;
	CButtonUI* m_pBtnShowAll;
	CButtonUI* m_pBtnDelete;
	CButtonUI* m_pBtnExit;
	CComboUI* m_pComboOpinion;

	CButtonUI* m_pBtnSelectedUser;

	CLabelUI* m_pTip1;
	CLabelUI* m_pTip2;

	void OnFindName();
	void OnFindSex();
	void OnFindAge();
	void OnFindShenHe();
	void OnFindBingLiHao();
	void OnFindDate();
	void OnBtnReport();
	void OnBtnExamine();
	void OnBtnRecalc();
	void OnBtnShowAll(int iSel=-1);
	void OnBtnDelete();
	void OnBtnExit();
	void OnBtnSelectedUserEx();

	void OnBtnSelectedUser(bool bClose=true, bool bTip=true);
	void ShowReport(int nListID);
	void FindReportPathByID(int nWantID, CString& strPath);
	void ModifyCheckID_ByID(int nModifyID, int nCheck);
	void ModifyPathByID(int nModifyID, CString sPath);
	bool FindInfoByID(int nModifyID);

	bool IsCurUser(int nListIndex);
	DWORD GetColorDword(LPCTSTR clr);
	void SelUserPropose(CString sBLH);
};


