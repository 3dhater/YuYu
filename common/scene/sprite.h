#ifndef _YUYU_SCENE_SPRITE_H_
#define _YUYU_SCENE_SPRITE_H_

#include "containers/array.h"
#include "strings/string.h"

struct yySprite;

struct yySpriteState
{

	yyStringA m_name;
	
	yyArraySmall<v4f> m_frames; // координаты UV
	u16 m_frameCurrent = 0;
	u16 m_frameSize = 0;

	bool m_invertX = false;
	bool m_invertY = false;

	bool m_isAnimation = false;
	f32 m_fps = 1.f;
	f32 m_fpsTime = 1.f;
	void SetFPS(f32 newFPS)
	{
		if(newFPS < 1.f)
			newFPS = 1.f;
		m_fps = newFPS;
		m_fpsTime = 1.f / newFPS;
	}

	//v4f m_mainFrame; // возможно лишнее/ возможно при окончании анимации 
	                   //     можно будет поставить указанный фрейм если анимашка не зациклена

	// чтобы анимация проигрывалась нужно вызывать функцию обновления для спрайта, в главном цикле (как обычное обновление объекта).
	yySprite* m_sprite = nullptr;
	void SetMainFrame(int leftTopX, int leftTopY, int rightBottomX, int rightBottomY);
	void AddAnimationFrame(int leftTopX, int leftTopY, int rightBottomX, int rightBottomY);
};

void yySprite_update(void * impl);
struct yySprite
{
	yySprite()
	{
		m_objectBase.m_objectType = yySceneObjectBase::ObjectType::Sprite;
		m_objectBase.m_implementationPtr = this;
		m_objectBase.m_updateImplementation = yySprite_update; // наверно m_updateImplementation в принципе лишнее
	}
	~yySprite()
	{
		for(u16 i = 0, sz = m_states.size(); i < sz; ++i)
		{
			yyDestroy(m_states[i]);
		}
	}
	void Update(float dt)
	{
		if(m_currentState)
		{
			if(m_currentState->m_isAnimation)
			{
				static f32 timer = 0.f;
				timer += dt;
				if(timer >= m_currentState->m_fpsTime)
				{
					timer = 0.f;
					++m_currentState->m_frameCurrent;
					if(m_currentState->m_frameCurrent == m_currentState->m_frameSize)
						m_currentState->m_frameCurrent = 0;
					m_tcoords_1.x = m_currentState->m_frames[m_currentState->m_frameCurrent].x;
					m_tcoords_1.y = m_currentState->m_frames[m_currentState->m_frameCurrent].y;
					m_tcoords_2.x = m_currentState->m_frames[m_currentState->m_frameCurrent].z;
					m_tcoords_2.y = m_currentState->m_frames[m_currentState->m_frameCurrent].w;
				}
			}
		}
	}
	yySceneObjectBase m_objectBase;


	yySpriteState* m_currentState = nullptr;
	void SetState(yySpriteState* state)
	{
		m_currentState = state;
		if(m_currentState->m_isAnimation && m_currentState->m_frames.size())
		{
			m_tcoords_1.x = m_currentState->m_frames[0].x;
			m_tcoords_1.y = m_currentState->m_frames[0].y;
			m_tcoords_2.x = m_currentState->m_frames[0].z;
			m_tcoords_2.y = m_currentState->m_frames[0].w;
		}
	}



	yyArraySmall<yySpriteState*> m_states;
	yySpriteState* AddState(const char* name)
	{
		yySpriteState* newState = yyCreate<yySpriteState>();
		newState->m_name = name;
		newState->m_sprite = this;
		m_states.push_back(newState);
		return newState;
	}


	yyResource* m_texture = nullptr;
	yyResource* m_model   = nullptr;

	// если m_currentState не установлен, то будет работать на основе этого vvv
	// если установлен, то m_tcoords_1 и m_tcoords_2 должны вычислятся на основе 
	//   m_mainFrame или анимации из m_currentState

	// to shader - то что идёт в шейдер.
	v2f m_tcoords_1 = v2f(0.f, 0.f); // UV left bottom
	v2f m_tcoords_2 = v2f(1.f, 1.f); // UV right top

