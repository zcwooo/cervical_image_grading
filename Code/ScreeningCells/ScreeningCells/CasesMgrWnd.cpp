#include "stdafx.h"
#include "CasesMgrWnd.h"
#include "ControlEx.h"
#include "ListTextElementUIEX.h"
#include "DatabaseMgr.h"
#include "MenuWndEx.h"

ModifyReportInfo g_ModifyReportInfo;

static void InitPath(TCHAR * lpBin, TCHAR  *lpINI, TCHAR  *lpLog)
{
	TCHAR lpPath[MAX_PATH];
	//char lpPath[MAX_PATH];
	//CString lpPath;

#if 1
	//方法1
	GetModuleFileName(NULL, lpPath, sizeof(lpPath) - 1);
	PathRemoveFileSpec(lpPath);
	PathRemoveBackslash(lpPath);
	PathAddBackslash(lpPath);
#else
	//方法2
	//获取路径
	::GetModuleFileName(NULL, lpPath, MAX_PATH - 1);
	PathAddBackslash(lpPath);
	*(strrchr(lpPath, '\\')) = '\0';
	strcat(lpPath, "\\");
#endif

	if (lpBin != NULL)
		//sprintf(lpBin, "%s", lpPath);
		swprintf(lpBin, 200, _T("%s"), lpPath);
	if (lpINI != NULL)
		swprintf(lpINI, 200, _T("%s%s"), lpPath, "pst.ini");
	if (lpLog != NULL)
		swprintf(lpLog, 200, _T("%s"), lpPath);
	//sprintf(lpLog,"%s%s",lpPath,"log.inf");
}

DUI_BEGIN_MESSAGE_MAP(CCasesMgrWnd, WindowImplBase)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_ON_MSGTYPE(DUI_MSGTYPE_ITEMSELECT, OnItemSelect)
DUI_END_MESSAGE_MAP()

CCasesMgrWnd* CCasesMgrWnd::pThis = NULL;

vector<PatientInfo> gVecPatientInfos;

extern PatiengInfo g_PatiengInfo;
extern PatientInfoCur g_PatientInfoCur;
extern CString m_FileDir;

CCasesMgrWnd::CCasesMgrWnd()
{
	m_curIndex = -1;
}


CCasesMgrWnd::~CCasesMgrWnd()
{
}


void CCasesMgrWnd::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

DuiLib::CDuiString CCasesMgrWnd::GetSkinFile()
{
	return _T("xmls/casesmgr.xml");
}

LPCTSTR CCasesMgrWnd::GetWindowClassName(void) const
{
	return _T("CasesMgrWnd");
}

void CCasesMgrWnd::InitWindow()
{
	m_pBtnFindName = static_cast<CButtonUI*>(m_pm.FindControl(_T("btnfindname")));
	m_pFindName = static_cast<CEditUI*>(m_pm.FindControl(_T("editname")));
	m_pBtnSex = static_cast<CButtonUI*>(m_pm.FindControl(_T("btnfindsex")));
	m_pMale = static_cast<COptionUI*>(m_pm.FindControl(_T("radio3")));
	m_pFemale = static_cast<COptionUI*>(m_pm.FindControl(_T("radio4")));
	m_pBtnAge = static_cast<CButtonUI*>(m_pm.FindControl(_T("btnfindage")));
	m_pAge1 = static_cast<CEditUI*>(m_pm.FindControl(_T("editage1")));
	m_pAge2 = static_cast<CEditUI*>(m_pm.FindControl(_T("editage2")));
	m_pBtnIsShenHe = static_cast<CButtonUI*>(m_pm.FindControl(_T("btnfindshenhe")));
	m_pNoShenhe = static_cast<COptionUI*>(m_pm.FindControl(_T("radio1")));
	m_pShenhe = static_cast<COptionUI*>(m_pm.FindControl(_T("radio2")));
	m_pBtnBingLiHao = static_cast<CButtonUI*>(m_pm.FindControl(_T("btnfindbinglihao")));
	m_pBingLiHao = static_cast<CEditUI*>(m_pm.FindControl(_T("editbinglihao")));
	m_pBtnDate = static_cast<CButtonUI*>(m_pm.FindControl(_T("btnfindate")));
	m_pDate1 = static_cast<CDateTimeUI*>(m_pm.FindControl(_T("time1")));
	m_pDate2 = static_cast<CDateTimeUI*>(m_pm.FindControl(_T("time2")));

	m_pBtnReport = static_cast<CButtonUI*>(m_pm.FindControl(_T("btnreport")));
	m_pBtnExamine = static_cast<CButtonUI*>(m_pm.FindControl(_T("btnexamine")));
	m_pBtnRecalc = static_cast<CButtonUI*>(m_pm.FindControl(_T("btnrecalc")));
	m_pBtnShowAll = static_cast<CButtonUI*>(m_pm.FindControl(_T("btnshowall")));
	m_pBtnDelete = static_cast<CButtonUI*>(m_pm.FindControl(_T("btndelete")));
	m_pBtnExit = static_cast<CButtonUI*>(m_pm.FindControl(_T("btnexit")));
	m_pComboOpinion = static_cast<CComboUI*>(m_pm.FindControl(_T("combomodify")));

	m_pBtnSelectedUser = static_cast<CButtonUI*>(m_pm.FindControl(_T("btnselecteduser")));
	//m_pBtnSelectedUser->SetEnabled(!g_bScreening);

	m_pTip1 = static_cast<CLabelUI*>(m_pm.FindControl(_T("tip1")));
	m_pTip2 = static_cast<CLabelUI*>(m_pm.FindControl(_T("tip2")));
	CString str(_T("ID:   姓名:      病历号:       "));
	if (g_PatientInfoCur.sID.GetLength() > 0 && g_PatientInfoCur.sName.GetLength() > 0 && g_PatientInfoCur.sBLH.GetLength() > 0)
	{
		str.Format(_T("ID:%s 姓名:%s 病历号:%s"), g_PatientInfoCur.sID.GetString(), g_PatientInfoCur.sName.GetString(), g_PatientInfoCur.sBLH.GetString());
	}
	m_pTip2->SetText(str.GetString());

	m_pList = static_cast<CListUI*>(m_pm.FindControl(_T("patientlist")));
	m_pList->SetContextMenuUsed(true);
	m_pList->SetItemRSelected(true);
	m_pList->RemoveAll();
	m_pList->SetTextCallback(this);
	//	m_pList->SetSelectedItemTextColor(GetColorDword(_T("ffff0000")));

	m_iRowCount = 0;

	// 	PatientInfo item;
	// 	item.id = _T("1");
	// 	item.name = _T("张三");
	// 	gVecPatientInfos.push_back(item);
	// 	item.id = _T("2");
	// 	item.name = _T("李四");
	// 	gVecPatientInfos.push_back(item);
	// 	item.id = _T("3");
	// 	item.name = _T("王五");
	// 	gVecPatientInfos.push_back(item);
	// 	AddListData(gVecPatientInfos.size());
	OnBtnShowAll();

	if (!g_PatientInfoCur.sBLH.IsEmpty())
	{
		SelUserPropose(g_PatientInfoCur.sBLH);
	}

	gMainDlgWnd->_iBingLiSel = -1;
}

