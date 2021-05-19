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
}

yyGUIRangeSlider::~yyGUIRangeSlider(){
//	if (m_bgElement) yyDestroy(m_bgElement);
}

void yyGUIRangeSlider::OnUpdate(f32 dt){
	if (!m_visible) return;
	
	yyGUIElement::CheckCursorInRect();

	if (m_ignoreInput) return;
}

void yyGUIRangeSlider::OnDraw(){
	if (!m_visible) return;
	//m_bgElement->OnDraw();
	//g_engine->m_videoAPI->DrawRectangle(m_buildRectInPixels, m_bgColorCurrent, m_bgColorCurrent);
}

void yyGUIRangeSlider::Rebuild() {
	yyGUIElement::CallOnRebuildSetRects();
	
	//m_textElement->SetBuildRect(m_buildRectInPixels);
	//m_textElement->Rebuild();
}

YY_API yyGUIRangeSlider* YY_C_DECL yyGUICreateRangeSliderInt(const v4f& rect, s32 minimum, s32 maximum, s32* value, bool vertical, yyGUIDrawGroup* drawGroup)
{
	assert(value);
	yyGUIRangeSlider* element = yyCreate<yyGUIRangeSlider>();
	element->m_sliderType = yyGUIRangeSliderType::IntLimits;
	element->m_ptr_i = value;
	element->m_minMax_i[0] = minimum;
	element->m_minMax_i[1] = maximum;
	
	/*element->m_bgElement = yyGUICreatePictureBox(rect, yyGetDefaultTexture(), -1, drawGroup);
	yyGUIRemoveElement(element->m_bgElement);
	element->m_bgElement->m_color = element->m_bgColor;*/


	element->SetDrawGroup(drawGroup);
	element->SetBuildRect(rect);

	return element;
}
