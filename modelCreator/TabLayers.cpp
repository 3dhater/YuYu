// TabLayers.cpp : implementation file
//

#include "stdafx.h"
#include "modelCreator.h"
#include "TabLayers.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "SetTextDialog.h"


extern CMainFrame * g_mainFrame;
TabLayers * g_tabLayers = 0;

// TabLayers dialog

IMPLEMENT_DYNAMIC(TabLayers, CDialog)

TabLayers::TabLayers(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_LAYERSDIALOG, pParent)
{
	Create(IDD_LAYERSDIALOG, pParent);
	EnableToolTips(TRUE);
	renameDialog = 0;
	g_tabLayers = this;
}

TabLayers::~TabLayers()
{
}

void TabLayers::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listBox);
	DDX_Control(pDX, IDC_COMBO1, m_shaderSelector);
	DDX_Control(pDX, IDC_BUTTON4, m_texture1Button);
}


BEGIN_MESSAGE_MAP(TabLayers, CDialog)
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_SYSKEYDOWN()
	ON_BN_CLICKED(IDC_BUTTON1, &TabLayers::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &TabLayers::OnBnClickedButton2)
	ON_LBN_SELCHANGE(IDC_LIST1, &TabLayers::OnLbnSelchangeList1)
	ON_BN_CLICKED(IDC_BUTTON3, &TabLayers::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &TabLayers::OnBnClickedButton4)
END_MESSAGE_MAP()

void TabLayers::InitShaderSelector()
{
	m_shaderSelector.AddString(L"Simple");
	m_shaderSelector.SetCurSel(0);
}
void TabLayers::OnCancel()
{
}
afx_msg void TabLayers::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}
afx_msg void TabLayers::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
}

// TabLayers message handlers
afx_msg void TabLayers::OnSize(UINT nType, int cx, int cy)
{
	RECT mainFrameRc;
	g_mainFrame->GetClientRect(&mainFrameRc);
	
	CDialog::OnSize(nType, cx, cy);

	/*GetParentFrame()->GetClientRect(&mainFrameRc);
	HWND hwnd = GetSafeHwnd();
	RECT rc;
	::GetClientRect(hwnd, &rc);
	::MoveWindow(hwnd, 7, 24, rc.right, 10, TRUE);*/
}

void TabLayers::OnBnClickedButton1()
{
	const TCHAR szFilter[] = _T("3D Files |*.obj;*.fbx");
	CFileDialog dlg(TRUE, 0, NULL, OFN_HIDEREADONLY, szFilter, this);
	if (dlg.DoModal() == IDOK)
	{
		CString sFilePath = dlg.GetPathName();
		auto cursel = g_tabLayers->m_listBox.GetCurSel();
		
		auto name = g_mainFrame->MDLNewLayer(sFilePath.GetBuffer());

		m_listBox.InsertString(m_listBox.GetCount(), name);
		m_listBox.SetCurSel(m_listBox.GetCount() - 1);

		HideRenameDialog();
		g_mainFrame->RedrawWindow();

		UpdateLayerParameters();
	}
}

void onRenameLayer(CString& s)
{
	auto cursel = g_tabLayers->m_listBox.GetCurSel();
	g_tabLayers->m_listBox.DeleteString(cursel);
	g_tabLayers->m_listBox.InsertString(cursel, s.GetBuffer());
	g_tabLayers->m_listBox.SetCurSel(cursel);
	g_mainFrame->MDLRenameLayer(cursel, s.GetBuffer());
}
void TabLayers::OnBnClickedButton2()
{
	if (!renameDialog)
	{
		renameDialog = new CSetTextDialog(this);
		renameDialog->SetWindowTextW(L"Rename");
		renameDialog->onSetText = onRenameLayer;
		HideRenameDialog();
	}

	if (renameDialog)
	{
		auto cnt = m_listBox.GetCount();
		if (!cnt)
			return;
		CString str;
		m_listBox.GetText(m_listBox.GetCurSel(), str);

		auto edit = (CEdit*)renameDialog->GetDlgItem(IDC_EDIT1);
		edit->SetWindowTextW(str.GetBuffer());
		edit->LimitText(32);
						
		renameDialog->ShowWindow(SW_SHOWNORMAL);
	}
}

void TabLayers::HideRenameDialog()
{
	if (renameDialog)
		renameDialog->ShowWindow(SW_HIDE);
}

void TabLayers::OnLbnSelchangeList1()
{
	HideRenameDialog();
	UpdateLayerParameters();
}


void TabLayers::OnBnClickedButton3()
{
	auto cnt = m_listBox.GetCount();
	if (!cnt)
		return;
	auto cursel = g_tabLayers->m_listBox.GetCurSel();
	g_tabLayers->m_listBox.DeleteString(cursel);
	g_mainFrame->MDLDeleteLayer(cursel);
		
	if (cnt > 1)
	{
		if (cursel >= cnt-1)
			--cursel;

		g_tabLayers->m_listBox.SetCurSel(cursel);
	}
	g_mainFrame->RedrawWindow();
	UpdateLayerParameters();
}

// Texture 1
void TabLayers::OnBnClickedButton4()
{
	auto cnt = m_listBox.GetCount();
	if (!cnt) return;

	HideRenameDialog();
	const TCHAR szFilter[] = _T("Image Files |*.dds;*.png");
	CFileDialog dlg(TRUE, 0, NULL, OFN_HIDEREADONLY, szFilter, this);
	if (dlg.DoModal() == IDOK)
	{
		CString sFilePath = dlg.GetPathName();
		auto cursel = g_tabLayers->m_listBox.GetCurSel();
		g_mainFrame->MDLLoadTexture(cursel, 0, sFilePath.GetBuffer());
	}
}

void TabLayers::UpdateLayerParameters()
{
	auto cnt = m_listBox.GetCount();
	if (!cnt)
	{
		m_shaderSelector.EnableWindow(0);
		m_texture1Button.EnableWindow(0);
	}
	else
	{
		m_shaderSelector.EnableWindow(1);
		m_texture1Button.EnableWindow(1);
	}
}