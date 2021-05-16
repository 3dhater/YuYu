#include "yy.h"
#include "yy_gui.h"
#include "math/vec.h"

#include "yy_model.h"
#include "yy_input.h"

#include "../engine.h"

extern Engine * g_engine;

yyGUITextInput::yyGUITextInput(){
	m_type = yyGUIElementType::TextInput;
	m_textElement = 0;
	m_bgElement = 0;
	SetBufferSize(1024);
	m_onMouseInRect = 0;
	m_onClick = 0;
}

yyGUITextInput::~yyGUITextInput(){
	if (m_textElement) yyDestroy(m_textElement);
	if (m_bgElement) yyDestroy(m_bgElement);
}

void yyGUITextInput::OnUpdate(f32 dt){
	if (!m_visible) return;
	
	yyGUIElement::CheckCursorInRect();

	if (m_ignoreInput) return;

	if (m_isInActiveAreaRect)
	{

		if (m_onMouseInRect)
			m_onMouseInRect(this,m_id);

		if (g_engine->m_inputContext->m_isLMBDown)
		{
			if (m_onClick)
				m_onClick(this, m_id);
		}
	}
}

void yyGUITextInput::OnDraw(){
	if (!m_visible) return;
	m_bgElement->OnDraw();
	m_textElement->OnDraw();
}

void yyGUITextInput::SetBufferSize(u32 newSize){
	if(m_textElement)
		m_textElement->SetBufferSize(newSize);
}


void yyGUITextInput::SetText(const wchar_t* format, ...){
	Clear();

	va_list arg;
	va_start(arg, format);
	_vsnwprintf(m_textElement->m_buffer, m_textElement->m_bufferSize, format, arg);
	va_end(arg);

	static yyStringW str;
	str.clear();
	str = m_textElement->m_buffer;
	m_textElement->SetText(m_textElement->m_buffer);
}

void yyGUITextInput::Clear(){
	m_textElement->Clear();
}

void yyGUITextInput::Rebuild() {
	yyGUIElement::CallOnRebuildSetRects();
	m_bgElement->Rebuild();
	m_textElement->Rebuild();
}

void yyGUITextInput::SetFont(yyGUIFont* newFont) {
	m_textElement->SetFont(newFont);
}

YY_API yyGUITextInput* YY_C_DECL yyGUICreateTextInput(const v4f& rect, yyGUIFont* font, const wchar_t* text, yyGUIDrawGroup* drawGroup)
{
	assert(font);
	yyGUITextInput* element = yyCreate<yyGUITextInput>();
	
	element->m_bgElement = yyGUICreatePictureBox(rect, yyGetDefaultTexture(), -1, drawGroup);
	yyGUIRemoveElement(element->m_bgElement);
	element->m_bgElement->m_color = ColorRed;

	element->m_textElement = yyGUICreateText(v2f(rect.x, rect.y), font, text, drawGroup);
	yyGUIRemoveElement(element->m_textElement);

	element->SetDrawGroup(drawGroup);
	element->SetBuildRect(rect);

	return element;
}
