#ifndef _YUYU_SCENE_SPRITE2_H_
#define _YUYU_SCENE_SPRITE2_H_

#include "containers/array.h"
#include "strings/string.h"
#include "yy_model.h"

YY_FORCE_INLINE
v4f yySpriteGetNewPivot(const v4f& r, u8 pivotPosition)
{
	assert(pivotPosition < 9);
	f32 half_w = (r.z - r.x) * 0.5f;
	f32 half_h = (r.w - r.y) * 0.5f;

	auto rect = r;
	switch (pivotPosition)
	{
	case 1:
		rect = v4f(
			rect.x - half_w,
			rect.y,
			rect.z - half_w,
			rect.w
		);
		break;
	case 2:
		rect = v4f(
			rect.x - half_w - half_w,
			rect.y,
			rect.z - half_w - half_w,
			rect.w
		);
		break;
	case 3:
		rect = v4f(
			rect.x - half_w - half_w,
			rect.y - half_h,
			rect.z - half_w - half_w,
			rect.w - half_h
		);
		break;
	case 4:
		rect = v4f(
			rect.x - half_w - half_w,
			rect.y - half_h - half_h,
			rect.z - half_w - half_w,
			rect.w - half_h - half_h
		);
		break;
	case 5:
		rect = v4f(
			rect.x - half_w,
			rect.y - half_h - half_h,
			rect.z - half_w,
			rect.w - half_h - half_h
		);
		break;
	case 6:
		rect = v4f(
			rect.x,
			rect.y - half_h - half_h,
			rect.z,
			rect.w - half_h - half_h
		);
		break;
	case 7:
		rect = v4f(
			rect.x,
			rect.y - half_h,
			rect.z,
			rect.w - half_h
		);
		break;
	case 8:
		rect = v4f(
			rect.x - half_w,
			rect.y - half_h,
			rect.z - half_w,
			rect.w - half_h
		);
		break;
	}
	return rect;
}

YY_FORCE_INLINE
yyModel* yySpriteCreateNew(const v4f& rect, const v2f& UV_leftBottom, const v2f& UV_rightTop)
{
	auto model = yyCreate<yyModel>();
	model->m_iCount = 6;
	model->m_vCount = 4;
	model->m_stride = sizeof(yyVertexGUI);
	model->m_vertexType = yyVertexType::GUI;
	model->m_vertices = (u8*)yyMemAlloc(model->m_vCount * model->m_stride);
	model->m_indices = (u8*)yyMemAlloc(model->m_iCount * sizeof(u16));
	u16* inds = (u16*)model->m_indices;

	yyVertexGUI * vertex = (yyVertexGUI*)model->m_vertices;
	vertex->m_position.set(rect.x, rect.w);
	vertex->m_tcoords.set(UV_leftBottom.x, UV_rightTop.y);
	vertex++;
	vertex->m_position.set(rect.x, rect.y);
	vertex->m_tcoords.set(UV_leftBottom.x, UV_leftBottom.y);
	vertex++;
	vertex->m_position.set(rect.z, rect.y);
	vertex->m_tcoords.set(UV_rightTop.x, UV_leftBottom.y);
	vertex++;
	vertex->m_position.set(rect.z, rect.w);
	vertex->m_tcoords.set(UV_rightTop.x, UV_rightTop.y);
	vertex++;

	inds[0] = 0;
	inds[1] = 1;
	inds[2] = 2;
	inds[3] = 0;
	inds[4] = 2;
	inds[5] = 3;
	return model;
}

/* Данный тип спрайта состоит из множества мешбуферов
    то есть каждый кадр это не новые текстурные координаты,
	а отдельная модель.
	Так можно будет делать такие спрайты, чьи кадры разного размера
*/

struct yySprite2;

struct yySprite2State
{
	yySprite2State()
	{
		m_isAnimation = false;
		m_fps = 1.f;
		m_fpsTime = 1.f;
		m_activeFrameGPU = 0;
		m_activeFrameCPU = 0;
		m_frameCurrent = 0;
		m_frameNum = 0;
	}
	~yySprite2State()
	{
		auto gpu = yyGetVideoDriverAPI();
		for(u16 i = 0, sz = m_framesGPU.size(); i < sz; ++i)
		{
			yyDestroy(m_framesGPU[i]);
		}

		DeleteFramesCPU();
	}
	
	yyArraySmall<yyResource*> m_framesGPU;
	yyArraySmall<yyModel*> m_framesCPU;
	yyResource* m_activeFrameGPU;
	yyModel* m_activeFrameCPU;

	void DeleteFramesCPU()
	{
		for (u16 i = 0, sz = m_framesCPU.size(); i < sz; ++i)
		{
			yyDestroy(m_framesCPU[i]);
		}
		m_framesCPU.clear();
		m_activeFrameCPU = 0;
	}

	yyStringA m_name;
	bool m_isAnimation;
	f32 m_fps;
	f32 m_fpsTime;
	u16 m_frameCurrent;
	u16 m_frameNum;

	void SetFPS(f32 newFPS)
	{
		if(newFPS < 1.f)
			newFPS = 1.f;
		m_fps = newFPS;
		m_fpsTime = 1.f / newFPS;
	}
};

