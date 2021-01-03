#pragma once


// CMainSplitter

class CMainSplitter : public CSplitterWnd
{
	DECLARE_DYNAMIC(CMainSplitter)

public:
	CMainSplitter();
	virtual ~CMainSplitter();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


