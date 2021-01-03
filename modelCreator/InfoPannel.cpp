// InfoPannel.cpp : implementation file
//

#include "stdafx.h"
#include "modelCreator.h"
#include "InfoPannel.h"
#include "MainFrm.h"
#include "TabLayers.h"

extern CMainFrame * g_mainFrame;

// CInfoPannel

IMPLEMENT_DYNCREATE(CInfoPannel, CFormView)

CInfoPannel::CInfoPannel()
	: CFormView(IDD_FORMVIEW)
{
	m_layersTab = 0;
}

CInfoPannel::~CInfoPannel()
{
}

void CInfoPannel::OnInitialUpdate()
{
	
}
afx_msg int CInfoPannel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	//if (CView::OnCreate(lpCreateStruct) == -1)
	//	return -1;
	
	
	return 0;
}
BOOL CInfoPannel::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{

	return TRUE;
}

void CInfoPannel::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	

}

void CInfoPannel::InitTabControl()
{
	auto tabs = (CTabCtrl*)GetDlgItem(IDC_TAB1);

	TCITEM newItem;
	newItem.mask = TCIF_PARAM;
	newItem.lParam = 0;
	auto newTab = tabs->InsertItem(tabs->GetItemCount(), L"Layers");
	tabs->SetItem(newTab, &newItem);

	m_layersTab = new TabLayers(this);
	m_layersTab->InitShaderSelector();
	m_activeTab = m_layersTab;
	m_activeTab->ShowWindow(SW_SHOWNORMAL);
}
void CInfoPannel::DoDataExchange(CDataExchange* pDX)
{
	//CFormView::DoDataExchange(pDX);
	//DDX_Control(pDX, IDC_TAB1, myTabControl);

}

BEGIN_MESSAGE_MAP(CInfoPannel, CFormView)
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CInfoPannel::OnTcnSelchangeTab1)
END_MESSAGE_MAP()


// CInfoPannel diagnostics

#ifdef _DEBUG
void CInfoPannel::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CInfoPannel::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CInfoPannel message handlers
afx_msg void CInfoPannel::OnSize(UINT nType, int cx, int cy)
{
	RECT mainFrameRc;
	g_mainFrame->GetClientRect(&mainFrameRc);

	CFormView::OnSize(nType, cx, cy);
	HWND hwnd = GetDlgItem(IDC_TAB1)->GetSafeHwnd();
	RECT rc;
	::GetClientRect(hwnd, &rc);
	::MoveWindow(hwnd, rc.left, rc.top, rc.right, mainFrameRc.bottom-50, TRUE);

	if (m_layersTab)
	{
		::MoveWindow(m_layersTab->GetSafeHwnd(), 7, 24, rc.right-14, mainFrameRc.bottom - 80, TRUE);
	}
}

void CInfoPannel::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;

	if (m_activeTab != NULL) {
		//activeTab->ShowWindow(SW_HIDE);
	}
}
