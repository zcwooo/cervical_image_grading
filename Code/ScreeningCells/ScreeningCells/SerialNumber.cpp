#include "stdafx.h"
#include "SerialNumber.h"

#include <iostream>
#include <string>
#include <windows.h>

using namespace std;

#include "md5_encode.h"


#include <intrin.h>
#include <string>
using namespace std;

#include "libtripledes.h"
#include <iostream>  
#include <fstream>


#pragma comment(lib, "Netapi32.lib")
#include <nb30.h>

SerialNumber::SerialNumber()
{
}


SerialNumber::~SerialNumber()
{
}


void SerialNumber::get_cpuId(char *pCpuId)
{

	int dwBuf[4];
	getcpuid((unsigned int *)dwBuf, 1);
	sprintf(pCpuId, "%08X", dwBuf[3]);
	sprintf(pCpuId + 8, "%08X", dwBuf[0]);
	return;
}
void SerialNumber::getcpuid(unsigned int *CPUInfo, unsigned int InfoType)
{
#if defined(__GNUC__)// GCC  
	__cpuid(InfoType, CPUInfo[0], CPUInfo[1], CPUInfo[2], CPUInfo[3]);
#elif defined(_MSC_VER)// MSVC  
#if _MSC_VER >= 1400 //VC2005��֧��__cpuid  
	__cpuid((int*)(void*)CPUInfo, (int)(InfoType));
#else //����ʹ��getcpuidex  
	getcpuidex(CPUInfo, InfoType, 0);
#endif  
#endif  
}
void SerialNumber::getcpuidex(unsigned int *CPUInfo, unsigned int InfoType, unsigned int ECXValue)
{
#if defined(_MSC_VER) // MSVC  
#if defined(_WIN64) // 64λ�²�֧���������. 1600: VS2010, ��˵VC2008 SP1֮���֧��__cpuidex.  
	__cpuidex((int*)(void*)CPUInfo, (int)InfoType, (int)ECXValue);
#else  
	if (NULL == CPUInfo)
		return;
	_asm{
		// load. ��ȡ�������Ĵ���.  
		mov edi, CPUInfo;
		mov eax, InfoType;
		mov ecx, ECXValue;
		// CPUID  
		cpuid;
		// save. ���Ĵ������浽CPUInfo  
		mov[edi], eax;
		mov[edi + 4], ebx;
		mov[edi + 8], ecx;
		mov[edi + 12], edx;
	}
#endif  
#endif  
}


///////////////////////////////////////////////////////////////////////////////////////////////
//�������к�

