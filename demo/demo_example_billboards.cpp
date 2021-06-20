#include "demo.h"
#include "demo_example.h"
#include "demo_example_billboards.h"

#include "yy_input.h"
#include "yy_gui.h"

extern Demo* g_demo;

DemoExample_Billboards::DemoExample_Billboards(){
	m_editorCamera = 0;
	
	m_textureLevel = 0;
	m_spriteLevel = 0;
}

DemoExample_Billboards::~DemoExample_Billboards(){
	Shutdown();
}

bool DemoExample_Billboards::Init(){

	m_textureLevel = yyCreateTextureFromFile("../res/GA3E/level1_ground.png");
	if (!m_textureLevel)
		return false;
	m_spriteLevel = yyCreateSprite(v4f(0.f, 0.f, 11.6f, 2.24f), m_textureLevel, 8);
	m_spriteLevel->m_useAsBillboard = true;

	m_editorCamera = yyCreate<yyOrbitCamera>();
	m_editorCamera->Reset();
	return true;
}


void DemoExample_Billboards::Shutdown(){
	if (m_spriteLevel)
	{
		yyDestroy(m_spriteLevel);
		m_spriteLevel = 0;
	}
	if (m_editorCamera)
	{
		yyDestroy(m_editorCamera);
		m_editorCamera = 0;
	}
}

const wchar_t* DemoExample_Billboards::GetTitle(){
	return L"Billboards";
}

const wchar_t* DemoExample_Billboards::GetDescription(){
	return L"Billboards";
}

bool DemoExample_Billboards::DemoStep(f32 deltaTime){
	m_editorCamera->Update();

	if (g_demo->m_inputContext->m_isMMBHold)
	{
		if (g_demo->m_inputContext->IsKeyHold(yyKey::K_LALT))
			m_editorCamera->Rotate(g_demo->m_inputContext->m_mouseDelta, g_demo->m_dt);
		else
			m_editorCamera->PanMove(g_demo->m_inputContext->m_mouseDelta, g_demo->m_dt);
	}

	if (g_demo->m_inputContext->IsKeyHold(yyKey::K_LALT))
	{
		if (g_demo->m_inputContext->m_isLMBHold)
			m_editorCamera->RotateZ(g_demo->m_inputContext->m_mouseDelta.x, g_demo->m_dt);
		if (g_demo->m_inputContext->m_isRMBHold)
			m_editorCamera->ChangeFOV(g_demo->m_inputContext->m_mouseDelta.x, g_demo->m_dt);
	}

	if(g_demo->m_inputContext->m_wheelDelta)
		m_editorCamera->Zoom(g_demo->m_inputContext->m_wheelDelta);

	yySetEyePosition(&m_editorCamera->m_positionInWorld);
	
	yySetMatrix(yyMatrixType::ViewProjection, &m_editorCamera->m_viewProjectionMatrix);
	m_gpu->DrawLine3D(v4f(-10.f, 0.f, 0.f, 0.f), v4f(10.f, 0.f, 0.f, 0.f), ColorYellow);
	m_gpu->DrawLine3D(v4f(0.f, 0.f, -10.f, 0.f), v4f(0.f,0.f,10.f,0.f), ColorYellow);
	
	m_spriteLevel->m_objectBase.m_globalMatrix.identity();
	m_spriteLevel->m_objectBase.m_globalMatrix.setBasis(m_editorCamera->m_viewMatrixInvert);

	Mat4 WVP;
	WVP = m_editorCamera->m_projectionMatrix * m_editorCamera->m_viewMatrix * m_spriteLevel->m_objectBase.m_globalMatrix;

	yySetMatrix(yyMatrixType::WorldViewProjection, &WVP);
	m_gpu->DrawSprite(m_spriteLevel);

	return g_demo->m_inputContext->IsKeyHit(yyKey::K_ESCAPE) == false;
}