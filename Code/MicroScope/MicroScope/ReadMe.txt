2018.6.8
1��EasySize.h
2��
include:
	$(PYLON_DEV_DIR)\include
lib:
	$(PYLON_DEV_DIR)\lib\Win32
3, ����ѡ�
	sprintf  ->  sprintf_s
	fopen    ->  fopen_s

	�һ����� - ���� - �������� - C/C++  - ������
		/D _CRT_SECURE_NO_WARNINGS
4,
	UNICODE  ->  Use Multi-Byte Character Set		

2018.6.16
	��1�������ʼ��ʱ��Ҫ��
	m_pCamera->ExposureAuto.SetValue(ExposureAuto_Off);
	������Զ��ع�״̬���������溯������
	m_pCamera->ExposureTime.SetValue(dExposureTime);
	��2��
	���һֱ�����Զ��ع�״̬��ͼ��û�з�Ӧ��
	AutoExposureContinuous(m_pCamera);
	
	=��
	��1��
		����ʱ����һ�Σ�������ǰ��ֵ����
		������1�Ρ�
	��2����ʱ����
	
2018.6.17
	��ʼ��ʱ���ȡ��ǰ���ع�ֵ��
	�˳�ʱ�򱣴浱ǰ�ع�ֵ��

***�޸ģ�
	�ڲɼ�ͼ��Ļص������в���ʾͼ�񣬵�invaliddate()ȥ��ʾ�����Խ��������Ҫ�������⡣
2018.7.14
	(1)ˮƽ����ÿ������ɲ����پ۽�
	(2)
		//����ĺ�����ʶ������·��
		//Pylon::CImagePersistence::Save(ImageFileFormat_Jpeg, lpFName, ptr);
		��SaveImageFile()����
2018.7.21
	��1�������ж���Ұ����û��ϸ����ͨ��ͼ����������
	��2����һ����Ҫ�Զ��۽��ķ�Χ��������һЩ��
��֪���⣺
	����Բ��ϸ����Ƭ�����ܲ��þ۽���
    ���վ���ɨ�裬��ʼλ�ö�λ�����Ͻǲ����˹��۽�����ɺ��治�׾۽���
    ����׼����Ϊ��λ��Բ�ģ�Ȼ����Բ��ɨ�衣
2018.7.22
	���ж�ͼ�����Ƿ���ϸ����Ȼ����ȥ�۽����ַ������ɼ��ٶ�����
=��
	��Ϊ��
	���������ȵ�ʱ�����ݽ����������ж�ͼ�����Ƿ���ϸ����
	
2018.7.25
	#define DON_NOT_WAIT_ADD_IMG	//���ٶ�ȴ�1��ͼ��
	
2018.9.6
	׼�������ͼ���ȣ���ɫ���ǣ�
	IDC_ACTIVE_IMG
	IDC_PROGRESS
2019.9.8
	�����ͼ���̵�״̬
2018.9.11
	fix a bug:
	ֹͣ�ͼ����˳�����ʱ���ٵ��ã�
	m_dExposureTime=m_pCamera->ExposureTime.GetValue();
	�ᱨ��
		
��֪��Ϊʲô�����ҵĿ��������ϣ�Basler������׳��֣�
	Err Img=19
	Trigger1Img err.

�汾��1.0.0.3

2018.9.12
	MFCʹ�ö�̬�⣬����ʹ�þ�̬�⡣
=>	ʹ��OpenCV����Ҫ��
	��1��shared dll��������static ����
	��2��code generation�е�runtime library��Ҫ multi-thread dllѡ�  MD����MDdѡ��
	�������
	1>c:\program files (x86)\microsoft visual studio 9.0\vc\atlmfc\include\afxtempl.h(1259) : error C3859: virtual memory range for PCH exceeded; please recompile with a command line option of '-Zm111' or greater
	1>c:\program files (x86)\microsoft visual studio 9.0\vc\atlmfc\include\afxtempl.h(1259) : fatal error C1076: compiler limit : internal heap limit reached; use /Zm to specify a higher limit
	1>Build log was saved at "file://f:\work.now\MicroScope\MicroScope\Debug\BuildLog.htm"

=��
	����취�ܼ򵥣������ϵ���Ҽ� -> ����(Properties) -> C/C++ -> ������(Command Line)�� �ڶ���ѡ��(Additional options)�а�������ʾ����-Zm111 ����������֣��ر��뼴�ɡ�

�汾��1.0.0.4

2018.9.13
	opencv��Ҫ�ģ��ڹ��̵����������ã���
	��1��include:
		.\opencv\build\include\opencv2
		.\opencv\build\include\opencv
		.\opencv\build\include

/*	
		D:\���\opencv\build\include\opencv2
		D:\���\opencv\build\include\opencv
		D:\���\opencv\build\include
		$(PYLON_DEV_DIR)\include
*/

	��2��lib��dll:
		·����	.\opencv\build\x86\vc9\lib
