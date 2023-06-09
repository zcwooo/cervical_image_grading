#ifndef __CONTROLEX_H__
#define __CONTROLEX_H__
#include "Global.h"

class CCircleProgressUI : public CProgressUI
{
	DECLARE_DUICONTROL(CCircleProgressUI)
public:
	CCircleProgressUI()
	{
	}

	LPCTSTR GetClass() const
	{
		return _T("CircleProgress" );
	}

	void PaintBkColor(HDC hDC)
	{
		Gdiplus::Graphics g(hDC);
		RECT rcPos = GetPos();
		Gdiplus::SolidBrush brush(m_dwBackColor);
		g.FillPie(&brush, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, 0, 360);
		g.ReleaseHDC(hDC);
	}

	void PaintForeColor(HDC hDC)
	{
		Gdiplus::Graphics g(hDC);
		RECT rcPos = GetPos();
		Gdiplus::SolidBrush brush(m_dwForeColor);
		int nStartDegree = 90;
		int nSweepDegree = (int)(360.0f * (m_nValue * 1.0f / (m_nMax - m_nMin)));
		g.FillPie(&brush, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, nStartDegree, nSweepDegree);

		Gdiplus::SolidBrush brushbk(0xFFFFFFFF);
		g.FillPie(&brushbk, rcPos.left + 10, rcPos.top + 10, rcPos.right - rcPos.left - 20, rcPos.bottom - rcPos.top - 20, 0, 360);

		g.ReleaseHDC(hDC);
	}

	void PaintForeImage(HDC hDC)
	{
		
	}
};


class CMyComboUI : public CComboUI
{
	DECLARE_DUICONTROL(CMyComboUI)
public:
	CMyComboUI()
	{
	}

	LPCTSTR GetClass() const
	{
		return _T("MyCombo" );
	}

	void PaintBkColor(HDC hDC)
	{
		Gdiplus::Graphics g(hDC);
		RECT rcPos = GetPos();
		Gdiplus::SolidBrush brush(m_dwBackColor);
		g.FillPie(&brush, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, 0, 360);
		g.ReleaseHDC(hDC);
	}
};

#define CHARTVIEW_PIE		0x0
#define CHARTVIEW_HISTOGRAM 0x01

typedef struct tagCHARTITEM
{
	CDuiString name;
	double value;
} CHARTITEM, *PCHARTITEM;

class CChartViewUI : 
	public CControlUI
{
public:
	DECLARE_DUICONTROL(CChartViewUI)
public:
	CChartViewUI(void);
	~CChartViewUI(void);

	bool Add(LPCTSTR name, double value);
	bool AddAt(LPCTSTR name, double value, int iIndex);

public:
	virtual void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void DoPaintPie(HDC hDC, const RECT& rcPaint);
	virtual void DoPaintHistogram(HDC hDC, const RECT& rcPaint);
private:
	vector<CHARTITEM> m_items;
	int m_ViewStyle;
	CDuiString m_sShadowImage;
	int m_ShadowImageHeight;
	CDuiString m_sPillarImage;
	int m_PillarImageWidth;
	DWORD m_dwTextColor;
	DWORD m_dwDisabledTextColor;
	bool m_bShowHtml;
	bool m_bShowText;
	int m_iFont;
};

class CCircleProgressExUI : public CProgressUI
{
	DECLARE_DUICONTROL(CCircleProgressExUI)
public:
	CCircleProgressExUI()
	{
	}

	LPCTSTR GetClass() const
	{
		return _T("CircleProgressEx");
	}

	void PaintBkColor(HDC hDC)
	{
		Gdiplus::Graphics g(hDC);
		RECT rcPos = GetPos();
		Gdiplus::SolidBrush brush(m_dwBackColor);
		g.FillPie(&brush, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, 0, 360);
		g.ReleaseHDC(hDC);
	}

	void PaintForeColor(HDC hDC)
	{
		Gdiplus::Graphics g(hDC);
		g.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeAntiAlias);

		RECT rcPos = GetPos();
		Gdiplus::SolidBrush brush(m_dwForeColor);
		int nStartDegree = 90;
		int nSweepDegree = (int)(360.0f * (m_nValue * 1.0f / (m_nMax - m_nMin)));
		g.FillPie(&brush, rcPos.left, rcPos.top, rcPos.right - rcPos.left, rcPos.bottom - rcPos.top, nStartDegree, nSweepDegree);

		Gdiplus::SolidBrush brushbk(0xFFFFFFFF);
		g.FillPie(&brushbk, rcPos.left + 10, rcPos.top + 10, rcPos.right - rcPos.left - 20, rcPos.bottom - rcPos.top - 20, 0, 360);

		g.ReleaseHDC(hDC);
	}

	void PaintForeImage(HDC hDC)
	{
		
	}
};

