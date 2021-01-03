// Viewport.cpp : implementation file
//

#include "stdafx.h"
#include "modelCreator.h"
#include "yy.h"
#include "yy_color.h"
#include "yy_model.h"
#include "scene\common.h"
#include "scene\camera.h"
#include "scene\sprite.h"
#include "Viewport.h"

#include "MainFrm.h"
extern CMainFrame * g_mainFrame;
// CViewport

IMPLEMENT_DYNCREATE(CViewport, CFormView)

CViewport::CViewport()
	: CFormView(IDD_FORMVIEW1)
{

}

CViewport::~CViewport()
{
	if (m_camera)
		yyDestroy(m_camera);
}

void CViewport::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CViewport, CFormView)
END_MESSAGE_MAP()


// CViewport diagnostics

#ifdef _DEBUG
void CViewport::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CViewport::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CViewport::InitForGPUDraw()
{
	m_gpu = yyGetVideoDriverAPI();
	m_gpu->SetClearColor(0.43f, 0.43f, 0.64f, 1.f);

	m_camera = yyCreate<yyCamera>();
	m_camera->m_objectBase.m_localPosition.set(10.f, 10.f, 10.f, 0.f);
	m_camera->m_aspect = (f32)800 / (f32)600;
	m_camera->m_target.set(0.f, 4.f, 0.f, 0.f);
	m_camera->Update();

}
// CViewport message handlers
void CViewport::OnDraw(CDC* pDC)
{
	if (m_gpu)
	{
		RECT rc;
		g_mainFrame->GetClientRect(&rc);
		//rc.left = 0;
		rc.top = 0;
		rc.right -= g_mainFrame->m_RIGHT_TAB_SIZE;
		//rc.bottom = 600;
		m_gpu->SetViewport(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
		m_camera->m_aspect = (f32)(rc.right - rc.left) / (f32)(rc.bottom - rc.top);
		m_camera->m_objectBase.UpdateBase();
		m_camera->Update();

		m_gpu->BeginDrawClearAll();
		m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::ViewProjection, m_camera->m_viewProjectionMatrix);
		m_gpu->DrawLine3D(v4f(-1.f, 0.f, 0.f, 0.f), v4f(1.f, 0.f, 0.f, 0.f), ColorRed);
		m_gpu->DrawLine3D(v4f(0.f, 0.f, -1.f, 0.f), v4f(0.f, 0.f, 1.f, 0.f), ColorRed);

		m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::Projection, m_camera->m_projectionMatrix);
		m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::View, m_camera->m_viewMatrix);

		if (g_mainFrame->m_mdlFile)
		{
			if (g_mainFrame->m_mdlFile->m_layers.size())
			{
				for (u16 i = 0, sz = g_mainFrame->m_mdlFile->m_layers.size(); i < sz; ++i)
				{
					auto layer = g_mainFrame->m_mdlFile->m_layers[i];

					Mat4 World;
					m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::World, World);
					m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::WorldViewProjection, m_camera->m_projectionMatrix * m_camera->m_viewMatrix * World);
					m_gpu->SetModel(layer->m_meshGPU);
					m_gpu->SetTexture(yyVideoDriverAPI::TextureSlot::Texture0, layer->m_textureGPU1);
					m_gpu->SetTexture(yyVideoDriverAPI::TextureSlot::Texture1, layer->m_textureGPU2);
					m_gpu->SetTexture(yyVideoDriverAPI::TextureSlot::Texture2, layer->m_textureGPU3);
					m_gpu->SetTexture(yyVideoDriverAPI::TextureSlot::Texture3, layer->m_textureGPU4);
					m_gpu->Draw();
				}
			}
		}


		m_gpu->EndDraw();
	}
}