#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// TabAnimations dialog

class TabAnimations : public CDialog
{
	DECLARE_DYNAMIC(TabAnimations)

public:
	TabAnimations(CWnd* pParent = NULL);   // standard constructor
	virtual ~TabAnimations();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ANIMATIONDIALOG };
#endif

	virtual void OnCancel();
	virtual void OnOK();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnSize(UINT nType, int cx, int cy);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLbnSelchangeList1();
	CListBox m_animationsList;
	CSliderCtrl m_slider;
};
