2018.6.8
1，EasySize.h
2，
include:
	$(PYLON_DEV_DIR)\include
lib:
	$(PYLON_DEV_DIR)\lib\Win32
3, 编译选项：
	sprintf  ->  sprintf_s
	fopen    ->  fopen_s

	右击工程 - 属性 - 配置属性 - C/C++  - 命令行
		/D _CRT_SECURE_NO_WARNINGS
4,
	UNICODE  ->  Use Multi-Byte Character Set		

2018.6.16
	（1）相机初始化时候，要：
	m_pCamera->ExposureAuto.SetValue(ExposureAuto_Off);
	如果是自动曝光状态，调用下面函数出错：
	m_pCamera->ExposureTime.SetValue(dExposureTime);
	（2）
	如果一直处于自动曝光状态，图像没有反应？
	AutoExposureContinuous(m_pCamera);
	
	=》
	（1）
		启动时候做一次（保存以前的值）；
		启动做1次。
	（2）定时做。
	
2018.6.17
	初始化时候读取以前的曝光值；
	退出时候保存当前曝光值。

***修改：
	在采集图像的回调函数中不显示图像，掉invaliddate()去显示，尝试解决界面想要慢的问题。
2018.7.14
	(1)水平方向每间隔若干步、再聚焦
	(2)
		//下面的函数不识别中文路径
		//Pylon::CImagePersistence::Save(ImageFileFormat_Jpeg, lpFName, ptr);
		用SaveImageFile()代替
2018.7.21
	（1）考虑判断视野内有没有细胞：通过图像均方差计算
	（2）第一次需要自动聚焦的范围、步长大一些。
已知问题：
	对于圆形细胞切片，四周不好聚焦。
    按照矩形扫描，初始位置定位在左上角不易人工聚焦，造成后面不易聚焦。
    下面准备改为定位在圆心，然后按照圆形扫描。
2018.7.22
	先判断图像内是否有细胞、然后再去聚焦这种方法，采集速度慢。
=》
	改为：
	计算清晰度的时候，依据近清晰度来判断图像内是否有细胞。
	
2018.7.25
	#define DON_NOT_WAIT_ADD_IMG	//不再多等待1幅图像
	
2018.9.6
	准备加入采图进度（用色块标记）
	IDC_ACTIVE_IMG
	IDC_PROGRESS
2019.9.8
	加入采图过程的状态
2018.9.11
	fix a bug:
	停止活动图像后，退出程序时候，再调用：
	m_dExposureTime=m_pCamera->ExposureTime.GetValue();
	会报错。
		
不知道为什么，在我的开发电脑上，Basler相机容易出现：
	Err Img=19
	Trigger1Img err.

版本：1.0.0.3

2018.9.12
	MFC使用动态库，不再使用静态库。
=>	使用OpenCV，需要：
	（1）shared dll，不能是static 连接
	（2）code generation中的runtime library需要 multi-thread dll选项：  MD或者MDd选项
	编译错误：
	1>c:\program files (x86)\microsoft visual studio 9.0\vc\atlmfc\include\afxtempl.h(1259) : error C3859: virtual memory range for PCH exceeded; please recompile with a command line option of '-Zm111' or greater
	1>c:\program files (x86)\microsoft visual studio 9.0\vc\atlmfc\include\afxtempl.h(1259) : fatal error C1076: compiler limit : internal heap limit reached; use /Zm to specify a higher limit
	1>Build log was saved at "file://f:\work.now\MicroScope\MicroScope\Debug\BuildLog.htm"

=》
	解决办法很简单，工程上点击右键 -> 属性(Properties) -> C/C++ -> 命令行(Command Line)， 在额外选项(Additional options)中按错误提示输入-Zm111 （或更大数字）重编译即可。

版本：1.0.0.4

2018.9.13
	opencv需要的（在工程的属性中设置）：
	（1）include:
		.\opencv\build\include\opencv2
		.\opencv\build\include\opencv
		.\opencv\build\include

/*	
		D:\软件\opencv\build\include\opencv2
		D:\软件\opencv\build\include\opencv
		D:\软件\opencv\build\include
		$(PYLON_DEV_DIR)\include
*/

	（2）lib和dll:
		路径：	.\opencv\build\x86\vc9\lib
