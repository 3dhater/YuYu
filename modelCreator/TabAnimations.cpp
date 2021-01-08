// TabAnimations.cpp : implementation file
//

#include "stdafx.h"
#include "yy.h"
#include "modelCreator.h"
#include "TabAnimations.h"
#include "afxdialogex.h"
#include "MainFrm.h"

extern CMainFrame * g_mainFrame;

// TabAnimations dialog

IMPLEMENT_DYNAMIC(TabAnimations, CDialog)

TabAnimations::TabAnimations(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_ANIMATIONDIALOG, pParent)
{
	Create(IDD_ANIMATIONDIALOG, pParent);
	EnableToolTips(TRUE);
}

TabAnimations::~TabAnimations()
{
}

void TabAnimations::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_animationsList);
	DDX_Control(pDX, IDC_SLIDER1, m_slider);
}


BEGIN_MESSAGE_MAP(TabAnimations, CDialog)
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_LBN_SELCHANGE(IDC_LIST1, &TabAnimations::OnLbnSelchangeList1)
END_MESSAGE_MAP()

afx_msg void TabAnimations::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	RECT mainFrameRc;
	g_mainFrame->GetClientRect(&mainFrameRc);

	RECT rc;
	::GetClientRect(m_slider.GetSafeHwnd(), &rc);
	::MoveWindow(
		m_slider.GetSafeHwnd(), 
		rc.left, mainFrameRc.bottom - 120, 
		rc.right, mainFrameRc.bottom - 50, 
		TRUE);
}

void TabAnimations::OnOK() {}
void TabAnimations::OnCancel() {}

void TabAnimations::OnLbnSelchangeList1()
{
	// TODO: Add your control notification handler code here
}
