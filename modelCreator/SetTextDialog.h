#pragma once
#include "afxwin.h"


// CSetTextDialog dialog

class CSetTextDialog : public CDialog
{
	DECLARE_DYNAMIC(CSetTextDialog)

public:
	CSetTextDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSetTextDialog();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOGSETTEXT };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	LRESULT OnGetDefId(WPARAM wParam, LPARAM lParam);


	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedSettextbutton();

	CString m_textBuffer;
	CEdit m_editBox;
	afx_msg void OnBnClickedMycancel();

	void(*onSetText)(CString&);
	afx_msg void OnEnSetfocusEdit1();
	afx_msg void OnEnChangeEdit1();
};