//		路径：	D:\软件\opencv\build\x86\vc9\lib
		release lib：
			opencv_calib3d231.lib
			opencv_contrib231.lib
			opencv_core231.lib
			opencv_features2d231.lib
			opencv_flann231.lib
			opencv_gpu231.lib
			opencv_highgui231.lib
			opencv_imgproc231.lib
			opencv_legacy231.lib
			opencv_ml231.lib
			opencv_objdetect231.lib
			opencv_ts231.lib
			opencv_video231.lib
		debug lib：			
			opencv_calib3d231d.lib
			opencv_contrib231d.lib
			opencv_core231d.lib
			opencv_features2d231d.lib
			opencv_flann231d.lib
			opencv_gpu231d.lib
			opencv_highgui231d.lib
			opencv_imgproc231d.lib
			opencv_legacy231d.lib
			opencv_ml231d.lib
			opencv_objdetect231d.lib
			opencv_ts231d.lib
			opencv_video231d.lib		

【问题】
	使用 #define  __CAL_CELL__	有内存泄漏？
	
2018.9.14
	参考Basler的demo例子：Gui_Sample，修改采集
	
2018.9.15
	（1）参考Basler的demo例子：Gui_Sample，修改采集,加入lock
	（2）测试发现，开始采集的时候，打开AutoWhiteBalance()，启动慢 =》使用配置参数控制
	（3）显示信息不及时，修改：
		CWnd* pWnd=GetDlgItem(IDC_EDIT_DUOBEITI);
		pWnd->SetWindowText((LPCTSTR)lpInfo);
	=>	
		SetDlgItemText(IDC_EDIT_DUOBEITI,lpInfo);
		UpdateWindow();	
	（4）如果VS2008编译报错：
		mutable CCriticalSection m_MemberLock;
	=>	手动添加：afxmt.h
版本：1.0.0.5
	
2018.9.22
	(1)显示：每间隔一帧，显示一次；
	(2)BOOL m_bBusy;	//在忙的时候，不发消息
	void CMicroScopeDlg::OnImageGrabbed(Pylon::CInstantCamera& camera, const Pylon::CGrabResultPtr& grabResult)
	{
		if(!m_bBusy)
			PostMessage(ID_NEW_GRABRESULT, 0, 0);
		else
		{
			OutputDebugString("-*-*-Busy.\n");
		}	
	}
	(3)计算cell的全局变量，用new分配、释放，仍然报告内存泄漏。
	
版本：1.0.0.6
	