void yySprite2_update(void * impl);
struct yySprite2
{
	yySprite2()	{
		m_objectBase.m_objectType = yySceneObjectBase::ObjectType::Sprite2;
		m_objectBase.m_implementationPtr = this;
		m_objectBase.m_updateImplementation = yySprite2_update; // наверно m_updateImplementation в принципе лишнее
		m_currentState = 0;
		m_texture = 0;
	}
	~yySprite2()	{
#ifdef YY_DEBUG
		yyLogWriteInfo("%s\n", YY_FUNCTION);
#endif
		for(u16 i = 0, sz = m_states.size(); i < sz; ++i)
		{
			yyDestroy(m_states[i]);
		}
		if (m_texture)
		{
			if (m_texture->IsFromCache())
			{
				m_texture->Unload();
				if (!m_texture->IsLoaded())
				{
					yyRemoveTextureFromCache(m_texture);
					yyDestroy(m_texture);
				}
			}
			else
			{
				yyDestroy(m_texture);
			}
		}
	}

	yySceneObjectBase m_objectBase;
	yySprite2State* m_currentState;

	void Update(float dt)	{
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
					if(m_currentState->m_frameCurrent == m_currentState->m_frameNum)
						m_currentState->m_frameCurrent = 0;
					
					if (m_currentState->m_framesCPU.size())
					{
						m_currentState->m_activeFrameCPU = m_currentState->m_framesCPU[m_currentState->m_frameCurrent];
					}
				
					m_currentState->m_activeFrameGPU = m_currentState->m_framesGPU[m_currentState->m_frameCurrent];
				}
			}
		}
	}
	yyArraySmall<yySprite2State*> m_states;

	void SetState(yySprite2State* state)
	{
		m_currentState = state;
	}
	
	yyResource* m_texture;

	// При создании ничего не будет рисоваться
	// Нужно вручную инициализировать спрайт
	//  ибо непонятно, нужна анимация или нет
	void _initFrame(yySprite2State* newState, 
		const v4f& meshr,
		const v4f& uvr,
		bool invertX,
		bool invertY)
	{
		v2f textureSize;
		m_texture->GetTextureSize(&textureSize);

		f32 xMulFactor = 1.f / (f32)textureSize.x;
		f32 yMulFactor = 1.f / (f32)textureSize.y;

		v2f UV_leftBottom;
		v2f UV_rightTop;

		UV_leftBottom.x = (f32)uvr.x * xMulFactor;
		UV_leftBottom.y = ((f32)uvr.y* yMulFactor);
		UV_rightTop.x = (f32)(uvr.z + 1) * xMulFactor;
		UV_rightTop.y = ((f32)(uvr.w + 1) * yMulFactor);

		if (invertX)
		{
			auto tmp = UV_leftBottom.x;
			UV_leftBottom.x = UV_rightTop.x;
			UV_rightTop.x = tmp;
		}

		if (invertY)
		{
			auto tmp = UV_leftBottom.y;
			UV_leftBottom.y = UV_rightTop.y;
			UV_rightTop.y = tmp;
		}

		yyModel* mesh = yySpriteCreateNew(meshr, UV_leftBottom, UV_rightTop);

		newState->m_framesCPU.push_back(mesh);
		newState->m_activeFrameCPU = mesh;

		yyResource* gpuMesh = yyCreateModel(mesh);
		gpuMesh->Load();
		newState->m_framesGPU.push_back(gpuMesh);
		newState->m_activeFrameGPU = gpuMesh;
	}
	yySprite2State* AddStateSingleFrame(const char* name, const v4f& uvr, u8 pivotPosition, 
		bool invertX = false,
		bool invertY = false)
	{
		assert(m_texture);

		yySprite2State* newState = yyCreate<yySprite2State>();
		newState->m_name = name;
		auto _r = uvr;
		_r.z = _r.z - _r.x;
		_r.w = _r.w - _r.y;
		_r.x = 0.f;
		_r.y = 0.f;
		auto meshr = yySpriteGetNewPivot(_r, pivotPosition);
		_initFrame(newState, meshr, uvr, invertX, invertY);
		m_states.push_back(newState);
		return newState;
	}
	yySprite2State* AddStateAnimation(
		const char* name, 
		v4f* frames,
		v2f* frameOffsets,
		s32 frameNum,
		u8 pivotPosition,
		bool invertX = false,
		bool invertY = false)
	{
		assert(m_texture);

		yySprite2State* newState = yyCreate<yySprite2State>();
		newState->m_name = name;
		newState->m_frameNum = frameNum;
		newState->m_isAnimation = true;
		for (s32 i = 0; i < frameNum; ++i)
		{
			auto _r = frames[i];
			_r.z = _r.z - _r.x;
			_r.w = _r.w - _r.y;
			_r.x = 0.f;
			_r.y = 0.f;

			auto meshr = yySpriteGetNewPivot(_r, pivotPosition);

			meshr.x += frameOffsets[i].x;
			meshr.z += frameOffsets[i].x;
			meshr.y += frameOffsets[i].y;
			meshr.w += frameOffsets[i].y;

			_initFrame(newState, meshr, frames[i], invertX, invertY);
		}

		m_states.push_back(newState);
		return newState;
	}

	void DeleteFramesCPU()
	{
		for (u16 i = 0, sz = m_states.size(); i < sz; ++i)
		{
			m_states[i]->DeleteFramesCPU();
		}
	}
};

YY_FORCE_INLINE 
void yySprite2_update(void * impl)
{
	/*auto sprite = (yySprite*)impl;
	sprite->m_tcoords_1.x = sprite->m_mainFrame.x;
	sprite->m_tcoords_1.y = sprite->m_mainFrame.y;
	sprite->m_tcoords_2.x = sprite->m_mainFrame.z;
	sprite->m_tcoords_2.y = sprite->m_mainFrame.w;*/
}

#endif