//		·����	D:\���\opencv\build\x86\vc9\lib
		release lib��
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
		debug lib��			
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

�����⡿
	ʹ�� #define  __CAL_CELL__	���ڴ�й©��
	
2018.9.14
	�ο�Basler��demo���ӣ�Gui_Sample���޸Ĳɼ�
	
2018.9.15
	��1���ο�Basler��demo���ӣ�Gui_Sample���޸Ĳɼ�,����lock
	��2�����Է��֣���ʼ�ɼ���ʱ�򣬴�AutoWhiteBalance()�������� =��ʹ�����ò�������
	��3����ʾ��Ϣ����ʱ���޸ģ�
		CWnd* pWnd=GetDlgItem(IDC_EDIT_DUOBEITI);
		pWnd->SetWindowText((LPCTSTR)lpInfo);
	=>	
		SetDlgItemText(IDC_EDIT_DUOBEITI,lpInfo);
		UpdateWindow();	
	��4�����VS2008���뱨��
		mutable CCriticalSection m_MemberLock;
	=>	�ֶ���ӣ�afxmt.h
�汾��1.0.0.5
	
2018.9.22
	(1)��ʾ��ÿ���һ֡����ʾһ�Σ�
	(2)BOOL m_bBusy;	//��æ��ʱ�򣬲�����Ϣ
	void CMicroScopeDlg::OnImageGrabbed(Pylon::CInstantCamera& camera, const Pylon::CGrabResultPtr& grabResult)
	{
		if(!m_bBusy)
			PostMessage(ID_NEW_GRABRESULT, 0, 0);
		else
		{
			OutputDebugString("-*-*-Busy.\n");
		}	
	}
	(3)����cell��ȫ�ֱ�������new���䡢�ͷţ���Ȼ�����ڴ�й©��
	
�汾��1.0.0.6
	
2018.9.23
	���Ż�,�ڻص���copy������onpaint��copy��
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
				������
			}
	}	
	
�汾��1.0.0.7

2018.9.24
	�Ż���
	�Ի�����ʼ����ʱ�򣬾ʹ��豸��
�汾��1.0.0.8
	
2018.9.25
	��1��todo��
		��ѯbasler�ļ���֧�֣�˵���ûص���ʽ�����̷߳�ʽ�ɼ����������̴߳���
	��2����һ���۽���
		pst.ini��
		first_focus_steps=7
	��3��ʹ���ڴ���������������ȥ����
		CSingleLock lock(&m_MemberLock, TRUE);
		lock.Unlock();
		
		�����ǡ�
		�����ڴ����Ļ����洢��ͼ���а��ͼ��������
		��CCriticalSection m_Lock;����mutable CCriticalSection m_MemberLock;
		

�򵥲��ԣ�	OK
todo��
(1)		�ɼ����̣��ε�USB�ӿ�
(2)		ֱ���ڻص��д�ͼ������ƽ̨����Ϣ����������Ϣ�д���
		
�汾��1.0.0.9

2018.9.29
    (1)�ɼ���ɣ���������
	(2)�������ĵ���ϸ�۽�������ʵ��Ч�����á�
=��	��Ϊ�����û�������
�汾��1.0.1.0
	
2018.9.30
	��������ʱ���������Զ����ȣ��������ý��������á�
�汾��1.0.1.1
	
��todo��
��ͼ���̡��β�USB����������²ɼ��������쳣��

	
2018.10.5
	��1��ȥ����
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
	�������洢����cell_result.ini
	
�汾��1.0.1.2	
	
2018.10.18
	���Ż�
�汾��1.0.2.0
	���Է��֣�
	ÿ���1����ϸ�۽���Ȼ���С�����۽���
	��1����ϸ�۽�ʱ������3�Ƚ�OK�����ĵ�����3����
	��2�����Ծ۽�������2�Ƚ�OK�����ĵ�����2����������1���С�

2018.10.24
	�߳���صĺ������ٷ������У���Ϊȫ�ֺ�����
�汾��1.0.2.2

2018.10.29
	���²�����

=======================	
2018.11.24
	��1��ʹ��ImageSource�������ʹ��Basler�����	
	��2��ANSI��UNICODE�ַ���������
		https://blog.csdn.net/chenzy945/article/details/51931148
	��3��Ϊ�˱����ͼ��֧������Ŀ¼��ʹ��UNICODE
	
