﻿#ifndef _YUYU_SCENE_SPRITE_H_
#define _YUYU_SCENE_SPRITE_H_

#include "containers/array.h"
#include "strings/string.h"

struct yySprite;

struct yySpriteState
{
	yySpriteState()
		:
		m_frameCurrent(0),
		m_frameNum(0),
		m_invertX(false),
		m_invertY(false),
		m_isAnimation(false),
		m_fps(1.f),
		m_fpsTime(1.f),
		m_sprite(nullptr)
	{}

	yyStringA m_name;
	
	yyArraySmall<v4f> m_frames; // координаты UV
	u16 m_frameCurrent;
	u16 m_frameNum;

	bool m_invertX;
	bool m_invertY;

	bool m_isAnimation;
	f32 m_fps;
	f32 m_fpsTime;

	void SetFPS(f32 newFPS){
		if(newFPS < 1.f)
			newFPS = 1.f;
		m_fps = newFPS;
		m_fpsTime = 1.f / newFPS;
	}

	//v4f m_mainFrame; // возможно лишнее/ возможно при окончании анимации 
	                   //     можно будет поставить указанный фрейм если анимашка не зациклена

	// чтобы анимация проигрывалась нужно вызывать функцию обновления для спрайта, в главном цикле (как обычное обновление объекта).
	yySprite* m_sprite;
	void SetMainFrame(int leftTopX, int leftTopY, int rightBottomX, int rightBottomY);
	void AddAnimationFrame(int leftTopX, int leftTopY, int rightBottomX, int rightBottomY);
};

void yySprite_update(void * impl);
struct yySprite
{
	yySprite()
		:
		m_currentState(nullptr),
		m_texture(nullptr),
		m_model(nullptr),
		m_tcoords_1(v2f(0.f, 0.f)),
		m_tcoords_2(v2f(1.f, 1.f))
	{
		m_updateTimer = 0.f;
		m_play = true;
		m_loop = true;
		m_onEndAnimation = 0;
		m_userData = 0;
		m_objectBase.m_objectType = yySceneObjectBase::ObjectType::Sprite;
		m_objectBase.m_implementationPtr = this;
		m_objectBase.m_updateImplementation = yySprite_update; // наверно m_updateImplementation в принципе лишнее
		m_useAsBillboard = false;
	}

	bool m_useAsBillboard;

	~yySprite(){
#ifdef YY_DEBUG
		yyLogWriteInfo("%s\n",YY_FUNCTION);
#endif
		for(u16 i = 0, sz = m_states.size(); i < sz; ++i)
		{
			yyDestroy(m_states[i]);
		}
		if (m_model)
			yyMegaAllocator::Destroy(m_model);
		if (m_texture)
		{
			if (m_texture->IsFromCache())
			{
				m_texture->Unload();
				if (!m_texture->IsLoaded())
				{
					yyRemoveTextureFromCache(m_texture);
					yyMegaAllocator::Destroy(m_texture);
				}
			}
			else
			{
				yyMegaAllocator::Destroy(m_texture);
			}
		}
	}

	void _updateUVCoords() {
		m_tcoords_1.x = m_currentState->m_frames[m_currentState->m_frameCurrent].x;
		m_tcoords_1.y = m_currentState->m_frames[m_currentState->m_frameCurrent].y;
		m_tcoords_2.x = m_currentState->m_frames[m_currentState->m_frameCurrent].z;
		m_tcoords_2.y = m_currentState->m_frames[m_currentState->m_frameCurrent].w;
	}

	f32 m_updateTimer;

	void Update(float dt){
		if (!m_currentState) return;
		if(m_currentState->m_isAnimation && m_play)
		{
			m_updateTimer += dt;
			if(m_updateTimer >= m_currentState->m_fpsTime)
			{
				m_updateTimer = 0.f;
				++m_currentState->m_frameCurrent;
				if (m_currentState->m_frameCurrent == m_currentState->m_frameNum)
				{
					if (m_loop)
					{
						m_currentState->m_frameCurrent = 0;
					}
					else
					{
						m_play = false;
					}
				}
				_updateUVCoords();

				if (m_onEndAnimation)
				{
					m_onEndAnimation(m_userData);
				}
			}
		}
	}

	bool m_play;
	bool m_loop;

	void PlayAnimation() {
		m_play = true;
	}

	void PauseAnimation() {
		m_play = false;
	}

	void StopAnimation() {
		m_play = false;
		m_updateTimer = 0.f;
		if (!m_currentState) return;
		m_currentState->m_frameCurrent = 0;
		_updateUVCoords();
	}

	bool IsLoop() { return m_loop; }
	void SetLoop(bool l) { m_loop = l; }

	void* m_userData;
	void(*m_onEndAnimation)(void*);
	void SetUserData(void* data) { m_userData = data; }

	yySceneObjectBase m_objectBase;
	yySpriteState* m_currentState;

