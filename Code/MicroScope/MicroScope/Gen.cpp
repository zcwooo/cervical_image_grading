#include "stdafx.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <direct.h>
#include "Shlwapi.h"
#include "gen.h"

void InitPath(TCHAR* lpBin,TCHAR *lpINI,TCHAR *lpLog)
{
	TCHAR lpPath[MAX_PATH];

#if 0
	//方法1
	GetModuleFileName(NULL,lpPath,sizeof(lpPath)-1);
	PathRemoveFileSpec(lpPath);
	PathRemoveBackslash(lpPath);
	PathAddBackslash(lpPath);
#else
	//方法2
	//获取路径
	::GetModuleFileName(NULL, (lpPath), MAX_PATH - 1);
	*(_tcsrchr(lpPath,'\\'))   =   '\0';
	_tcscat(lpPath,_T("\\"));
#endif

	if(lpBin!=NULL)
		_stprintf(lpBin,_T("%s"),lpPath);
	if(lpINI!=NULL)
		_stprintf(lpINI,_T("%s%s"),lpPath,_T("pst.ini"));
	if(lpLog!=NULL)
		_stprintf(lpLog,_T("%s"),lpPath);
		//_stprintf(lpLog,"%s%s",lpPath,"log.inf");
}

void SaveLogInfo(TCHAR *lpIP,TCHAR* lpInfo,TCHAR* lpLogPath,BOOL bLogOn)
{
/*
	if(bLogOn)
	{
		CTime atime;
		atime = CTime::GetCurrentTime();
		TCHAR lpStr[64];

		_stprintf(lpStr,"%02d月%02d日%02d:%02d:%02d",atime.GetMonth(),atime.GetDay(),atime.GetHour(),atime.GetMinute(),atime.GetSecond());

		WritePrivateProfileString("INFO",lpStr,lpInfo,lpLogPath);
	}
*/
	if(bLogOn)
	{
		TCHAR lpTitle[64];
		TCHAR lpSub[64];
		TCHAR lpIniFName[MAX_PATH];
		SYSTEMTIME tmCurrent;
		::GetLocalTime( &tmCurrent );

		_stprintf( lpIniFName, _T("%s%s%04d%02d%s"), lpLogPath, _T("LOG_"),tmCurrent.wYear, tmCurrent.wMonth, _T(".ini") );
		_stprintf( lpTitle, _T("%04d%02d%02d"), tmCurrent.wYear, tmCurrent.wMonth, tmCurrent.wDay );
		_stprintf( lpSub, _T("%02d%02d%02d.%03d"), tmCurrent.wHour, tmCurrent.wMinute, tmCurrent.wSecond, tmCurrent.wMilliseconds );
		TCHAR lpStr[256];
		if(lpIP!=NULL)
			_stprintf(lpStr,_T("%s:%s"),lpIP,lpInfo);
		else
			_stprintf(lpStr,_T("%s"),lpInfo);
		WritePrivateProfileString( lpTitle, lpSub, lpStr, lpIniFName );
	}
}

BOOL WritePrivateProfileInt( LPCTSTR lpAppName, LPCTSTR lpKeyName, int nVal, LPCTSTR lpFileName )
{
    TCHAR lpString[128];
    _stprintf( lpString, _T("%d"), nVal );
    return WritePrivateProfileString( lpAppName, lpKeyName, lpString, lpFileName );
}

void SetStatusText(int id,const TCHAR* text)
{
	if(id>3)
		return;
	CStatusBar* pStatus=(CStatusBar*)
		AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR); 
	//     AfxGetApp()->m_pMainWnd->GetDescendantWindow(ID_MY_STATUS_BAR);
	if(pStatus)
	{
		pStatus->SetPaneText(id,text);
	}          
} 

void MakeImgPath_BL_No(TCHAR *lpPath,TCHAR *m_lpBL_No) 
{
	TCHAR FilePath[MAX_PATH];
	CString szAdjFilePath;

	//获取路径
	::GetModuleFileName(NULL, FilePath, MAX_PATH - 1);
	*(_tcsrchr(FilePath,_T('\\')))   =   '\0';
	
	szAdjFilePath.Format(_T("%s\\Img-%s\\"), FilePath,m_lpBL_No);
	_wmkdir(szAdjFilePath);
	_stprintf(lpPath,_T("%s"),szAdjFilePath);
}
void MakeImgPath(TCHAR *lpPath) 
{
	TCHAR FilePath[MAX_PATH];
	CString szAdjFilePath;

	//获取路径
	::GetModuleFileName(NULL, FilePath, MAX_PATH - 1);
	*(_tcsrchr(FilePath,_T('\\')))   =   '\0';
	
	
	CTime aTime = CTime::GetCurrentTime();
	
	szAdjFilePath.Format(_T("%s\\Img-%04d%02d%02d%02d%02d%02d\\"), FilePath,
		aTime.GetYear(),aTime.GetMonth(),aTime.GetDay(),aTime.GetHour(),aTime.GetMinute(),aTime.GetSecond());
	_wmkdir(szAdjFilePath);
	_stprintf(lpPath,_T("%s"),szAdjFilePath);
}


