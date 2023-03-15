#include "stdafx.h"
#include "DatabaseMgr.h"
#include "MainDlgWnd.h"
#include "PatientInfoWnd.h"
#include "CasesMgrWnd.h"

extern CMainDlgWnd* gMainDlgWnd;
extern PatiengInfo g_PatiengInfo;

CDatabaseMgr* CDatabaseMgr::pIns = NULL;

CString g_field_name[] = {				//字段ID
	_T("ID"),							//1.int								
	_T("patient_name"),					//2.string
	_T("sex"),							//3.int
	_T("age"),							//4.int
	_T("BL_ID"),						//5.string
	_T("PhoneNum"),						//6.string
	_T("create_date"),					//7.date string
	_T("check_ID"),						//8.int
	_T("diag"),		    				//9.备注
	_T("path"),							//10.
	_T("zhu_yuan_hao"),					//11.
	_T("chuang_hao"),					//12.
	_T("take_sample_date"),				//13.date string
	_T("checking_hospital"),			//14.
	_T("checking_depart"),				//15.
	_T("checking_doctor"),				//16.
	_T("path_result"),					//17.
	_T("pdf_img1"),						//18.
	_T("pdf_img2"),						//19.
	_T("pdf_img3"),						//20.
	_T("pdf_img4"),						//21.
	_T("attribute1"),					//22.
	_T("attribute2"),					//23.
	_T("attribute3"),					//24.
	_T("attribute4"),					//25.
	_T("attribute5"),					//26.
	_T("diagnose_result"),				//27.
	_T("diagnose_propose"),				//28.
	_T("reseve1"),						//29. 细胞总数
	_T("reseve2"),						//30.
	_T("reseve3"),						//31.
	_T("reseve4"),						//32.
	_T("reseve5"),						//33.
};
//string 0 int 1
int field_type[] = {1, 0, 1, 1, 0, 0, 0, 1, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0 };

struct _g_record g_record;
struct _g_record g_record_temp;

CDatabaseMgr::CDatabaseMgr()
{
	m_hWnd = gMainDlgWnd->GetHWND();
}


CDatabaseMgr::~CDatabaseMgr()
{
	if (pIns)
	{
		delete pIns;
		pIns = NULL;
	}
}