	void SetState(yySpriteState* state){
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

	yySpriteState* AddState(const char* name){
		yySpriteState* newState = yyCreate<yySpriteState>();
		if(name)
			newState->m_name = name;
		newState->m_sprite = this;
		m_states.push_back(newState);
		return newState;
	}


	yyResource* m_texture;
	yyResource* m_model;

	v4f m_creationRect;

	// если m_currentState не установлен, то будет работать на основе этого vvv
	// если установлен, то m_tcoords_1 и m_tcoords_2 должны вычислятся на основе 
	//   m_mainFrame или анимации из m_currentState

	// to shader - то что идёт в шейдер.
	v2f m_tcoords_1; // UV left bottom
	v2f m_tcoords_2; // UV right top

	v4f m_mainFrame; // при создании оно не играет роли. изменяется только в SetMainFrame. просто хранится значение

	void ResetMainFrame(){
		m_tcoords_1 = v2f(0.f, 0.f);
		m_tcoords_2 = v2f(1.f, 1.f);
	}

	// Установить область текстуры которая будет натянута на прямоугольник
	void SetMainFrame(int leftTopX, int leftTopY, int rightBottomX, int rightBottomY){
		if(!m_texture)
			return;

		v2f textureSize;
		m_texture->GetTextureSize(&textureSize);

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

	bool RayTest(const yyRay& ray, f32 * T ) {
		{
			v3f v1(m_creationRect.x, m_creationRect.y, 0.f);
			v3f v2(m_creationRect.z, m_creationRect.y, 0.f);
			v3f v3(m_creationRect.z, m_creationRect.w, 0.f);

			v1 = math::mulBasis(v1, m_objectBase.m_globalMatrix) + m_objectBase.m_globalMatrix.m_data[3];
			v2 = math::mulBasis(v2, m_objectBase.m_globalMatrix) + m_objectBase.m_globalMatrix.m_data[3];
			v3 = math::mulBasis(v3, m_objectBase.m_globalMatrix) + m_objectBase.m_globalMatrix.m_data[3];

			v4f e1 = v4f(
				v2.x - v1.x,
				v2.y - v1.y,
				v2.z - v1.z,
				0.f);
			v4f e2 = v4f(
				v3.x - v1.x,
				v3.y - v1.y,
				v3.z - v1.z,
				0.f);
			v4f pvec;
			ray.m_direction.cross2(e2, pvec);
			f32 det = e1.dot(pvec);

			bool withBackFace = true;
			if (withBackFace)
			{
				if (std::fabs(det) < Epsilon)
					goto second;
			}
			else
			{
				if (det < Epsilon && det > -Epsilon)
					goto second;
			}

			v4f tvec(
				ray.m_origin.x - v1.x,
				ray.m_origin.y - v1.y,
				ray.m_origin.z - v1.z,
				0.f);

			f32 inv_det = 1.f / det;
			auto U = tvec.dot(pvec) * inv_det;

			if (U < 0.f || U > 1.f)
				goto second;

			v4f  qvec;
			tvec.cross2(e1, qvec);
			auto V = ray.m_direction.dot(qvec) * inv_det;

			if (V < 0.f || U + V > 1.f)
				goto second;

			// T is length from origin to intersection point
			*T = e2.dot(qvec) * inv_det;

			if (*T < Epsilon) 
				goto second;

			auto W = 1.f - U - V;

			return true;
		}
		second:
		{
			v3f v1(m_creationRect.x, m_creationRect.y, 0.f);
			v3f v2(m_creationRect.z, m_creationRect.w, 0.f);
			v3f v3(m_creationRect.x, m_creationRect.w, 0.f);

			v1 = math::mulBasis(v1, m_objectBase.m_globalMatrix) + m_objectBase.m_globalMatrix.m_data[3];
			v2 = math::mulBasis(v2, m_objectBase.m_globalMatrix) + m_objectBase.m_globalMatrix.m_data[3];
			v3 = math::mulBasis(v3, m_objectBase.m_globalMatrix) + m_objectBase.m_globalMatrix.m_data[3];

			v4f e1 = v4f(
				v2.x - v1.x,
				v2.y - v1.y,
				v2.z - v1.z,
				0.f);
			v4f e2 = v4f(
				v3.x - v1.x,
				v3.y - v1.y,
				v3.z - v1.z,
				0.f);
			v4f pvec;
			ray.m_direction.cross2(e2, pvec);
			f32 det = e1.dot(pvec);

			bool withBackFace = true;
			if (withBackFace)
			{
				if (std::fabs(det) < Epsilon)
					return false;
			}
			else
			{
				if (det < Epsilon && det > -Epsilon)
					return false;
			}

			v4f tvec(
				ray.m_origin.x - v1.x,
				ray.m_origin.y - v1.y,
				ray.m_origin.z - v1.z,
				0.f);

			f32 inv_det = 1.f / det;
			auto U = tvec.dot(pvec) * inv_det;

			if (U < 0.f || U > 1.f)
				return false;

			v4f  qvec;
			tvec.cross2(e1, qvec);
			auto V = ray.m_direction.dot(qvec) * inv_det;

			if (V < 0.f || U + V > 1.f)
				return false;

			// T is length from origin to intersection point
			*T = e2.dot(qvec) * inv_det;

			if (*T < Epsilon) return false;

			auto W = 1.f - U - V;

			return true;
		}
		return false;
	}
};

YY_FORCE_INLINE void yySprite_update(void * impl){
	auto sprite = (yySprite*)impl;
	sprite->m_tcoords_1.x = sprite->m_mainFrame.x;
	sprite->m_tcoords_1.y = sprite->m_mainFrame.y;
	sprite->m_tcoords_2.x = sprite->m_mainFrame.z;
	sprite->m_tcoords_2.y = sprite->m_mainFrame.w;
}

YY_FORCE_INLINE void yySpriteState::SetMainFrame(int leftTopX, int leftTopY, int rightBottomX, int rightBottomY){
	if(!m_sprite->m_texture)
		return;

	v2f textureSize;
	m_sprite->m_texture->GetTextureSize(&textureSize);

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

YY_FORCE_INLINE void yySpriteState::AddAnimationFrame(int leftTopX, int leftTopY, int rightBottomX, int rightBottomY){
	if(!m_sprite->m_texture)
		return;
	
	v2f textureSize;
	m_sprite->m_texture->GetTextureSize(&textureSize);

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
	m_frameNum = m_frames.size();
}

#endif