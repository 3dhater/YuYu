﻿#include "yy.h"
#include "yy_gui.h"
#include "math/vec.h"
#include "math/math.h"

#include "yy_model.h"
#include "yy_input.h"

#include "../engine.h"

extern Engine * g_engine;

yyGUIButton::yyGUIButton()
	:
	m_basePB(nullptr),
	m_mouseHoverPB(0),
	m_mouseClickPB(0),
	m_onRelease(0),
	m_onClick(nullptr)
{
	m_type = yyGUIElementType::Button;
	m_isInActiveAreaRect = false;
	m_isClicked = false;
	m_isChecked = false;
	m_useAsCheckbox = false;
	m_isAnimated = false;
	m_gpu = yyGetVideoDriverAPI();
	m_baseTexture = 0;
	m_onMouseEnter = 0;
	m_onMouseLeave = 0;
}

yyGUIButton::~yyGUIButton(){
	if (m_basePB) yyDestroy(m_basePB);
	if (m_mouseHoverPB) yyDestroy(m_mouseHoverPB);
	if (m_mouseClickPB) yyDestroy(m_mouseClickPB);
}

void yyGUIButton::SetOffset(const v2f& o){
	m_offset = o;
	if (m_mouseClickPB) m_mouseClickPB->m_offset = o;
	if (m_mouseHoverPB) m_mouseHoverPB->m_offset = o;
	if (m_basePB) m_basePB->m_offset = o;
}

void yyGUIButton::SetOpacity(f32 v, s32 pictureBox) {
	switch (pictureBox)
	{
	default:
	case 0:
		m_basePB->m_color.m_data[3] = v;
		m_color.m_data[3] = v;
		break;
	case 1:
		if (m_mouseHoverPB) m_mouseHoverPB->m_color.m_data[3] = v;
		break;
	case 2:
		if (m_mouseClickPB) m_mouseClickPB->m_color.m_data[3] = v;
		break;
	}
}

void yyGUIButton::SetColor(const yyColor& c) {
	m_basePB->m_color = c;
	m_color = c;
}

void yyGUIButton::OnUpdate(f32 dt){
	if (!m_visible) return;

	bool mouseInRectBefore = m_isInActiveAreaRect;

	yyGUIElement::CheckCursorInRect();

	if (m_isInActiveAreaRect && !mouseInRectBefore)
	{
		if (m_onMouseEnter)
			m_onMouseEnter(this, m_id);
	}

	if (!m_isInActiveAreaRect && mouseInRectBefore)
	{
		if (m_onMouseLeave)
			m_onMouseLeave(this, m_id);
	}

	if (m_ignoreInput) return;


	if (m_isAnimated && m_mouseHoverPB)
	{
		f32 sp = 10.f  * dt;
		if (m_isInActiveAreaRect)
		{
			m_mouseHoverPB->m_color.m_data[3] += sp;
			if (m_mouseHoverPB->m_color.m_data[3] > 1.f)
				m_mouseHoverPB->m_color.m_data[3] = 1.f;
		}
		else
		{
			m_mouseHoverPB->m_color.m_data[3] -= sp;
			if (m_mouseHoverPB->m_color.m_data[3] < 0.f)
				m_mouseHoverPB->m_color.m_data[3] = 0.f;
		}
	}

	if (g_engine->m_inputContext->m_isLMBUp)
	{
		if (!m_useAsCheckbox)
		{
			m_isClicked = false;
		}


		if (m_onRelease && m_isInActiveAreaRect
			&& (g_engine->m_guiElementInMouseFocus == (yyGUIElement*)this))
			m_onRelease(this, m_id);
		
	}

	if (g_engine->m_inputContext->m_isLMBDown)
	{
		if (m_isInActiveAreaRect)
		{
			if (m_useAsCheckbox)
			{
				if (m_isChecked)
				{
					m_isChecked = false;
				}
				else
				{
					m_isChecked = true;
				}
			}
			else
			{
				m_isClicked = true;
			}

			if(m_onClick)
				m_onClick(this, m_id);
			g_engine->m_guiElementInMouseFocus = this;
		}
	}
}