class CWndUI : public CControlUI
{
	DECLARE_DUICONTROL(CWndUI)
public:
	CWndUI()
	{

	}

	LPCTSTR GetClass() const
	{
		return _T("Wnd");
	}
public:
	void Attach(HWND hWnd)
	{
		m_hWnd = hWnd;
		AdjustPos();
	}

	HWND Detach()
	{
		HWND hWnd = m_hWnd;
		m_hWnd = NULL;
		return hWnd;
	}

	virtual void SetVisible(bool bVisible = true)
	{
		CControlUI::SetVisible(bVisible);
		AdjustPos();
	}

	virtual void SetInternVisible(bool bVisible = true)
	{
		CControlUI::SetInternVisible(bVisible);
		AdjustPos();
	}

	virtual void SetPos(RECT rc, bool bNeedInvalidate /* = true */)
	{
		CControlUI::SetPos(rc, bNeedInvalidate);
		AdjustPos();
	}

	void AdjustPos()
	{
		if(::IsWindow(m_hWnd)) {
			if(m_pManager) {
				RECT rcItem = m_rcItem;
				if(!::IsChild(m_pManager->GetPaintWindow(), m_hWnd)) {
					RECT rcWnd = {0};
					::GetWindowRect(m_pManager->GetPaintWindow(), &rcWnd);
					::OffsetRect(&rcItem, rcWnd.left, rcWnd.top);
				}
				SetWindowPos(m_hWnd, NULL, rcItem.left, rcItem.top, rcItem.right - rcItem.left, rcItem.bottom - rcItem.top, SWP_NOACTIVATE | SWP_NOZORDER);
			}
			ShowWindow(m_hWnd, IsVisible() ? SW_SHOW : SW_HIDE);
		}
	}

protected:
	HWND m_hWnd;
};

class CHistogramUI : public CControlUI
{
	DECLARE_DUICONTROL(CHistogram)
public:
	CHistogramUI()
	{
	}

	LPCTSTR GetClass() const
	{
		return _T("CHistogramUI");
	}

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
};

class CDiagramUI : public CControlUI
{
	DECLARE_DUICONTROL(CDiagramUI)
public:
	CDiagramUI()
	{
	}

	LPCTSTR GetClass() const
	{
		return _T("CDiagramUI");
	}

	virtual bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
};

class CLoginWnd;
class CEditUIEx : public CEditUI
{
	DECLARE_DUICONTROL(CEditUIEx)
public:
	CEditUIEx()
	{
	}

	LPCTSTR GetClass() const
	{
		return _T("CEditUIEx");
	}

	void DoEvent(TEventUI& event);

	void SetParentEx(CLoginWnd * pParent)
	{
		m_pParentEx = pParent;
	}

	CLoginWnd* m_pParentEx;
};

class CMainDlgWnd;
class CEditUIEx1 : public CEditUI //skip
{
	DECLARE_DUICONTROL(CEditUIEx1)
public:
	CEditUIEx1()
	{
	}

	LPCTSTR GetClass() const
	{
		return _T("CEditUIEx1");
	}

	///void DoEvent(TEventUI& event);

	void SetParentEx(CMainDlgWnd * pParent)
	{
		m_pParentEx = pParent;
	}

	CMainDlgWnd* m_pParentEx;
};

class CMenuWndEx : public CMenuWnd
{
	DECLARE_DUICONTROL(CMenuWndEx)
public:
	CMenuWndEx()
	{
	}

	LPCTSTR GetClass() const
	{
		return _T("CMenuWndEx");
	}

	void Notify(TNotifyUI& msg);
};

class CVerticalLayoutUIEX : public CVerticalLayoutUI
{
	DECLARE_DUICONTROL(CVerticalLayoutUIEX)
public:
	CVerticalLayoutUIEX()
	{
	}

	LPCTSTR GetClass() const
	{
		return _T("CVerticalLayoutUIEX");
	}

	void DoEvent(TEventUI& event);
};

class CLabelUIEx : public CLabelUI
{
	DECLARE_DUICONTROL(CVerticalLayoutUIEX)
public:
	CLabelUIEx()
	{
	}

	LPCTSTR GetClass() const
	{
		return _T("CLabelUIEx");
	}

	void DoEvent(TEventUI& event);
};

class SettingPathWnd;
class COptionUIEx : public COptionUI
{
	DECLARE_DUICONTROL(COptionUIEx)
public:
	COptionUIEx()
	{
	}

	SettingPathWnd* m_SettingPathWnd;

	LPCTSTR GetClass() const
	{
		return _T("COptionUIEx");
	}

	void DoEvent(TEventUI& event);
};

///////////////////////////////
#endif __CONTROLEX_H__
