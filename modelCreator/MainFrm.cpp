#include "stdafx.h"
#include "modelCreator.h"

#include "MainFrm.h"

#include <cmath>
#include "yy.h"
#include "yy_window.h"
#include "yy_model.h"

CMainFrame * g_mainFrame = nullptr;

struct yyEngineContext
{
	yyEngineContext()
	{
		m_state = nullptr;
	}
	~yyEngineContext()
	{
		yyStop(); // destroy main class, free memory
	}

	void init(yyInputContext* input)
	{
		m_state = yyStart(input); // allocate memory for main class inside yuyu.dll
	}

	yySystemState * m_state;
};
yyInputContext*  g_inputContext = 0;
yyEngineContext* g_engineContext = 0;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

#define WINDOW_SIZE_X 800
#define WINDOW_SIZE_Y 600


IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_COMMAND(ID_TOOLBAR_BUTTON1, OnToolbarButton)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	g_mainFrame = this;
	m_RIGHT_TAB_SIZE = 300;
	m_mdlFile = nullptr;
}

CMainFrame::~CMainFrame()
{
	MDLDelete();

	if (g_engineContext)
		yyDestroy(g_engineContext);
}

CString CMainFrame::MDLNewLayer(const wchar_t* filePath)
{
	CString name = L"Model";
	yyModel* newModel = 0;

	auto slen = wcslen(filePath);
	if (filePath[slen - 1] == L'j')
		newModel = _importOBJ(filePath, name);
	else
		newModel = _importFBX(filePath, name);
	newModel->m_vertexType = yyVertexType::Model;

	yyMDLLayer * newLayer = yyCreate<yyMDLLayer>();
	newLayer->m_model = newModel;

	m_mdlFile->m_layers.push_back(newLayer);
	MDLUpdateAABB();

	newLayer->m_meshGPU = yyGetVideoDriverAPI()->CreateModel(newLayer->m_model);

	return name;
}
void CMainFrame::MDLUpdateAABB()
{
	m_mdlFile->m_aabb.reset();
	for (u16 i = 0; i < m_mdlFile->m_layers.size(); ++i)
	{
		m_mdlFile->m_aabb.add(m_mdlFile->m_layers[i]->m_model->m_aabb);
	}
}
void CMainFrame::MDLDeleteLayer(int layerIndex)
{
	auto layer = m_mdlFile->m_layers[layerIndex];
	m_mdlFile->m_layers.erase(layerIndex);
	if (layer->m_meshGPU)
	{
		yyGetVideoDriverAPI()->DeleteModel(layer->m_meshGPU);
		layer->m_meshGPU = nullptr;
	}
	yyDestroy(layer);

	MDLUpdateAABB();
}
void CMainFrame::MDLRenameLayer(int layerIndex, const wchar_t* name)
{
	m_mdlFile->m_layers[layerIndex]->m_model->m_name = name;
}
void CMainFrame::MDLDelete()
{
	if (!m_mdlFile)
		return;
	yyDestroy(m_mdlFile);
	m_mdlFile = nullptr;
}
void CMainFrame::MDLCreateNew()
{
	MDLDelete();
	m_mdlFile = yyCreate<yyMDLFile>();
}
void CMainFrame::MDLLoadTexture(int layerIndex, int textureSlot, const wchar_t* name)
{
	if (!m_mdlFile)
		return;
	if (layerIndex >= m_mdlFile->m_layers.size())
	{
		MessageBoxW(L"MDLLoadTexture: layerIndex >= m_layers.size()");
		return;
	}
	auto layer = m_mdlFile->m_layers[layerIndex];
	if (layer->m_textureGPU1)
		yyGetVideoDriverAPI()->DeleteTexture(layer->m_textureGPU1);

	yyStringW utf16 = name;
	yyStringA utf8;
	util::utf16_to_utf8(&utf16, &utf8);
	layer->m_textureGPU1 = yyGetVideoDriverAPI()->CreateTextureFromFile(utf8.data(), true, false, true);
}

void CMainFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
	lpMMI->ptMinTrackSize.x = WINDOW_SIZE_X;
	lpMMI->ptMinTrackSize.y = WINDOW_SIZE_Y;
}

void CMainFrame::OnToolbarButton()
{
	PostQuitMessage(0);
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC)) {
		TRACE0("Failed to create toolbar\n");
		return -1;	// fail to create
	}
	if (!m_wndToolBar.LoadToolBar(IDR_TOOLBAR1))
	{
		TRACE0("Failed to load toolbar\n");
		return -1;	// fail to create
	}

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));


	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	CRect cr;
	GetClientRect(&cr);

	if (!m_mainSplitter.CreateStatic(this, 1, 2))
	{
		MessageBox(L"Error setting up splitter frames!", L"Init Error!", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	if (!m_mainSplitter.CreateView(0, 0, RUNTIME_CLASS(CViewport),
		CSize(m_RIGHT_TAB_SIZE, cr.Height()), pContext))
	{
		MessageBox(L"Error setting up splitter frames!", L"Init Error!", MB_OK | MB_ICONERROR);
		return FALSE;
	}


	if (!m_mainSplitter.CreateView(0, 1, RUNTIME_CLASS(CInfoPannel),
		CSize(m_RIGHT_TAB_SIZE, cr.Height()), pContext))
	{
		MessageBox(L"Error setting up splitter frames!", L"Init Error!", MB_OK | MB_ICONERROR);
		return FALSE;
	}
	((CInfoPannel*)(m_mainSplitter.GetPane(0, 1)))->InitTabControl();

	//((CInfoPannel*)(m_mainSplitter.GetPane(0, 1)))->m_viewports = &m_viewportSplitter;

	f32 fv = 410.1245f;
	u16 sh = 0;

	if (fv >= 1.f || fv <= -1.f)
		sh |= 0x8000;
	if (fv <= 0.f)
		sh |= 0x4000;

	int iv = (int)fv;
	fv -= (iv = fv);

	f32 f2 = 0.f;
	auto f = std::modf(12.345f, &f2);

	g_engineContext = yyCreate<yyEngineContext>();
	g_engineContext->init(0);
	static yyWindow yywnd;
	yywnd.m_hWnd = ((CInfoPannel*)(m_mainSplitter.GetPane(0, 0)))->GetSafeHwnd();
	yywnd.m_dc = ((CInfoPannel*)(m_mainSplitter.GetPane(0, 0)))->GetDC()->GetSafeHdc();
	if (!yyInitVideoDriver("opengl.yyvd", &yywnd))
	{
		MessageBox(L"Can't create OpenGL video driver", L"Init Error!", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	((CViewport*)(m_mainSplitter.GetPane(0, 0)))->InitForGPUDraw();
	
	MDLCreateNew();

	return TRUE;
}

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);
	//CRect cr;
	if (nType != SIZE_MINIMIZED)
	{
		m_mainSplitter.SetRowInfo(0, cy, 0);
		m_mainSplitter.SetColumnInfo(0, cx - m_RIGHT_TAB_SIZE, 0);
		m_mainSplitter.SetColumnInfo(1, m_RIGHT_TAB_SIZE, 0);

		m_mainSplitter.RecalcLayout();
	}
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	cs.cx = WINDOW_SIZE_X;
	cs.cy = WINDOW_SIZE_Y;
	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// forward focus to the view window
//	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
//	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
//		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}