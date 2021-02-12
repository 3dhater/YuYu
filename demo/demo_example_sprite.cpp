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
	m_spriteHero = 0;
	m_textureLevel = 0;

	for (u32 i = 0; i < 9; ++i)
	{
		m_spriteTest[i] = 0;
	}
}

DemoExample_Sprite::~DemoExample_Sprite()
{
	Shutdown();
}

bool DemoExample_Sprite::Init()
{
	m_textureLevel = m_gpu->CreateTextureFromFile("../res/GA3E/level1_ground.png", false, false, true);
	if (!m_textureLevel)
		return false;

	m_spriteLevel = yyCreateSprite(v4f(0.f, 0.f, 1160.f, 224.f), m_textureLevel, false);
//	m_spriteHero  = yyCreateSprite(v4f(0.f, 0.f, 1160.f, 224.f), m_textureLevel, false);
	
	m_spriteTest[0] = yyCreateSprite(v4f(0.f, 0.f, 64.f, 64.f), yyGetTextureResource("../res/image.dds", false, false, true));
	m_spriteTest[1] = yyCreateSprite(v4f(0.f, 0.f, 64.f, 64.f), yyGetTextureResource("../res/image.dds", false, false, true), 1);
	m_spriteTest[2] = yyCreateSprite(v4f(0.f, 0.f, 64.f, 64.f), yyGetTextureResource("../res/image.dds", false, false, true), 2);
	m_spriteTest[3] = yyCreateSprite(v4f(0.f, 0.f, 64.f, 64.f), yyGetTextureResource("../res/image.dds", false, false, true), 3);
	m_spriteTest[4] = yyCreateSprite(v4f(0.f, 0.f, 64.f, 64.f), yyGetTextureResource("../res/image.dds", false, false, true), 4);
	m_spriteTest[5] = yyCreateSprite(v4f(0.f, 0.f, 64.f, 64.f), yyGetTextureResource("../res/image.dds", false, false, true), 5);
	m_spriteTest[6] = yyCreateSprite(v4f(0.f, 0.f, 64.f, 64.f), yyGetTextureResource("../res/image.dds", false, false, true), 6);
	m_spriteTest[7] = yyCreateSprite(v4f(0.f, 0.f, 64.f, 64.f), yyGetTextureResource("../res/image.dds", false, false, true), 7);
	m_spriteTest[8] = yyCreateSprite(v4f(0.f, 0.f, 64.f, 64.f), yyGetTextureResource("../res/image.dds", false, false, true), 8);
	m_spriteTest[0]->m_objectBase.m_localPosition.set(0.f, 0.f, 0.f, 0.f);
	m_spriteTest[1]->m_objectBase.m_localPosition.set(100.f, 0.f, 0.f, 0.f);
	m_spriteTest[2]->m_objectBase.m_localPosition.set(200.f, 0.f, 0.f, 0.f);
	m_spriteTest[3]->m_objectBase.m_localPosition.set(300.f, 0.f, 0.f, 0.f);
	m_spriteTest[4]->m_objectBase.m_localPosition.set(400.f, 0.f, 0.f, 0.f);
	m_spriteTest[5]->m_objectBase.m_localPosition.set(500.f, 0.f, 0.f, 0.f);
	m_spriteTest[6]->m_objectBase.m_localPosition.set(600.f, 0.f, 0.f, 0.f);
	m_spriteTest[7]->m_objectBase.m_localPosition.set(700.f, 0.f, 0.f, 0.f);
	m_spriteTest[8]->m_objectBase.m_localPosition.set(800.f, 0.f, 0.f, 0.f);

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
	if (m_textureLevel)
	{
		m_gpu->DeleteTexture(m_textureLevel);
		m_textureLevel = 0;
	}
	if (m_spriteLevel)
	{
		yyDestroy(m_spriteLevel);
		m_spriteLevel = 0;
	}
	for (u32 i = 0; i < 9; ++i)
	{
		if (m_spriteTest[i])
		{
			yyDestroy(m_spriteTest[i]);
			m_spriteTest[i] = 0;
		}
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

bool DemoExample_Sprite::DemoStep(f32 deltaTime)
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

	for (u32 i = 0; i < 9; ++i)
	{
		m_spriteTest[i]->m_objectBase.SetRotation(m_spriteTest[i]->m_objectBase.m_rotation + v4f(0.f, 0.f, 1.f * deltaTime, 0.f));
		m_spriteTest[i]->Update(deltaTime);
		m_spriteTest[i]->m_objectBase.UpdateBase();
		m_gpu->DrawSprite(m_spriteTest[i]);
	}

	return g_demo->m_inputContext->isKeyHit(yyKey::K_ESCAPE) == false;
}