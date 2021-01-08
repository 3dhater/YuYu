#pragma once
#include "afxwin.h"

#include "EditFloat.h"
#include "afxcmn.h"

// TabLayers dialog
class CSetTextDialog;
struct LayerInfo;

class TabLayers : public CDialog
{
	DECLARE_DYNAMIC(TabLayers)

public:
	TabLayers(CWnd* pParent = NULL);   // standard constructor
	virtual ~TabLayers();

	void addLayer(const wchar_t*);

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
	virtual void OnOK();
	CSetTextDialog* renameDialog;

	void HideRenameDialog();
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	CComboBox m_shaderSelector;

	void UpdateLayerParameters();
	CButton m_texture1Button;
	CEditFloat m_editPositionX;
	afx_msg void OnDeltaposSpin1(NMHDR *pNMHDR, LRESULT *pResult);
	CSpinButtonCtrl m_spinPositionX;

	void _updateEditPositionText(const LayerInfo&);
	CEditFloat m_editPositionY;
	CEditFloat m_editPositionZ;
	CSpinButtonCtrl m_spinPositionY;
	CSpinButtonCtrl m_spinPositionZ;
	afx_msg void OnDeltaposSpin2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpin3(NMHDR *pNMHDR, LRESULT *pResult);
};
