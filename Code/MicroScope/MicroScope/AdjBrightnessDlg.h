#pragma once


// AdjBrightnessDlg dialog

class AdjBrightnessDlg : public CDialog
{
	DECLARE_DYNAMIC(AdjBrightnessDlg)

public:
	AdjBrightnessDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~AdjBrightnessDlg();
private:
	CWnd* m_pParent;
	BOOL m_bInited;

// Dialog Data
	enum { IDD = IDD_DLG_ADJ_LIGHT };

public:
	int* m_pHist;
	void DrawLine();
	void DrawCurve();
	void ClearCurve();
	BOOL bFirstDraw;
	int m_nBrightness;
	int m_nDelta;
	int m_nEnlarge;
	int m_nLastX[256];
	int m_nLastY[256];
	void CalXY();
	int GetPeakV();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedBtnDraw();
	afx_msg void OnBnClickedBtnClear();
	int m_nBKBrightness;
};
