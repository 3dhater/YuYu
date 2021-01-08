// TabLayers.cpp : implementation file
//

#include "stdafx.h"
#include "yy.h"
#include "yy_model.h"
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

void EditPositionX_onCharEnter(CEditFloat* e)
{
	CString text;
	e->GetWindowTextW(text);
	if (!g_tabLayers) return;

	auto cnt = g_tabLayers->m_listBox.GetCount();
	if (!cnt) return;

	auto cursel = g_tabLayers->m_listBox.GetCurSel();
	auto & layerInfo = g_mainFrame->m_layerInfo[cursel];
	
	layerInfo.m_offset.x = util::to_float((const char16_t*)text.GetBuffer());
}
void EditPositionY_onCharEnter(CEditFloat* e)
{
	CString text;
	e->GetWindowTextW(text);
	if (!g_tabLayers) return;

	auto cnt = g_tabLayers->m_listBox.GetCount();
	if (!cnt) return;

	auto cursel = g_tabLayers->m_listBox.GetCurSel();
	auto & layerInfo = g_mainFrame->m_layerInfo[cursel];

	layerInfo.m_offset.y = util::to_float((const char16_t*)text.GetBuffer());
}
void EditPositionZ_onCharEnter(CEditFloat* e)
{
	CString text;
	e->GetWindowTextW(text);
	if (!g_tabLayers) return;

	auto cnt = g_tabLayers->m_listBox.GetCount();
	if (!cnt) return;

	auto cursel = g_tabLayers->m_listBox.GetCurSel();
	auto & layerInfo = g_mainFrame->m_layerInfo[cursel];

	layerInfo.m_offset.z = util::to_float((const char16_t*)text.GetBuffer());
}
void TabLayers::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_listBox);
	DDX_Control(pDX, IDC_COMBO1, m_shaderSelector);
	DDX_Control(pDX, IDC_BUTTON4, m_texture1Button);
	DDX_Control(pDX, IDC_EDIT2, m_editPositionX);
	DDX_Control(pDX, IDC_SPIN1, m_spinPositionX);
	DDX_Control(pDX, IDC_EDIT3, m_editPositionY);
	DDX_Control(pDX, IDC_EDIT4, m_editPositionZ);
	DDX_Control(pDX, IDC_SPIN2, m_spinPositionY);
	DDX_Control(pDX, IDC_SPIN3, m_spinPositionZ);
	m_editPositionX.m_onCharEnter = EditPositionX_onCharEnter;
	m_editPositionY.m_onCharEnter = EditPositionY_onCharEnter;
	m_editPositionZ.m_onCharEnter = EditPositionZ_onCharEnter;
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
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN1, &TabLayers::OnDeltaposSpin1)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN2, &TabLayers::OnDeltaposSpin2)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN3, &TabLayers::OnDeltaposSpin3)
END_MESSAGE_MAP()

void TabLayers::InitShaderSelector()
{
	m_shaderSelector.AddString(L"Simple");
	m_shaderSelector.SetCurSel(0);
}
void TabLayers::OnOK(){}
void TabLayers::OnCancel(){}
afx_msg void TabLayers::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){}
afx_msg void TabLayers::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){}

// TabLayers message handlers
afx_msg void TabLayers::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
}

void TabLayers::addLayer(const wchar_t* name)
{
	m_listBox.InsertString(m_listBox.GetCount(), name);
	m_listBox.SetCurSel(m_listBox.GetCount() - 1);
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

		if(!name.IsEmpty())
			addLayer(name);

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
		g_mainFrame->RedrawWindow();
	}
}

void TabLayers::UpdateLayerParameters()
{
	auto cnt = m_listBox.GetCount();
	if (!cnt)
	{
		m_shaderSelector.EnableWindow(0);
		m_texture1Button.EnableWindow(0);
		m_editPositionX.EnableWindow(0);
		m_editPositionY.EnableWindow(0);
		m_editPositionZ.EnableWindow(0);
		m_spinPositionX.EnableWindow(0);
		m_spinPositionY.EnableWindow(0);
		m_spinPositionZ.EnableWindow(0);
	}
	else
	{
		auto cursel = g_tabLayers->m_listBox.GetCurSel();
		auto & layerInfo = g_mainFrame->m_layerInfo[cursel];

		m_shaderSelector.EnableWindow(1);
		m_texture1Button.EnableWindow(1);		
		m_editPositionX.EnableWindow(1);
		m_editPositionY.EnableWindow(1);
		m_editPositionZ.EnableWindow(1);
		m_spinPositionX.EnableWindow(1);
		m_spinPositionY.EnableWindow(1);
		m_spinPositionZ.EnableWindow(1);

		_updateEditPositionText(layerInfo);
	}
}
void TabLayers::_updateEditPositionText(const LayerInfo& layerInfo)
{
	wchar_t wbuff[64];
	swprintf(wbuff, 64, L"%f", layerInfo.m_offset.x);
	m_editPositionX.SetWindowTextW(wbuff);

	swprintf(wbuff, 64, L"%f", layerInfo.m_offset.y);
	m_editPositionY.SetWindowTextW(wbuff);

	swprintf(wbuff, 64, L"%f", layerInfo.m_offset.z);
	m_editPositionZ.SetWindowTextW(wbuff);
}

void TabLayers::OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult)
{
	auto cnt = m_listBox.GetCount();
	if (!cnt) return;

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	auto cursel = g_tabLayers->m_listBox.GetCurSel();
	auto & layerInfo = g_mainFrame->m_layerInfo[cursel];
	layerInfo.m_offset.x -= ((float)pNMUpDown->iDelta)*0.01;

	_updateEditPositionText(layerInfo);
}


void TabLayers::OnDeltaposSpin2(NMHDR *pNMHDR, LRESULT *pResult)
{
	auto cnt = m_listBox.GetCount();
	if (!cnt) return;

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	auto cursel = g_tabLayers->m_listBox.GetCurSel();
	auto & layerInfo = g_mainFrame->m_layerInfo[cursel];
	layerInfo.m_offset.y -= ((float)pNMUpDown->iDelta)*0.01;

	_updateEditPositionText(layerInfo);
}


void TabLayers::OnDeltaposSpin3(NMHDR *pNMHDR, LRESULT *pResult)
{
	auto cnt = m_listBox.GetCount();
	if (!cnt) return;

	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;

	auto cursel = g_tabLayers->m_listBox.GetCurSel();
	auto & layerInfo = g_mainFrame->m_layerInfo[cursel];
	layerInfo.m_offset.z -= ((float)pNMUpDown->iDelta)*0.01;

	_updateEditPositionText(layerInfo);
}
