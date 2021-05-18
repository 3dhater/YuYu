#include "yy.h"
#include "yy_gui.h"
#include "math/vec.h"

#include "yy_model.h"
#include "yy_input.h"

#include "../engine.h"

extern Engine * g_engine;

yyGUITextInput::yyGUITextInput(){
	m_type = yyGUIElementType::TextInput;
	m_horScroll = 0.f;
	m_clickCount = 0;
	m_textCursorTimerLimit = 0.55f;
	m_textCursorPositionInChars = 0;
	m_onCharacter = 0;
	m_drawTextCursor = true;
	m_isSelected = false;
	m_textElement = 0;
//	m_bgElement = 0;
	m_textCursorElement = 0;
	SetBufferSize(1024);
	m_onClickLMB = 0;
	m_onClickMMB = 0;
	m_onClickRMB = 0;
	m_onClickX1MB = 0;
	m_onClickX2MB = 0;
	m_bgColor.set(0.5f);
	m_bgColorHover.set(0.55f);
	m_bgColorActive.set(0.25f);
	m_selectionLeft = 0;
	m_selectionRight = 0;
}

yyGUITextInput::~yyGUITextInput(){
	if (m_textCursorElement) yyDestroy(m_textCursorElement);
	if (m_textElement) yyDestroy(m_textElement);
//	if (m_bgElement) yyDestroy(m_bgElement);
}