CControlUI* CCasesMgrWnd::CreateControl(LPCTSTR pstrClass)
{
	return NULL;
}

void CCasesMgrWnd::Notify(TNotifyUI& msg)
{
	CDuiString name = msg.pSender->GetName();
	if (msg.sType == DUI_MSGTYPE_MENU)
	{
		m_curIndex = m_pList->GetCurSel();
	}
	else if (msg.sType == _T("ldbclick"))
	{
		OnBtnReport();
	}
	else if (msg.sType == _T("lrightclick"))
	{
		//if (msg.pSender->GetName() != _T("patientlist")) return;
		CMenuWndRight* pMenu = new CMenuWndRight();
		if (pMenu == NULL) { return; }
		pMenu->setXmlFile(_T("xmls/menu.xml"));
		POINT pt = { msg.ptMouse.x, msg.ptMouse.y };
		::ClientToScreen(*this, &pt);
		pMenu->Init(m_pList, pt, m_hWnd);
	}
	else if (msg.sType == _T("lleftclick"))
	{
		int iSel = m_pList->GetCurSel();
		if (iSel >= 0)
		{
			int nListID = -10000;
			CListTextElementUIEX* pControl = (CListTextElementUIEX*)m_pList->GetItemAt(iSel);
			if (pControl)
			{
				CString sText = pControl->GetText(0);
				nListID = _wtoi(sText.GetString());
				if (nListID != -10000)
				{
					CString sBLH = pControl->GetText(4);
					if (!g_PatientInfoCur.sBLH.IsEmpty() && g_PatientInfoCur.sBLH.Compare(sBLH) == 0)
					{
						if (IsCurUser(iSel))
						{
							if (gMainDlgWnd->IsDisposeProgress())
							{
								return;
							}
						}
					}

					SelUserPropose(sBLH);
				}
			}
		}
	}
	else if (msg.sType == _T("dbclick"))
	{

	}
	else if (msg.sType == _T("itemclick")) {

	}
	else if (msg.sType == _T("predropdown") && name == _T("combomodify"))
	{
		CComboUI* combomodify = static_cast<CComboUI*>(m_pm.FindControl(_T("combomodify")));
		int i = combomodify->GetCurSel();
	}

	WindowImplBase::Notify(msg);
}

void CCasesMgrWnd::OnClick(TNotifyUI &msg)
{
	CDuiString sName = msg.pSender->GetName();
	sName.MakeLower();
	if (0 == sName.CompareNoCase(_T("close")))
	{
		Close(MSGID_OK);
	}
	else if (0 == sName.CompareNoCase(_T("btnfindname")))
	{
		// 		m_pList->SetMultiSelect(true);
		// 		AddListDataItem();
		OnFindName();
	}
	else if (0 == sName.CompareNoCase(_T("btnfindsex")))
	{
		//		CControlUI* pUI = m_pList->GetItemAt(0);
		//		int i1 = m_pList->GetItemIndex(pUI);
		//		CControlUI* pUI1 = m_pList->GetItemAt(1);
		//		int i2 = m_pList->GetItemIndex(pUI1);
		//		CControlUI* pUI2 = m_pList->GetItemAt(2);
		//		int i3 = m_pList->GetItemIndex(pUI2);
		//
		//		CControlUI* pUI3 = m_pList->GetItemAt(3);
		//		int i4 = m_pList->GetItemIndex(pUI3);
		//		m_pList->SelectItem(3);
		//
		//		//m_pList->SelectItem(0);
		//// 		m_pList->SelectItem(1);
		//// 		m_pList->SelectItem(2);
		//		//m_pList->SelectAllItems();
		//		int icount = m_pList->GetSelectItemCount();
		//		int in = m_pList->GetCount();

		OnFindSex();
	}
	else if (0 == sName.CompareNoCase(_T("btnfindage")))
	{
		// 		m_pList->SelectItem(2);
		// 		//UpdateWindow(m_hWnd);
		// 		m_pList->SelectItem(3);
		// 		//UpdateWindow(m_hWnd);

		// 		CControlUI* pUI = m_pList->GetItemAt(0);
		// 		int i1 = m_pList->GetItemIndex(pUI);
		// 		m_pList->SetItemIndex(pUI, 3);
		// 
		// 		CControlUI* pUI1 = m_pList->GetItemAt(1);
		// 		int i2 = m_pList->GetItemIndex(pUI1);
		// 		m_pList->SetItemIndex(pUI1, 2);
		// 
		// 		CControlUI* pUI2 = m_pList->GetItemAt(2);
		// 		int i3 = m_pList->GetItemIndex(pUI2);
		// 		m_pList->SetItemIndex(pUI2, 1);
		// 
		// 		CControlUI* pUI3 = m_pList->GetItemAt(3);
		// 		int i4 = m_pList->GetItemIndex(pUI3);
		// 		m_pList->SetItemIndex(pUI3, 0);
		// 		m_pList->SelectItem(3);

		OnFindAge();
	}

	else if (0 == sName.CompareNoCase(_T("btnfindshenhe")))
	{
		OnFindShenHe();
	}
	else if (0 == sName.CompareNoCase(_T("btnfindbinglihao")))
	{
		OnFindBingLiHao();
	}
	else if (0 == sName.CompareNoCase(_T("btnfindate")))
	{
		OnFindDate();
	}
	else if (0 == sName.CompareNoCase(_T("btnreport")))
	{
		OnBtnReport();
	}
	else if (0 == sName.CompareNoCase(_T("btnexamine")))
	{
		OnBtnExamine();
	}
	else if (0 == sName.CompareNoCase(_T("btnrecalc")))
	{
		OnBtnRecalc();
	}
	else if (0 == sName.CompareNoCase(_T("btnshowall")))
	{
		OnBtnShowAll();
	}
	else if (0 == sName.CompareNoCase(_T("btndelete")))
	{
		OnBtnDelete();
	}
	else if (0 == sName.CompareNoCase(_T("btnexit")))
	{
		OnBtnExit();
	}
	else if (0 == sName.CompareNoCase(_T("btnselecteduser")))
	{
		OnBtnSelectedUserEx();
	}
}

