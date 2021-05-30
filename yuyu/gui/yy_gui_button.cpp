#include "yy.h"
#include "yy_gui.h"
#include "math/vec.h"
#include "math/math.h"

#include "yy_model.h"
#include "yy_input.h"

#include "../engine.h"

extern yyEngine * g_engine;

yyGUIButton::yyGUIButton()
	:
	m_basePB(nullptr),
	m_mouseHoverPB(0),
	m_mouseClickPB(0),
	m_onRelease(0),
	m_onClick(nullptr)
{
	m_type = yyGUIElementType::Button;
	m_textResizeButton = true;
	m_isInActiveAreaRect = false;
	m_isClicked = false;
	m_isChecked = false;
	m_useAsCheckbox = false;
	m_isAnimated = false;
	m_gpu = yyGetVideoDriverAPI();
	m_buttonGroup = 0;
	m_baseTexture = 0;
	m_onMouseEnter = 0;
	m_onMouseLeave = 0;
	m_textElement = 0;
	m_textColor = ColorWhite;
	m_textColorHover = ColorRed;
	m_textColorPress = ColorLime;

	m_useBackground = false;
	m_bgColor.set(0.41f);
	m_bgColorHover.set(0.53f);
	m_bgColorPress.set(0.3f);
}


yyGUIButton::~yyGUIButton(){
	if (m_textElement) yyDestroy(m_textElement);
	if (m_basePB) yyDestroy(m_basePB);
	if (m_mouseHoverPB) yyDestroy(m_mouseHoverPB);
	if (m_mouseClickPB) yyDestroy(m_mouseClickPB);
}

void yyGUIButton::SetTextColor(const yyColor& c) {
	m_textColor = c;
	m_textColorHover = c;
	m_textColorPress = c;
	if (!m_textElement)
		return;

	m_textElement->m_color = c;
}

void yyGUIButton::SetText(const wchar_t* text, yyGUIFont* font, bool resizeButton) {
	m_text = text;
	
	if (m_textElement) yyDestroy(m_textElement); m_textElement = 0;

	if (!text)
	{
		m_text.clear();
		return;
	}

	if (m_text.size() == 0)
	{
		yyLogWriteWarning("%s - text size == 0\n", YY_FUNCTION);
		return;
	}

	m_textResizeButton = resizeButton;

	this->m_useBackground = true;

	m_textElement = yyGUICreateText(v2f(m_buildRectInPixels.x, m_buildRectInPixels.y), font, m_text.data(), m_drawGroup);
	yyGUIRemoveElement(m_textElement); // remove from gui elements list
	m_textElement->IgnoreInput(true);

	v2f buttonRectHalf;
	buttonRectHalf.x = (f32(m_buildRectInPixels.z - m_buildRectInPixels.x) * 0.5f);
	buttonRectHalf.y = (f32(m_buildRectInPixels.w - m_buildRectInPixels.y) * 0.5f);
	
	v4f textBuildingRect = m_textElement->m_buildRectInPixels;
	v2f textBuildingRectHalf;
	textBuildingRectHalf.x = (f32(textBuildingRect.z - textBuildingRect.x) * 0.5f);
	textBuildingRectHalf.y = (f32(textBuildingRect.w - textBuildingRect.y) * 0.5f);
	
	m_textElement->m_buildRectInPixels.x += buttonRectHalf.x - textBuildingRectHalf.x;
	m_textElement->m_buildRectInPixels.y += buttonRectHalf.y - textBuildingRectHalf.y;
	m_textElement->SetBuildRect(m_textElement->m_buildRectInPixels);
	//m_textElement->m_buildRectInPixels = m_textElement->m_buildRectInPixels;

	m_textElement->Rebuild();

	m_textElement->m_color = m_textColor;


	/*
	// update clip rect here
	*/



}

void yyGUIButton::SetOffset(const v2f& o){
	m_offset = o;
	if (m_mouseClickPB) m_mouseClickPB->m_offset = o;
	if (m_mouseHoverPB) m_mouseHoverPB->m_offset = o;
	if (m_basePB) m_basePB->m_offset = o;
}

yyColor& yyGUIButton::GetColor(s32 pictureBox) {
	switch (pictureBox)
	{
	default:
	case 0:
		return m_color;
	case 1:
		if (m_mouseHoverPB) return m_mouseHoverPB->m_color;
		break;
	case 2:
		if (m_mouseClickPB) return m_mouseClickPB->m_color;
		break;
	}
	return m_color;
}