	v4f m_mainFrame; // при создании оно не играет роли. изменяется только в SetMainFrame. просто хранится значение
	void ResetMainFrame()
	{
		m_tcoords_1 = v2f(0.f, 0.f);
		m_tcoords_2 = v2f(1.f, 1.f);
	}
	// Установить область текстуры которая будет натянута на прямоугольник
	void SetMainFrame(int leftTopX, int leftTopY, int rightBottomX, int rightBottomY)
	{
		if(!m_texture)
			return;

		v2i textureSize;
		yyGetTextureSize(m_texture, &textureSize);

		f32 xMulFactor = 1.f / (f32)textureSize.x;
		f32 yMulFactor = 1.f / (f32)textureSize.y;

		m_mainFrame.x = (f32)leftTopX * xMulFactor;
		m_mainFrame.w = ((f32)(rightBottomY+1) * yMulFactor);

		m_mainFrame.z = (f32)(rightBottomX+1) * xMulFactor;
		m_mainFrame.y = ((f32)leftTopY * yMulFactor);
		

		m_tcoords_1.x = m_mainFrame.x;
		m_tcoords_1.y = m_mainFrame.y;
		m_tcoords_2.x = m_mainFrame.z;
		m_tcoords_2.y = m_mainFrame.w;
	}
};

YY_FORCE_INLINE void yySprite_update(void * impl)
{
	auto sprite = (yySprite*)impl;

	sprite->m_tcoords_1.x = sprite->m_mainFrame.x;
	sprite->m_tcoords_1.y = sprite->m_mainFrame.y;
	sprite->m_tcoords_2.x = sprite->m_mainFrame.z;
	sprite->m_tcoords_2.y = sprite->m_mainFrame.w;
}

YY_FORCE_INLINE void yySpriteState::SetMainFrame(int leftTopX, int leftTopY, int rightBottomX, int rightBottomY)
{
	if(!m_sprite->m_texture)
		return;

	v2i textureSize;
	yyGetTextureSize(m_sprite->m_texture, &textureSize);

	f32 xMulFactor = 1.f / (f32)textureSize.x;
	f32 yMulFactor = 1.f / (f32)textureSize.y;

	/*m_mainFrame.x = (f32)leftTopX * xMulFactor;
	m_mainFrame.w = ((f32)(rightBottomY+1) * yMulFactor);

	m_mainFrame.z = (f32)(rightBottomX+1) * xMulFactor;
	m_mainFrame.y = ((f32)leftTopY * yMulFactor);

	m_sprite->m_tcoords_1.x = m_mainFrame.x;
	m_sprite->m_tcoords_1.y = m_mainFrame.y;
	m_sprite->m_tcoords_2.x = m_mainFrame.z;
	m_sprite->m_tcoords_2.y = m_mainFrame.w;*/
	m_sprite->m_tcoords_1.x = (f32)leftTopX * xMulFactor;
	m_sprite->m_tcoords_1.y = ((f32)leftTopY * yMulFactor);
	m_sprite->m_tcoords_2.x = (f32)(rightBottomX+1) * xMulFactor;
	m_sprite->m_tcoords_2.y = ((f32)(rightBottomY+1) * yMulFactor);
}
YY_FORCE_INLINE void yySpriteState::AddAnimationFrame(int leftTopX, int leftTopY, int rightBottomX, int rightBottomY)
{
	if(!m_sprite->m_texture)
		return;

	v2i textureSize;
	yyGetTextureSize(m_sprite->m_texture, &textureSize);

	f32 xMulFactor = 1.f / (f32)textureSize.x;
	f32 yMulFactor = 1.f / (f32)textureSize.y;

	/*v4f newFrame;
	newFrame.x = (f32)leftTopX * xMulFactor;
	newFrame.w = ((f32)(rightBottomY+1) * yMulFactor);
	newFrame.z = (f32)(rightBottomX+1) * xMulFactor;
	newFrame.y = ((f32)leftTopY * yMulFactor);*/
	v4f newFrame;
	newFrame.x = (f32)leftTopX * xMulFactor;
	newFrame.y = ((f32)leftTopY * yMulFactor);
	newFrame.z = (f32)(rightBottomX+1) * xMulFactor;
	newFrame.w = ((f32)(rightBottomY+1) * yMulFactor);
	this->m_frames.push_back(newFrame);
	m_frameSize = m_frames.size();
}

#endif