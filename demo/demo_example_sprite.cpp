#include "demo.h"
#include "demo_example.h"
#include "demo_example_sprite.h"

#include "scene\common.h"
#include "scene\sprite.h"
#include "yy_input.h"

extern Demo* g_demo;

DemoExample_Sprite::DemoExample_Sprite()
{
	m_spriteLevel = 0;
}

DemoExample_Sprite::~DemoExample_Sprite()
{
	Shutdown();
}

bool DemoExample_Sprite::Init()
{

	auto texture = m_gpu->CreateTextureFromFile("../res/GA3E/level1_ground.png", false, false, true);
	if (!texture)
		return false;

	m_spriteLevel = yyCreateSprite(v4f(0.f, 0.f, 1160.f, 224.f), texture, false);
	
	m_spriteCameraPosition = m_gpu->GetSpriteCameraPosition();
	m_spriteCameraPosition->x = 280.f;
	m_spriteCameraPosition->y = 35.f;

	m_spriteCameraScale = m_gpu->GetSpriteCameraScale();
	m_spriteCameraScale->x = 1.7f;
	m_spriteCameraScale->y = 1.7f;

	return true;
}

void DemoExample_Sprite::Shutdown()
{
	if (m_spriteLevel)
	{
		yyDestroy(m_spriteLevel);
		m_spriteLevel = 0;
	}
}

const wchar_t* DemoExample_Sprite::GetTitle()
{
	return L"Sprites";
}

const wchar_t* DemoExample_Sprite::GetDescription()
{
	return L"Everything about yySprite";
}

void DemoExample_Sprite::DemoStep(f32 deltaTime)
{
	f32  spriteCameraMoveSpeed = 50.f;
	f32  heroMoveSpeed = 70.f;
	f32  spriteCameraScaleSpeed = 1.f;
	if (g_demo->m_inputContext->isKeyHold(yyKey::K_NUM_4))
	{
		m_spriteCameraPosition->x -= spriteCameraMoveSpeed * deltaTime;
		m_spriteCameraPosition->x = std::floor(m_spriteCameraPosition->x);
	}
	if (g_demo->m_inputContext->isKeyHold(yyKey::K_NUM_6))
	{
		m_spriteCameraPosition->x += spriteCameraMoveSpeed * deltaTime;
		m_spriteCameraPosition->x = std::ceil(m_spriteCameraPosition->x);
	}
	if (g_demo->m_inputContext->isKeyHold(yyKey::K_NUM_8))
	{
		m_spriteCameraPosition->y -= spriteCameraMoveSpeed * deltaTime;
		m_spriteCameraPosition->y = std::floor(m_spriteCameraPosition->y);
	}
	if (g_demo->m_inputContext->isKeyHold(yyKey::K_NUM_2))
	{
		m_spriteCameraPosition->y += spriteCameraMoveSpeed * deltaTime;
		m_spriteCameraPosition->y = std::ceil(m_spriteCameraPosition->y);
	}
	if (g_demo->m_inputContext->isKeyHold(yyKey::K_NUM_7))
		m_spriteCameraScale->x -= spriteCameraScaleSpeed * deltaTime;
	if (g_demo->m_inputContext->isKeyHold(yyKey::K_NUM_9))
		m_spriteCameraScale->x += spriteCameraScaleSpeed * deltaTime;
	if (g_demo->m_inputContext->isKeyHold(yyKey::K_NUM_1))
		m_spriteCameraScale->y -= spriteCameraScaleSpeed * deltaTime;
	if (g_demo->m_inputContext->isKeyHold(yyKey::K_NUM_3))
		m_spriteCameraScale->y += spriteCameraScaleSpeed * deltaTime;


	m_gpu->UseDepth(false);
	m_gpu->DrawSprite(m_spriteLevel);
}