void yyGUITextInput::OnUpdate(f32 dt){
	if (!m_visible) return;
	
	yyGUIElement::CheckCursorInRect();

	if (m_ignoreInput) return;

	m_bgColorCurrent = m_bgColor;

	auto GUIElementInputFocus_old = g_engine->m_GUIElementInputFocus;
	
	if (g_engine->m_inputContext->m_isLMBHold && (g_engine->m_guiElementInMouseFocus == this)
		&& (m_clickCount > 1))
	{
		yySetCursorDisableAutoChange(true);
		_calculate_text_cursor_position_from_mouse();
		_calculate_text_cursor_rect();
	}
	
	if (g_engine->m_inputContext->m_isLMBUp)
	{
		yySetCursorDisableAutoChange(false);
		yyGetCursor(yyCursorType::Arrow)->Activate();
	}

	if (m_isInActiveAreaRect)
	{
		m_bgColorCurrent = m_bgColorHover;

		yyGetCursor(yyCursorType::IBeam)->Activate();

		if (m_onMouseInRect)
			m_onMouseInRect(this,m_id);

		if (g_engine->m_inputContext->m_isLMBDown)
		{
			g_engine->m_GUIElementInputFocus = this;
			g_engine->m_guiElementInMouseFocus = this;
			
			++m_clickCount; // 1,2,3,4...

			/*if (GUIElementInputFocus_old == this)
			{
				_calculate_text_cursor_position_from_mouse();
				_calculate_text_cursor_rect();
			}*/

			if (m_onClickLMB)
				m_onClickLMB(this, m_id);
		}
		if (g_engine->m_inputContext->m_isMMBDown)
		{
			g_engine->m_GUIElementInputFocus = this;
			++m_clickCount; // 1,2,3,4...

			if (m_onClickMMB)
				m_onClickMMB(this, m_id);
		}
		if (g_engine->m_inputContext->m_isRMBDown)
		{
			g_engine->m_GUIElementInputFocus = this;
			++m_clickCount; // 1,2,3,4...

			if (m_onClickRMB)
				m_onClickRMB(this, m_id);
		}
		if (g_engine->m_inputContext->m_isX1MBDown)
		{
			g_engine->m_GUIElementInputFocus = this;
			++m_clickCount; // 1,2,3,4...

			if (m_onClickX1MB)
				m_onClickX1MB(this, m_id);
		}
		if (g_engine->m_inputContext->m_isX2MBDown)
		{
			g_engine->m_GUIElementInputFocus = this;
			++m_clickCount; // 1,2,3,4...

			if (m_onClickX2MB)
				m_onClickX2MB(this, m_id);
		}
	}


	if (g_engine->m_GUIElementInputFocus == this)
	{
		if (GUIElementInputFocus_old != this)
		{
			m_textCursorTimer = 0.f;
		}

		m_textCursorTimer += dt;
		if (m_textCursorTimer > m_textCursorTimerLimit)
		{
			m_textCursorTimer = 0.f;
			m_drawTextCursor = m_drawTextCursor ? false : true;
		}

		auto text_size = m_textElement->m_text.size();
		if (text_size)
		{
			if (g_engine->m_inputContext->IsKeyHit(yyKey::K_LEFT) && m_textCursorPositionInChars)
			{
				--m_textCursorPositionInChars;
				_calculate_text_cursor_rect();
			}
			if (g_engine->m_inputContext->IsKeyHit(yyKey::K_RIGHT) && (m_textCursorPositionInChars < text_size))
			{
				++m_textCursorPositionInChars;
				_calculate_text_cursor_rect();
			}
			else if (g_engine->m_inputContext->IsKeyHit(yyKey::K_DELETE) && (m_textCursorPositionInChars < text_size))
			{
				if (m_isSelected)
				{
					_delete_selected();
				}
				else
				{
					bool ok = false;
					auto buf = m_textElement->m_text.data();
					auto str_len = m_textElement->m_text.size();
					for (size_t i = m_textCursorPositionInChars; i < str_len; ++i)
					{
						ok = true;
						if (i + 1 == str_len)
							break;
						buf[i] = buf[i + 1];
					}
					if (ok)
					{
						buf[str_len - 1] = 0;
						m_textElement->m_text.setSize(m_textElement->m_text.size() - 1);
						m_textElement->Rebuild();
					}
				}
			}
			else if (g_engine->m_inputContext->IsKeyHit(yyKey::K_BACKSPACE) && m_textCursorPositionInChars)
			{
				if (m_isSelected)
				{
					_delete_selected();
				}
				else
				{
					bool ok = false;
					auto buf = m_textElement->m_text.data();
					auto str_len = m_textElement->m_text.size();
					for (size_t i = m_textCursorPositionInChars; i < str_len; ++i)
					{
						if (i == 0)
							break;
						ok = true;
						buf[i - 1] = buf[i];
					}
					if (m_textCursorPositionInChars == str_len && !ok)
						ok = true;
					if (ok)
					{
						if (str_len - 1 >= 0)
						{
							--m_textCursorPositionInChars;
							_calculate_text_cursor_rect();
							buf[str_len - 1] = 0;
							m_textElement->m_text.setSize(m_textElement->m_text.size() - 1);
							m_textElement->Rebuild();
						}
					}
				}
			}
			else if (g_engine->m_inputContext->IsKeyHit(yyKey::K_HOME))
			{
				m_textCursorPositionInChars = 0;
				_calculate_text_cursor_rect();
			}
			else if (g_engine->m_inputContext->IsKeyHit(yyKey::K_END))
			{
				m_textCursorPositionInChars = m_textElement->m_text.size();
				_calculate_text_cursor_rect();
			}
		}
		m_bgColorCurrent = m_bgColorActive;

		if (g_engine->m_inputContext->IsKeyHit(yyKey::K_ESCAPE))
		{
			g_engine->m_inputContext->m_key_hit[(u32)yyKey::K_ESCAPE] = 0;
			_end_edit();
		}
		if (g_engine->m_inputContext->IsKeyHit(yyKey::K_ENTER))
		{
			g_engine->m_inputContext->m_key_hit[(u32)yyKey::K_ENTER] = 0;
			_end_edit();
		}
		if (g_engine->m_inputContext->m_character)
		{
			if (g_engine->m_inputContext->m_character != L'\n'
				&& g_engine->m_inputContext->m_character != 8 // backspace
				&& g_engine->m_inputContext->m_character != 13 // enter
				)
			{
				bool ok = true;
				//wprintf(L"%c %i\n", g_engine->m_inputContext->m_character,(s32)g_engine->m_inputContext->m_character);
				if (m_onCharacter)
				{
					if (!m_onCharacter(g_engine->m_inputContext->m_character))
						ok = false;
				}
				if (ok)
				{
					if (m_isSelected)
						_delete_selected();

					m_textElement->m_text.insert(g_engine->m_inputContext->m_character, m_textCursorPositionInChars);
					m_textElement->Rebuild();

					++m_textCursorPositionInChars;
					_calculate_text_cursor_rect();
				}
			}
		}
	}
}
void yyGUITextInput::_end_edit() {
	m_clickCount = 0;
	g_engine->m_GUIElementInputFocus = 0;
	m_horScroll = 0.f;
	m_textCursorPositionInChars = 0;
	_calculate_text_cursor_rect();
}
void yyGUITextInput::_delete_selected() {

}

