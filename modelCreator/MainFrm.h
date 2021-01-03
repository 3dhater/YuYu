
// MainFrm.h : interface of the CMainFrame class
//

#pragma once
#include "ChildView.h"
#include "InfoPannel.h"
#include "MainSplitter.h"
#include "Viewport.h"

struct yyMDLFile;
struct yyResource;
struct yyModel;
class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Attributes
public:

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	virtual void OnSize(UINT nType, int cx, int cy);

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	int m_RIGHT_TAB_SIZE;
	
	yyMDLFile* m_mdlFile;

	void MDLCreateNew();
	void MDLDelete();
	void MDLRenameLayer(int layerIndex, const wchar_t* name);
	void MDLDeleteLayer(int layerIndex);
	void MDLUpdateAABB();
	CString MDLNewLayer(const wchar_t* filePath);
	void MDLLoadTexture(int layerIndex, int textureSlot, const wchar_t* name);


protected:  // control bar embedded members
	CStatusBar        m_wndStatusBar;
	CChildView    m_wndView;
	CToolBar m_wndToolBar;

	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	CMainSplitter m_mainSplitter;
	CInfoPannel*	m_infoPanel;
	CViewport*		m_viewport;

	void OnToolbarButton();

	yyModel* _importOBJ(const wchar_t* filePath, CString& outName);
	yyModel* _importFBX(const wchar_t* filePath, CString& outName);

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	DECLARE_MESSAGE_MAP()

};