void yyGUIButton::OnDraw(){
	if (!m_visible) return;
	
	if(m_isAnimated)
		m_basePB->OnDraw();

	if (m_isClicked || m_isChecked)
	{
		if (m_mouseClickPB)
		{
			m_mouseClickPB->OnDraw();
		}
		else if(m_mouseHoverPB)
		{
			m_mouseHoverPB->OnDraw();
		}
		else
		{
			m_basePB->OnDraw();
		}
	}
	else
	{
		m_basePB->OnDraw();
		if (m_mouseHoverPB)
		{
			if (m_isInActiveAreaRect || (m_isAnimated && m_mouseHoverPB->m_color.m_data[3] > 0.f))
			{
				m_mouseHoverPB->OnDraw();
			}
			else
			{
				m_basePB->OnDraw();
			}
		}
	}
}

void yyGUIButton::Rebuild(){
	yyGUIElement::CallOnRebuildSetRects();
	if (m_basePB)
	{
		m_baseTexture = m_basePB->DropTexture();
		m_color = m_basePB->m_color;
		yyDestroy(m_basePB);
	}
	m_basePB = yyGUICreatePictureBox(m_buildingRect_global, m_baseTexture, m_id, m_drawGroup, &m_uvRect);
	yyGUIRemoveElement(m_basePB);
	m_basePB->IgnoreInput(true);
	m_basePB->m_color = m_color;

	if (m_mouseHoverPB)
	{
		auto t = m_mouseHoverPB->DropTexture();
		auto uvRect = m_mouseHoverPB->m_uvRect;
		auto color = m_mouseHoverPB->m_color;
		yyDestroy(m_mouseHoverPB);
		m_mouseHoverPB = yyGUICreatePictureBox(m_buildingRect_global, t, -1, m_drawGroup, &uvRect);
		m_mouseHoverPB->IgnoreInput(true);
		m_mouseHoverPB->m_color = color;
		yyGUIRemoveElement(m_mouseHoverPB);
	}

	if (m_mouseClickPB)
	{
		auto t = m_mouseClickPB->DropTexture();
		auto uvRect = m_mouseClickPB->m_uvRect;
		auto color = m_mouseClickPB->m_color;
		yyDestroy(m_mouseClickPB);
		m_mouseClickPB = yyGUICreatePictureBox(m_buildingRect_global, t, -1, m_drawGroup, &uvRect);
		m_mouseClickPB->IgnoreInput(true);
		m_mouseClickPB->m_color = color;
		yyGUIRemoveElement(m_mouseClickPB);
	}
}

YY_API yyGUIButton* YY_C_DECL yyGUICreateButton(const v4f& rect, yyResource* baseTexture, s32 id, yyGUIDrawGroup* drawGroup, v4i* uv){
	assert(baseTexture);
	yyGUIButton* element = yyCreate<yyGUIButton>();
	element->SetDrawGroup(drawGroup);
	element->m_activeAreaRect = rect;
	element->m_clipRect = rect;
	element->m_buildingRect = rect;
	element->m_activeAreaRect_global = element->m_activeAreaRect;
	element->m_clipRect_global = element->m_clipRect;
	element->m_buildingRect_global = element->m_buildingRect;
	element->m_id = id;
	element->m_baseTexture = baseTexture;
	if (uv)
	{
		element->m_uvRect = *uv;
	}
	else
	{
		auto gpu = yyGetVideoDriverAPI();
		v2i s;
		gpu->GetTextureSize(baseTexture, &s);
		element->m_uvRect.z = s.x;
		element->m_uvRect.w = s.y;
	}

	element->Rebuild();
	//element->m_basePB = yyGUICreatePictureBox(element->m_buildingRect_global, baseTexture, -1, element->m_drawGroup);

	element->m_basePB->IgnoreInput(true);

	yyGUIRemoveElement(element->m_basePB);
	
	return element;
}

void yyGUIButton::SetVisible(bool v) {
	m_visible = v; 
}

void yyGUIButton::SetMouseHoverTexture(yyResource* t, v4i* uv){
	if (!m_mouseHoverPB)
	{
		m_mouseHoverPB = yyGUICreatePictureBox(m_buildingRect, t, -1, m_drawGroup, uv);
		m_mouseHoverPB->IgnoreInput(true);
		yyGUIRemoveElement(m_mouseHoverPB);
	}
	else
	{
		m_mouseHoverPB->m_texture = t;
	}
}

void yyGUIButton::SetMouseClickTexture(yyResource* t, v4i* uv){
	if (!m_mouseClickPB)
	{
		m_mouseClickPB = yyGUICreatePictureBox(m_buildingRect, t, -1, m_drawGroup, uv);
		m_mouseClickPB->IgnoreInput(true);
		yyGUIRemoveElement(m_mouseClickPB);
	}
	else
	{
		m_mouseClickPB->m_texture = t;
	}
}
