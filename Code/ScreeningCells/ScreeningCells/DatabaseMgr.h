#pragma once

extern struct _g_record g_record;
extern struct _g_record g_record_temp;
extern int field_type[];
extern CString g_field_name[];

struct PatientInfo;

struct _g_record
{
	CString a[33];
	int b[33];
	_g_record()
	{
		reset();
	}
	void reset()
	{
		for (int i = 0; i < 33; ++i)
			a[i] = _T("");
		memset(b, -1, sizeof(b));
	}
	int getcount() { return 33; }
	void copydata(_g_record& dest, _g_record& src)
	{
		memcpy_s(dest.b, sizeof(dest.b), src.b, sizeof(src.b));
		for (int i = 0; i < 33; ++i)
		{
			dest.a[i] = src.a[i];
		}
	}
	void setdata(CString sField, CString _a, int _b)
	{
		if (sField.IsEmpty()) return;
		for (int i = 0; i < 33; ++i)
		{
			if (g_field_name[i].Compare(sField) == 0)
			{
				if (field_type[i] == 0)
				{
					a[i] = _a;
					break;
				}
				else if (field_type[i] == 1)
				{
					b[i] = _b;
					break;
				}
			}
		}
	}
	void setdata_index(int idx, CString _a, int _b)
	{
		if (!(idx>=0 && idx<33)) return;
		if (field_type[idx] == 0)
		{
			a[idx] = _a;
		}
		else if (field_type[idx] == 1)
		{
			b[idx] = _b;
		}
	}
	bool getdataex(CStringA sField, CString& _a, int& _b)
	{
		CString str;
		str = sField;
		return getdata(str, _a, _b);
	}
	bool getdata(CString sField, CString& _a, int& _b)
	{
		_a = _T("");
		_b = -1;
		if (sField.IsEmpty()) return false;
		for (int i = 0; i < 33; ++i)
		{
			if (g_field_name[i].Compare(sField) == 0)
			{
				if (field_type[i] == 0)
				{
					_a = a[i];
					return !_a.IsEmpty();
				}
				else if (field_type[i] == 1)
				{
					_b = b[i];
					return _b != -1;
				}
			}
		}
		return false;
	}
	void convertdata_string(_g_record& dest, CString result[33])
	{
		for (int i = 0; i < 33; ++i)
		{
			if (field_type[i]==0)
			{
				if (!a[i].IsEmpty())
					result[i] = a[i];
				else
					result[i] = _T("");
			}
			else if (field_type[i]==1)
			{
				if (b[i] != -1)
				{
					CString str;
					str.Format(_T("%d"), b[i]);
					result[i] = str;
				}
				else
				{
					if (i==2) //age
						result[i] = _T("");
					else
						result[i] = _T("0");
				}
					
			}
		}
	}
};

class CDatabaseMgr
{
public:
	static CDatabaseMgr* pIns;
	static CDatabaseMgr* GetInstance()
	{
		pIns = new CDatabaseMgr;
		return pIns;
	}
public:
	CDatabaseMgr();
	~CDatabaseMgr();

	HWND m_hWnd;
	_ConnectionPtr m_pConnection;
	_RecordsetPtr m_pRecordset;

public:
	BOOL OpenDB(CString csSQLstr=_T(""));
	void CloseDB();

	CString GetSQLString(int filedID, CString sFindName1, CString sFindName2);

	void SaveDataInfo(_g_record& temp, CString sBLH);
	bool GetDataInfo(_g_record& temp);
	bool FindInfoByID(int nModifyID, _g_record& temp);
	bool FindInfoByID(CString sBLH, _g_record& temp);

	void GetSQLRecords(vector<PatientInfo>& infos); //根据查询条件获取所有记录集
	
	///////////
	bool FindFromBLH(CString sBLH); //默认打开数据库
	void FindRecordItem(int nCol, _variant_t var, _g_record& temp);

	/////////////////////////////////////////

	CString GetBinPath();
};