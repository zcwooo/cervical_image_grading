#include "stdafx.h"
#include "Register.h"


CRegister::CRegister()
{
}

CRegister::~CRegister()
{
}

BOOL CRegister::OpenDB()
{
	BOOL bOK = FALSE;
	m_pConnection = _ConnectionPtr(__uuidof(Connection));
	try
	{
		//https://bbs.csdn.net/topics/330263772
		CString strDBName;
		strDBName = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=";
		strDBName += GetExePath();
		strDBName += "patient.mdb;";
		strDBName += "User ID='admin';Password=;Jet OLEDB:Database Password='yzy_db_2019'";

		//	m_pConnection->ConnectionString = (_bstr_t)strDBName;
		//m_pConnection->ConnectionString = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source=patient.mdb;";
		m_pConnection->Open(strDBName.GetString(), "", "", adConnectUnspecified);
		bOK = TRUE;
	}
	catch (...)
	{
		//::MessageBox(NULL, _T("�û��������������"), _T("����"), MB_OKCANCEL);
		return FALSE;
	}

	CString csSQLstr;
	// 	csSQLstr.Format(_T("SELECT * FROM patient_login where username=%s AND userpwd=%s"), 
	// 		gLoginParam.sName.GetData(), gLoginParam.sPassword.GetData());
	csSQLstr = "SELECT * FROM patient_login";
	csSQLstr += " order by ID desc";

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

void CRegister::CloseDB()
{
	if (m_pRecordset != NULL)
	{
		try
		{
			if (m_pRecordset->State)
			{
				m_pRecordset->Close();//�رռ�¼��
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

void CRegister::SaveRegisterInfo(CString name, CString pwd, CString newPwd, bool bNewRecord)
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
			//AfxMessageBox(_T("��������Ϊ��"));
			//::MessageBox(m_hWnd, _T("δ��ѯ��������������"), _T("����"), MB_OK);
			return;
		}
		CString strName, strPwd;
		int row = 0;
		int nCol = 0;
		_variant_t var;
		while (!m_pRecordset->adoEOF)
		{
			nCol = 0;
			var = m_pRecordset->GetCollect("username");
			if (var.vt != VT_NULL)
			{
				strName = (LPCSTR)_bstr_t(var);
			}
			var = m_pRecordset->GetCollect("userpwd");
			if (var.vt != VT_NULL)
			{
				strPwd = (LPCSTR)_bstr_t(var);
			}
			if (strName.Compare(name) == 0)
			{
				bFindName = true;
			}

			if (strName.Compare(name) == 0 && strPwd.Compare(pwd) == 0)
			{
				if (!bNewRecord)
				{
					m_pRecordset->PutCollect("username", name.GetString());
					m_pRecordset->PutCollect("userpwd", newPwd.GetString());
					m_pRecordset->Update();

					bSuccess = true;
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

	if (!bNewRecord)
	{
		if (bSuccess)
		{
			::MessageBox(m_hWnd, _T("�޸�����ɹ�"), _T("��ʾ"), MB_OK);
		}
		else
		{
			if (bFindName)
			{
				SOUND_WARNING
				::MessageBox(m_hWnd, _T("ԭʼ�������"), _T("����"), MB_OK);
			}
			else
			{
				SOUND_WARNING
				::MessageBox(m_hWnd, _T("�޸�����ʧ��"), _T("����"), MB_OK);
			}
		}
	}
	else
	{
		if (bFindName)
		{
			SOUND_WARNING
			::MessageBox(m_hWnd, _T("���û�����ע��,ע��ʧ��."), _T("����"), MB_OK);
		}
		else
		{
			if (!OpenDB())
				return;
			if (!m_pRecordset->BOF)
				m_pRecordset->MoveFirst();
			else
			{
				//::MessageBox(m_hWnd, _T("δ��ѯ��������������"), _T("����"), MB_OK);
				return;
			}
			m_pRecordset->AddNew();///�����¼�¼
			m_pRecordset->PutCollect("username", name.GetString());
			m_pRecordset->PutCollect("userpwd", pwd.GetString());
			m_pRecordset->Update();
			CloseDB();
			
			::MessageBox(m_hWnd, _T("�û�ע��ɹ�"), _T("��ʾ"), MB_OK);
		}
	}
}

CString CRegister::GetExePath()
{
	TCHAR lpPath[MAX_PATH];
	GetModuleFileName(NULL, lpPath, sizeof(lpPath) - 1);
	PathRemoveFileSpec(lpPath);
	PathRemoveBackslash(lpPath);
	PathAddBackslash(lpPath);
	return lpPath;
}