LRESULT CCasesMgrWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch (uMsg)
	{
	case WM_USER+2000:
		if (m_curIndex >= 0 && m_curIndex < m_pList->GetCount())
		{
			int val = (int)wParam;
			if (0 == val)
			{//选择样本
				OnBtnSelectedUserEx();
			}
			else if (1 == val)
			{//显示报告
				OnBtnReport();
			}
			else if (2 == val) //这段逻辑来自于 OnBtnReport()
			{//打开路径
				int nListID = -10000;
				CListTextElementUIEX* pControl = (CListTextElementUIEX*)m_pList->GetItemAt(m_curIndex);
				if (pControl)
				{
					CString sText = pControl->GetText(0);
					nListID = _wtoi(sText.GetString());
					if (nListID != -10000)
					{
						CString sBLH = pControl->GetText(4);
						if (!g_PatientInfoCur.sBLH.IsEmpty() && g_PatientInfoCur.sBLH.Compare(sBLH) == 0)
						{
							if (IsCurUser(m_curIndex) && gMainDlgWnd->IsDisposeProgress())
							{
								// 									SOUND_WARNING
								// 									::MessageBox(m_hWnd, _T("当前样本检测中，请等待操作完成."), _T("警告"), MB_OKCANCEL);
								// 									return;
							}
						}

						CString strPath;
						FindReportPathByID(nListID, strPath);
						int ipos = strPath.ReverseFind('\\');
						if (ipos != -1)
						{
							CString path;
							path = strPath.Mid(0, ipos + 1);
							if (!PathFileExists(path.GetString()))
							{
								SOUND_WARNING
								::MessageBox(m_hWnd, _T("文件路径错误."), _T("警告"), MB_OKCANCEL);
							}
							CStringA patha;
							patha = path;
							ShellExecuteA(NULL, "open", NULL, NULL, patha.GetString(), SW_SHOWNORMAL);
						}
						else
						{
							SOUND_WARNING
							::MessageBox(m_hWnd, _T("文件路径错误."), _T("警告"), MB_OKCANCEL);
						}
					}
				}
			}
			else if (3 == val)
			{//删除
				OnBtnDelete();
			}

			bHandled = true;
			return 0;
		}

	default:
		break;
	}

	bHandled = FALSE;
	return 0;
}

LRESULT CCasesMgrWnd::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0L;
}

void CCasesMgrWnd::AddListData(int count, int iSel)
{
	if (count <= 0)
		return;
	for (int i = 0; i < count; ++i)
	{
		CListTextElementUIEX* pListElement = new CListTextElementUIEX;
		if (pListElement != NULL)
		{
			//CLabelUI* pUI = new CLabelUI;
			pListElement->SetOwner(m_pList);
			PatientInfo info = gVecPatientInfos[i];
			pListElement->SetText(0, info.id);
			pListElement->SetText(1, info.name);
			pListElement->SetText(2, info.sex);
			pListElement->SetText(3, info.age);
			pListElement->SetText(4, info.bingLiHao);
			pListElement->SetText(5, info.mobileNumber);
			pListElement->SetText(6, info.date);
			pListElement->SetText(7, info.examine);
			pListElement->SetText(8, info.diagnose);
			pListElement->SetText(9, info.path);
			m_pList->AddAt(pListElement, i);

			if (iSel>-1)
			{
				m_pList->SelectItem(iSel);
			}
			else
			{
				if (!g_PatientInfoCur.sBLH.IsEmpty() && g_PatientInfoCur.sBLH.Compare(info.bingLiHao) == 0)
				{
					m_pList->SelectItem(i);
				}
			}
		}
	}
}

void CCasesMgrWnd::AddListDataItem(PatientInfo info)
{
	int i = m_pList->GetCurSel();
	CListTextElementUIEX* pListElement = new CListTextElementUIEX;
	if (pListElement != NULL)
	{
		int in = m_pList->GetCount();
		pListElement->SetTag(in);
		m_pList->GetTag();

		pListElement->SetText(0, info.id);
		pListElement->SetText(1, info.name);
		pListElement->SetText(2, info.sex);
		pListElement->SetText(3, info.age);
		pListElement->SetText(4, info.bingLiHao);
		pListElement->SetText(5, info.mobileNumber);
		pListElement->SetText(6, info.date);
		pListElement->SetText(7, info.examine);
		pListElement->SetText(8, info.diagnose);
		pListElement->SetText(9, info.path);

		gVecPatientInfos.insert(gVecPatientInfos.begin(), info);
		m_pList->AddAt(pListElement, 0);
	}
}

LPCTSTR CCasesMgrWnd::GetItemText(CControlUI* pControl, int iIndex, int iSubItem)
{
	TCHAR szBuf[MAX_PATH] = { 0 };

	if (iIndex >= gVecPatientInfos.size())
	{
		return L"";
	}
	CDuiString sTemp = _T("");
	PatientInfo& info = gVecPatientInfos[iIndex];
	switch (iSubItem)
	{
	case 0:
	{
		sTemp = info.id;
		break;
	}
	case 1:
	{
		sTemp = info.name;
		break;
	}
	case 2:
	{
		sTemp = info.sex;
		break;
	}
	case 3:
	{
		sTemp = info.age;
		break;
	}
	case 4:
	{
		sTemp = info.bingLiHao;
		break;
	}
	case 5:
	{
		sTemp = info.mobileNumber;
		break;
	}
	case 6:
	{
		sTemp = info.date;
		break;
	}
	case 7:
	{
		CDuiString sExam(_T("未审核"));
		if (info.examine.Compare(_T("1")) == 0)
			sExam = _T("已审核");
		sTemp = sExam;
		break;
	}
	case 8:
	{
		sTemp = info.diagnose;
		CString s(sTemp.GetData());
		int pos = s.Find(_T(":"));
		if (pos>0)
		{
			s = s.Mid(pos+1);
			sTemp = s.GetString();
		}
		break;
	}
	case 9:
	{
		sTemp = info.path;
		break;
	}
	}
	pControl->SetUserData(sTemp);
	return pControl->GetUserData();
}

void CCasesMgrWnd::OnItemSelect(TNotifyUI &msg)
{
	CDuiString str = msg.pSender->GetClass();
	if (str.Compare(_T("ListUI")) == 0)
	{
		CListUI* pListUI = static_cast<CListUI*>(msg.pSender);
		CControlUI* pC = pListUI->GetItemAt(0);
		CDuiString str1 = pC->GetClass();
		pC->SetBkColor(gMainDlgWnd->GetColorDword(_T("ffffff66")));
		//pC->SetVisible(false);
		pC->SetText(_T("aaaaaaaaaaaaaaaaaaaaaaaaaaa"));
		CDuiString s1 = pC->GetParent()->GetClass();
		CDuiString s2 = pC->GetParent()->GetParent()->GetClass();

		gMainDlgWnd->_iBingLiSel = pListUI->GetCurSel();

		//CDuiString str1 = msg.pSender->GetItemAt(1);
		//msg.pSender->GetParent()->GetParent()->SetBkColor(gMainDlgWnd->GetColorDword(_T("ffff00ff")));
		//msg.pSender->SetBkColor(gMainDlgWnd->GetColorDword(_T("ff0000ff")));
		int i = -1;
	}
}

