// AdjBrightnessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MicroScope.h"
#include "AdjBrightnessDlg.h"
#include "MicroScopeDlg.h"


#define X_FACTOR	4
#define X_START		150
#define X_END		220
extern TCHAR g_lpINIPath[MAX_PATH];
// AdjBrightnessDlg dialog

IMPLEMENT_DYNAMIC(AdjBrightnessDlg, CDialog)

AdjBrightnessDlg::AdjBrightnessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(AdjBrightnessDlg::IDD, pParent)
	, m_nBKBrightness(0)
{
	m_pParent = pParent;
	m_bInited = FALSE;
	m_pHist=new int[256];
	bFirstDraw = TRUE;
	for(int i=0;i<256;i++)
		m_nLastY[i]=0;
}

AdjBrightnessDlg::~AdjBrightnessDlg()
{
	delete m_pHist;
}

void AdjBrightnessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_BK_B, m_nBKBrightness);
}


BEGIN_MESSAGE_MAP(AdjBrightnessDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_DRAW, &AdjBrightnessDlg::OnBnClickedBtnDraw)
	ON_BN_CLICKED(IDC_BTN_CLEAR, &AdjBrightnessDlg::OnBnClickedBtnClear)
END_MESSAGE_MAP()


// AdjBrightnessDlg message handlers

BOOL AdjBrightnessDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here

	m_nBrightness=GetPrivateProfileInt(_T("AdjBrightness"),_T("nBrightness"),200,g_lpINIPath);
	m_nDelta=GetPrivateProfileInt(_T("AdjBrightness"),_T("nDelta"),10,g_lpINIPath);
	m_nEnlarge=GetPrivateProfileInt(_T("AdjBrightness"),_T("nEnarge"),4,g_lpINIPath);
	SetTimer(2,250,NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void AdjBrightnessDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CDialog::OnPaint() for painting messages

	CMicroScopeDlg* pWnd = (CMicroScopeDlg*)m_pParent;
	pWnd->m_bAdjBrigntnessOn=TRUE;
	
	//DrawLine();
	memcpy(m_pHist,pWnd->m_pHist,256*sizeof(int));
	GetPeakV();
	CalXY();
	DrawCurve();
	DrawLine();
	bFirstDraw=FALSE;
	UpdateData(FALSE);
}

void AdjBrightnessDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: Add your message handler code here
	KillTimer(2);
}

void AdjBrightnessDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	CMicroScopeDlg* pWnd = (CMicroScopeDlg*)m_pParent;

	//DrawLine();

	//if(!bFirstDraw)
	{
		//ClearCurve();
		//DrawCurve();
		CRect rec;
		rec.left=m_nLastX[0];
		rec.right=m_nLastX[255];
		rec.top=0;
		rec.bottom=m_nLastY[0];
		InvalidateRect(&rec);
	}
/*
	memcpy(m_pHist,pWnd->m_pHist,256*sizeof(int));
	CalXY();
	DrawCurve();
	DrawLine();
	bFirstDraw=FALSE;
*/
	CDialog::OnTimer(nIDEvent);
}


void AdjBrightnessDlg::ClearCurve()
{
	CDC* pDC=GetDC();
	short old=old=pDC->SetROP2(R2_NOTXORPEN);

	CRect rec;
	GetClientRect(&rec);

	CPen pen(PS_SOLID,1,RGB(0,0,0));
	CPen* oldpen = pDC->SelectObject(&pen);

	pDC->MoveTo(m_nLastX[0],m_nLastY[0]);
	for(int i=1;i<256;i++)
	{
		pDC->LineTo(m_nLastX[i],m_nLastY[i]);
	}

	pDC->SelectObject(oldpen);
	pDC->SetROP2(old);
	ReleaseDC(pDC);
}
void AdjBrightnessDlg::DrawCurve()
{
	CDC* pDC=GetDC();
	//short old=old=pDC->SetROP2(R2_XORPEN);

	CRect rec;
	GetClientRect(&rec);

	CPen pen(PS_SOLID,2,RGB(255,0,0));
	CPen pen1(PS_SOLID,2,RGB(0,255,0));
	CPen* oldpen = pDC->SelectObject(&pen);
	//CBrush brush(RGB(0,0,0));
	//CBrush *oldbrush=pDC->SelectObject(&brush);

	//pDC->MoveTo(m_nLastX[0],m_nLastY[0]);
	pDC->MoveTo(0,m_nLastY[X_START]);
	for(int i=(X_START+1);i<X_END;i++)
	{
		if((i>(m_nBrightness-m_nDelta)) && (i<=(m_nBrightness+m_nDelta)))
			pDC->SelectObject(&pen1);
		else
			pDC->SelectObject(&pen);

		pDC->LineTo((i-X_START)*X_FACTOR,m_nLastY[i]);
	}

	pDC->SelectObject(oldpen);
	//pDC->SetROP2(old);
	//if(oldbrush)
	//	pDC->SelectObject(oldbrush);

	ReleaseDC(pDC);
}