BOOL CDatabaseMgr::OpenDB(CString csSQLstr)
{
	BOOL bOK = FALSE;
	m_pConnection = _ConnectionPtr(__uuidof(Connection));
	try
	{
		CString strDBName;
		strDBName = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=";
		strDBName += GetBinPath();
		strDBName += "patient.mdb;";
		strDBName += "User ID='admin';Password=;Jet OLEDB:Database Password='yzy_db_2019'";

		m_pConnection->ConnectionString = (_bstr_t)strDBName;
		m_pConnection->Open("", "", "", adConnectUnspecified);
		bOK = TRUE;
	}
	catch (_com_error *e)
	{
		SOUND_WARNING
		AfxMessageBox(e->ErrorMessage());
		bOK = FALSE;
	}

	if (csSQLstr.IsEmpty())
	{
		csSQLstr = "SELECT * FROM patient_info";
		csSQLstr += " order by ID desc";
	}

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


void CDatabaseMgr::CloseDB()
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

CString CDatabaseMgr::GetSQLString(int filedID, CString sFindName1, CString sFindName2)
{
	CString sFilter;
	if (filedID == 1)	//姓名
	{
		//sFilter.Format(_T("[姓名] LIKE '%s'"), m_strName);
		sFilter = "SELECT * FROM patient_info where [patient_name] LIKE '%";
		//sFilter = "SELECT * FROM patient_info where patient_name='";
		sFilter += sFindName1;
		sFilter += "%'";
		sFilter += " order by ID desc";

	}
	else if (filedID == 2)	//性别
	{
		sFilter = "SELECT * FROM patient_info where sex=";
		sFilter += sFindName1;
		sFilter += " order by ID desc";

	}
	else if (filedID == 3)	//年龄
	{
		sFilter = "SELECT * FROM patient_info where age BETWEEN ";
		sFilter += sFindName1;
		sFilter += "AND ";
		sFilter += sFindName2;
		sFilter += " order by ID desc";

	}
	else if (filedID == 4)	//病历号
	{
		sFilter = "SELECT * FROM patient_info where BL_ID LIKE '%";
		sFilter += sFindName1;
		sFilter += "%'";
		sFilter += " order by ID desc";

	}
	else if (filedID == 5)	//送检日期
	{
		sFilter = "SELECT * FROM patient_info where format(create_date, 'yyyy-mm-dd') BETWEEN format(#";
		sFilter += sFindName1;
		sFilter += "#, 'yyyy-mm-dd') and format(#";
		sFilter += sFindName2;
		sFilter += "#, 'yyyy-mm-dd')";
		sFilter += " order by ID desc";

	}
	else if (filedID == 6)	//审核
	{
		sFilter = "SELECT * FROM patient_info where check_ID=";
		sFilter += sFindName1;
		sFilter += " order by ID desc";

	}
	else if (filedID == 7)
	{
		sFilter = "SELECT * FROM patient_info where BL_ID=";
		sFilter += sFindName1;
	}
	return sFilter;
}

void CDatabaseMgr::SaveDataInfo(_g_record& temp, CString sBLH)
{
	if (!OpenDB())
		return;

	if (!FindFromBLH(sBLH))
	{
		m_pRecordset->AddNew();
	}

	CString a;
	int b;
	bool ret = false;

#define sentence_save(s, a, b, r) \
	r = temp.getdataex(s, a, b); \
	assert(ret); \
	if(r) \
		m_pRecordset->PutCollect(s, a.GetString()); 

	//2
	sentence_save("patient_name", a, b, ret);

	//3
	ret = temp.getdata(_T("sex"), a, b);
	if (ret)
	{
		if (b) //1女 1男
			m_pRecordset->PutCollect("sex", int(1));
		else
			m_pRecordset->PutCollect("sex", int(0));
	}

	//4
	ret = temp.getdata(_T("age"), a, b);
	if (ret)
		m_pRecordset->PutCollect("age", int(b));

	//5
	sentence_save("BL_ID", a, b, ret);

	//6
	sentence_save("PhoneNum", a, b, ret);

	//7 时间
	ret = temp.getdata(_T("create_date"), a, b);
	if (ret)
	{
		COleDateTime	bTime = COleDateTime::GetCurrentTime();
		m_pRecordset->PutCollect("create_date", _variant_t(bTime));
	}

	//8
	ret = temp.getdata(_T("check_ID"), a, b);
	assert(ret);
	if (ret)
		m_pRecordset->PutCollect("check_ID", int(b));

	//9
	sentence_save("diag", a, b, ret);

	//10
	sentence_save("path", a, b, ret);

	//11
	sentence_save("zhu_yuan_hao", a, b, ret);

	//12
	sentence_save("chuang_hao", a, b, ret);

	//13
	ret = temp.getdata(_T("take_sample_date"), a, b);
	if (ret)
	{
		COleDateTime	bTime = COleDateTime::GetCurrentTime();
		m_pRecordset->PutCollect("take_sample_date", _variant_t(bTime));
	}

	//14
	sentence_save("checking_hospital", a, b, ret);

	//15
	sentence_save("checking_depart", a, b, ret);

	//16
	sentence_save("checking_doctor", a, b, ret);

	//18
	sentence_save("pdf_img1", a, b, ret);

	//19
	sentence_save("pdf_img2", a, b, ret);

	//20
	sentence_save("pdf_img3", a, b, ret);

	//21
	sentence_save("pdf_img4", a, b, ret);

	//22
	sentence_save("attribute1", a, b, ret);

	//23
	sentence_save("attribute2", a, b, ret);

	//24
	sentence_save("attribute3", a, b, ret);

	//25
	sentence_save("attribute4", a, b, ret);

	//26
	sentence_save("attribute5", a, b, ret);

	//27
	sentence_save("diagnose_result", a, b, ret);

	//28
	sentence_save("diagnose_propose", a, b, ret);

	//29
	sentence_save("reseve1", a, b, ret);

	//30
	sentence_save("reseve2", a, b, ret);

	//31
	sentence_save("reseve3", a, b, ret);

	//32
	sentence_save("reseve4", a, b, ret);

	//33
	sentence_save("reseve5", a, b, ret);

	//m_pRecordset->PutCollect("diag", "diag_1111");

	m_pRecordset->Update();
	CloseDB();
}

bool CDatabaseMgr::GetDataInfo(_g_record& temp)
{

	return true;
}

#define sentence(a, idx, r, val) \
r = m_pRecordset->GetCollect(a); \
if (r.vt != VT_NULL) \
{ \
	 CString str; \
	 str = (LPCSTR)_bstr_t(r); \
	 val.setdata_index(idx++, str, -1);\
}\
else \
	 idx++; 

#define sentence_int(a, idx, r, val) \
r = m_pRecordset->GetCollect(a); \
if (r.vt != VT_NULL) \
{ \
	 int n =-1; \
	 n = int(r); \
	 val.setdata_index(idx++, _T(""), n);\
} \
else \
	 idx++;

bool CDatabaseMgr::FindInfoByID(int nModifyID, _g_record& temp)
{
	bool bRet = false;

	if (!OpenDB())
		return bRet;

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
					FindRecordItem(nCol, var, temp);

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

	CloseDB();

	return bRet;
}

bool CDatabaseMgr::FindInfoByID(CString sBLH, _g_record& temp)
{
	bool bRet = false;

	if (!OpenDB())
		return bRet;

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
			var = m_pRecordset->GetCollect("BL_ID");

			if (var.vt != VT_NULL)
			{
				CString str;
				str = (LPCSTR)_bstr_t(var);
				if (str.Compare(sBLH)==0)
				{
					FindRecordItem(nCol, var, temp);

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

	CloseDB();

	return bRet;
}

void CDatabaseMgr::GetSQLRecords(vector<PatientInfo>& infos)
{
// 	m_pList->RemoveAll();
// 	gVecPatientInfos.clear();

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
	}

	catch (_com_error *e)
	{
		SOUND_WARNING
		AfxMessageBox(e->ErrorMessage());
	}
}

bool CDatabaseMgr::FindFromBLH(CString sBLH)
{
	bool bFindName = false;

	try
	{
		if (!m_pRecordset->BOF)
			m_pRecordset->MoveFirst();
		else
		{
			//::MessageBox(m_hWnd, _T("未查询符合条件的数据"), _T("警告"), MB_OK);
			return false;
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

			if (strBLHFind.Compare(sBLH) == 0)
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
	return bFindName;
}

void CDatabaseMgr::FindRecordItem(int nCol, _variant_t var, _g_record& temp)
{
	//1
	var = m_pRecordset->GetCollect("ID");
	if (var.vt != VT_NULL)
	{
		int nID = (int)(var);
		temp.setdata_index(nCol++, _T(""), nID);
	}
	else
		nCol++;

	//2
	sentence("patient_name", nCol, var, temp);

	//3
	sentence_int("sex", nCol, var, temp);

	//4
	sentence_int("age", nCol, var, temp);

	//5
	sentence("BL_ID", nCol, var, temp);

	//6
	sentence("PhoneNum", nCol, var, temp);

	//7
	var = m_pRecordset->GetCollect("create_date");
	if (var.vt != VT_NULL)
	{
		DATE date;
		date = (DATE)(var);
		COleDateTime a_Time = date;
		CString s = a_Time.Format();
		temp.setdata_index(nCol++, s, -1);
	}
	else
		nCol++;

	//8
	sentence_int("check_ID", nCol, var, temp);

	//9
	sentence("diag", nCol, var, temp);

	//10
	sentence("path", nCol, var, temp);

	//11
	sentence("zhu_yuan_hao", nCol, var, temp);

	//12
	sentence("chuang_hao", nCol, var, temp);

	//13
	var = m_pRecordset->GetCollect("take_sample_date");
	if (var.vt != VT_NULL)
	{
		DATE date;
		date = (DATE)(var);
		COleDateTime a_Time = date;
		CString s = a_Time.Format();
		temp.setdata_index(nCol++, s, -1);
	}
	else
		nCol++;

	//14
	sentence("checking_hospital", nCol, var, temp);

	//15
	sentence("checking_depart", nCol, var, temp);

	//16
	sentence("checking_doctor", nCol, var, temp);

	//17
	sentence("path_result", nCol, var, temp);

	//18
	sentence("pdf_img1", nCol, var, temp);

	//19
	sentence("pdf_img2", nCol, var, temp);

	//20
	sentence("pdf_img3", nCol, var, temp);

	//21
	sentence("pdf_img4", nCol, var, temp);

	//22
	sentence("attribute1", nCol, var, temp);

	//23
	sentence("attribute2", nCol, var, temp);

	//24
	sentence("attribute3", nCol, var, temp);

	//25
	sentence("attribute4", nCol, var, temp);

	//26
	sentence("attribute5", nCol, var, temp);

	//27
	sentence("diagnose_result", nCol, var, temp);

	//28
	sentence("diagnose_propose", nCol, var, temp);

	//29
	sentence("reseve1", nCol, var, temp);

	//30
	sentence("reseve2", nCol, var, temp);

	//31
	sentence("reseve3", nCol, var, temp);

	//32
	sentence("reseve4", nCol, var, temp);

	//33
	sentence("reseve5", nCol, var, temp);
}

CString CDatabaseMgr::GetBinPath()
{
	TCHAR lpPath[MAX_PATH];
	GetModuleFileName(NULL, lpPath, sizeof(lpPath) - 1);
	PathRemoveFileSpec(lpPath);
	PathRemoveBackslash(lpPath);
	PathAddBackslash(lpPath);
	return CString(lpPath);
}
