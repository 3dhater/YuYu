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
#include "scene\mdl_object.h"
#include "math\math.h"
#include "Viewport.h"
#include "TabLayers.h"

#include "MainFrm.h"
extern CMainFrame * g_mainFrame;
// CViewport

#define IDT_TIMER1 1

IMPLEMENT_DYNCREATE(CViewport, CFormView)

CViewport* g_viewport = 0;

CViewport::CViewport()
	: CFormView(IDD_FORMVIEW1)
{
	g_viewport = this;
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
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
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

void CALLBACK lpfnTimer(HWND, UINT, UINT_PTR, DWORD)
{
	g_viewport->Draw();
}
void CViewport::Draw()
{
	OnDraw(0);
}
void CViewport::InitForGPUDraw()
{
	m_gpu = yyGetVideoDriverAPI();
	m_gpu->SetClearColor(0.43f, 0.43f, 0.64f, 1.f);

	m_camera = yyCreate<yyCamera>();

	m_cameraPosition.set(0.f, 0.f, 10.f, 0.f);
	m_camera->m_objectBase.m_localPosition = m_cameraPosition;
	m_camera->m_aspect = (f32)800 / (f32)600;
	m_camera->m_target.set(0.f, 0.f, 0.f, 0.f);
	m_camera->m_near = 0.1f;
	m_camera->m_far = 3000.f;
	m_camera->Update();

	SetTimer(IDT_TIMER1, 20, lpfnTimer);
}

afx_msg BOOL CViewport::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	m_cameraPosition.z -= float(zDelta) / 120.f;
	if (m_cameraPosition.z <= 0.1f)
		m_cameraPosition.z = 0.1f;

	_updateCamera();
	return 0;
}
afx_msg void CViewport::OnMouseMove(UINT nFlags, CPoint point)
{
	if (nFlags & MK_LBUTTON)
	{
		bool update = false;
		float delta_x = (float)(point.x - m_old_point.x);
		float speed_x = 0.02f * std::fabs(delta_x);
		if (delta_x > 0.f)
		{
			m_cameraRotation.y += speed_x;
			if (m_cameraRotation.y >= math::PIPI)  m_cameraRotation.y = 0.f;
			update = true;
		}

		if (delta_x < 0.f)
		{
			m_cameraRotation.y -= speed_x;
			if (m_cameraRotation.y <= -math::PIPI) m_cameraRotation.y = 0.f;
			update = true;
		}

		float y_limit = 0.9f;
		float delta_y = (float)(point.y - m_old_point.y);
		float speed_y = 0.02f * std::fabs(delta_y);
		if (delta_y > 0.f)
		{
			m_cameraRotation.x += speed_y;
			if (m_cameraRotation.x >= math::PIPI)  m_cameraRotation.x = 0.f;
			if (m_cameraRotation.x >= y_limit)  m_cameraRotation.x = y_limit;
			update = true;

		}

		if (delta_y < 0.f)
		{
			m_cameraRotation.x -= speed_y;
			if (m_cameraRotation.x <= -math::PIPI) m_cameraRotation.x = 0.f;
			if (m_cameraRotation.x <= -y_limit)  m_cameraRotation.x = -y_limit;
			update = true;
		}

		if (update)
			_updateCamera();
	}
	m_old_point = point;
}
afx_msg void CViewport::OnKillFocus(CWnd* pNewWnd)
{
}
afx_msg void CViewport::OnMouseLeave()
{
}
afx_msg void CViewport::OnSetFocus(CWnd* pOldWnd)
{
//	this->m_old_point.x = 0;
//	this->m_old_point.y = 0;
}
void CViewport::_updateCamera()
{
	Quat Qy(0.f, m_cameraRotation.y, 0.f);
	Mat4 Ry;
	Ry.setRotation(Qy);

	Quat Qx(m_cameraRotation.x, 0.f, 0.f);
	Mat4 Rx;
	Rx.setRotation(Qx);

	m_camera->m_objectBase.m_localPosition = math::mul(m_cameraPosition, Ry * Rx);

	m_camera->m_objectBase.UpdateBase();
	m_camera->Update();
}
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
	//	m_gpu->SetViewport(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
	//	m_camera->m_aspect = (f32)(rc.right - rc.left) / (f32)(rc.bottom - rc.top);
		m_camera->m_objectBase.UpdateBase();
		m_camera->Update();

		m_gpu->BeginDraw();
		m_gpu->ClearAll();
		m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::ViewProjection, m_camera->m_viewProjectionMatrix);
		m_gpu->DrawLine3D(v4f(-100.f, 0.f, 0.f, 0.f), v4f(100.f, 0.f, 0.f, 0.f), ColorRed);
		m_gpu->DrawLine3D(v4f(0.f, 0.f, -100.f, 0.f), v4f(0.f, 0.f, 100.f, 0.f), ColorRed);

		m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::Projection, m_camera->m_projectionMatrix);
		m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::View, m_camera->m_viewMatrix);

	//	rotation.y += 0.1f;
		
		//if (g_mainFrame->m_mdlObject)
		//{
		//	if (g_mainFrame->m_mdlObject->m_mdl->m_layers.size())
		//	{
		//		int selectedLayer = g_mainFrame->m_infoPanel->m_layersTab->m_listBox.GetCurSel();

		//		for (u16 i = 0, sz = g_mainFrame->m_mdlObject->m_mdl->m_layers.size(); i < sz; ++i)
		//		{
		//			auto layer = g_mainFrame->m_mdlObject->m_mdl->m_layers[i];
		//			auto & layerInfo = g_mainFrame->m_layerInfo[i];

		//			Mat4 World;
		//			World[3] = layerInfo.m_offset;
		//			World[3].w = 1.f;

		//			m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::World, World);
		//			m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::WorldViewProjection, m_camera->m_projectionMatrix * m_camera->m_viewMatrix * World);
		//			m_gpu->SetModel(layer->m_meshGPU);
		//		//	m_gpu->SetTexture(yyVideoDriverAPI::TextureSlot::Texture0, layer->m_textureGPU1 ? layer->m_textureGPU1 : 0);
		//		//	m_gpu->SetTexture(yyVideoDriverAPI::TextureSlot::Texture1, layer->m_textureGPU2 ? layer->m_textureGPU2 : 0);
		//		//	m_gpu->SetTexture(yyVideoDriverAPI::TextureSlot::Texture2, layer->m_textureGPU3 ? layer->m_textureGPU3 : 0);
		//		//	m_gpu->SetTexture(yyVideoDriverAPI::TextureSlot::Texture3, layer->m_textureGPU4 ? layer->m_textureGPU4 : 0);
		//			m_gpu->Draw();


		//			if (selectedLayer == i)
		//			{
		//				m_gpu->DrawLine3D(v4f(-1.f, 0.f, 0.f, 0.f), v4f(1.f, 0.f, 0.f, 0.f), ColorRed);
		//				m_gpu->DrawLine3D(v4f(0.f, -1.f, 0.f, 0.f), v4f(0.f, 1.f, 0.f, 0.f), ColorLime);
		//				m_gpu->DrawLine3D(v4f(0.f, 0.f, -1.f, 0.f), v4f(0.f, 0.f, 1.f, 0.f), ColorBlue);
		//			}
		//		}

		//		m_gpu->UseDepth(false);
		//		/*if (g_mainFrame->m_mdlFile->m_skeleton)
		//		{
		//			for (u16 i = 0, sz = g_mainFrame->m_mdlFile->m_joints.size(); i < sz; ++i)
		//			{
		//				auto joint = g_mainFrame->m_mdlFile->m_joints[i];
		//				m_gpu->DrawLine3D(
		//					joint->m_matrix.m_data[3] - v4f(0.1f, 0.f, 0.f, 0.f),
		//					joint->m_matrix.m_data[3] + v4f(0.1f, 0.f, 0.f, 0.f), ColorCyan);
		//				m_gpu->DrawLine3D(
		//					joint->m_matrix.m_data[3] - v4f(0.f, 0.1f, 0.f, 0.f),
		//					joint->m_matrix.m_data[3] + v4f(0.f, 0.1f, 0.f, 0.f), ColorCyan);
		//				m_gpu->DrawLine3D(
		//					joint->m_matrix.m_data[3] - v4f(0.f, 0.f, 0.1f, 0.f),
		//					joint->m_matrix.m_data[3] + v4f(0.f, 0.f, 0.1f, 0.f), ColorCyan);

		//				if (joint->m_parent)
		//				{
		//					m_gpu->DrawLine3D(
		//						joint->m_matrix.m_data[3],
		//						joint->m_parent->m_matrix.m_data[3], ColorLime);
		//				}
		//			}
		//		}*/
		//		m_gpu->UseDepth(true);
		//	}
		//}


		m_gpu->EndDraw();
	}
}