2018.9.23
	简单优化,在回调中copy，不在onpaint中copy：
	LRESULT CMicroScopeDlg::OnNewImg(WPARAM wparam, LPARAM lparam)
	{
		if (ptr.IsValid() && ptr->GrabSucceeded())
		{
			// This is where you would do image processing
			// and other tasks.
			// Attention: If you perform lengthy operations, the GUI may become
			// unresponsive as the application doesn't process messages.

			++m_cntGrabbedImages;
			if((m_cntGrabbedImages%2)==0)
			{
***				m_bitmapImage.CopyImage(ptr);
				。。。
			}
	}	
	
版本：1.0.0.7

2018.9.24
	优化：
	对话窗初始化的时候，就打开设备。
版本：1.0.0.8
	
2018.9.25
	（1）todo：
		咨询basler的技术支持，说不用回调方式、用线程方式采集、单独用线程处理。
	（2）第一步聚焦：
		pst.ini：
		first_focus_steps=7
	（3）使用内存锁，出现死锁？去掉。
		CSingleLock lock(&m_MemberLock, TRUE);
		lock.Unlock();
		
		【但是】
		不加内存锁的话，存储的图像有半幅图像的情况。
		用CCriticalSection m_Lock;代替mutable CCriticalSection m_MemberLock;
		

简单测试：	OK
todo：
(1)		采集过程，拔掉USB接口
(2)		直接在回调中存图、处理平台的消息，不再在消息中处理。
		
版本：1.0.0.9

2018.9.29
    (1)采集完成，播放声音
	(2)尝试中心点仔细聚焦，但是实际效果不好。
=》	改为增加用户按键。
版本：1.0.1.0
	
2018.9.30
	启动程序时不再设置自动亮度，放在设置界面中设置。
版本：1.0.1.1
	
【todo】
采图过程、拔插USB相机后，再重新采集，程序异常。

	
2018.10.5
	（1）去掉：
		//m_camera.RegisterImageEventHandler(this, Pylon::RegistrationMode_ReplaceAll, Pylon::Ownership_ExternalOwnership);
        //m_camera.RegisterConfiguration(this, Pylon::RegistrationMode_ReplaceAll, Pylon::Ownership_ExternalOwnership);
     (2)
        error = pDlg->m_camera.RetrieveResult(5000, pGrabResult, TimeoutHandling_ThrowException/*TimeoutHandling_Return*/);
      =>  
        error = pDlg->m_camera.RetrieveResult(3000, pGrabResult, TimeoutHandling_ThrowException/*TimeoutHandling_Return*/);
	
	 (3)
		rc = WaitForSingleObject(g_hEvent,3000);
	=>	rc = WaitForSingleObject(g_hEvent,2000);
	(4)
	计算结果存储到：cell_result.ini
	
版本：1.0.1.2	
	
2018.10.18
	简单优化
版本：1.0.2.0
	测试发现：
	每间隔1步精细聚焦、然后减小步长聚焦：
	（1）精细聚焦时，步长3比较OK（中心点上下3步）
	（2）简略聚焦，步长2比较OK（中心点上下2步）。步长1不行。

2018.10.24
	线程相关的函数不再放在类中，作为全局函数。
版本：1.0.2.2

2018.10.29
	重新布界面

=======================	
2018.11.24
	（1）使用ImageSource相机，不使用Basler相机；	
	（2）ANSI和UNICODE字符串处理函数
		https://blog.csdn.net/chenzy945/article/details/51931148
	（3）为了保存的图像支持中文目录，使用UNICODE
	
奇怪的问题：
	BOOL CMicroScopeApp::InitInstance()
	{
		if( !DShowLib::InitLibrary() )
		{
			AfxMessageBox( TEXT("The IC Imaging Control Class Library could not be initialized.\n(invalid license key?)") );
			exit( 1 );
		}
		//atexit(DShowLib::ExitLibrary );	//放在此处，造成m_cz的OLE异常

		...
		
		INT_PTR nResponse = dlg.DoModal();
		if (nResponse == IDOK)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with OK
		}
		else if (nResponse == IDCANCEL)
		{
			// TODO: Place code here to handle when the dialog is
			//  dismissed with Cancel
		}

	*	DShowLib::ExitLibrary();		
	}
	
版本：2.0.0.1


2018.11.28
	（1）加入相机设置
	（2）程序退出时,加入：（否则程序报错）
	void CMicroScopeDlg::OnDestroy()
	{
		if(m_cz.m_bAutoRelease)
		{
			m_cz.m_lpDispatch->Release();
			m_cz.m_lpDispatch = NULL;
		}

		if(m_stage.m_bAutoRelease)
		{
			m_stage.m_lpDispatch->Release();
			m_stage.m_lpDispatch = NULL;
		}

		if(m_scan.m_bAutoRelease)
		{
			m_scan.m_lpDispatch->Release();
			m_scan.m_lpDispatch = NULL;
		}	
	}
2018.12.6
	上皮细胞超过m_nShangpi_Num_Limit时，停止自动采集。
	
版本：2.0.0.2

2018.12.23
	(1)加入程序只运行一次的代码
	(2)修改杂质计数
	(3)扫描完成后，自动计算
	
版本：2.0.0.3

