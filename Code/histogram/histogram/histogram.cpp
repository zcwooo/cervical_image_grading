// ImageHistogram.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "opencv2\opencv.hpp"
#include "opencv2\core\core.hpp"
#include "cv.h"
#include <iostream>
#include <io.h>
#include <string>
using namespace std;
using namespace cv;

int nImageNumAll = 0;

TCHAR gszFolder[MAX_PATH * 2];
INT CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData)
{
	LPITEMIDLIST tmp = (LPITEMIDLIST)lp;
	switch (uMsg)
	{
	case   BFFM_INITIALIZED:
		SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)gszFolder);
		break;

	case   BFFM_SELCHANGED:
		TCHAR szc[MAX_PATH] = { 0 };
		if (SHGetPathFromIDList((LPITEMIDLIST)tmp, szc))
		{
			SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)szc);
		}
		break;
	}
	return   0;
}

int _tmain(int argc, _TCHAR* argv[])
{
	vector<CStringA> vecImageFileNameA;

	char *FileName;
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.pidlRoot = NULL;
	bi.hwndOwner = NULL;
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_USENEWUI;
	bi.lpfn = BrowseCallbackProc;
	bi.lParam = (LPARAM)&gszFolder;
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
	BOOL bRet = FALSE;
	if (pidl)
	{
		if (SHGetPathFromIDList(pidl, gszFolder))bRet = TRUE;
		IMalloc *pMalloc = NULL;
		if (SUCCEEDED(SHGetMalloc(&pMalloc)) && pMalloc)
		{
			pMalloc->Free(pidl);
			pMalloc->Release();
		}
	}
	CString str = gszFolder;

	//TCHAR lpPath[MAX_PATH];
	//GetModuleFileName(NULL, lpPath, sizeof(lpPath) - 1);
	//PathRemoveFileSpec(lpPath);
	//PathRemoveBackslash(lpPath);
	//PathAddBackslash(lpPath);
	CStringA pathA, pathB;
	pathA = str;
	pathB = pathA + "\\*.jpg";
	nImageNumAll = 0;
	struct _finddata_t fileinfo;
	long fHandle;
	if ((fHandle = _findfirst(pathB.GetString(), &fileinfo)) == -1L)
	{
		printf("当前目录下没有bmp文件\n");
	}
	else
	{
		do
		{
			CStringA name;
			name = pathA + "\\" + fileinfo.name;
			vecImageFileNameA.push_back(name.GetString());
			nImageNumAll++;
		} while (_findnext(fHandle, &fileinfo) == 0);
	}

	for (int i = 0; i < nImageNumAll; ++i)
	{
		cv::Mat mat;
		mat = cv::imread(vecImageFileNameA.at(i).GetString(), cv::IMREAD_GRAYSCALE);
		int iHeight = mat.rows, iWidth = mat.cols;

		int igray[256] = { 0 };
		int image_bk_gray = -1;
		uchar u = 0;
		for (int m = 0; m < iHeight; ++m)
		{
			for (int n = 0; n < iWidth; ++n)
			{
				u = mat.at<uchar>(m, n);
				if (u >= 0 && u < 256)
				{
					igray[u]++;
				}
				else
				{
					printf("////////////////////////////////////// error: 1\n");
				}
			}
		}
		int  _temp = -1;
		for (int m = 0; m < 256; ++m)
		{
			if (igray[m]>_temp)
			{
				_temp = igray[m];
				image_bk_gray = m;
			}
		}
		if (image_bk_gray < 0 || image_bk_gray >= 256)
		{
			printf("////////////////////////////////////// error: 2\n");
		}
		char buf[512] = { 0 };
		sprintf_s(buf, "%d  [%s]	gray=[%d]\n", i, vecImageFileNameA.at(i).GetString(), image_bk_gray);
		printf(buf);
	}

	printf("succeed.\n");
	getchar();
	return 0;
}