BOOL SerialNumber::GetBaseBoardByCmd(char *lpszBaseBoard, int len/*=128*/)
{
	const long MAX_COMMAND_SIZE = 10000; // ��������������С	
	WCHAR szFetCmd[] = L"wmic BaseBoard get SerialNumber"; // ��ȡ�������к�������	
	const string strEnSearch = "SerialNumber"; // �������кŵ�ǰ����Ϣ

	BOOL   bret = FALSE;
	HANDLE hReadPipe = NULL; //��ȡ�ܵ�
	HANDLE hWritePipe = NULL; //д��ܵ�	
	PROCESS_INFORMATION pi;   //������Ϣ	
	STARTUPINFO			si;	  //���������д�����Ϣ
	SECURITY_ATTRIBUTES sa;   //��ȫ����

	char			szBuffer[MAX_COMMAND_SIZE + 1] = { 0 }; // ���������н�������������
	string			strBuffer;
	unsigned long	count = 0;
	long			ipos = 0;

	memset(&pi, 0, sizeof(pi));
	memset(&si, 0, sizeof(si));
	memset(&sa, 0, sizeof(sa));

	pi.hProcess = NULL;
	pi.hThread = NULL;
	si.cb = sizeof(STARTUPINFO);
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;

	//1.0 �����ܵ�
	bret = CreatePipe(&hReadPipe, &hWritePipe, &sa, 0);
	if (!bret)
	{
		goto END;
	}

	//2.0 ���������д��ڵ���ϢΪָ���Ķ�д�ܵ�
	GetStartupInfo(&si);
	si.hStdError = hWritePipe;
	si.hStdOutput = hWritePipe;
	si.wShowWindow = SW_HIDE; //���������д���
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	//3.0 ������ȡ�����еĽ���
	bret = CreateProcess(NULL, szFetCmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	if (!bret)
	{
		goto END;
	}

	//4.0 ��ȡ���ص�����
	WaitForSingleObject(pi.hProcess, 500/*INFINITE*/);
	bret = ReadFile(hReadPipe, szBuffer, MAX_COMMAND_SIZE, &count, 0);
	if (!bret)
	{
		goto END;
	}

	//5.0 �����������к�
	bret = FALSE;
	strBuffer = szBuffer;
	ipos = strBuffer.find(strEnSearch);

	if (ipos < 0) // û���ҵ�
	{
		goto END;
	}
	else
	{
		strBuffer = strBuffer.substr(ipos + strEnSearch.length());
	}

	memset(szBuffer, 0x00, sizeof(szBuffer));
	strcpy_s(szBuffer, strBuffer.c_str());

	//ȥ���м�Ŀո� \r \n
	int j = 0;
	for (int i = 0; i < strlen(szBuffer); i++)
	{
		if (szBuffer[i] != ' ' && szBuffer[i] != '\n' && szBuffer[i] != '\r')
		{
			lpszBaseBoard[j] = szBuffer[i];
			j++;
		}
	}

	bret = TRUE;

END:
	//�ر����еľ��
	CloseHandle(hWritePipe);
	CloseHandle(hReadPipe);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return(bret);

}

bool SerialNumber::GetAdapterInfo(int adapterNum, std::string& macOUT)
{
	NCB Ncb;
	memset(&Ncb, 0, sizeof(Ncb));
	Ncb.ncb_command = NCBRESET; // �����������Ա����ǿ��Բ�ѯ
	Ncb.ncb_lana_num = adapterNum;
	if (Netbios(&Ncb) != NRC_GOODRET)
		return false;

	// ׼��ȡ�ýӿڿ���״̬��
	memset(&Ncb, sizeof(Ncb), 0);
	Ncb.ncb_command = NCBASTAT;
	Ncb.ncb_lana_num = adapterNum;
	strcpy((char *)Ncb.ncb_callname, "*");
	struct ASTAT
	{
		ADAPTER_STATUS adapt;
		NAME_BUFFER nameBuff[30];
	}adapter;
	memset(&adapter, sizeof(adapter), 0);
	Ncb.ncb_buffer = (unsigned char *)&adapter;
	Ncb.ncb_length = sizeof(adapter);
	if (Netbios(&Ncb) != 0)
		return false;
	char acMAC[32];
	sprintf(acMAC, "%02X-%02X-%02X-%02X-%02X-%02X",
		int(adapter.adapt.adapter_address[0]),
		int(adapter.adapt.adapter_address[1]),
		int(adapter.adapt.adapter_address[2]),
		int(adapter.adapt.adapter_address[3]),
		int(adapter.adapt.adapter_address[4]),
		int(adapter.adapt.adapter_address[5]));
	macOUT = acMAC;
	return true;
}
bool SerialNumber::GetMacByNetBIOS(std::string& macOUT)
{
	// ȡ�������б�
	LANA_ENUM adapterList;
	NCB Ncb;
	memset(&Ncb, 0, sizeof(NCB));
	Ncb.ncb_command = NCBENUM;
	Ncb.ncb_buffer = (unsigned char *)&adapterList;
	Ncb.ncb_length = sizeof(adapterList);
	Netbios(&Ncb);

	// ȡ��MAC
	for (int i = 0; i < adapterList.length; ++i)
	{
		if (GetAdapterInfo(adapterList.lana[i], macOUT))
			return true;
	}

	return false;
}

//ע���
void SerialNumber::read_reg_sz(char buf[256])
{
	HKEY hkey;
	LPCTSTR data_set = _T("Software\\System2");

	if (ERROR_SUCCESS == ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, data_set, 0, KEY_READ, &hkey))
	{
		char dwValue[256];
		DWORD dwSzType = REG_SZ;
		DWORD dwSize = sizeof(dwValue);
		if (::RegQueryValueExA(hkey, "Name", 0, &dwSzType, (LPBYTE)&dwValue, &dwSize) != ERROR_SUCCESS)
		{
			cout << "�޷���ѯ�йص�ע�����Ϣ" << endl;
		}
		cout << dwValue << endl;
		memcpy(buf, dwValue, strlen(dwValue) + 1);
	}
	::RegCloseKey(hkey);
}

void SerialNumber::write_reg_sz(const char* m_name_set)
{
	HKEY hkey;
	HKEY hTempKey;
	//char m_name_set[256] = "China";

	DWORD len = strlen(m_name_set) + 1;
	LPCTSTR data_set = _T("Software");
	if (ERROR_SUCCESS == ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, data_set, 0, KEY_WRITE, &hkey))
	{
		if (ERROR_SUCCESS == ::RegCreateKey(hkey, _T("System2\\"), &hTempKey))
		{
			if (ERROR_SUCCESS != ::RegSetValueExA(hTempKey, "Name", 0, REG_SZ, (const BYTE*)m_name_set, len))
			{
				cout << "д�����" << endl;
			}
		}
	}
	::RegCloseKey(hkey);
}

void SerialNumber::delete_value()
{
	HKEY hkey;
	LPCTSTR data_set = _T("Software\\Chicony\\Lenovo1\\test1112");

	if (ERROR_SUCCESS == ::RegOpenKeyEx(HKEY_CURRENT_USER, data_set, 0, KEY_SET_VALUE, &hkey))
	{
		if (ERROR_SUCCESS != ::RegDeleteValue(hkey, _T("Name")))
		{
			cout << "ɾ������" << endl;
		}
	}
	::RegCloseKey(hkey);
}