void CCasesMgrWnd::LoadData()
{
	_ConnectionPtr m_pConnection = gMainDlgWnd->m_pConnection;
	_RecordsetPtr m_pRecordset = gMainDlgWnd->m_pRecordset;

	//BeginWaitCursor();

	m_pList->RemoveAll();
	gVecPatientInfos.clear();

#if 0
	try
	{
		CString search_sql;
		//search_sql.Format(_T("SELECT * FROM patient_info where "));
		//search_sql = "SELECT * FROM patient_info where patient_name='张三'";
		search_sql = "SELECT * FROM patient_info where ID=2";
		m_pConnection->Execute(search_sql.AllocSysString(), NULL, adCmdText);
	}
	catch (_com_error e)
	{
		AfxMessageBox(_T("搜索失败！"));
		return;
	}

#endif
	_variant_t var;
	CString strName, strBLNo, strPhone, strDiag, strV;;
	int nSex, nAge, nCheckID;
	DATE date;

	try
	{
		if (!m_pRecordset->BOF)
			m_pRecordset->MoveFirst();
		else
		{
			//AfxMessageBox(_T("表内数据为空"));
			//			::MessageBox(m_hWnd, _T("未查询符合条件的数据"), _T("警告"), MB_OK);
			return;
		}
		// 读入库中各字段并加入列表框中

		int row = 0;
		int nCol = 0;
		while (!m_pRecordset->adoEOF)
		{
			PatientInfo info;

			nCol = 0;
			var = m_pRecordset->GetCollect("ID");
			if (var.vt != VT_NULL)
			{
				int nID = (int)(var);
				strV.Format(_T("%d"), nID);
				info.id = strV;
			}
			nCol++;

			var = m_pRecordset->GetCollect("patient_name");
			if (var.vt != VT_NULL)
			{
				strName = (LPCSTR)_bstr_t(var);
				info.name = strName;
			}
			nCol++;
			var = m_pRecordset->GetCollect("sex");
			if (var.vt != VT_NULL)
			{
				nSex = (int)(var);
				if (nSex)
					info.sex = _T("女");
				else
					info.sex = _T("男");
			}
			nCol++;
			var = m_pRecordset->GetCollect("age");
			if (var.vt != VT_NULL)
			{
				nAge = (int)(var);
				CString strAge;
				strAge.Format(_T("%d"), nAge);
				info.age = strAge;
			}
			nCol++;
			var = m_pRecordset->GetCollect("BL_ID");
			if (var.vt != VT_NULL)
			{
				strBLNo = (LPCSTR)_bstr_t(var);
				info.bingLiHao = strBLNo;
			}
			nCol++;
			var = m_pRecordset->GetCollect("PhoneNum");
			if (var.vt != VT_NULL)
			{
				strPhone = (LPCSTR)_bstr_t(var);
				info.mobileNumber = strPhone;
			}
			nCol++;
			var = m_pRecordset->GetCollect("create_date");
			if (var.vt != VT_NULL)
			{
				date = (DATE)(var);
				COleDateTime a_Time = date;
				strV = a_Time.Format();
				info.date = strV;
			}
			nCol++;
			var = m_pRecordset->GetCollect("check_ID");
			if (var.vt != VT_NULL)
			{
				nCheckID = (int)(var);
				strV.Format(_T("%d"), nCheckID);
				info.examine = strV;
			}
			nCol++;
			var = m_pRecordset->GetCollect("diagnose_propose");
			if (var.vt != VT_NULL)
			{
				strDiag = (LPCSTR)_bstr_t(var);
				info.diagnose = strDiag;
			}
			nCol++;
			var = m_pRecordset->GetCollect("path");
			if (var.vt != VT_NULL)
			{
				strV = (LPCSTR)_bstr_t(var);
				info.path = strV;
			}

			gVecPatientInfos.push_back(info);

			m_pRecordset->MoveNext();
			row++;
		}
		m_iRowCount = row;

		//AddListData(m_iRowCount);
	}

	catch (_com_error *e)
	{
		AfxMessageBox(e->ErrorMessage());
	}

	//EndWaitCursor();
}

void CCasesMgrWnd::OnFindName()
{
	CString sName = m_pFindName->GetText();
	sName.Trim();
	if (sName.IsEmpty())
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("请输入姓名"), _T("警告"), MB_OK);
		return;
	}

	CDatabaseMgr* pMgr = CDatabaseMgr::GetInstance();
	CString sql = pMgr->GetSQLString(1, sName, _T(""));
	if (pMgr->OpenDB(sql))
	{
		m_pList->RemoveAll();
		gVecPatientInfos.clear();
		pMgr->GetSQLRecords(gVecPatientInfos);
		pMgr->CloseDB();
		m_iRowCount = gVecPatientInfos.size();

		AddListData(m_iRowCount);
	}
}

void CCasesMgrWnd::OnFindSex()
{
	bool bFemale = m_pFemale->IsSelected();
	CString sSex;
	if (bFemale)
	{
		sSex = "1";	//nv
	}
	else
		sSex = "0";	//nan

	CDatabaseMgr* pMgr = CDatabaseMgr::GetInstance();
	CString sql = pMgr->GetSQLString(2, sSex, _T(""));
	if (pMgr->OpenDB(sql))
	{
		m_pList->RemoveAll();
		gVecPatientInfos.clear();
		pMgr->GetSQLRecords(gVecPatientInfos);
		pMgr->CloseDB();
		m_iRowCount = gVecPatientInfos.size();

		AddListData(m_iRowCount);
	}
}

void CCasesMgrWnd::OnFindAge()
{
	CString sAge1 = m_pAge1->GetText();
	CString sAge2 = m_pAge2->GetText();
	if (sAge1.IsEmpty() || sAge2.IsEmpty())
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("请输入年龄"), _T("警告"), MB_OK);
		return;
	}
	int iAge1, iAge2;
	iAge1 = _wtoi(sAge1.GetString());
	iAge2 = _wtoi(sAge2.GetString());
	if (iAge1 > 100 || iAge2 > 100)
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("年龄不能超过100"), _T("警告"), MB_OK);
		return;
	}
	int iTemp = max(iAge1, iAge2);
	int iTemp2 = min(iAge1, iAge2);
	iAge1 = iTemp2;
	iAge2 = iTemp;

	CString str1, str2;
	str1.Format(_T("%d"), iAge1);
	str2.Format(_T("%d"), iAge2);

	CDatabaseMgr* pMgr = CDatabaseMgr::GetInstance();
	CString sql = pMgr->GetSQLString(3, str1, str2);
	if (pMgr->OpenDB(sql))
	{
		m_pList->RemoveAll();
		gVecPatientInfos.clear();
		pMgr->GetSQLRecords(gVecPatientInfos);
		pMgr->CloseDB();
		m_iRowCount = gVecPatientInfos.size();

		AddListData(m_iRowCount);
	}
}

void CCasesMgrWnd::OnFindShenHe()
{
	bool bNoShenHe = m_pNoShenhe->IsSelected();
	CString str1;
	if (bNoShenHe)
		str1 = "0";	//未审核
	else
		str1 = "1";	//审核

	CDatabaseMgr* pMgr = CDatabaseMgr::GetInstance();
	CString sql = pMgr->GetSQLString(6, str1, _T(""));
	if (pMgr->OpenDB(sql))
	{
		m_pList->RemoveAll();
		gVecPatientInfos.clear();
		pMgr->GetSQLRecords(gVecPatientInfos);
		pMgr->CloseDB();
		m_iRowCount = gVecPatientInfos.size();

		AddListData(m_iRowCount);
	}
}

void CCasesMgrWnd::OnFindBingLiHao()
{
	CString sNum = m_pBingLiHao->GetText();
	if (sNum.IsEmpty())
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("请输入病历号"), _T("警告"), MB_OK);
		return;
	}

	CDatabaseMgr* pMgr = CDatabaseMgr::GetInstance();
	CString sql = pMgr->GetSQLString(4, sNum, _T(""));
	if (pMgr->OpenDB(sql))
	{
		m_pList->RemoveAll();
		gVecPatientInfos.clear();
		pMgr->GetSQLRecords(gVecPatientInfos);
		pMgr->CloseDB();
		m_iRowCount = gVecPatientInfos.size();

		AddListData(m_iRowCount);
	}
}

