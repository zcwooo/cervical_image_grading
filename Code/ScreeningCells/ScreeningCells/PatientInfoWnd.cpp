#include "stdafx.h"
#include "PatientInfoWnd.h"
#include "patientinfo_t.h"
#include "MainDlgWnd.h"
#include "DatabaseMgr.h"

DUI_BEGIN_MESSAGE_MAP(CPatientInfoWnd, WindowImplBase)
DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK, OnClick)
DUI_END_MESSAGE_MAP()

PatiengInfo g_PatiengInfo;
PatientInfoCur g_PatientInfoCur;
extern CMainDlgWnd* gMainDlgWnd;
extern void InitPath(TCHAR * lpBin, TCHAR  *lpINI, TCHAR  *lpLog);

CPatientInfoWnd::CPatientInfoWnd()
{
}

CPatientInfoWnd::~CPatientInfoWnd()
{
}

void CPatientInfoWnd::OnFinalMessage(HWND hWnd)
{
	__super::OnFinalMessage(hWnd);
	delete this;
}

DuiLib::CDuiString CPatientInfoWnd::GetSkinFile()
{
	return _T("xmls/patientinfo.xml");
}

LPCTSTR CPatientInfoWnd::GetWindowClassName(void) const
{
	return _T("LoginWnd");
}

void CPatientInfoWnd::InitWindow()
{
	m_pEditName = static_cast<CEditUI*>(m_pm.FindControl(_T("editname")));
	m_pOptionUI1 = static_cast<COptionUI*>(m_pm.FindControl(_T("radio1")));
	m_pOptionUI2 = static_cast<COptionUI*>(m_pm.FindControl(_T("radio2")));
	m_pEditAge = static_cast<CEditUI*>(m_pm.FindControl(_T("editage")));
	m_pEditBingLiHao = static_cast<CEditUI*>(m_pm.FindControl(_T("editbinglihao")));
	m_pEditMobilePhone = static_cast<CEditUI*>(m_pm.FindControl(_T("editphone")));
	m_pOption = static_cast<COptionUI*>(m_pm.FindControl(_T("option1")));

	m_pEditUI[0] = static_cast<CEditUI*>(m_pm.FindControl(_T("zhuyuanhao")));
	m_pEditUI[1] = static_cast<CEditUI*>(m_pm.FindControl(_T("chuanghao")));
	m_pEditUI[2] = static_cast<CEditUI*>(m_pm.FindControl(_T("checkinghopital")));
	m_pEditUI[3] = static_cast<CEditUI*>(m_pm.FindControl(_T("checkingdepartment")));
	m_pEditUI[4] = static_cast<CEditUI*>(m_pm.FindControl(_T("checkingdoctor")));
	m_pDate1 = static_cast<CDateTimeUI*>(m_pm.FindControl(_T("takesampledate")));

	TCHAR lpBin[MAX_PATH];
	InitPath(lpBin, NULL, NULL);
	TCHAR lpini[MAX_PATH];
	wsprintf(lpini, _T("%s%s"), lpBin, _T("number.ini"));
	UINT iSel = GetPrivateProfileInt(_T("select_user"), _T("select"), 1, lpini);
	m_pOption->Selected(iSel);

	CButtonUI* pBtn = static_cast<CButtonUI*>(m_pm.FindControl(_T("save")));
	pBtn->SetFocus();
}

void CPatientInfoWnd::OnClick(TNotifyUI &msg)
{
	CDuiString sName = msg.pSender->GetName();
	sName.MakeLower();
	if (0 == sName.Compare(_T("close")))
	{
		Close(MSGID_OK);
	}
	else if (0 == sName.Compare(_T("save")))
	{
		save();
	}
	else if (0 == sName.Compare(_T("cancel")))
	{
		Close(MSGID_OK);
	}
}

LRESULT CPatientInfoWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (uMsg == WM_TIMER)
	{
		if (wParam == 1000)
		{
		}
	}
	bHandled = FALSE;
	return 0;
}

LRESULT CPatientInfoWnd::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0L;
}