void SerialNumber::delete_key()
{
	HKEY hkey;
	LPCTSTR data_set = _T("Software\\Chicony\\Lenovo1");

	if (ERROR_SUCCESS == ::RegOpenKeyEx(HKEY_CURRENT_USER, data_set, 0, KEY_SET_VALUE, &hkey))
	{
		if (ERROR_SUCCESS != ::RegDeleteKeyA(hkey, "test1112"))
		{
			cout << "ɾ������" << endl;
		}
	}
	::RegCloseKey(hkey);
}

void trimString(std::string & str)
{
	int s = str.find_first_not_of(" ");
	int e = str.find_last_not_of(" ");
	str = str.substr(s, e - s + 1);
	return;
}

bool SerialNumber::CheckPcID()
{
#define fname "skin\\sa"
	bool bRet = false;

	//guid
	char pCpuId[32] = "";
	get_cpuId(pCpuId);
	char baseBoard[128] = { 0 };
	GetBaseBoardByCmd(baseBoard, 128);
	string strMac;
	GetMacByNetBIOS(strMac);
	string temp, temp1;
	temp1 = temp + pCpuId + "|" + baseBoard /*+ "|" + strMac*/;
	std::string src = temp1;
	//if (src.compare("BFEBFBFF000906EA|180630832801250|0C-9D-92-75-65-4F") != 0)
	//{
	//	char buf[256] = { 0 };
	//	sprintf_s(buf, "yinpanma error, %s", src.c_str());
	//	MessageBoxA(NULL, buf, "", MB_OK);
	//	return false; 
	//}
	Md5Encode encode_obj;
	std::string ret = encode_obj.Encode(src);
	std::cout << "info: " << src.c_str() << std::endl;
	std::cout << "md5: " << ret.c_str() << std::endl;
	int len = ret.length();
	trimString(ret);

	{
		SetFileAttributesA(fname, FILE_ATTRIBUTE_HIDDEN);
		char buffer[256] = { 0 };
		ofstream out(fname);
		ifstream in(fname);
		if (in.is_open())
		{
			while (!in.eof())
			{
				in.getline(buffer, 256);
				cout << buffer << endl;
				break;
			}
			in.close();
		}
		if (strlen(buffer)>0)
		{
			string a(buffer);
			trimString(a);
			if (a.compare(ret) == 0)
			{
				bRet = true;
			}
		}
		else
		{
			if (out.is_open())
			{
				out << ret;
				out.close();
				bRet = true;
			}
		}
	}
	return bRet;

	//��ȡע���
	char reg_read_buf[256] = { 0 };
	read_reg_sz(reg_read_buf);
	int len1 = strlen(reg_read_buf);

	char key[16];
	//memcpy(key,"\xDC\x32\x07\x1C\x1F\xBC\xF7\x0D\x67\xA8\x0B\x6E\x34\x49\x98\x51",16);
	memcpy(key, "\x01\x09\x08\x00\x09\x01\x07\x04\x01\x09\x09\x01\x00\x07\x02\x08", 16);

	//�����ļ���ȡguid����
	char buffer[256] = { 0 };
	ifstream in(fname);
	if (!in.is_open())
	{
		cout << "Error opening file";
	}
	else
	{
		while (!in.eof())
		{
			in.getline(buffer, 256);
			cout << buffer << endl;
			break;
		}
	}

	char strr[256] = { 0 };
	if (strlen(buffer) > 0)
	{
		memcpy(strr, buffer, 255);
		//��ȡ֮�����
		libTripleDes((uchar*)strr, (uchar*)strr, (uchar*)key, 0);
		//libDes(str, key, 0);
		printf("after decrypt...\n");
		printf("str = %s\n", strr);

		if (ret.compare(strr) == 0)
		{
			bRet = true;
		}
		SetFileAttributesA(fname, FILE_ATTRIBUTE_HIDDEN);
	}
	else
	{
		ofstream out(fname);
		if (out.is_open())
		{
			char str[256] = { 0 };
			memcpy(str, ret.c_str(), strlen(ret.c_str()));

			printf("before encrypt...\n");
			printf("str = %s\n", str);
			libTripleDes((uchar*)str, (uchar*)str, (uchar*)key, 1);
			//libDes(str, key, 1);
			printf("after encrypt...\n");
			printf("str = %s\n", str);

			out << str << "\n";
			//out << "abcdef is another line.\n";
			out.close();

			if (strlen(str) == 32)
			{
				bRet = true;
			}
		}
		SetFileAttributesA(fname, FILE_ATTRIBUTE_HIDDEN);
	}

	// 	if (len1!=32)
	// 	{
	// 		write_reg_sz(ret.c_str());
	// 	}
	return bRet;
}





