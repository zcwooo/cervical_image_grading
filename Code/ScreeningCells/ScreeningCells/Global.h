#pragma once

#define MSGID_OK		1
#define MSGID_CANCEL	0

struct LoginParam
{
	CDuiString sName;
	CDuiString sPassword;
	bool bLogin;
	LoginParam()
	{
		bLogin = false;
	}
};

extern struct LoginParam gLoginParam;

//流程标志
enum FlowDef
{
	FLOW_INVALID = 0,
	FLOW_PATIENTINFO,
	FLOW_SELECTEDUSER,
	FLOW_SAOMIAO,
	FLOW_ARCHIEVEOPEN,
	FLOW_SCREENING,
	FLOW_STATISTICS,
	FLOW_STATISTICSSHOW,
	FLOW_CELLLOOK,
	FLOW_REPORTE,
	FLOW_REPORTECHECKED,
};
extern WORD g_FLOW;
extern bool g_bScreening;


#define MSG_USER_SWITCH_MAIN       WM_USER+1000 
#define MSG_USER_SWITCH_MAIN1      WM_USER+1001
#define TIMER_USER_SWITCH_MAIN     5000
#define TIMER_USER_SWITCH_MAIN1    5001


//////////////////////////////////////////////////////////////////////////
#define WM_USER_CAL					(WM_USER + 102)

#define WM_LOGIN					WM_USER+110
#define WM_MAINDLG					WM_USER+111


//////////////////////////////////////////////////////////////////////////
///声音
#include "MMSYSTEM.h"
#pragma comment(lib,"winmm.lib")

#define SOUND_WARNING			PlaySound(_T("message_error.wav"), NULL, SND_FILENAME | SND_ASYNC);
#define SOUND_SUCCEED			PlaySound(_T("message.wav"), NULL, SND_FILENAME | SND_ASYNC);

//////////////////////////////////////////////////////////////////////////
//加密狗
#include <iostream>
#include <algorithm>
#include <sstream>
#include <stdio.h>

#include <string.h>

#include "hasp_api_cpp.h"
#include "errorprinter.h"

using namespace std;

//Data for the demo of the encryption/decryption functionality
unsigned char data1[] =
{
	0x74, 0x65, 0x73, 0x74, 0x20, 0x73, 0x74, 0x72,
	0x69, 0x6e, 0x67, 0x20, 0x31, 0x32, 0x33, 0x34
};
const unsigned int dataLen = sizeof(data1);

//The size of the part of the memory which 
//will be printed during the memory demo
const hasp_size_t numBytesToShow = 64;

void displayCloseMessage();

//Displays the content of mdata in a nice format with an ascii
//and a hexadecimal area.
void displayMemory(unsigned char *mdata, unsigned int mdataLen);

//Displays a ChaspTime in a nice format
void displayTime(const ChaspTime& haspTime);

//加密狗判断
#define IsDog		1
#define FeatureId	10618
haspStatus gEncryptionDogVertify();
//查询加密狗次数
int gEncryptionDogLeftCount();