#if 0
void AdjBrightnessDlg::ClearCurve()
{
	CDC* pDC=GetDC();
	short old=old=pDC->SetROP2(R2_NOTXORPEN);

	CRect rec;
	GetClientRect(&rec);

	CPen pen(PS_SOLID,1,RGB(0,0,0));
	CPen* oldpen = pDC->SelectObject(&pen);

	int x,y,y0;
	x=0;
	y0=rec.Size().cy -10;

	pDC->MoveTo(x,y0);
	for(int i=0;i<256;i++)
	{
		pDC->LineTo(x+i,m_nLastY[i]);
	}

	pDC->SelectObject(oldpen);
	pDC->SetROP2(old);
	ReleaseDC(pDC);
}

void AdjBrightnessDlg::DrawCurve()
{
	CDC* pDC=GetDC();
	CMicroScopeDlg* pWnd = (CMicroScopeDlg*)m_pParent;
	
	int nMaxCnt=pWnd->m_nImgWid*pWnd->m_nImgHei;
	if(nMaxCnt<=0)
		return;

	int x,y,y0;
	CRect rec;
	GetClientRect(&rec);
	
	int nMaxHei = rec.Size().cy - 20;

	CPen pen(PS_SOLID,1,RGB(0,0,0));
	CPen* oldpen = pDC->SelectObject(&pen);

	x=0;
	y0=rec.Size().cy -10;

	pDC->MoveTo(x,y0);
	for(int i=0;i<256;i++)
	{
		int v = nMaxHei*m_pHist[i]/nMaxCnt;
		y=y0 - m_nEnlarge*v;		//放大
		m_nLastY[i]=y;
		pDC->LineTo(x+i,y);
	}
	pDC->SelectObject(oldpen);

	ReleaseDC(pDC);
}
#endif

void AdjBrightnessDlg::DrawLine()
{
	int x,y0,y;
	CRect rec;
	GetClientRect(&rec);
	CDC* pDC=GetDC();

	CPen pen(PS_SOLID,1,RGB(0,0,0));
	CPen* oldpen = pDC->SelectObject(&pen);

	x=0;
	//y0=rec.Size().cy -10;
	y0=rec.Size().cy -9;		//比线低一个像素
	pDC->MoveTo(x,y0);
	//pDC->LineTo(255,y0);
	pDC->LineTo((X_END-X_START)*X_FACTOR,y0);

	//x=m_nBrightness-m_nDelta;
	x=(m_nBrightness-m_nDelta-X_START)*X_FACTOR;
	y=y0-rec.Size().cy+20;
	pDC->MoveTo(x,y0);
	pDC->LineTo(x,y);

	//x=m_nBrightness+m_nDelta;
	x=(m_nBrightness+m_nDelta-X_START)*X_FACTOR;
	pDC->MoveTo(x,y0);
	pDC->LineTo(x,y);

	pDC->SelectObject(oldpen);
	ReleaseDC(pDC);
}

void AdjBrightnessDlg::OnBnClickedBtnDraw()
{
	CMicroScopeDlg* pWnd = (CMicroScopeDlg*)m_pParent;
	memcpy(m_pHist,pWnd->m_pHist,256*sizeof(int));
	DrawCurve();
}

void AdjBrightnessDlg::OnBnClickedBtnClear()
{
	//ClearCurve();
}

void AdjBrightnessDlg::CalXY()
{
	CMicroScopeDlg* pWnd = (CMicroScopeDlg*)m_pParent;
	
	int nMaxCnt=pWnd->m_nImgWid*pWnd->m_nImgHei;
	if(nMaxCnt<=0)
		return;

	int y,y0;
	CRect rec;
	GetClientRect(&rec);
	
	int nMaxHei = rec.Size().cy - 20;

	y0=rec.Size().cy -10;
	for(int i=0;i<256;i++)
	{
		m_nLastX[i]=i*X_FACTOR;
		int v = nMaxHei*m_pHist[i]/nMaxCnt;
		y=y0 - m_nEnlarge*v;		//放大
		m_nLastY[i]=y;
	}
}
int AdjBrightnessDlg::GetPeakV()
{
	int i;
	int nMaxV=0;
	int nMaxHist=0;
	for(i=0;i<256;i++)
	{
		if(nMaxHist<m_pHist[i])
		{
			nMaxHist=m_pHist[i];
			nMaxV=i;
		}
	}
	m_nBKBrightness=nMaxV;
	return m_nBKBrightness;
}

