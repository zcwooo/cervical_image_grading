
#ifndef _PATIENTINFO_STRUC_
#define _PATIENTINFO_STRUC_

typedef struct __PatientInfo
{
	TCHAR lpName[64];
	int nSex; //0�� 1Ů
	int nAge;
	TCHAR lpBLId[32];//������
	TCHAR lpPhoneNum[16];
	int db_ID; //0 ��һ�δ洢 1�޸�
	DWORD wCheckId; //0 δ��� 1���
}PatiengInfo;

typedef struct PatientInfoCur
{
	CString sName;
	CString sBLH;
	CString sID;
}PatientInfoCur;

#endif