#include "yy.h"
#include "yy_gui.h"
#include "math/vec.h"

#include "yy_model.h"
#include "yy_input.h"

#include "../engine.h"

extern Engine * g_engine;

yyGUIRangeSlider::yyGUIRangeSlider(){
	m_type = yyGUIElementType::RangeSlider;
	m_sliderType = yyGUIRangeSliderType::IntLimits;
	m_vertical = false;
	m_bgColor.set(0.2f);
	m_bgColorHover.set(0.45f);
	m_bgColorCurrent = m_bgColor;
	m_limitRectColor = ColorBlue;
	m_text = 0;
}

yyGUIRangeSlider::~yyGUIRangeSlider(){
	if (m_text) yyDestroy(m_text);
}

void yyGUIRangeSlider::OnUpdate(f32 dt){
	if (!m_visible) return;
	
	yyGUIElement::CheckCursorInRect();

	if (m_ignoreInput) return;

	m_bgColorCurrent = m_bgColor;

	static v2f cursor_coord_on_click;

	if (m_isInActiveAreaRect && (!g_engine->m_guiElementInMouseFocus))
	{
		m_bgColorCurrent = m_bgColorHover;

		if (!g_engine->m_guiElementInMouseFocus)
		{
			if (m_vertical)
			{
				yyGetCursor(yyCursorType::SizeNS)->Activate();
			}
			else
			{
				yyGetCursor(yyCursorType::SizeWE)->Activate();
			}
		}

		if (g_engine->m_inputContext->m_isLMBDown)
		{
			g_engine->m_guiElementInMouseFocus = this;
			g_engine->m_GUIElementInputFocus = this;
			yySetCursorDisableAutoChange(true);
			yyShowCursor(false);
			cursor_coord_on_click = g_engine->m_inputContext->m_cursorCoordsForGUI;
		}
	}

	if (g_engine->m_inputContext->m_isLMBUp)
	{
		yySetCursorDisableAutoChange(false);
	}

	static f32 counter = 0.f;
	const f32 slower = 10.f;

	if (g_engine->m_guiElementInMouseFocus == this)
	{
		yySetCursorPosition(cursor_coord_on_click.x, cursor_coord_on_click.y, yyGetMainWindow());
		yyShowCursor(false);
		
		m_bgColorCurrent = m_bgColorHover;

		f32 value_add = 0;
		if (m_vertical)
		{
			counter += g_engine->m_inputContext->m_mouseDelta.y;
		}
		else
		{
			counter += g_engine->m_inputContext->m_mouseDelta.x;
		}
		if (counter >= slower)
		{
			value_add = counter / slower;
			counter = 0.f;
		}
		if (counter <= -slower)
		{
			value_add = counter / slower;
			counter = 0.f;
		}
		
		switch (m_sliderType)
		{
		case yyGUIRangeSliderType::Int:
		case yyGUIRangeSliderType::IntLimits:
			*m_ptr_i += (s32)value_add;
			if (value_add != 0.f)
			{
				if (m_sliderType == yyGUIRangeSliderType::IntLimits)
				{
					if (*m_ptr_i < m_minMax_i[0]) *m_ptr_i = m_minMax_i[0];
					if (*m_ptr_i > m_minMax_i[1]) *m_ptr_i = m_minMax_i[1];
					_calculate_limit_rectangle();
				}
				if (m_text)
					m_text->SetText(L"%i", *m_ptr_i);
			}
			break;
		case yyGUIRangeSliderType::Float:
		case yyGUIRangeSliderType::FloatLimits:
			*m_ptr_f += value_add;
			
			if (value_add != 0.f)
			{
				if (m_sliderType == yyGUIRangeSliderType::FloatLimits) {
					if (*m_ptr_f < m_minMax_f[0]) *m_ptr_f = m_minMax_f[0];
					if (*m_ptr_f > m_minMax_f[1]) *m_ptr_f = m_minMax_f[1];
					_calculate_limit_rectangle();
				}
				if (m_text)
					m_text->SetText(L"%f", *m_ptr_f);
			}
			break;
		default:
			break;
		}
	}

	if (m_text) 
	{
		//g_engine->m_guiIgnoreUpdateInput = false;
		m_text->OnUpdate(dt);
	}
}

void yyGUIRangeSlider::OnDraw(){
	if (!m_visible) return;
	//m_bgElement->OnDraw();
	g_engine->m_videoAPI->DrawRectangle(m_buildRectInPixels, m_bgColorCurrent, m_bgColorCurrent);
	switch (m_sliderType)
	{
	case yyGUIRangeSliderType::IntLimits:
	case yyGUIRangeSliderType::FloatLimits:
		g_engine->m_videoAPI->DrawRectangle(m_limitRectangle, m_limitRectColor, m_limitRectColor);
		break;
	}
	if (m_text)m_text->OnDraw();
}