void CCasesMgrWnd::OnFindDate()
{
	SYSTEMTIME sys1 = m_pDate1->GetTime();
	SYSTEMTIME sys2 = m_pDate2->GetTime();
	CString str1, str2;
	str1.Format(_T("%d-%d-%d"), sys1.wYear, sys1.wMonth, sys1.wDay);
	str2.Format(_T("%d-%d-%d"), sys2.wYear, sys2.wMonth, sys2.wDay);

	CDatabaseMgr* pMgr = CDatabaseMgr::GetInstance();
	CString sql = pMgr->GetSQLString(5, str1, str2);
	if (pMgr->OpenDB(sql))
	{
		m_pList->RemoveAll();
		gVecPatientInfos.clear();
		pMgr->GetSQLRecords(gVecPatientInfos);
		pMgr->CloseDB();
		m_iRowCount = gVecPatientInfos.size();

		AddListData(m_iRowCount);
	}
}

void CCasesMgrWnd::OnBtnReport()
{
	int iSel = m_pList->GetCurSel();
	if (iSel >= 0)
	{
		int nListID = -10000;
		CListTextElementUIEX* pControl = (CListTextElementUIEX*)m_pList->GetItemAt(iSel);
		if (pControl)
		{
			//			{
			//// 				CControlUI* pControl1 = m_pList->GetItemAt(0);
			//// 				pControl1->SetBorderColor(GetColorDword(_T("ffff0000")));
			//// 				pControl1->SetBorderSize(5);			//
			//				
			//				CListBodyUI* plist = (CListBodyUI*)m_pList->GetList();
			//				//plist->SetVisible(false);
			//				CControlUI* pContrl = plist->m_pOwner->GetItemAt(1);
			//				CDuiString sss = pContrl->GetClass();
			//				CDuiString ss = pContrl->GetParent()->GetClass();
			//				CDuiString s23s = pContrl->GetParent()->GetParent()->GetClass();
			//				//pContrl->SetVisible(false);
			//				CDuiString sname = pContrl->GetName();
			//				//pContrl->SetAttribute(_T("padding"), _T("5,50,5,5"));
			////				//pContrl->SetAttribute(_T("childpadding"), _T("5,5,5,5"));
			////				//CControlUI* pContrl = plist->GetItemAt(0);
			////				//pContrl->SetAttribute(_T("inset"), _T("5,5,5,5"));
			////				//pContrl->SetBkColor(GetColorDword(_T("ffff0000")));
			////				pContrl->SetBorderSize(1);
			////				pContrl->SetBorderColor(GetColorDword(_T("ffff0000")));
			////// 				CString s = pContrl->GetText();
			////// 				OutputDebugString(_T("ggggggggg"));
			////// 				OutputDebugString(s.GetString());
			//
			////				pContrl->GetParent()->SetBkColor(GetColorDword(_T("ffff0000")));
			//
			//// 				pContrl->GetParent()->SetBorderSize(1);
			//// 				pContrl->GetParent()->SetBorderColor(GetColorDword(_T("ff00ff00")));
			//				pContrl->GetParent()->SetAttribute(_T("inset"), _T("1,1,1,1"));
			//				pContrl->GetParent()->SetAttribute(_T("childpadding"), _T("1"));
			//
			//				pContrl->SetBorderSize(1);
			//				pContrl->SetBorderColor(GetColorDword(_T("ff00ff00")));
			//			}

			CString sText = pControl->GetText(0);
			nListID = _wtoi(sText.GetString());
			if (nListID != -10000)
			{
				CString sBLH = pControl->GetText(4);
				if (!g_PatientInfoCur.sBLH.IsEmpty() && g_PatientInfoCur.sBLH.Compare(sBLH) == 0)
				{
					if (IsCurUser(iSel) && gMainDlgWnd->IsDisposeProgress())
					{
						SOUND_WARNING
						::MessageBox(m_hWnd, _T("当前样本检测中，请等待操作完成."), _T("警告"), MB_OKCANCEL);
						return;
					}
				}

				ShowReport(nListID);
			}
		}
	}
	else
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("请选择一个病厉"), _T("警告"), MB_OK);
		return;
	}
}

void CCasesMgrWnd::OnBtnExamine()
{
	// 	int iret = ::MessageBox(m_hWnd, _T("请确认是否审核通过."), _T("提示"), MB_OKCANCEL);
	// 	if (iret != IDOK)
	// 	{
	// 		return;
	// 	}

	g_FLOW = FLOW_REPORTECHECKED;

	int iSel = m_pList->GetCurSel();
	if (iSel >= 0)
	{
		int nListID = -10000;
		CListTextElementUIEX* pControl = (CListTextElementUIEX*)m_pList->GetItemAt(iSel);
		if (pControl)
		{
			CString sText = pControl->GetText(0);
			nListID = _wtoi(sText.GetString());
			if (nListID != -10000)
			{
				CString sBLH = pControl->GetText(4);
				if (!g_PatientInfoCur.sBLH.IsEmpty() && g_PatientInfoCur.sBLH.Compare(sBLH) == 0)
				{
					if (IsCurUser(iSel) && gMainDlgWnd->IsDisposeProgress())
					{
						SOUND_WARNING
						::MessageBox(m_hWnd, _T("当前样本检测中，请等待操作完成."), _T("警告"), MB_OKCANCEL);
						return;
					}
				}

				CString sPath = pControl->GetText(9);
				if (!::PathFileExists(sPath.GetString()))
				{
					SOUND_WARNING
					::MessageBox(m_hWnd, _T("报告路径不存在，请重新生成报告."), _T("警告"), MB_OKCANCEL);
					return;
				}

				CString sText = m_pComboOpinion->GetText();
// 				if (m_pComboOpinion->GetCurSel() == 0)
// 				{
// 					sText = _T(" 无修改意见");
// 					g_ModifyReportInfo.bValid = true;
// 				}
// 				else
// 				{
// 					g_ModifyReportInfo.bValid = true;
// 					g_ModifyReportInfo.sOpinion = sText;
// 
// 				}

				bool b = FindInfoByID(nListID);//初始化数据
				{
					if (m_pComboOpinion->GetCurSel() > 0)
					{
						CString s1(_T("定期复查(1年)")), s2(_T("月复查")), s3(_T("活"));
						CString s(_T("定期复查(1年)"));
						if (sText.Find(s1) > 0)
						{
						}
						else if (sText.Find(s2) > 0)
						{
							s = _T("3～6月复查");
						}
						else if (sText.Find(s3) > 0)
						{
							s = _T("阴道镜检查及活体组织检查");
						}
						bool bSuc = gMainDlgWnd->On32815(true, sBLH, s);
					}
					//if (bSuc)
					{
						ModifyCheckID_ByID(nListID, 1);
						OnBtnShowAll(iSel);
						SOUND_SUCCEED
						::MessageBox(m_hWnd, _T("审核通过"), _T("提示"), MB_OK);
					}
				}
			}
		}
	}
	else
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("请选择一个病厉"), _T("警告"), MB_OK);
		return;
	}
}