��ֵ����⣺
	BOOL CMicroScopeApp::InitInstance()
	{
		if( !DShowLib::InitLibrary() )
		{
			AfxMessageBox( TEXT("The IC Imaging Control Class Library could not be initialized.\n(invalid license key?)") );
			exit( 1 );
		}
		//atexit(DShowLib::ExitLibrary );	//���ڴ˴������m_cz��OLE�쳣

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
	
�汾��2.0.0.1


2018.11.28
	��1�������������
	��2�������˳�ʱ,���룺��������򱨴�
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
	��Ƥϸ������m_nShangpi_Num_Limitʱ��ֹͣ�Զ��ɼ���
	
�汾��2.0.0.2

2018.12.23
	(1)�������ֻ����һ�εĴ���
	(2)�޸����ʼ���
	(3)ɨ����ɺ��Զ�����
	
�汾��2.0.0.3

2019.1.10
	���ӣ�
	(1)��Ƥϸ����ĿС��8000ʱ���Զ���������ɨ�衣
	double m_dMaxRadius_mm;		//�����Բ��ɨ�裬Բ�뾶

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
	(5)�洢ͼ��·��
	LRESULT CMicroScopeDlg::OnChangeTtnName(WPARAM wparam, LPARAM lparam)
	{
		m_cBtnAuto.SetWindowText(TEXT("�Զ�ɨƬ"));
		if(wparam==1)
		{
			OnBnClickedBtnStatistic();
		}
		TCHAR lpINI[MAX_PATH];
		wsprintf(lpINI,_T("%s%s"),g_lpBinPath,_T("ImgDir.ini"));
*		WritePrivateProfileString(_T("LastDir"),_T("img_dir"),m_lpDir,lpINI);
		return 1;
	}
	
�汾��	
	2.0.0.4
	
2019.1.11
	��	number.ini��ȡBL�ţ��ļ��洢����Ŀ¼�¡�

	rdBL_No();
	if(_tcscpy(m_lpBL_No,_T("11223344"))==0)
		MakeImgPath(m_lpDir);
	else
		MakeImgPath_BL_No(m_lpDir,m_lpBL_No);	
		
2019.1.14
	//�ܰ�
	_stprintf(pDlg->m_lpResult_LinBa, _T("%d"), g_nCountLymphocyte);
=��
	_stprintf(pDlg->m_lpResult_LinBa, _T("%d"), g_nCountLymphocyte*0.04);

	//����
	_stprintf(pDlg->m_lpResult_Impurity, _T("%d"), (int)(g_nCountImpurity*0.1));
=>
	_stprintf(pDlg->m_lpResult_Impurity, _T("%d"), (int)(g_nCountImpurity*0.1*25));

2019.1.15
(1)	������ϸ������������ԭ���Ļ����ϳ�0.4
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
	���ɨ�������Ĭ����5.5�����Դ������ļ���ȡ��

��1��	
//#define MAX_RADIUS	5.5		//���Բ�뾶6.5mm
double MAX_RADIUS=5.5;
��2��
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
	
�汾��
	2.0.0.6
	
2019.3.3 ������ɨƬ���Զ����㹦�� 
if(g_nCountCells>=pDlg->m_nShangpi_Num_Limit)
		pDlg->PostMessage(WM_AUTO_CAL,0,0);
		
2019.3.23
	�������ȡ�ֱ��ͼ��ʾ�����ڵ���ͼ�����ȡ�
	
�汾��
	2.0.0.7
		
2019.6.24
	��ɫͼ��ͳ������ֱ��ͼ���ûҶȣ�������ɫͨ����
	
	else if(nBits==24)	//Gray = (R*299 + G*587 + B*114 + 500) / 1000	//Gray = (R*19595 + G*38469 + B*7472) >> 16
	{
#if 0
		nPos=1;	//��ͨ��
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
		
	
�汾��
	2.0.0.8
		
2019.9.1
	(1)�����ĵ������8����۽���
	�������ò�ֵ��������Z�����ٵ���ÿ������Z���Զ��۽���
	(2)
	��ͼ��ʱ�򣬶�ȴ�1��ͼ��
	case STATUS_XY_WAIT_1_IMG:
		g_nStatus = STATUS_XY_WAIT_1_IMG_ADD;
		//g_nStatus = STATUS_SAVE_1_IMG;
		break;
	case STATUS_XY_WAIT_1_IMG_ADD:
		g_nStatus = STATUS_SAVE_1_IMG;
		break;
	case STATUS_SAVE_1_IMG:
		
�汾��
	2.0.0.9

2019.9.2
	��1������ɨ���ʱ�򣬶���߽磺
	DWORD WINAPI doAuto_NoZ_Focus(LPVOID lpParam)
	{
			if(nDirX==0)	//�ı�X�˶�����
			{
				err = m_stage.MoveLeft( pDlg->m_nX_step_um );	//����һ��,����߽�
				nDirX=1;
			}
			else
			{
				err = m_stage.MoveRight( pDlg->m_nX_step_um );//����һ��,����߽�
				nDirX=0;
			}	
	}
	��2���������ܾ۽���ķ�Χ��
	void CMicroScopeDlg::CalFocusBorderPt(double* pH,double* pV)
	
�汾��
	2.0.1.0	
	
2019.10.5
	�ı�������ʾ��
	1��ֻ��ʾ�������ߣ���X������ʾ��������1����X_FACTOR
		#define X_FACTOR	4
		#define X_START		150
		#define X_END		220
	2����ʾ��������ֵ
	3�����ߵ��м䲿�ָ�Ϊ��ɫ
�汾��
	2.0.1.1
	