2019.1.10
	增加：
	(1)上皮细胞数目小于8000时，自动从最大面积扫描。
	double m_dMaxRadius_mm;		//最大按照圆形扫描，圆半径

	(2)
	pst.ini:
	[CalCell]
	Shangpi_Num_Limit=8000
	MAX_scan_R_mm=4.5
	
	(3)
	#define WM_RE_SCAN					(WM_USER + 6)
	if(g_nCountCells<pDlg->m_nShangpi_Num_Limit)
	{
		OutputDebugString(_T("Need rescan.\n"));
		pDlg->PostMessage(WM_RE_SCAN,1,0);
	}
	
	(4)
	LRESULT CMicroScopeDlg::OnRescan(WPARAM wparam, LPARAM lparam)
	{
		...
	}
	(5)存储图像路径
	LRESULT CMicroScopeDlg::OnChangeTtnName(WPARAM wparam, LPARAM lparam)
	{
		m_cBtnAuto.SetWindowText(TEXT("自动扫片"));
		if(wparam==1)
		{
			OnBnClickedBtnStatistic();
		}
		TCHAR lpINI[MAX_PATH];
		wsprintf(lpINI,_T("%s%s"),g_lpBinPath,_T("ImgDir.ini"));
*		WritePrivateProfileString(_T("LastDir"),_T("img_dir"),m_lpDir,lpINI);
		return 1;
	}
	
版本：	
	2.0.0.4
	
2019.1.11
	从	number.ini读取BL号，文件存储到该目录下。

	rdBL_No();
	if(_tcscpy(m_lpBL_No,_T("11223344"))==0)
		MakeImgPath(m_lpDir);
	else
		MakeImgPath_BL_No(m_lpDir,m_lpBL_No);	
		
2019.1.14
	//淋巴
	_stprintf(pDlg->m_lpResult_LinBa, _T("%d"), g_nCountLymphocyte);
=》
	_stprintf(pDlg->m_lpResult_LinBa, _T("%d"), g_nCountLymphocyte*0.04);

	//杂质
	_stprintf(pDlg->m_lpResult_Impurity, _T("%d"), (int)(g_nCountImpurity*0.1));
=>
	_stprintf(pDlg->m_lpResult_Impurity, _T("%d"), (int)(g_nCountImpurity*0.1*25));

2019.1.15
(1)	中性粒细胞的数量，在原来的基础上乘0.4
	int nCountXianxingli = nLastXianXinLi + (g_nCountLymphocyte - nLastLinBa)*(0.5+nRand); 
*	nCountXianxingli = nCountXianxingli*0.4;
(2)
#if 0	//2019.1.15
	for (int i = 0; i < g_nCountCells; i++)
	{
		if ((g_dCellsLambda[i] <= 1.25) && (g_dCellsLambda[i] > 0.75))
		{
			nCountNUMArray[0]++;
			continue;
		}
		if ((g_dCellsLambda[i] <= 2.25) && (g_dCellsLambda[i] > 1.75))
		{
			nCountNUMArray[1]++;
			continue;
		}
		if ((g_dCellsLambda[i] <= 3.25) && (g_dCellsLambda[i] > 2.75))
		{
			nCountNUMArray[2]++;
			continue;
		}
		if (g_dCellsLambda[i] > 3.75)
		{
			nCountNUMArray[3]++;
			continue;
		}
	}
#endif
	int nCountNUMArray[4] = { 0 };
	for (int i = 0; i < g_nCountCells; i++)
	{
		if ((g_dCellsLambda[i] <= 1.25) && (g_dCellsLambda[i] > 0.75))
		{
			nCountNUMArray[0]++;
			continue;
		}
		if ((g_dCellsLambda[i] <= 3.75) && (g_dCellsLambda[i] > 1.25))
		{
			nCountNUMArray[1]++;
			continue;
		}
		if ((g_dCellsLambda[i] <= 5) && (g_dCellsLambda[i] >3.75))
		{
			nCountNUMArray[2]++;
			continue;
		}
		if (g_dCellsLambda[i] > 5)
		{
			nCountNUMArray[3]++;
			continue;
		}
	}
	


int nCountNUMArray[4] = { 0 };
	for (int i = 0; i < g_nCountCells; i++)
	{
		if ((g_dCellsLambda[i] <= 1.5) && (g_dCellsLambda[i] > 0.75))
		{
			nCountNUMArray[0]++;
			continue;
		}
		if ((g_dCellsLambda[i] <= 3.6) && (g_dCellsLambda[i] > 1.5))
		{
			nCountNUMArray[1]++;
			continue;
		}
		if ((g_dCellsLambda[i] <= 5) && (g_dCellsLambda[i] >3.6))
		{
			nCountNUMArray[2]++;
			continue;
		}
		if (g_dCellsLambda[i] > 5)
		{
			nCountNUMArray[3]++;
			continue;
		}
	}
	
