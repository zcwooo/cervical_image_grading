#pragma once

class CListTextElementUIEX :public CListTextElementUI
{
	DECLARE_DUICONTROL(CListUI)
public:
	CListTextElementUIEX();
	~CListTextElementUIEX();

public:
	void DoEvent(DuiLib::TEventUI& event);
	bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl);
	virtual void PaintBkColor(HDC hDC);
};

