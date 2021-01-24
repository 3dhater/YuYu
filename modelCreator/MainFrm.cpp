#include "stdafx.h"
#include "yy.h"

#include "modelCreator.h"

#include "MainFrm.h"

#include <cmath>
#include "yy_window.h"
#include "yy_model.h"
#include "scene\common.h"
#include "scene\mdl_object.h"

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags

#include <filesystem>

CMainFrame * g_mainFrame = nullptr;

Mat4 aiMatrixToGameMatrix(const aiMatrix4x4& a)
{
	Mat4 m;
	auto dst = m.getPtr();
	f32 * src = (f32*)&a.a1;
	for (int i = 0; i < 16; ++i)
	{
		dst[i] = src[i];
	}
	return m;
}

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
	//m_mdlFile = nullptr;
	m_mdlObject = 0;

	AllocConsole();
	freopen("CONOUT$", "w", stdout);
}

CMainFrame::~CMainFrame()
{
	MDLDelete();
	if (g_engineContext)
		yyDestroy(g_engineContext);
}
//
//void print_tree(aiNode* node, int depth)
//{
//	printf("NODE:");
//	for (int i = 0; i < depth; ++i)
//	{
//		printf("  ");
//	}
//	printf(" %s\n", node->mName.C_Str());
//	for (int i = 0; i < node->mNumChildren; ++i)
//	{
//		print_tree(node->mChildren[i], depth + 1);
//	}
//}
void CMainFrame::MDLNewLayer(const wchar_t* filePath)
{
	CString name = L"Model";

	
	std::tr2::sys::path p;
	p = filePath;
	Assimp::Importer Importer;
	const aiScene* pScene = Importer.ReadFile(p.generic_string().c_str(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
	
	m_mdlObject->m_GlobalInverseTransform = aiMatrixToGameMatrix(pScene->mRootNode->mTransformation);
	m_mdlObject->m_GlobalInverseTransform.invert();

	for (int i = 0; i < pScene->mNumMeshes; ++i)
	{
		yyArray<yyVertexAnimatedModel> verts;
		yyArray<u32> inds;

		auto assMesh = pScene->mMeshes[i];

		for (int o = 0; o < assMesh->mNumVertices; o++) 
		{
			yyVertexAnimatedModel newVertex;

			const aiVector3D* pPos = &(assMesh->mVertices[o]);

			newVertex.Position.x = pPos->x;
			newVertex.Position.y = pPos->y;
			newVertex.Position.z = pPos->z;
			
			if (assMesh->HasNormals())
			{
				const aiVector3D* pNormal = &(assMesh->mNormals[o]);
				newVertex.Normal.x = pNormal->x;
				newVertex.Normal.y = pNormal->y;
				newVertex.Normal.z = pNormal->z;
			}
			if (assMesh->HasTextureCoords(0))
			{
				const aiVector3D* pTexCoord = &(assMesh->mTextureCoords[0][o]);
				newVertex.TCoords.x = pTexCoord->x;
				newVertex.TCoords.y = pTexCoord->y;
			}
			verts.push_back(newVertex);
		}

		for (int o = 0; o < assMesh->mNumFaces; o++)
		{
			const aiFace& Face = assMesh->mFaces[i];
			assert(Face.mNumIndices == 3);
			inds.push_back(Face.mIndices[0]);
			inds.push_back(Face.mIndices[1]);
			inds.push_back(Face.mIndices[2]);
		}

		yyMDLLayer* newMDLLayer = yyCreate<yyMDLLayer>();
		newMDLLayer->m_model = yyCreate<yyModel>();

		//newMDLLayer->m_model->m_aabb
		// теперь можно собрать модель
		bool is_animated = false;
		if (is_animated)
		{
			newMDLLayer->m_model->m_vertexType = yyVertexType::AnimatedModel;
			newMDLLayer->m_model->m_stride = sizeof(yyVertexAnimatedModel);
			newMDLLayer->m_model->m_vertices = (u8*)yyMemAlloc(verts.size() * sizeof(yyVertexAnimatedModel));
			memcpy(newMDLLayer->m_model->m_vertices, verts.data(), verts.size() * sizeof(yyVertexAnimatedModel));
		}
		else
		{
			newMDLLayer->m_model->m_vertexType = yyVertexType::Model;
			newMDLLayer->m_model->m_stride = sizeof(yyVertexModel);
			newMDLLayer->m_model->m_vertices = (u8*)yyMemAlloc(verts.size() * sizeof(yyVertexModel));
			auto v_ptr = (yyVertexModel*)newMDLLayer->m_model->m_vertices;
			for (u32 o = 0, sz = verts.size(); o < sz; ++o)
			{
				v_ptr[o].Position = verts[o].Position;
				v_ptr[o].Normal = verts[o].Normal;
				v_ptr[o].TCoords = verts[o].TCoords;
				v_ptr[o].Binormal = verts[o].Binormal;
				v_ptr[o].Tangent = verts[o].Tangent;
			}
		}
		
		if (inds.size() / 3 > 21845)
		{
			newMDLLayer->m_model->m_indexType = yyMeshIndexType::u32;
			newMDLLayer->m_model->m_indices = (u8*)yyMemAlloc(inds.size() * sizeof(u32));
			memcpy(newMDLLayer->m_model->m_indices, inds.data(), inds.size() * sizeof(u32));
		}
		else
		{
			newMDLLayer->m_model->m_indexType = yyMeshIndexType::u16;
			newMDLLayer->m_model->m_indices = (u8*)yyMemAlloc(inds.size() * sizeof(u16));
			u16 * i_ptr = (u16*)newMDLLayer->m_model->m_indices;
			for (u32 o = 0, sz = inds.size(); o < sz; ++o)
			{
				*i_ptr = (u16)inds[o];
				++i_ptr;
			}
		}
		
		newMDLLayer->m_model->m_name = assMesh->mName.C_Str();
		newMDLLayer->m_model->m_vCount = verts.size();
		newMDLLayer->m_model->m_iCount = inds.size();

		newMDLLayer->m_meshGPU = yyGetVideoDriverAPI()->CreateModel(newMDLLayer->m_model);

		m_mdlObject->m_mdl->m_layers.push_back(newMDLLayer);
		m_layerInfo.push_back(LayerInfo());
	}

//	print_tree(pScene->mRootNode, 0);

	/*auto slen = wcslen(filePath);
	if (filePath[slen - 1] == L'j')
	{
		newModel = _importOBJ(filePath, name);

		yyMDLLayer * newLayer = yyCreate<yyMDLLayer>();
		newLayer->m_model = newModel;

		m_mdlFile->m_layers.push_back(newLayer);
		
		MDLUpdateAABB();

		newLayer->m_meshGPU = yyGetVideoDriverAPI()->CreateModel(newLayer->m_model);
		m_layerInfo.push_back(LayerInfo());
	}
	else
	{
		_importFBX(filePath, name);
	}*/

}
void CMainFrame::MDLUpdateAABB()
{
	m_mdlObject->m_mdl->m_aabb.reset();
	for (u16 i = 0; i < m_mdlObject->m_mdl->m_layers.size(); ++i)
	{
		m_mdlObject->m_mdl->m_aabb.add(m_mdlObject->m_mdl->m_layers[i]->m_model->m_aabb);
	}
}
void CMainFrame::MDLDeleteLayer(int layerIndex)
{
	auto layer = m_mdlObject->m_mdl->m_layers[layerIndex];
	m_mdlObject->m_mdl->m_layers.erase(layerIndex);
	m_layerInfo.erase(layerIndex);
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
	m_mdlObject->m_mdl->m_layers[layerIndex]->m_model->m_name = name;
}
void CMainFrame::MDLDelete()
{
	if (!m_mdlObject)
		return;
	yyDestroy(m_mdlObject);
	m_mdlObject = nullptr;
}
void CMainFrame::MDLCreateNew()
{
	MDLDelete();
	m_mdlObject = yyCreate<yyMDLObject>();
	m_mdlObject->m_mdl = yyCreate<yyMDL>();
}
void CMainFrame::MDLLoadTexture(int layerIndex, int textureSlot, const wchar_t* name)
{
	if (!m_mdlObject)
		return;
	if (layerIndex >= m_mdlObject->m_mdl->m_layers.size())
	{
		MessageBoxW(L"MDLLoadTexture: layerIndex >= m_layers.size()");
		return;
	}
	auto layer = m_mdlObject->m_mdl->m_layers[layerIndex];
//	if (layer->m_textureGPU1)
//		yyGetVideoDriverAPI()->DeleteTexture(layer->m_textureGPU1);

	yyStringW utf16 = name;
	yyStringA utf8;
	util::utf16_to_utf8(&utf16, &utf8);
//	layer->m_textureGPU1 = yyGetVideoDriverAPI()->CreateTextureFromFile(utf8.data(), true, false, true);
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

	m_infoPanel = (CInfoPannel*)(m_mainSplitter.GetPane(0, 1));
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