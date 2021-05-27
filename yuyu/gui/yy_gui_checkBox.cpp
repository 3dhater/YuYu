#include "yy.h"
#include "yy_gui.h"
#include "math/vec.h"
#include "math/math.h"

#include "yy_model.h"
#include "yy_input.h"

#include "../engine.h"

extern yyEngine * g_engine;

yyGUICheckBox::yyGUICheckBox() {
	m_type = yyGUIElementType::CheckBox;
	m_checkBoxType = yyGUICheckBoxType::Type1;
	m_textPositionAdd = 0.f;
	m_basePB = 0;
	m_hoverPB = 0;
	m_checkPB = 0;
	m_isAnimated = true;
	m_isChecked = false;
	m_onClick = 0;
	m_onRelease = 0;
	m_text = 0;
	m_textOffset.set(0.f, 3.f);
}

yyGUICheckBox::~yyGUICheckBox() {
	if (m_basePB) yyDestroy(m_basePB);
	if (m_hoverPB) yyDestroy(m_hoverPB);
	if (m_checkPB) yyDestroy(m_checkPB);
	if (m_text) yyDestroy(m_text);
}

void yyGUICheckBox::OnUpdate(f32 dt) {
	if (!m_visible) return;
	yyGUIElement::CheckCursorInRect();
	if (m_ignoreInput) return;

	if (g_engine->m_inputContext->m_isLMBUp)
	{
		if (m_onRelease && m_isInActiveAreaRect
			&& (g_engine->m_guiElementInMouseFocus == (yyGUIElement*)this))
			m_onRelease(this, m_id);

	}

	if (m_isAnimated)
	{
		f32 sp = 10.f  * dt;
		if (m_isInActiveAreaRect)
		{
			m_hoverPB->m_color.m_data[3] += sp;
			if (m_hoverPB->m_color.m_data[3] > 1.f)
				m_hoverPB->m_color.m_data[3] = 1.f;
		}
		else
		{
			m_hoverPB->m_color.m_data[3] -= sp;
			if (m_hoverPB->m_color.m_data[3] < 0.f)
				m_hoverPB->m_color.m_data[3] = 0.f;
		}
	}

	if (m_isInActiveAreaRect)
	{
		if (g_engine->m_inputContext->m_isLMBDown)
		{
			m_isChecked = m_isChecked ? false : true;
			if (m_onClick)
				m_onClick(this, m_id);
			g_engine->m_guiElementInMouseFocus = this;
		}
	}
}

void yyGUICheckBox::OnDraw(f32 dt) {
	if (!m_visible) return;
	m_basePB->OnDraw(dt);

	if (!m_isChecked)
	{
		if (!m_isAnimated)
		{
			if (m_isInActiveAreaRect)
				m_hoverPB->OnDraw(dt);
		}
		else
		{
			m_hoverPB->OnDraw(dt);
		}
	}
	else
	{
		m_checkPB->OnDraw(dt);
	}

	if (m_text)
	{
		m_text->OnDraw(dt);
	}
}

void yyGUICheckBox::Rebuild() {
	yyGUIElement::CallOnRebuildSetRects();
	
	auto old = m_basePB->m_buildRectInPixels;
	
	m_basePB->m_buildRectInPixels.x = m_buildRectInPixels.x;
	m_basePB->m_buildRectInPixels.y = m_buildRectInPixels.y;
	m_basePB->m_buildRectInPixels.z = m_buildRectInPixels.x + (old.z - old.x);
	m_basePB->m_buildRectInPixels.w = m_buildRectInPixels.y + (old.w - old.y);

	m_basePB->SetBuildRect(m_basePB->m_buildRectInPixels);
	m_basePB->Rebuild();

	m_hoverPB->SetBuildRect(m_basePB->m_buildRectInPixels);
	m_hoverPB->Rebuild();
	
	m_checkPB->SetBuildRect(m_basePB->m_buildRectInPixels);
	m_checkPB->Rebuild();

	if (m_text)
	{
		f32 w = m_text->m_buildRectInPixels.z - m_text->m_buildRectInPixels.x;
		//f32 h = m_text->m_buildRectInPixels.w - m_text->m_buildRectInPixels.y;

		m_text->m_buildRectInPixels = v4f(
			m_basePB->m_buildRectInPixels.x + m_textPositionAdd + m_textOffset.x,
			m_basePB->m_buildRectInPixels.y + m_textOffset.y,
			m_basePB->m_buildRectInPixels.x + m_textPositionAdd + w + m_textOffset.x,
			m_basePB->m_buildRectInPixels.w + m_textOffset.y
		);
		m_text->Rebuild();
	}
}

YY_API yyGUICheckBox* YY_C_DECL yyGUICreateCheckBox(const v2f& position, yyGUICheckBoxType t, yyGUIFont* font, const wchar_t* text, yyGUIDrawGroup* drawGroup) {
	yyGUICheckBox* element = yyCreate<yyGUICheckBox>();
	element->SetDrawGroup(drawGroup);

	v4f rect;
	rect.x = rect.z = position.x;
	rect.y = rect.w = position.y;
	
	v4f PBrect;

	v2f text_position = position;

	v4f uv_rect_base;
	v4f uv_rect_hover;
	v4f uv_rect_check;
	if (text)
	{
		rect.z = rect.x + font->GetTextLength(text);
		rect.w = rect.y + font->m_maxHeight;
	}
	
	switch (t)
	{
	case yyGUICheckBoxType::Type1:
		rect.z += 16.f;
		if ((rect.w - rect.y) < 16.f)
		{
			rect.w += 16.f - (rect.w - rect.y);
		}
		PBrect.x = position.x;
		PBrect.y = position.y;
		PBrect.z = PBrect.x + 16.f;
		PBrect.w = PBrect.y + 16.f;

		element->m_textPositionAdd = 16.f;

		uv_rect_base.set(0.f, 0.f, 15.f, 15.f);
		uv_rect_hover.set(33.f, 0.f, 48.f, 15.f);
		uv_rect_check.set(17.f, 0.f, 32.f, 15.f);
		break;
	default:
		break;
	}

	if (text)
	{
		text_position.x += element->m_textPositionAdd;
		element->m_text = yyGUICreateText(text_position, font, text, 0);
		yyGUIRemoveElement(element->m_text);
	}

	element->m_basePB = yyGUICreatePictureBox(PBrect, g_engine->GetGUITextureCheckBox(), -1, 0, &uv_rect_base);
	yyGUIRemoveElement(element->m_basePB);

	element->m_hoverPB = yyGUICreatePictureBox(PBrect, g_engine->GetGUITextureCheckBox(), -1, 0, &uv_rect_hover);
	yyGUIRemoveElement(element->m_hoverPB);

	element->m_checkPB = yyGUICreatePictureBox(PBrect, g_engine->GetGUITextureCheckBox(), -1, 0, &uv_rect_check);
	yyGUIRemoveElement(element->m_checkPB);

	element->SetBuildRect(rect);
	return element;
}