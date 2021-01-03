#pragma once
#include "afxcmn.h"
#include "MyTabControl.h"

class TabLayers;

// CInfoPannel form view
class CInfoPannel : public CFormView
{
	DECLARE_DYNCREATE(CInfoPannel)

protected:
	CInfoPannel();           // protected constructor used by dynamic creation
	virtual ~CInfoPannel();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FORMVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual void OnInitialUpdate();
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);

	BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	MyTabControl myTabControl;
	
	CWnd *		 m_activeTab;
	TabLayers *	 m_layersTab;

	void InitTabControl();
};