void yyGUIRangeSlider::_calculate_limit_rectangle() {
	f32 range_value = 0.f;
	switch (m_sliderType)
	{
	case yyGUIRangeSliderType::IntLimits:
	{
		f32 range = (f32)m_minMax_i[1] - (f32)m_minMax_i[0];
		if (range == 0.f) range = 1.f;
		range_value = ((f32)*m_ptr_i) * (1.f / range);
	}break;
	case yyGUIRangeSliderType::FloatLimits:
	{
		f32 range = m_minMax_f[1] - m_minMax_f[0];
		if (range == 0.f) range = 1.f;
		range_value = (*m_ptr_f) * (1.f / range);
	}break;
	}

	if (m_vertical)
	{

	}
	else
	{
		m_limitRectangle.x = m_buildRectInPixels.x;
		m_limitRectangle.y = m_buildRectInPixels.y;
		m_limitRectangle.w = m_buildRectInPixels.w;
		m_limitRectangle.z = m_buildRectInPixels.x + (range_value / (1.f / (m_buildRectInPixels.z - m_buildRectInPixels.x)));
	}
}

void yyGUIRangeSlider::Rebuild() {
	yyGUIElement::CallOnRebuildSetRects();
	
	if (m_text) m_text->Rebuild();
}

void yyGUIRangeSlider::UseText(yyGUIFont* f) {
	if (!f) return;
	if (m_text) return;

	if (!m_vertical)
	{
		m_text = yyGUICreateTextInput(m_buildRectInPixels, f, 0, 0);
		m_text->SetLimit(20);
		m_text->m_bgColor.set(0.f, 0.f, 0.f, 0.f);
		m_text->m_bgColorHover.set(0.f, 0.f, 0.f, 0.f);
		m_text->m_bgColorActive.set(0.f, 0.f, 0.f, 0.f);
		switch (m_sliderType)
		{
		default:
		case yyGUIRangeSliderType::Int:
		case yyGUIRangeSliderType::IntLimits:
			m_text->SetText(L"%i", *m_ptr_i);
			break;
		case yyGUIRangeSliderType::Float:
		case yyGUIRangeSliderType::FloatLimits:
			m_text->SetText(L"%f", *m_ptr_f);
			break;
		}
		yyGUIRemoveElement(m_text);
	}
}

YY_API yyGUIRangeSlider* YY_C_DECL yyGUICreateRangeSliderInt(const v4f& rect, s32 minimum, s32 maximum, s32* value, bool vertical, yyGUIDrawGroup* drawGroup){
	assert(value);
	yyGUIRangeSlider* element = yyCreate<yyGUIRangeSlider>();
	element->m_sliderType = yyGUIRangeSliderType::IntLimits;
	element->m_ptr_i = value;
	element->m_minMax_i[0] = minimum;
	element->m_minMax_i[1] = maximum;
	element->m_vertical = vertical;
	element->SetDrawGroup(drawGroup);
	element->SetBuildRect(rect);
	return element;
}
YY_API yyGUIRangeSlider* YY_C_DECL yyGUICreateRangeSliderFloat(const v4f& rect, f32 minimum, f32 maximum, f32* value, bool vertical, yyGUIDrawGroup* drawGroup) {
	assert(value);
	yyGUIRangeSlider* element = yyCreate<yyGUIRangeSlider>();
	element->m_sliderType = yyGUIRangeSliderType::FloatLimits;
	element->m_ptr_f = value;
	element->m_minMax_f[0] = minimum;
	element->m_minMax_f[1] = maximum;
	element->m_vertical = vertical;
	element->SetDrawGroup(drawGroup);
	element->SetBuildRect(rect);
	return element;
}
YY_API yyGUIRangeSlider* YY_C_DECL yyGUICreateRangeSliderIntNoLimit(const v4f& rect, s32* value, bool vertical, yyGUIDrawGroup* drawGroup) {
	assert(value);
	yyGUIRangeSlider* element = yyCreate<yyGUIRangeSlider>();
	element->m_sliderType = yyGUIRangeSliderType::Int;
	element->m_ptr_i = value;
	element->m_vertical = vertical;
	element->SetDrawGroup(drawGroup);
	element->SetBuildRect(rect);
	return element;
}
YY_API yyGUIRangeSlider* YY_C_DECL yyGUICreateRangeSliderFloatNoLimit(const v4f& rect, f32* value, bool vertical, yyGUIDrawGroup* drawGroup) {
	assert(value);
	yyGUIRangeSlider* element = yyCreate<yyGUIRangeSlider>();
	element->m_sliderType = yyGUIRangeSliderType::Float;
	element->m_ptr_f = value;
	element->m_vertical = vertical;
	element->SetDrawGroup(drawGroup);
	element->SetBuildRect(rect);
	return element;
}