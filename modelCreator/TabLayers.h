#pragma once
#include "afxwin.h"


// TabLayers dialog
class CSetTextDialog;

class TabLayers : public CDialog
{
	DECLARE_DYNAMIC(TabLayers)

public:
	TabLayers(CWnd* pParent = NULL);   // standard constructor
	virtual ~TabLayers();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LAYERSDIALOG };
#endif

	void InitShaderSelector();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	CListBox m_listBox;
	afx_msg void OnBnClickedButton2();
	virtual void OnCancel();
	CSetTextDialog* renameDialog;

	void HideRenameDialog();
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	CComboBox m_shaderSelector;

	void UpdateLayerParameters();
	CButton m_texture1Button;
};