void yyGUITextInput::_calculate_text_cursor_position_from_mouse() {
	f32 x1 = m_buildRectInPixels.x + m_horScroll;
	for (size_t i = 0, sz = m_textElement->m_text.size(); i < sz; ++i)
	{
		auto ch = m_textElement->m_text[i];
		auto w = m_textElement->m_font->GetGlyph(ch)->width;
		
		f32 x2 = x1 + w;

		//printf("%f : %f %f\n", g_engine->m_inputContext->m_cursorCoordsForGUI.x, x1, x2);

		if (g_engine->m_inputContext->m_cursorCoordsForGUI.x >= x1
			&& g_engine->m_inputContext->m_cursorCoordsForGUI.x <= x2)
		{
			m_textCursorPositionInChars = i;
			return;
		}

		x1 += w;
	}
	if(g_engine->m_inputContext->m_cursorCoordsForGUI.x < m_buildRectInPixels.x + m_horScroll)
		m_textCursorPositionInChars = 0;
	else
		m_textCursorPositionInChars = m_textElement->m_text.size();
}

f32 yyGUITextInput::_get_text_cursor_position_in_pixels() {
	f32 v = 0.f;
	for (size_t i = 0; i < m_textCursorPositionInChars; ++i)
	{
		auto ch = m_textElement->m_text[i];
		v += m_textElement->m_font->GetGlyph(ch)->width;
	}
	return v;
}

void yyGUITextInput::_calculate_text_cursor_rect() {
	f32 width = _get_text_cursor_position_in_pixels();

begin:
	m_textCursorElement->m_offset.x = m_horScroll + width;
	m_textElement->m_offset.x = m_horScroll;

	//printf("m_textCursorElement->m_offset.x %f\n", m_textCursorElement->m_offset.x);
	if (m_textCursorElement->m_offset.x >= m_buildRectInPixels.z - m_buildRectInPixels.x)
	{
		m_horScroll -= 10.f;
		goto begin;
	}
	if (m_textCursorElement->m_offset.x < 0.f )
	{
		m_horScroll += 10.f;
		if (m_horScroll > 0.f)
			m_horScroll = 0.f;
		goto begin;
	}

	m_drawTextCursor = true;
}

void yyGUITextInput::OnDraw(){
	if (!m_visible) return;
	//m_bgElement->OnDraw();
	g_engine->m_videoAPI->DrawRectangle(m_buildRectInPixels, m_bgColorCurrent, m_bgColorCurrent);
	m_textElement->OnDraw();
	if (g_engine->m_GUIElementInputFocus == this && m_drawTextCursor)
		m_textCursorElement->OnDraw();
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
	
	/*m_bgElement->SetBuildRect(m_buildRectInPixels);
	m_bgElement->Rebuild();*/

	m_textElement->SetBuildRect(m_buildRectInPixels);
	m_textElement->Rebuild();

	m_textCursorElement->SetBuildRect(v4f(m_buildRectInPixels.x, m_buildRectInPixels.y + 1.f, m_buildRectInPixels.x + 1, m_buildRectInPixels.w - 1.f));
	m_textCursorElement->Rebuild();
}

void yyGUITextInput::SetFont(yyGUIFont* newFont) {
	m_textElement->SetFont(newFont);
}

YY_API yyGUITextInput* YY_C_DECL yyGUICreateTextInput(const v4f& rect, yyGUIFont* font, const wchar_t* text, yyGUIDrawGroup* drawGroup)
{
	assert(font);
	yyGUITextInput* element = yyCreate<yyGUITextInput>();
	
	/*element->m_bgElement = yyGUICreatePictureBox(rect, yyGetDefaultTexture(), -1, drawGroup);
	yyGUIRemoveElement(element->m_bgElement);
	element->m_bgElement->m_color = element->m_bgColor;*/

	element->m_textElement = yyGUICreateText(v2f(rect.x, rect.y), font, text, drawGroup);
	yyGUIRemoveElement(element->m_textElement);

	element->m_textCursorElement = yyGUICreatePictureBox(
		v4f(rect.x, rect.y + 1.f, rect.x + 1, rect.w - 1.f), 
		yyGetDefaultTexture(), -1, drawGroup);
	yyGUIRemoveElement(element->m_textCursorElement);
	element->m_textCursorElement->m_color = ColorWhite;

	element->SetDrawGroup(drawGroup);
	element->SetBuildRect(rect);

	return element;
}
