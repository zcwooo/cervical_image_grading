
#ifndef _PATIENTINFO_STRUC_
#define _PATIENTINFO_STRUC_

typedef struct __PatientInfo
{
	TCHAR lpName[64];
	int nSex; //0男 1女
	int nAge;
	TCHAR lpBLId[32];//病历号
	TCHAR lpPhoneNum[16];
	int db_ID; //0 第一次存储 1修改
	DWORD wCheckId; //0 未检查 1检查
}PatiengInfo;

typedef struct PatientInfoCur
{
	CString sName;
	CString sBLH;
	CString sID;
}PatientInfoCur;

#endif