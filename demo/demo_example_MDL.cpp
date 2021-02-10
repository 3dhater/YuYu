#include "demo.h"
#include "demo_example.h"
#include "demo_example_MDL.h"

#include "yy_model.h"

#include "yy_input.h"

extern Demo* g_demo;

DemoExample_MDL::DemoExample_MDL()
{
	m_mdl_struct = 0;
	m_flyCamera = 0;
}

DemoExample_MDL::~DemoExample_MDL()
{
	Shutdown();
}

bool DemoExample_MDL::Init()
{
	m_flyCamera = yyCreate<yyFlyCamera>();

	m_mdl_struct = yyGetModel("../res/test/mdl/struct.mdl");

	return true;
}

void DemoExample_MDL::Shutdown()
{
	if (m_flyCamera)
	{
		yyDestroy(m_flyCamera);
		m_flyCamera = 0;
	}
	if (m_mdl_struct)
	{
		yyDeleteModel(m_mdl_struct);
		m_mdl_struct = 0;
	}
}

const wchar_t* DemoExample_MDL::GetTitle()
{
	return L"MDL";
}

const wchar_t* DemoExample_MDL::GetDescription()
{
	return L"Everything about MDL";
}

bool DemoExample_MDL::DemoStep(f32 deltaTime)
{
	m_flyCamera->Update();
	if (g_demo->m_inputContext->m_isRMBHold)
	{
		m_flyCamera->Rotate(v2f(-g_demo->m_inputContext->m_mouseDelta.x, -g_demo->m_inputContext->m_mouseDelta.y), deltaTime);

		if (g_demo->m_inputContext->isKeyHold(yyKey::K_W))
			m_flyCamera->MoveForward(deltaTime);

		if (g_demo->m_inputContext->isKeyHold(yyKey::K_S))
			m_flyCamera->MoveBackward(deltaTime);

		if (g_demo->m_inputContext->isKeyHold(yyKey::K_A))
			m_flyCamera->MoveLeft(deltaTime);

		if (g_demo->m_inputContext->isKeyHold(yyKey::K_D))
			m_flyCamera->MoveRight(deltaTime);

		if (g_demo->m_inputContext->isKeyHold(yyKey::K_E))
			m_flyCamera->MoveUp(deltaTime);

		if (g_demo->m_inputContext->isKeyHold(yyKey::K_Q))
			m_flyCamera->MoveDown(deltaTime);

		auto cursorX = std::floor((f32)g_demo->m_window->m_currentSize.x / 2.f);
		auto cursorY = std::floor((f32)g_demo->m_window->m_currentSize.y / 2.f);
		g_demo->m_inputContext->m_cursorCoordsOld.set(cursorX, cursorY);

		yySetCursorPosition(cursorX, cursorY, g_demo->m_window);
	}

	auto size = m_mdl_struct->m_layers.size();
	for (u16 i = 0; i < size; ++i)
	{
		auto layer = m_mdl_struct->m_layers[i];
		m_gpu->SetModel(layer->m_meshGPU);

		Mat4 WorldMatrix;
		m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::World, WorldMatrix);
		m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::WorldViewProjection,
			m_flyCamera->m_camera->m_projectionMatrix * m_flyCamera->m_camera->m_viewMatrix * WorldMatrix);

		for (u16 m = 0; m < YY_MDL_LAYER_NUM_OF_TEXTURES; ++m)
		{
			m_gpu->SetTexture(m, layer->m_textureGPU[m]);
		}

		m_gpu->Draw();
	}

	return g_demo->m_inputContext->isKeyHit(yyKey::K_ESCAPE) == false;
}