void CCasesMgrWnd::OnBtnRecalc()
{
	int     nItemIndex = -1;//当前所选记录的索引
	BOOL    hasDel = false;
	BOOL  	hasClear = false;
	BOOL    IsBottom = false;
	int     colomn = -1;

	bool bCurSelUserOperate = false;

	int iSel = m_pList->GetCurSel();
	if (iSel >= 0)
	{
		int nListID = -10000;
		CListTextElementUIEX* pControl = (CListTextElementUIEX*)m_pList->GetItemAt(iSel);
		if (pControl)
		{
			CString sText = pControl->GetText(0);
			nListID = _wtoi(sText.GetString());
			if (nListID != -10000)
			{
				// 				if (g_PatientInfoCur.sName.IsEmpty() || g_PatientInfoCur.sBLH.IsEmpty())
				// 				{
				// 					::MessageBox(m_hWnd, _T("请新建一个用户或者病历管理选择一个用户样本"), _T("警告"), MB_OKCANCEL);
				// 					return;
				// 				}

				CString sBLH = pControl->GetText(4);
				if (!g_PatientInfoCur.sBLH.IsEmpty() && g_PatientInfoCur.sBLH.Compare(sBLH) == 0)
				{
					bCurSelUserOperate = true;

					if (IsCurUser(iSel) && gMainDlgWnd->IsDisposeProgress())
					{
						SOUND_WARNING
						::MessageBox(m_hWnd, _T("当前样本检测中，请等待操作完成."), _T("警告"), MB_OKCANCEL);
						return;
					}
				}
				SOUND_WARNING
				int iret = ::MessageBox(m_hWnd, _T("重新计算将认为该样本审核未通过，是否继续."), _T("提示"), MB_OKCANCEL);
				if (iret != IDOK)
				{
					return;
				}

				OnBtnSelectedUser(true, false);

				ModifyCheckID_ByID(nListID, 0);

				bool bFilePathExists = false;
				if (bCurSelUserOperate && !m_FileDir.IsEmpty() && PathFileExists(m_FileDir.GetString()))
				{
					bFilePathExists = true;
				}
				else
				{
					CString strRptPath = pControl->GetText(9);
					if (!strRptPath.IsEmpty())
					{
						bool bFileOk = true;
						int ipos = strRptPath.ReverseFind('\\');
						if (ipos == -1)
						{
							SOUND_WARNING
							::MessageBox(m_hWnd, _T("文件路径错误"), _T("警告"), MB_OK);
							//重新选择档案
							::PostMessage(gMainDlgWnd->GetHWND(), MSG_USER_SWITCH_MAIN1, NULL, NULL);
							return;
						}
						CString sFN = strRptPath.Mid(ipos + 1);
						CString sTemp = strRptPath.Mid(0, ipos);
						int ipos1 = sTemp.ReverseFind('\\');
						if (ipos1 == -1)
						{
							SOUND_WARNING
							::MessageBox(m_hWnd, _T("文件路径错误"), _T("警告"), MB_OK);
							//重新选择档案
							::PostMessage(gMainDlgWnd->GetHWND(), MSG_USER_SWITCH_MAIN1, NULL, NULL);
							return;
						}
						CString sRoot = sTemp.Mid(ipos1 + 1);
						CString sRootHead = sTemp.Mid(0, ipos1);
						int ipos2 = sRoot.Find(_T("Result"));
						if (ipos2 == -1)
						{
							SOUND_WARNING
							::MessageBox(m_hWnd, _T("文件路径错误"), _T("警告"), MB_OK);
							//重新选择档案
							::PostMessage(gMainDlgWnd->GetHWND(), MSG_USER_SWITCH_MAIN1, NULL, NULL);
							return;
						}
						CString sRoot1 = sRoot.Mid(0, ipos2);
						CString sDest = sRootHead + _T("\\") + sRoot1;
						if (!PathFileExists(sDest.GetString()))
						{
							SOUND_WARNING
							::MessageBox(m_hWnd, _T("文件路径错误"), _T("警告"), MB_OK);
							//重新选择档案
							::PostMessage(gMainDlgWnd->GetHWND(), MSG_USER_SWITCH_MAIN1, NULL, NULL);
							return;
						}

						gMainDlgWnd->OpenFolder(sDest);

						bFilePathExists = true;
					}
				}

				if (bFilePathExists)
				{
					CString sName = pControl->GetText(1);
					_stprintf_s(g_PatiengInfo.lpName, sizeof(g_PatiengInfo.lpName), _T("%s"), sName.GetString());

					sName = pControl->GetText(2);
					if (sName.Compare(_T("男")) == 0)
						g_PatiengInfo.nSex = 0;
					else
						g_PatiengInfo.nSex = 1;

					sName = pControl->GetText(3);
					g_PatiengInfo.nAge = _ttoi(sName.GetString());

					sName = pControl->GetText(4);
					_stprintf_s(g_PatiengInfo.lpBLId, sizeof(g_PatiengInfo.lpBLId), _T("%s"), sName.GetString());

					sName = pControl->GetText(5);
					_stprintf_s(g_PatiengInfo.lpPhoneNum, sizeof(g_PatiengInfo.lpPhoneNum), _T("%s"), sName.GetString());

					//重新计算
					::PostMessage(gMainDlgWnd->GetHWND(), MSG_USER_SWITCH_MAIN, NULL, NULL);
				}

				Close(MSGID_OK);
				UpdateWindow(m_hWnd);
			}
		}
	}
	else
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("请选择一个病厉"), _T("警告"), MB_OK);
		return;
	}
}

void CCasesMgrWnd::OnBtnShowAll(int iSel)
{
	CDatabaseMgr* pMgr = CDatabaseMgr::GetInstance();
	if (pMgr->OpenDB())
	{
		m_pList->RemoveAll();
		gVecPatientInfos.clear();
		pMgr->GetSQLRecords(gVecPatientInfos);
		pMgr->CloseDB();
		m_iRowCount = gVecPatientInfos.size();

		AddListData(m_iRowCount, iSel);
	}
}

