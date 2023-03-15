// dlgSetting.cpp : implementation file
//

#include "stdafx.h"
#include "MicroScope.h"
#include "dlgSetting.h"
#include "MicroScopeDlg.h"

// dlgSetting dialog

extern CScan m_scan;
extern CStage m_stage;
extern CZ m_cz;


IMPLEMENT_DYNAMIC(dlgSetting, CDialog)

dlgSetting::dlgSetting(CWnd* pParent /*=NULL*/)
	: CDialog(dlgSetting::IDD, pParent)
	, m_nXStep(0)
	, m_nYStep(0)
	, m_strRadius(_T(""))
	, m_bAutoCal(FALSE)
{
	m_pParent = pParent;
	m_bInited = FALSE;
}

dlgSetting::~dlgSetting()
{
}

void dlgSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_X_STEP, m_nXStep);
	DDX_Text(pDX, IDC_EDIT_Y_STEP, m_nYStep);
	DDX_Text(pDX, IDC_EDIT_RADIUS, m_strRadius);
	DDX_Check(pDX, IDC_CHK_AUTO_CAL, m_bAutoCal);
}


BEGIN_MESSAGE_MAP(dlgSetting, CDialog)
	ON_BN_CLICKED(IDC_BTN_LEFT, &dlgSetting::OnBnClickedBtnLeft)
	ON_BN_CLICKED(IDC_BTN_RIGHT, &dlgSetting::OnBnClickedBtnRight)
	ON_BN_CLICKED(IDC_BTN_FORWARD, &dlgSetting::OnBnClickedBtnForward)
	ON_BN_CLICKED(IDC_BTN_BACK, &dlgSetting::OnBnClickedBtnBack)
	ON_BN_CLICKED(IDC_BUTTON_UP, &dlgSetting::OnBnClickedButtonUp)
	ON_BN_CLICKED(IDC_BUTTON_DOWN, &dlgSetting::OnBnClickedButtonDown)
END_MESSAGE_MAP()


// dlgSetting message handlers

void dlgSetting::OnBnClickedBtnLeft()
{
	CMicroScopeDlg* pWnd = (CMicroScopeDlg*)m_pParent;
	//pWnd->m_stage.MoveLeft ( pWnd->m_nX_step_um );
	m_stage.MoveLeft ( pWnd->m_nX_step_um );
}

void dlgSetting::OnBnClickedBtnRight()
{
	CMicroScopeDlg* pWnd = (CMicroScopeDlg*)m_pParent;
	//pWnd->m_stage.MoveRight ( pWnd->m_nX_step_um );
	m_stage.MoveRight ( pWnd->m_nX_step_um );
}

void dlgSetting::OnBnClickedBtnForward()
{
	CMicroScopeDlg* pWnd = (CMicroScopeDlg*)m_pParent;
	//pWnd->m_stage.MoveForward ( pWnd->m_nY_step_um );
	m_stage.MoveForward ( pWnd->m_nY_step_um );
}

void dlgSetting::OnBnClickedBtnBack()
{
	CMicroScopeDlg* pWnd = (CMicroScopeDlg*)m_pParent;
	//pWnd->m_stage.MoveBack ( pWnd->m_nY_step_um );
	m_stage.MoveBack ( pWnd->m_nY_step_um );
}

void dlgSetting::OnBnClickedButtonUp()
{
	CMicroScopeDlg* pWnd = (CMicroScopeDlg*)m_pParent;
	//pWnd->m_cz.MoveUp(pWnd->m_zFineFocus_Step_um);
	m_cz.MoveUp(pWnd->m_zFineFocus_Step_um);
}

void dlgSetting::OnBnClickedButtonDown()
{
	CMicroScopeDlg* pWnd = (CMicroScopeDlg*)m_pParent;
	//pWnd->m_cz.MoveDown(pWnd->m_zFineFocus_Step_um);
	m_cz.MoveDown(pWnd->m_zFineFocus_Step_um);
}

BOOL dlgSetting::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CMicroScopeDlg* pMicroScopeDlg = (CMicroScopeDlg*)m_pParent;
	CWnd* pWnd=NULL;

	if(!pMicroScopeDlg->m_bStageEn)
	{
		pWnd=GetDlgItem(IDC_BTN_LEFT);
		pWnd->EnableWindow(FALSE);

		pWnd=GetDlgItem(IDC_BTN_RIGHT);
		pWnd->EnableWindow(FALSE);

		pWnd=GetDlgItem(IDC_BTN_UP);
		pWnd->EnableWindow(FALSE);

		pWnd=GetDlgItem(IDC_BTN_DOWN);
		pWnd->EnableWindow(FALSE);

		pWnd=GetDlgItem(IDC_BUTTON_UP);
		pWnd->EnableWindow(FALSE);

		pWnd=GetDlgItem(IDC_BUTTON_DOWN);
		pWnd->EnableWindow(FALSE);

	}
	m_bInited = TRUE;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
