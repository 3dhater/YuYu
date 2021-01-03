// SetTextDialog.cpp : implementation file
//

#include "stdafx.h"
#include "modelCreator.h"
#include "SetTextDialog.h"
#include "afxdialogex.h"


// CSetTextDialog dialog

IMPLEMENT_DYNAMIC(CSetTextDialog, CDialog)

CSetTextDialog::CSetTextDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DIALOGSETTEXT, pParent)
{
	Create(IDD_DIALOGSETTEXT, pParent);
	onSetText = 0;
}

CSetTextDialog::~CSetTextDialog()
{
}

void CSetTextDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_editBox);
}


BEGIN_MESSAGE_MAP(CSetTextDialog, CDialog)
	ON_WM_KEYDOWN()
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(DM_GETDEFID, OnGetDefId)
	ON_BN_CLICKED(IDOK, &CSetTextDialog::OnBnClickedOk)
	ON_BN_CLICKED(ID_SETTEXTBUTTON, &CSetTextDialog::OnBnClickedSettextbutton)
	ON_BN_CLICKED(ID_MYCANCEL, &CSetTextDialog::OnBnClickedMycancel)
	ON_EN_SETFOCUS(IDC_EDIT1, &CSetTextDialog::OnEnSetfocusEdit1)
	ON_EN_CHANGE(IDC_EDIT1, &CSetTextDialog::OnEnChangeEdit1)
END_MESSAGE_MAP()


// CSetTextDialog message handlers

LRESULT CSetTextDialog::OnGetDefId(WPARAM wParam, LPARAM lParam)
{
	return MAKELRESULT(ID_SETTEXTBUTTON, DC_HASDEFID);
}
void CSetTextDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}

afx_msg void CSetTextDialog::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{

}
void CSetTextDialog::OnBnClickedSettextbutton()
{
	m_editBox.GetWindowTextW(m_textBuffer);
	this->ShowWindow(SW_HIDE);
	if (onSetText)
		onSetText(m_textBuffer);
}

afx_msg void CSetTextDialog::OnShowWindow(BOOL bShow, UINT nStatus)
{
	//OnEnSetfocusEdit1();
//	m_editBox.SetSel((DWORD)0xFFFF);
}
void CSetTextDialog::OnBnClickedMycancel()
{
	this->ShowWindow(SW_HIDE);
}


void CSetTextDialog::OnEnSetfocusEdit1()
{
}


void CSetTextDialog::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
}