void CCasesMgrWnd::OnBtnDelete()
{

	if (!gMainDlgWnd->OpenDB())
		return;

	int iSel = m_pList->GetCurSel();
	if (iSel >= 0)
	{
		// 		if (g_PatientInfoCur.sName.IsEmpty() || g_PatientInfoCur.sBLH.IsEmpty())
		// 		{
		// 			::MessageBox(m_hWnd, _T("请新建一个用户或者病历管理选择一个用户样本"), _T("警告"), MB_OKCANCEL);
		// 			return;
		// 		}

		int nListID = -10000;
		CListTextElementUIEX* pControl = (CListTextElementUIEX*)m_pList->GetItemAt(iSel);
		if (pControl)
		{
			CString sText = pControl->GetText(0);
			nListID = _wtoi(sText.GetString());
			if (nListID != -10000)
			{
				CString sBLH = pControl->GetText(4);
				bool b1 = !g_PatientInfoCur.sBLH.IsEmpty();
				bool b2 = g_PatientInfoCur.sBLH.Compare(sBLH) == 0;
				bool b3 = IsCurUser(iSel);
				bool b4 = gMainDlgWnd->IsDisposeProgress();
				if (!g_PatientInfoCur.sBLH.IsEmpty() && g_PatientInfoCur.sBLH.Compare(sBLH) == 0)
				{
					if (IsCurUser(iSel) && gMainDlgWnd->IsDisposeProgress())
					{
						SOUND_WARNING
						::MessageBox(m_hWnd, _T("当前样本检测中，请等待操作完成."), _T("警告"), MB_OKCANCEL);
						return;
					}
				}

				int iret = ::MessageBox(m_hWnd, _T("是否删除样本"), _T("提示"), MB_OKCANCEL);
				if (iret != IDOK)
				{
					return;
				}

				CString str;
				try
				{
					str.Format(_T("DELETE from patient_info where ID=%d"), nListID);
					_variant_t RecordsAffected;
					gMainDlgWnd->m_pConnection->Execute((_bstr_t)str, &RecordsAffected, adCmdText);
					OutputDebugString(_T("删除成功!\n"));

					CString sBLH = pControl->GetText(4);
					if (!g_PatientInfoCur.sBLH.IsEmpty() && g_PatientInfoCur.sBLH.Compare(sBLH) == 0)
					{
						g_PatientInfoCur.sBLH = _T("");
						g_PatientInfoCur.sID = _T("");
						g_PatientInfoCur.sName = _T("");
					}
					gMainDlgWnd->ClearCurUser();

					CString str(_T(""));
					if (g_PatientInfoCur.sID.GetLength() > 0 && g_PatientInfoCur.sName.GetLength() > 0 && g_PatientInfoCur.sBLH.GetLength() > 0)
					{
						str.Format(_T("ID:%s 姓名:%s 病历号:%s"), g_PatientInfoCur.sID.GetString(), g_PatientInfoCur.sName.GetString(), g_PatientInfoCur.sBLH.GetString());
					}
					m_pTip2->SetText(str.GetString());
				}
				catch (_com_error*e)
				{
					SOUND_WARNING
					AfxMessageBox(e->ErrorMessage());
				}

				m_pList->RemoveAt(iSel);
				vector<PatientInfo>::iterator it;
				for (it = gVecPatientInfos.begin(); it != gVecPatientInfos.end(); ++it)
				{
					PatientInfo info = *it;
					if (_wtoi(info.id.GetData()) == nListID)
					{
						gVecPatientInfos.erase(it);
						break;
					}
				}
			}
		}
	}
	else
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("请选择一个病厉"), _T("警告"), MB_OK);
		return;
	}

	gMainDlgWnd->CloseDB();
}

void CCasesMgrWnd::OnBtnExit()
{
	Close(MSGID_OK);
}

void CCasesMgrWnd::OnBtnSelectedUserEx()
{
	if (g_PatientInfoCur.sName.IsEmpty())
	{
		OnBtnSelectedUser();
		//Close(MSGID_OK);
	}
	else
	{
		int iSel = m_pList->GetCurSel();
		if (iSel >= 0)
		{
			int nListID = -10000;
			CListTextElementUIEX* pControl = (CListTextElementUIEX*)m_pList->GetItemAt(iSel);
			if (pControl)
			{
				CString sBLH = pControl->GetText(4);
				if (sBLH.Compare(g_PatientInfoCur.sBLH) == 0)
				{
					SOUND_WARNING
					::MessageBox(m_hWnd, _T("已选择样本"), _T("提示"), MB_OKCANCEL);
					return;
				}
			}
		}
		if (g_bScreening)
		{
			SOUND_WARNING
				::MessageBox(m_hWnd, _T("当前样本检测中，请完成后选择其它样本."), _T("提示"), MB_OKCANCEL);
		}
		else
		{
			OnBtnSelectedUser();
			//Close(MSGID_OK);
		}
	}
}

void CCasesMgrWnd::OnBtnSelectedUser(bool bClose, bool bTip)
{
	int iSel = m_pList->GetCurSel();
	if (iSel >= 0)
	{
		int nListID = -10000;
		CListTextElementUIEX* pControl = (CListTextElementUIEX*)m_pList->GetItemAt(iSel);
		if (pControl)
		{
			CString sText = pControl->GetText(0);
			CString sName = pControl->GetText(1);
			CString sSex = pControl->GetText(2);
			CString sAge = pControl->GetText(3);
			CString sBLID = pControl->GetText(4);
			CString sPhone = pControl->GetText(5);
			CString sTime = pControl->GetText(6);
			CString sCheckId = pControl->GetText(7);
			CString sDiag = pControl->GetText(8);
			CString sPath = pControl->GetText(9);
			nListID = _wtoi(sText.GetString());
			if (nListID != -10000)
			{
				g_FLOW = FLOW_SELECTEDUSER;

				bool bFemale = false;
				if (1 == _ttoi(sSex.GetString()))
					bFemale = true;
				int iAge = _ttoi(sAge.GetString());
				int iChecked = 0;
				if (sCheckId.Compare(_T("已审核"))==0)
					iChecked = 1;

				g_PatientInfoCur.sName = sName;
				g_PatientInfoCur.sBLH = sBLID;
				g_PatientInfoCur.sID = sText.Trim();
				gMainDlgWnd->ShowCurUser();
				CString str(_T(""));
				if (g_PatientInfoCur.sID.GetLength() > 0 && g_PatientInfoCur.sName.GetLength() > 0 && g_PatientInfoCur.sBLH.GetLength() > 0)
				{
					str.Format(_T("ID:%s 姓名:%s 病历号:%s"), g_PatientInfoCur.sID.GetString(), g_PatientInfoCur.sName.GetString(), g_PatientInfoCur.sBLH.GetString());
				}
				m_pTip2->SetText(str.GetString());

				TCHAR lpBin[MAX_PATH];
				InitPath(lpBin, NULL, NULL);
				TCHAR lpini[MAX_PATH];
				wsprintf(lpini, _T("%s%s"), lpBin, _T("number.ini"));
				WritePrivateProfileString(_T("info"), _T("number"), sBLID.GetString(), lpini);

				_tcscpy_s(g_PatiengInfo.lpName, sizeof(g_PatiengInfo.lpName), sName.GetString());
				if (bFemale)
					g_PatiengInfo.nSex = 1;
				else
					g_PatiengInfo.nSex = 0;
				g_PatiengInfo.nAge = iAge;
				_tcscpy_s(g_PatiengInfo.lpBLId, sizeof(g_PatiengInfo.lpBLId), sBLID.GetString());
				_tcscpy_s(g_PatiengInfo.lpPhoneNum, sizeof(g_PatiengInfo.lpPhoneNum), sPhone.GetString());
				g_PatiengInfo.db_ID = 1;
				g_PatiengInfo.wCheckId = iChecked;

				//gMainDlgWnd->ModifyDBByID(nListID, &g_PatiengInfo);
				UpdateWindow(gMainDlgWnd->GetHWND());
				//gMainDlgWnd->ResetDataParam();

				// 				pControl->SetForeColor(GetColorDword(_T("ffff0000")));
				// 				pControl->SetBorderColor(GetColorDword(_T("ffff0000")));
				// 				pControl->SetBorderSize(3);

				if (bTip)
				{
					::MessageBox(m_hWnd, _T("已选择样本"), _T("提示"), MB_OKCANCEL);
				}
				return;
			}
		}
	}
	else
	{
		if (!bClose)
		{
			SOUND_WARNING
			::MessageBox(m_hWnd, _T("请选择一个病厉"), _T("警告"), MB_OK);
		}
		return;
	}
}