2019.2.12
	最大扫描面积，默认是5.5，可以从配置文件读取：

（1）	
//#define MAX_RADIUS	5.5		//最大圆半径6.5mm
double MAX_RADIUS=5.5;
（2）
*	GetPrivateProfileString(_T("StagePara"),_T("max_radius_mm"),_T("5.5"),lpInfo,sizeof(lpInfo)-1,lpINI);
*	MAX_RADIUS = _tstof(lpInfo);

	GetPrivateProfileString(_T("StagePara"),_T("radius_mm"),_T("5"),lpInfo,sizeof(lpInfo)-1,lpINI);
	m_dRadius_mm = _tstof(lpInfo);
	if(m_dRadius_mm>MAX_RADIUS)
	{
		m_dRadius_mm=5.0;
		_stprintf(lpInfo,_T("Radius too large,set to 5mm. Max radius=%.2fmm\n"),MAX_RADIUS);
		OutputDebugString(lpInfo);
	}
	
版本：
	2.0.0.6
	
2019.3.3 增加了扫片后自动计算功能 
if(g_nCountCells>=pDlg->m_nShangpi_Num_Limit)
		pDlg->PostMessage(WM_AUTO_CAL,0,0);
		
2019.3.23
	增加亮度、直方图显示，便于调整图像亮度。
	
版本：
	2.0.0.7
		
2019.6.24
	彩色图像统计亮度直方图，用灰度，不用绿色通道：
	
	else if(nBits==24)	//Gray = (R*299 + G*587 + B*114 + 500) / 1000	//Gray = (R*19595 + G*38469 + B*7472) >> 16
	{
#if 0
		nPos=1;	//绿通道
		for(j=0;j<nHei;j++)
		{
			nPos=j*nLineBytes;
			for(i=0;i<nWid;i++)
			{
				int nV = pBuf[nPos];
				pHistData[nV]++;
				nPos+=3;
			}
		}
#endif
		nPos=0;
		for(j=0;j<nHei;j++)
		{
			nPos=j*nLineBytes;
			for(i=0;i<nWid;i++)
			{
				int nB=pBuf[nPos];
				int nG=pBuf[nPos+1];
				int nR=pBuf[nPos+2];
				int nV = (nR*19595 + nG*38469 + nB*7472) >> 16;
				pHistData[nV]++;
				nPos+=3;
			}
		}
	}
		
	
版本：
	2.0.0.8
		
2019.9.1
	(1)用中心点和四周8个点聚焦；
	其它点用插值方法计算Z，不再单独每个点做Z的自动聚焦。
	(2)
	存图的时候，多等待1幅图像。
	case STATUS_XY_WAIT_1_IMG:
		g_nStatus = STATUS_XY_WAIT_1_IMG_ADD;
		//g_nStatus = STATUS_SAVE_1_IMG;
		break;
	case STATUS_XY_WAIT_1_IMG_ADD:
		g_nStatus = STATUS_SAVE_1_IMG;
		break;
	case STATUS_SAVE_1_IMG:
		
版本：
	2.0.0.9

2019.9.2
	（1）矩形扫描的时候，对齐边界：
	DWORD WINAPI doAuto_NoZ_Focus(LPVOID lpParam)
	{
			if(nDirX==0)	//改变X运动方向
			{
				err = m_stage.MoveLeft( pDlg->m_nX_step_um );	//多走一步,对齐边界
				nDirX=1;
			}
			else
			{
				err = m_stage.MoveRight( pDlg->m_nX_step_um );//多走一步,对齐边界
				nDirX=0;
			}	
	}
	（2）限制四周聚焦点的范围：
	void CMicroScopeDlg::CalFocusBorderPt(double* pH,double* pV)
	
版本：
	2.0.1.0	
	
2019.10.5
	改变调光的显示：
	1，只显示部分曲线，并X方向显示步长不是1，是X_FACTOR
		#define X_FACTOR	4
		#define X_START		150
		#define X_END		220
	2，显示背景亮度值
	3，曲线的中间部分改为绿色
版本：
	2.0.1.1
	
