#ifndef __Q_GEN__
#define __Q_GEN__

typedef struct tagQPal
{
	char strID[8];
	short nStart;
	short nEnd;
}QPal;


typedef struct tagOEMInfo
{
	int bLargeIcon_ON;
	RECT rectLargeIcon;
	int bSmallIcon_ON;
	//RECT rectSmallIcon;
	int nSmallIconXSize;
	int nSmallIconYSize;
	int hLargeIcon;
	int hSmallIcon;
	char lpTitle[128];
}stOEMInfo;

//测试用户控制用
//
typedef struct
{
	int x;   
	int y;            
}TestPara_t;
//

#define IS_WIN30_DIB(lpbi)		((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))

void InitPath(TCHAR* lpBin,TCHAR *lpINI,TCHAR *lpLog);
void SaveLogInfo(TCHAR *lpIP,TCHAR* lpInfo,TCHAR* lpLogPath,BOOL bLogOn);
void SetStatusText(int id,const TCHAR* text);



void MakeImgName(TCHAR *lpDir,TCHAR *lpName,int xSeri,int ySeri);
void MakeImgPath(TCHAR *lpPath);
void MakeImgPath_BL_No(TCHAR *lpPath,TCHAR *m_lpBL_No);


BOOL WritePrivateProfileInt( LPCTSTR lpAppName, LPCTSTR lpKeyName, int nVal, LPCTSTR lpFileName );

int time_to_long_mil(int hour,int nMin,int nSec,int nMil);

void Get_PC_Time(TCHAR* lpTime);

double Tenengrad(unsigned char *pSrc, int nWidth, int nHeight);

double rd_V_FromIni(TCHAR* lpSec,TCHAR* lpIni);


void TcharToChar(const TCHAR * tchar, char * _char);
void TcharToChar(const TCHAR * tchar, char * _char);
#endif
