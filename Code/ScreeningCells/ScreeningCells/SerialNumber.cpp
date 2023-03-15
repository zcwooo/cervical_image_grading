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
#if _MSC_VER >= 1400 //VC2005才支持__cpuid  
	__cpuid((int*)(void*)CPUInfo, (int)(InfoType));
#else //其他使用getcpuidex  
	getcpuidex(CPUInfo, InfoType, 0);
#endif  
#endif  
}
void SerialNumber::getcpuidex(unsigned int *CPUInfo, unsigned int InfoType, unsigned int ECXValue)
{
#if defined(_MSC_VER) // MSVC  
#if defined(_WIN64) // 64位下不支持内联汇编. 1600: VS2010, 据说VC2008 SP1之后才支持__cpuidex.  
	__cpuidex((int*)(void*)CPUInfo, (int)InfoType, (int)ECXValue);
#else  
	if (NULL == CPUInfo)
		return;
	_asm{
		// load. 读取参数到寄存器.  
		mov edi, CPUInfo;
		mov eax, InfoType;
		mov ecx, ECXValue;
		// CPUID  
		cpuid;
		// save. 将寄存器保存到CPUInfo  
		mov[edi], eax;
		mov[edi + 4], ebx;
		mov[edi + 8], ecx;
		mov[edi + 12], edx;
	}
#endif  
#endif  
}


///////////////////////////////////////////////////////////////////////////////////////////////
//主板序列号

BOOL SerialNumber::GetBaseBoardByCmd(char *lpszBaseBoard, int len/*=128*/)
{
	const long MAX_COMMAND_SIZE = 10000; // 命令行输出缓冲大小	
	WCHAR szFetCmd[] = L"wmic BaseBoard get SerialNumber"; // 获取主板序列号命令行	
	const string strEnSearch = "SerialNumber"; // 主板序列号的前导信息

	BOOL   bret = FALSE;
	HANDLE hReadPipe = NULL; //读取管道
	HANDLE hWritePipe = NULL; //写入管道	
	PROCESS_INFORMATION pi;   //进程信息	
	STARTUPINFO			si;	  //控制命令行窗口信息
	SECURITY_ATTRIBUTES sa;   //安全属性

	char			szBuffer[MAX_COMMAND_SIZE + 1] = { 0 }; // 放置命令行结果的输出缓冲区
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

	//1.0 创建管道
	bret = CreatePipe(&hReadPipe, &hWritePipe, &sa, 0);
	if (!bret)
	{
		goto END;
	}

	//2.0 设置命令行窗口的信息为指定的读写管道
	GetStartupInfo(&si);
	si.hStdError = hWritePipe;
	si.hStdOutput = hWritePipe;
	si.wShowWindow = SW_HIDE; //隐藏命令行窗口
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

	//3.0 创建获取命令行的进程
	bret = CreateProcess(NULL, szFetCmd, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi);
	if (!bret)
	{
		goto END;
	}

	//4.0 读取返回的数据
	WaitForSingleObject(pi.hProcess, 500/*INFINITE*/);
	bret = ReadFile(hReadPipe, szBuffer, MAX_COMMAND_SIZE, &count, 0);
	if (!bret)
	{
		goto END;
	}

	//5.0 查找主板序列号
	bret = FALSE;
	strBuffer = szBuffer;
	ipos = strBuffer.find(strEnSearch);

	if (ipos < 0) // 没有找到
	{
		goto END;
	}
	else
	{
		strBuffer = strBuffer.substr(ipos + strEnSearch.length());
	}

	memset(szBuffer, 0x00, sizeof(szBuffer));
	strcpy_s(szBuffer, strBuffer.c_str());

	//去掉中间的空格 \r \n
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
	//关闭所有的句柄
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
	Ncb.ncb_command = NCBRESET; // 重置网卡，以便我们可以查询
	Ncb.ncb_lana_num = adapterNum;
	if (Netbios(&Ncb) != NRC_GOODRET)
		return false;

	// 准备取得接口卡的状态块
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
	// 取得网卡列表
	LANA_ENUM adapterList;
	NCB Ncb;
	memset(&Ncb, 0, sizeof(NCB));
	Ncb.ncb_command = NCBENUM;
	Ncb.ncb_buffer = (unsigned char *)&adapterList;
	Ncb.ncb_length = sizeof(adapterList);
	Netbios(&Ncb);

	// 取得MAC
	for (int i = 0; i < adapterList.length; ++i)
	{
		if (GetAdapterInfo(adapterList.lana[i], macOUT))
			return true;
	}

	return false;
}

//注册表
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
			cout << "无法查询有关的注册表信息" << endl;
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
				cout << "写入错误" << endl;
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
			cout << "删除错误" << endl;
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
			cout << "删除错误" << endl;
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

	//读取注册表
	char reg_read_buf[256] = { 0 };
	read_reg_sz(reg_read_buf);
	int len1 = strlen(reg_read_buf);

	char key[16];
	//memcpy(key,"\xDC\x32\x07\x1C\x1F\xBC\xF7\x0D\x67\xA8\x0B\x6E\x34\x49\x98\x51",16);
	memcpy(key, "\x01\x09\x08\x00\x09\x01\x07\x04\x01\x09\x09\x01\x00\x07\x02\x08", 16);

	//本地文件读取guid数据
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
		//读取之后解密
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