//bBMP_JPEG:  0: BMP, 1: jpeg
void MakeImgName(TCHAR *lpDir,TCHAR *lpName,int xSeri,int ySeri) 
{
	TCHAR lpFName[MAX_PATH];

//	_stprintf(lpFName,_T("%s%03d-%03d.jpg"),lpDir,xSeri,ySeri);
	_stprintf(lpFName,_T("%s%03d-%03d.jpg"),lpDir,ySeri,xSeri);

	if(lpName!=NULL)
		_stprintf(lpName,_T("%s"),lpFName);
}

int time_to_long_mil(int hour,int nMin,int nSec,int nMil)
{
	return (hour*3600 + nMin*60 + nSec)*1000 + nMil;
}


void Get_PC_Time(TCHAR* lpTime)
{
	//TCHAR lpTime[32];
	SYSTEMTIME tmCurrent0;
	::GetLocalTime( &tmCurrent0 );
	int nSleepMS = 20;
	for(int i=0;i<2*(1000/nSleepMS);i++)
	{
		Sleep(nSleepMS);
		SYSTEMTIME tmCurrent;
		::GetLocalTime( &tmCurrent );

		//_stprintf(lpTime,"%d: (%d,%d)\n",i,tmCurrent0.wSecond,tmCurrent.wSecond);
		//OutputDebugString(lpTime);

		if(tmCurrent.wSecond != tmCurrent0.wSecond)	//秒变化了。
		{
			_stprintf( lpTime, _T("%04d.%02d.%02d-%02d:%02d:%02d"), tmCurrent.wYear, tmCurrent.wMonth, tmCurrent.wDay, tmCurrent.wHour, tmCurrent.wMinute, tmCurrent.wSecond);
			break;
		}
	}
}


/*
//return 1:busy;  0: end auto focus
int qAutoFocus::CheckAutoFocusStatus()
{
	return m_nAutoFocusStep;
}
*/
//计算图像的梯度.
//参考：  《自动对焦方案-from彭-20170508-Qiao.docx》
double Tenengrad(unsigned char *pSrc, int nWidth, int nHeight)
{
/*
    if(pSrc == NULL) 
    {
        return -1;
    }
    if ((nWidth <= 3) || (nHeight <= 3))
    {
        return -2;
    }
*/
    int i = 0;
    int j = 0;
    int nGX = 0;
    int nGY = 0;
	double nSumG = 0;

	unsigned char *pPreRow = pSrc;
	unsigned char *pCurRow = pSrc +  nWidth;
	unsigned char *pNexRow = pSrc + nWidth + nWidth;

    for (i = 1; i < nHeight - 1; i++)
    {
        for (j = 1; j <nWidth - 1; j++)
        {
            nGX = (int)pPreRow[j + 1] + (((int)pCurRow[j + 1])<<1) + (int)pNexRow[j + 1] - (int)pPreRow[j - 1] - (((int)pCurRow[j - 1])<<1) - (int)pNexRow[j - 1];

            nGY = (int)pPreRow[j - 1] + (((int)pPreRow[j])<<1) + (int)pPreRow[j + 1] - (int)pNexRow[j - 1] - (((int)pNexRow[j])<<1) - (int)pNexRow[j + 1];

            nSumG += nGX * nGX + nGY * nGY;

        }
		pPreRow += nWidth;
		pCurRow += nWidth;
		pNexRow += nWidth;
    }

    return nSumG / ((nWidth - 2) * (nHeight - 2));
}

double rd_V_FromIni(TCHAR* lpSec,TCHAR* lpIni)
{
	TCHAR lpV[64];
	GetPrivateProfileString(_T("T_POS"),lpSec,_T("0"),lpV,sizeof(lpV)-1,lpIni);
	return _tstof(lpV);
}

void TcharToChar(const TCHAR * tchar, char * _char)
{
    int iLength;
    //获取字节长度   
    iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);
    //将tchar值赋给_char    
    WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);
}

void CharToTchar(const char * _char, TCHAR * tchar)
{
    int iLength;
    iLength = MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, NULL, 0);
    MultiByteToWideChar(CP_ACP, 0, _char, strlen(_char) + 1, tchar, iLength);
}