LRESULT CPatientInfoWnd::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
	if (uMsg == WM_KEYDOWN)
	{
		switch (wParam)
		{
		case VK_RETURN:
		{
			save();
			return 1L;
		}
		default:
			break;
		}
	}
	WindowImplBase::MessageHandler(uMsg, wParam, lParam, bHandled);
	bHandled = false;
	return 0L;
}

void CPatientInfoWnd::save()
{
	CString sName = m_pEditName->GetText();
	if (sName.IsEmpty())
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("用户名不能为空"), _T("警告"), MB_OK);
		return;
	}
	if (sName.GetLength() > 60)
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("用户名大于60个字符"), _T("警告"), MB_OK);
		return;
	}
	CStringA strA = sName;
	for (int i = 0; i < strA.GetLength(); ++i)
	{
		TCHAR c = strA.GetAt(i);
		if (c >= 0 && c <= 127)
		{
			if (c >= 48 && c <= 57) // || c >= 65 && c <= 90 || c >= 97 && c <= 122)
			{
			}
			else
			{
				SOUND_WARNING
				::MessageBox(m_hWnd, _T("用户名只能为汉字和数字"), _T("警告"), MB_OK);
				return;
			}
		}
		else
		{
			i++;
		}
	}

	bool bFemale = m_pOptionUI2->IsSelected();

	CString sAge = m_pEditAge->GetText();
	int iAge = _wtoi(sAge.GetString());
	if (iAge > 99)
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("用户年龄不能超过99岁"), _T("警告"), MB_OK);
		return;
	}
	if (iAge < 10)
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("用户年龄不能低于10岁"), _T("警告"), MB_OK);
		return;
	}

	CString sBLH = m_pEditBingLiHao->GetText();
	sBLH.Trim();
	if (sBLH.IsEmpty())
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("病历号不能为空"), _T("警告"), MB_OK);
		return;
	}
	if (sBLH.GetLength() != 8)
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("病历号必须由前两位英文字母加后六位数字组成."), _T("警告"), MB_OK);
		return;
	}
	for (int i = 0; i < sBLH.GetLength(); ++i)
	{
		TCHAR c = sBLH.GetAt(i);
		if (c >= 0 && c <= 127)
		{
			if (i < 2)
			{
				if (c >= 65 && c <= 90 || c >= 97 && c <= 122)
				{
				}
				else
				{
					SOUND_WARNING
					::MessageBox(m_hWnd, _T("病历号必须由前两位英文字母加后六位数字组成."), _T("警告"), MB_OK);
					return;
				}
			}
			else
			{
				if (c >= 48 && c <= 57)
				{
				}
				else
				{
					SOUND_WARNING
					::MessageBox(m_hWnd, _T("病历号必须由前两位英文字母加后六位数字组成."), _T("警告"), MB_OK);
					return;
				}
			}

		}
		else
		{
			SOUND_WARNING
			::MessageBox(m_hWnd, _T("病历号必须由前两位英文字母加后六位数字组成."), _T("警告"), MB_OK);
			return;
		}
	}

	CString sPhone = m_pEditMobilePhone->GetText();
	sPhone.Trim();
	if (sPhone.IsEmpty())
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("手机号不能为空"), _T("警告"), MB_OK);
		return;
	}
	if (!sPhone.IsEmpty())
	{
		TCHAR c = sPhone.GetAt(0);
		int ione = _ttoi(&c);
		if (ione != 1)
		{
			if (sPhone.GetLength() < 7 || sPhone.GetLength() > 12)
			{
				SOUND_WARNING
				::MessageBox(m_hWnd, _T("手机号错误"), _T("警告"), MB_OK);
				return;
			}
		}
		else if (ione == 1)
		{
			if (sPhone.GetLength() != 11)
			{
				SOUND_WARNING
				::MessageBox(m_hWnd, _T("手机号错误"), _T("警告"), MB_OK);
				return;
			}
		}
	}

	CDatabaseMgr* pMgr = CDatabaseMgr::GetInstance();
	_g_record t;
	pMgr->FindInfoByID(sBLH, t);
	CString a;
	int b;
	t.getdata(_T("BL_ID"), a, b);
	if (a.Compare(sBLH)==0)
	{
		SOUND_WARNING
		::MessageBox(m_hWnd, _T("数据库中已保存当前用户,请检查病历号是否正确."), _T("警告"), MB_OKCANCEL);
		return;
	}

	bool bChecking = false;
	bool bSel = m_pOption->IsSelected();
	if (bSel)
	{
		if (gMainDlgWnd->IsDisposeProgress())
		{
			bChecking = true;
			SOUND_WARNING
			::MessageBox(m_hWnd, _T("当前样本进行中，无法修改当前用户，请取消勾选[默认选择当前样本]."), _T("警告"), MB_OKCANCEL);
			return;
		}
	}

	if (bSel && !bChecking)
	{
		_tcscpy_s(g_PatiengInfo.lpName, sizeof(g_PatiengInfo.lpName), sName.GetString());
		if (bFemale)
			g_PatiengInfo.nSex = 1;
		else
			g_PatiengInfo.nSex = 0;
		g_PatiengInfo.nAge = iAge;
		_tcscpy_s(g_PatiengInfo.lpBLId, sizeof(g_PatiengInfo.lpBLId), sBLH.GetString());
		_tcscpy_s(g_PatiengInfo.lpPhoneNum, sizeof(g_PatiengInfo.lpPhoneNum), sPhone.GetString());
		g_PatiengInfo.db_ID = 0;
		g_PatiengInfo.wCheckId = 0;
	}

	CString strBlock[5], strDate;
	strBlock[0] = m_pEditUI[0]->GetText();
	strBlock[0].Trim();
	strBlock[1] = m_pEditUI[1]->GetText();
	strBlock[1].Trim();
	strBlock[2] = m_pEditUI[2]->GetText();
	strBlock[2].Trim();
	strBlock[3] = m_pEditUI[3]->GetText();
	strBlock[3].Trim();
	strBlock[4] = m_pEditUI[4]->GetText();
	strBlock[4].Trim();
	SYSTEMTIME sys1 = m_pDate1->GetTime();
	strDate.Format(_T("%d-%d-%d"), sys1.wYear, sys1.wMonth, sys1.wDay);

	_g_record temp;
	temp.setdata(_T("patient_name"), sName, -1);
	temp.setdata(_T("sex"), _T(""), bFemale ? 1 : 0);
	temp.setdata(_T("age"), _T(""), iAge);
	temp.setdata(_T("BL_ID"), sBLH, -1);
	temp.setdata(_T("PhoneNum"), sPhone, -1);
	temp.setdata(_T("check_ID"), _T(""), 0);
	temp.setdata(_T("zhu_yuan_hao"), strBlock[0], -1);
	temp.setdata(_T("chuang_hao"), strBlock[1], -1);
	temp.setdata(_T("create_date"), strDate, -1);
	temp.setdata(_T("checking_hospital"), strBlock[2], -1);
	temp.setdata(_T("checking_depart"), strBlock[3], -1);
	temp.setdata(_T("checking_doctor"), strBlock[4], -1);
	temp.copydata(g_record, temp);
	pMgr->SaveDataInfo(temp, sBLH);

	if (bSel && !bChecking)
	{
		g_PatientInfoCur.sName = sName;
		g_PatientInfoCur.sBLH = sBLH;
		CString sID;
		bool bRet = gMainDlgWnd->find_info_toid(sBLH, sID);
		g_PatientInfoCur.sID = sID;
		assert(bRet);
		gMainDlgWnd->ShowCurUser();
//		gMainDlgWnd->ResetDataParam();
		TCHAR lpBin[MAX_PATH];
		InitPath(lpBin, NULL, NULL);
		TCHAR lpini[MAX_PATH];
		wsprintf(lpini, _T("%s%s"), lpBin, _T("number.ini"));
		WritePrivateProfileString(_T("info"), _T("number"), sBLH.GetString(), lpini);
	}

	::MessageBox(m_hWnd, _T("保存成功"), _T("提示"), MB_OK);

	Close(MSGID_OK);
}