f32 yyGUIButton::GetOpacity(s32 pictureBox) {
	switch (pictureBox)
	{
	default:
	case 0:
		return m_color.m_data[3];
	case 1:
		if (m_mouseHoverPB) return m_mouseHoverPB->m_color.m_data[3];
		break;
	case 2:
		if (m_mouseClickPB) return m_mouseClickPB->m_color.m_data[3];
		break;
	}
	return m_color.m_data[3];
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

void yyGUIButton::SetColor(const yyColor& c, s32 pictureBox) {
	switch (pictureBox)
	{
	default:
	case 0:
		if(m_basePB)
			m_basePB->m_color = c;
		m_color = c;
		break;
	case 1:
		if (m_mouseHoverPB) m_mouseHoverPB->m_color = c;
		break;
	case 2:
		if (m_mouseClickPB) m_mouseClickPB->m_color = c;
		break;
	}
}

void yyGUIButton::OnUpdate(f32 dt){
	if (!m_visible) return;

	bool mouseInRectBefore = m_isInActiveAreaRect;

	yyGUIElement::CheckCursorInRect();

	if (m_isInActiveAreaRect)
	{
		if (m_onMouseInRect)
			m_onMouseInRect(this, m_id);

		if (!mouseInRectBefore)
		{
			if (m_onMouseEnter)
				m_onMouseEnter(this, m_id);
		}
	}


	if (!m_isInActiveAreaRect && mouseInRectBefore)
	{
		if (m_onMouseLeave)
			m_onMouseLeave(this, m_id);
	}

	if (m_ignoreInput) return;

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
				if (m_buttonGroup)
				{
					auto buttons = m_buttonGroup->m_buttons.data();
					for (u32 i = 0, sz = m_buttonGroup->m_buttons.size(); i < sz; ++i)
					{
						buttons[i]->m_isChecked = false;
					}
				}

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

void yyGUIButton::OnDraw(f32 dt){
	if (!m_visible) return;
	
	f32 lerp_t = 10.f *dt;

	if (m_useBackground)
	{
		if (m_isInActiveAreaRect)
		{
		
			if (m_isAnimated)
			{
				if (m_isClicked || m_isChecked)
				{
					m_bgCurrCol.m_data[0] = math::lerp(m_bgCurrCol.m_data[0], m_bgColorPress.m_data[0], lerp_t);
					m_bgCurrCol.m_data[1] = math::lerp(m_bgCurrCol.m_data[1], m_bgColorPress.m_data[1], lerp_t);
					m_bgCurrCol.m_data[2] = math::lerp(m_bgCurrCol.m_data[2], m_bgColorPress.m_data[2], lerp_t);
				//	m_bgCurrCol.m_data[3] = math::lerp(m_bgCurrCol.m_data[3], m_bgColorPress.m_data[3], lerp_t);
				}
				else
				{
					m_bgCurrCol.m_data[0] = math::lerp(m_bgCurrCol.m_data[0], m_bgColorHover.m_data[0], lerp_t);
					m_bgCurrCol.m_data[1] = math::lerp(m_bgCurrCol.m_data[1], m_bgColorHover.m_data[1], lerp_t);
					m_bgCurrCol.m_data[2] = math::lerp(m_bgCurrCol.m_data[2], m_bgColorHover.m_data[2], lerp_t);
					//m_bgCurrCol.m_data[3] = math::lerp(m_bgCurrCol.m_data[3], m_bgColorHover.m_data[3], lerp_t);
				}
			}
			else
			{
				if (m_isClicked || m_isChecked)
					m_bgCurrCol = m_bgColorPress;
				else
					m_bgCurrCol = m_bgColorHover;
			}
		}
		else
		{
			if (m_isAnimated)
			{
				if (m_isClicked || m_isChecked)
				{
					m_bgCurrCol.m_data[0] = math::lerp(m_bgCurrCol.m_data[0], m_bgColorPress.m_data[0], lerp_t);
					m_bgCurrCol.m_data[1] = math::lerp(m_bgCurrCol.m_data[1], m_bgColorPress.m_data[1], lerp_t);
					m_bgCurrCol.m_data[2] = math::lerp(m_bgCurrCol.m_data[2], m_bgColorPress.m_data[2], lerp_t);
				//	m_bgCurrCol.m_data[3] = math::lerp(m_bgCurrCol.m_data[3], m_bgColorPress.m_data[3], lerp_t);
				}
				else
				{
					m_bgCurrCol.m_data[0] = math::lerp(m_bgCurrCol.m_data[0], m_bgColor.m_data[0], lerp_t);
					m_bgCurrCol.m_data[1] = math::lerp(m_bgCurrCol.m_data[1], m_bgColor.m_data[1], lerp_t);
					m_bgCurrCol.m_data[2] = math::lerp(m_bgCurrCol.m_data[2], m_bgColor.m_data[2], lerp_t);
				//	m_bgCurrCol.m_data[3] = math::lerp(m_bgCurrCol.m_data[3], m_bgColor.m_data[3], lerp_t);
				}
			}
			else
			{
				if (m_isClicked || m_isChecked)
					m_bgCurrCol = m_bgColorPress;
				else
					m_bgCurrCol = m_bgColor;
			}
		}
	
		g_engine->m_videoAPI->DrawRectangle(m_buildRectInPixels, m_bgCurrCol, m_bgCurrCol);
	}

	if (m_isAnimated)
	{
		f32 sp = 10.f  * dt;

		if (m_mouseHoverPB)
		{
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
	}

	if (m_isAnimated)
	{
		if(m_basePB)
			m_basePB->OnDraw(dt);
	}

	if (m_isClicked || m_isChecked)
	{
		if (m_mouseClickPB)
		{
			m_mouseClickPB->OnDraw(dt);
		}
		else if(m_mouseHoverPB)
		{
			m_mouseHoverPB->OnDraw(dt);
		}
		else if(m_basePB)
		{
			m_basePB->OnDraw(dt);
		}
	}
	else
	{
		if (m_basePB)
			m_basePB->OnDraw(dt);
		if (m_mouseHoverPB)
		{
			if (m_isInActiveAreaRect || (m_isAnimated && m_mouseHoverPB->m_color.m_data[3] > 0.f))
			{
				m_mouseHoverPB->OnDraw(dt);
			}
			else if (m_basePB)
			{
				m_basePB->OnDraw(dt);
			}
		}
	}

	if (m_textElement)
	{
		if (m_textElement->m_onDraw)
			m_textElement->m_onDraw(m_textElement, m_textElement->m_id);
		
		if (m_isClicked || m_isChecked)
		{
			m_textElement->m_color = m_textColorPress;
		}
		else
		{
			if (m_isInActiveAreaRect)
			{
				m_textElement->m_color = m_textColorHover;
			}
			else
			{
				m_textElement->m_color = m_textColor;
			}
		}
		m_textElement->OnDraw(dt);
	}
}

void yyGUIButton::Rebuild(){
	yyGUIElement::CallOnRebuildSetRects();

	if (m_basePB)
	{
		m_basePB->SetBuildRect(m_buildRectInPixels);
		m_basePB->Rebuild();
	}

	if (m_mouseHoverPB)
	{
		m_mouseHoverPB->SetBuildRect(m_basePB->m_buildRectInPixels);
		m_mouseHoverPB->Rebuild();
	}

	if (m_mouseClickPB)
	{
		m_mouseClickPB->SetBuildRect(m_basePB->m_buildRectInPixels);
		m_mouseClickPB->Rebuild();
	}

	if (m_textElement)
	{
		this->SetText(m_textElement->m_text.data(), m_textElement->m_font, m_textResizeButton);
	}
}

YY_API yyGUIButton* YY_C_DECL yyGUICreateButton(const v4f& rect, yyResource* baseTexture, s32 id, yyGUIDrawGroup* drawGroup, v4f* uv){
	yyGUIButton* element = yyCreate<yyGUIButton>();
	element->SetDrawGroup(drawGroup);
	element->SetBuildRect(rect);
	element->m_id = id;
	element->m_baseTexture = baseTexture;

	if (baseTexture)
	{
		if (!element->m_baseTexture->IsLoaded())
			element->m_baseTexture->Load();

		if (uv)
		{
			element->m_uvRect = *uv;
		}
		else
		{
			auto gpu = yyGetVideoDriverAPI();
			v2f s;
			element->m_baseTexture->GetTextureSize(&s);
			element->m_uvRect.z = s.x;
			element->m_uvRect.w = s.y;
		}
		//m_basePB = yyGUICreatePictureBox(m_buildRectInPixels, m_baseTexture, m_id, m_drawGroup, &m_uvRect);
		//m_basePB->m_color = m_color;

		element->m_basePB = yyGUICreatePictureBox(element->m_buildRectInPixels, baseTexture, -1, element->m_drawGroup, &element->m_uvRect);
		yyGUIRemoveElement(element->m_basePB);
	}

	element->Rebuild();
	return element;
}

void yyGUIButton::SetVisible(bool v) {
	m_visible = v; 
}

void yyGUIButton::SetMouseHoverTexture(yyResource* t, v4f* uv){
	if (!m_mouseHoverPB)
	{
		m_mouseHoverPB = yyGUICreatePictureBox(m_buildRectInPixels, t, -1, m_drawGroup, uv);
		m_mouseHoverPB->IgnoreInput(true);
		m_mouseHoverPB->m_color = m_color;
		yyGUIRemoveElement(m_mouseHoverPB);
	}
	else
	{
		m_mouseHoverPB->m_texture = t;
	}
}

void yyGUIButton::SetMouseClickTexture(yyResource* t, v4f* uv){
	if (!m_mouseClickPB)
	{
		m_mouseClickPB = yyGUICreatePictureBox(m_buildRectInPixels, t, -1, m_drawGroup, uv);
		m_mouseClickPB->IgnoreInput(true);
		m_mouseClickPB->m_color = m_color;
		yyGUIRemoveElement(m_mouseClickPB);
	}
	else
	{
		m_mouseClickPB->m_texture = t;
	}
}