void CCasesMgrWnd::ShowReport(int nListID)
{
	CString strPath;
	FindReportPathByID(nListID, strPath);
	if (!PathFileExists(strPath.GetString()))
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("报告文件不存在."), _T("警告"), MB_OKCANCEL);
		return;
	}
	TCHAR lpBin[MAX_PATH];
	InitPath(lpBin, NULL, NULL);
	CString name = _T("AcroRd32.exe");
	CString name1 = _T("  ");
	name1 += strPath;
	ShellExecute(NULL, _T("open"), name, name1, lpBin, SW_SHOWNORMAL);
}

void CCasesMgrWnd::FindReportPathByID(int nWantID, CString& strPath)
{
	if (!gMainDlgWnd->OpenDB())
		return;
	_ConnectionPtr m_pConnection = gMainDlgWnd->m_pConnection;
	_RecordsetPtr m_pRecordset = gMainDlgWnd->m_pRecordset;
	CString strV;
	try
	{
		if (!m_pRecordset->BOF)
			m_pRecordset->MoveFirst();
		else
		{
			//AfxMessageBox(_T("表内数据为空"));
			//::MessageBox(m_hWnd, _T("表内数据为空"), _T("警告"), MB_OK);
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
				if (nID == nWantID)
				{
					var = m_pRecordset->GetCollect("path");
					if (var.vt != VT_NULL)
					{
						strV = (LPCSTR)_bstr_t(var);
						OutputDebugString(strV);
					}
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

	strPath = strV;
	gMainDlgWnd->CloseDB();

}

void CCasesMgrWnd::ModifyCheckID_ByID(int nModifyID, int nCheck)
{
	if (!gMainDlgWnd->OpenDB())
		return;
	_ConnectionPtr m_pConnection = gMainDlgWnd->m_pConnection;
	_RecordsetPtr m_pRecordset = gMainDlgWnd->m_pRecordset;
	try
	{
		if (!m_pRecordset->BOF)
			m_pRecordset->MoveFirst();
		else
		{
			//::MessageBox(m_hWnd, _T("表内数据为空"), _T("警告"), MB_OK);
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
					m_pRecordset->PutCollect("check_ID", nCheck);
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

	gMainDlgWnd->CloseDB();

}

void CCasesMgrWnd::ModifyPathByID(int nModifyID, CString sPath)
{
	if (!gMainDlgWnd->OpenDB())
		return;
	_ConnectionPtr m_pConnection = gMainDlgWnd->m_pConnection;
	_RecordsetPtr m_pRecordset = gMainDlgWnd->m_pRecordset;
	try
	{
		if (!m_pRecordset->BOF)
			m_pRecordset->MoveFirst();
		else
		{
			//::MessageBox(m_hWnd, _T("表内数据为空"), _T("警告"), MB_OK);
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
					m_pRecordset->PutCollect("path", sPath.GetString());
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

	gMainDlgWnd->CloseDB();
}

bool CCasesMgrWnd::FindInfoByID(int nModifyID)
{
	bool bRet = false;

	if (!gMainDlgWnd->OpenDB())
		return bRet;
	_ConnectionPtr m_pConnection = gMainDlgWnd->m_pConnection;
	_RecordsetPtr m_pRecordset = gMainDlgWnd->m_pRecordset;
	try
	{
		if (!m_pRecordset->BOF)
			m_pRecordset->MoveFirst();
		else
		{
			//::MessageBox(m_hWnd, _T("表内数据为空"), _T("警告"), MB_OK);
			return bRet;
		}

		CString strName;
		int nSex, nAge;

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
					var = m_pRecordset->GetCollect("patient_name");
					if (var.vt != VT_NULL)
					{
						g_ModifyReportInfo.name = (LPCSTR)_bstr_t(var);
					}
					nCol++;
					var = m_pRecordset->GetCollect("sex");
					if (var.vt != VT_NULL)
					{
						nSex = (int)(var);
						if (nSex)
							g_ModifyReportInfo.sex = _T("女");
						else
							g_ModifyReportInfo.sex = _T("男");
					}
					nCol++;
					var = m_pRecordset->GetCollect("age");
					if (var.vt != VT_NULL)
					{
						nAge = (int)(var);
						CString strAge;
						strAge.Format(_T("%d"), nAge);
						g_ModifyReportInfo.age = strAge;
					}
					nCol++;
					var = m_pRecordset->GetCollect("BL_ID");
					if (var.vt != VT_NULL)
					{
						g_ModifyReportInfo.bingLiHao = (LPCSTR)_bstr_t(var);
					}
					nCol++;
					var = m_pRecordset->GetCollect("PhoneNum");
					if (var.vt != VT_NULL)
					{
						g_ModifyReportInfo.sMobile = (LPCSTR)_bstr_t(var);
					}
					nCol++;

					m_pRecordset->Update();
					bRet = true;
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
		bRet = false;
	}

	gMainDlgWnd->CloseDB();

	return bRet;
}

bool CCasesMgrWnd::IsCurUser(int nListIndex)
{
	if (nListIndex < 0)
	{
		return false;
	}
	int nCount = m_pList->GetCount();
	if (nListIndex >= nCount)
	{
		return false;
	}
	int iSel = nListIndex;
	if (iSel >= 0)
	{
		int nListID = -10000;
		CListTextElementUIEX* pControl = (CListTextElementUIEX*)m_pList->GetItemAt(iSel);
		if (pControl)
		{
			CString sText = pControl->GetText(0);
			nListID = _wtoi(sText.GetString());
			if (nListID != -10000)
			{
				CString sBLH = pControl->GetText(4);
				if (sText.Compare(g_PatientInfoCur.sID) == 0 && !g_PatientInfoCur.sBLH.IsEmpty() && g_PatientInfoCur.sBLH.Compare(sBLH) == 0)
				{
					return true;
				}
			}
		}
	}
	return false;
}

DWORD CCasesMgrWnd::GetColorDword(LPCTSTR clr)
{
	if (clr == NULL) return 0;
	LPCTSTR colVal = clr;
	if (*colVal == _T('#')) colVal = ::CharNext(colVal);
	LPTSTR pstr = NULL;
	DWORD clrColor = _tcstoul(colVal, &pstr, 16);
	return clrColor;
}

void CCasesMgrWnd::SelUserPropose(CString sBLH)
{
	CDatabaseMgr* pMgr = CDatabaseMgr::GetInstance();
	_g_record t;
	bool bret = pMgr->FindInfoByID(sBLH, t);
	if (bret)
	{
		CString a;
		int b;
		bool ret = t.getdata(_T("diagnose_propose"), a, b);
		assert(ret);
		if (!a.IsEmpty())
		{
			CString s1(_T("定期复查(1年)")), s2(_T("月复查")), s3(_T("活"));
			if (a.Find(s1) > 0)
			{
				m_pComboOpinion->SelectItem(1);
				return;
			}
			else if (a.Find(s2) > 0)
			{
				m_pComboOpinion->SelectItem(2);
				return;
			}
			else if (a.Find(s3) > 0)
			{
				m_pComboOpinion->SelectItem(3);
				return;
			}
		}
	}

	m_pComboOpinion->SelectItem(0);
}
