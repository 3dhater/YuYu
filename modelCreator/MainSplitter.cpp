// MainSplitter.cpp : implementation file
//

#include "stdafx.h"
#include "modelCreator.h"
#include "MainSplitter.h"


// CMainSplitter

IMPLEMENT_DYNAMIC(CMainSplitter, CSplitterWnd)

CMainSplitter::CMainSplitter()
{

}

CMainSplitter::~CMainSplitter()
{
}


BEGIN_MESSAGE_MAP(CMainSplitter, CSplitterWnd)
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSELEAVE()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CMainSplitter message handlers
afx_msg void CMainSplitter::OnMouseHover(UINT nFlags, CPoint point)
{

}
afx_msg void CMainSplitter::OnMouseLeave()
{

}
afx_msg void CMainSplitter::OnMouseMove(UINT nFlags, CPoint point)
{

}
afx_msg void CMainSplitter::OnLButtonDown(UINT nFlags, CPoint point)
{

}