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
	m_charLimit = 1000;
	m_clickCount = 0;
	m_defaultTextElement = 0;
	m_textCursorTimerLimit = 0.55f;
	m_textCursorPositionInChars = 0;
	m_onCharacter = 0;
	m_drawTextCursor = true;
	m_isSelected = false;
	m_textElement = 0;
	m_textCursorPositionWhenClick = 0;
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
	m_selectionStart = 0;
	m_selectionEnd = 0;
	m_selectColor.set(0.f, 0.f, 0.8f);
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
	//printf("m_textCursorPositionWhenClick %u\n", (u32)m_textCursorPositionWhenClick);

	if (m_isInActiveAreaRect)
	{
		if (g_engine->m_inputContext->m_isLMBDown)
		{
			_calculate_text_cursor_position_from_mouse();
			m_textCursorPositionWhenClick = m_textCursorPositionInChars;
			DeselectAll();
			_calculate_rects();
		}
	}
	else 
	{
		if (g_engine->m_inputContext->m_isLMBDown)
		{
			if (g_engine->m_guiElementInMouseFocus != this)
			{
				DeselectAll();
				this->_end_edit();
			}
		}
	}

	if ((g_engine->m_guiElementInMouseFocus == this) && (m_clickCount > 1))
	{
		if (g_engine->m_inputContext->m_isLMBHold)
		{
			yySetCursorDisableAutoChange(true);
			_calculate_text_cursor_position_from_mouse();
		
			if (g_engine->m_inputContext->m_mouseDelta.x != 0.f ||
				g_engine->m_inputContext->m_mouseDelta.y != 0.f)
			{
				if (m_textCursorPositionWhenClick != m_textCursorPositionInChars)
				{
					m_isSelected = true;
					m_selectionStart = m_textCursorPositionWhenClick;
					m_selectionEnd = m_textCursorPositionInChars;
					//printf("%i %i\n", m_selectionStart, m_selectionEnd);
				}
			}

			_calculate_rects();
		}
	}
	
	if (g_engine->m_inputContext->m_isLMBUp)
	{
		yySetCursorDisableAutoChange(false);
		yyGetCursor(yyCursorType::Arrow)->Activate();
	}

	if (m_isInActiveAreaRect && (!g_engine->m_guiElementInMouseFocus))
	{
		m_bgColorCurrent = m_bgColorHover;

		if (!g_engine->m_guiElementInMouseFocus)
			yyGetCursor(yyCursorType::IBeam)->Activate();

		if (m_onMouseInRect)
			m_onMouseInRect(this,m_id);

		if (g_engine->m_inputContext->m_isLMBDown)
		{
			g_engine->m_GUIElementInputFocus = this;
			g_engine->m_guiElementInMouseFocus = this;

			++m_clickCount; // 1,2,3,4...
			
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
			if (g_engine->m_inputContext->m_LMBClickCount > 1)
			{
				SelectAll();
			}

			if(g_engine->m_inputContext->IsKeyHit(yyKey::K_LEFT))
			{
				if (m_textCursorPositionInChars)
				{
					if (g_engine->m_inputContext->m_kbm == yyKeyboardModifier::Shift)
					{
						if ((m_selectionStart == 0) && (m_selectionEnd == 0))
						{
							m_isSelected = true;
							m_selectionStart = m_textCursorPositionInChars;
						}
						m_selectionEnd = m_textCursorPositionInChars - 1;
					}
					else
					{
						if (m_isSelected)
						{
							if (m_selectionStart < m_selectionEnd)
								m_textCursorPositionInChars = m_selectionStart + 1;
							else
								++m_textCursorPositionInChars;
							DeselectAll();
						}
					}

					--m_textCursorPositionInChars;
					m_textCursorPositionWhenClick = m_textCursorPositionInChars;
					_calculate_rects();
				}
				else
				{
					if (m_isSelected && g_engine->m_inputContext->m_kbm != yyKeyboardModifier::Shift)
						DeselectAll();
				}
			}else if (g_engine->m_inputContext->IsKeyHit(yyKey::K_RIGHT))
			{
				if (m_textCursorPositionInChars < text_size)
				{
					if (g_engine->m_inputContext->m_kbm == yyKeyboardModifier::Shift)
					{
						if ((m_selectionStart == 0) && (m_selectionEnd == 0))
						{
							m_isSelected = true;
							m_selectionStart = m_textCursorPositionInChars;
						}
						m_selectionEnd = m_textCursorPositionInChars + 1;
					}
					else
					{
						if (m_isSelected)
						{
							if (m_selectionStart > m_selectionEnd)
								m_textCursorPositionInChars = m_selectionStart - 1;
							else
								--m_textCursorPositionInChars;

							DeselectAll();
						}
					}

					++m_textCursorPositionInChars;
					m_textCursorPositionWhenClick = m_textCursorPositionInChars;
					_calculate_rects();
				}
				else
				{
					if (m_isSelected && g_engine->m_inputContext->m_kbm != yyKeyboardModifier::Shift)
						DeselectAll();
				}
			}
			else if (g_engine->m_inputContext->IsKeyHit(yyKey::K_DELETE)
				&& g_engine->m_inputContext->m_kbm != yyKeyboardModifier::Shift)
			{
				if ((m_textCursorPositionInChars < text_size))
				{
					if (m_isSelected)
					{
						DeleteSelected();
						_calculate_rects();
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
				else
				{
					if (m_isSelected)
					{
						DeleteSelected();
						_calculate_rects();
					}
				}
			}
			else if (g_engine->m_inputContext->IsKeyHit(yyKey::K_BACKSPACE) && m_textCursorPositionInChars)
			{
				if (m_isSelected)
				{
					DeleteSelected();
					_calculate_rects();
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
							_calculate_rects();
							buf[str_len - 1] = 0;
							m_textElement->m_text.setSize(m_textElement->m_text.size() - 1);
							m_textElement->Rebuild();
						}
					}
				}
			}
			else if (g_engine->m_inputContext->IsKeyHit(yyKey::K_HOME))
			{
				if (g_engine->m_inputContext->m_kbm == yyKeyboardModifier::Shift)
				{
					if ((m_selectionStart == 0) && (m_selectionEnd == 0))
					{
						m_isSelected = true;
						m_selectionStart = m_textCursorPositionInChars;
					}
					m_selectionEnd = 0;
				}
				else
				{
					if (m_isSelected)
						DeselectAll();
				}
				m_textCursorPositionInChars = 0;
				_calculate_rects();
			}
			else if (g_engine->m_inputContext->IsKeyHit(yyKey::K_END))
			{
				if (g_engine->m_inputContext->m_kbm == yyKeyboardModifier::Shift)
				{
					if ((m_selectionStart == 0) && (m_selectionEnd == 0))
					{
						m_isSelected = true;
						m_selectionStart = m_textCursorPositionInChars;
					}
					m_selectionEnd = m_textElement->m_text.size();
				}
				else
				{
					if (m_isSelected)
						DeselectAll();
				}
				m_textCursorPositionInChars = m_textElement->m_text.size();
				_calculate_rects();
			}
		}

		m_bgColorCurrent = m_bgColorActive;

		if (g_engine->m_inputContext->IsKeyHit(yyKey::K_ESCAPE))
		{
			g_engine->m_inputContext->m_key_hit[(u32)yyKey::K_ESCAPE] = 0;
			if (m_isSelected)
			{
				DeselectAll();
			}
			else
			{
				_end_edit();
			}
		}
		if (g_engine->m_inputContext->IsKeyHit(yyKey::K_ENTER))
		{
			g_engine->m_inputContext->m_key_hit[(u32)yyKey::K_ENTER] = 0;
			_end_edit();
		}
		
		
		if (g_engine->m_inputContext->m_kbm == yyKeyboardModifier::Ctrl)
		{
			if (g_engine->m_inputContext->IsKeyHit(yyKey::K_A))
				SelectAll();
			else if (g_engine->m_inputContext->IsKeyHit(yyKey::K_X))
				CutToClipboard();
			else if (g_engine->m_inputContext->IsKeyHit(yyKey::K_C))
				CopyToClipboard();
			else if (g_engine->m_inputContext->IsKeyHit(yyKey::K_V))
				PasteFromClipboard();
			else if (g_engine->m_inputContext->IsKeyHit(yyKey::K_INSERT))
				CopyToClipboard();
		}
		else if (g_engine->m_inputContext->m_kbm == yyKeyboardModifier::Shift)
		{
			if (g_engine->m_inputContext->IsKeyHit(yyKey::K_INSERT))
				PasteFromClipboard();
			if (g_engine->m_inputContext->IsKeyHit(yyKey::K_DELETE))
				CutToClipboard();
		}
		else if (g_engine->m_inputContext->m_character)
		{
			if (g_engine->m_inputContext->m_character != L'\n'
				&& g_engine->m_inputContext->m_character != 8 // backspace
				&& g_engine->m_inputContext->m_character != 13 // enter
				&& g_engine->m_inputContext->m_character != 27 // escape
				)
			{
				bool ok = true;
				//wprintf(L"%c %i\n", g_engine->m_inputContext->m_character,(s32)g_engine->m_inputContext->m_character);
				if (m_onCharacter)
				{
					if (!m_onCharacter(g_engine->m_inputContext->m_character))
						ok = false;
				}
				
				if (m_textElement->m_text.size() >= m_charLimit)
				{
					ok = false;
				}

				if (ok)
				{
					if (m_isSelected)
						DeleteSelected();

					m_textElement->m_text.insert(g_engine->m_inputContext->m_character, m_textCursorPositionInChars);
					m_textElement->SetBufferSize(m_textElement->m_text.capacity());
					m_textElement->Rebuild();

					++m_textCursorPositionInChars;
					_calculate_rects();
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
	_calculate_rects();
}
void yyGUITextInput::CutToClipboard() {
	if (!m_isSelected)
		return;
	CopyToClipboard();
	this->DeleteSelected();
	_calculate_rects();
}
void yyGUITextInput::CopyToClipboard() {
	if (!m_isSelected)
		return;
	yyStringW str;
	size_t s1 = m_selectionStart;
	size_t s2 = m_selectionEnd;
	if (s1 > s2)
	{
		s1 = s2;
		s2 = m_selectionStart;
	}
	auto buf = m_textElement->m_text.data();
	auto num_to_select = s2 - s1;
	for (size_t i = s1; i < s2; ++i)
	{
		str += buf[i];
	}
	yyCopyTextToClipboard(&str);
}
void yyGUITextInput::SetLimit(s32 l) {
	m_charLimit = l;
}
void yyGUITextInput::PasteFromClipboard() {
	this->DeleteSelected();
	yyStringW text;
	yyGetTextFromClipboard(&text);
	//yyLogWriteInfoW(L"Paste: %s\n", text.data());
	auto text_size = text.size();
	if (text_size)
	{
		if (m_textElement->m_text.size() + text_size >= m_charLimit)
		{
			return;
		}

		m_textElement->m_text.insert(text.data(), m_textCursorPositionInChars);
		m_textElement->SetBufferSize(m_textElement->m_text.capacity());
		m_textElement->Rebuild();
		m_textCursorPositionInChars += text_size;
	}
	_calculate_rects();
}
void yyGUITextInput::DeleteAll() {
	SelectAll();
	DeleteSelected();
}
void yyGUITextInput::DeleteSelected() {
	if (!m_isSelected)
		return;
	auto s1 = m_selectionStart;
	auto s2 = m_selectionEnd;
	if (s1 > s2)
	{
		s1 = s2;
		s2 = m_selectionStart;
	}
	//printf("Delete %i %i\n", s1, s2);
	auto num_to_delete = s2 - s1;
	auto str_len = m_textElement->m_text.size();
	auto buf = m_textElement->m_text.data();
	for (size_t i = s1; i < str_len; ++i)
	{
		buf[i] = buf[i + num_to_delete];
	}
	buf[str_len - num_to_delete] = 0;

	m_textCursorPositionInChars = s1;
	m_textElement->m_text.setSize(m_textElement->m_text.size() - num_to_delete);
	m_textElement->Rebuild();

	DeselectAll();
}
void yyGUITextInput::DeselectAll() {
	m_selectionStart = 0;
	m_selectionEnd = 0;
	m_isSelected = false;
	m_selectionRect.set(0.f);
}
void yyGUITextInput::SelectAll() {
	if (m_textElement->m_text.size())
	{
		m_textCursorPositionInChars = m_textElement->m_text.size();
		m_selectionStart = 0;
		m_selectionEnd = m_textCursorPositionInChars;
		m_isSelected = true;
		_calculate_rects();
	}
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

f32 yyGUITextInput::_get_text_width_in_pixels(size_t char_index) {
	f32 v = 0.f;
	for (size_t i = 0; i < char_index; ++i)
	{
		auto ch = m_textElement->m_text[i];
		v += m_textElement->m_font->GetGlyph(ch)->width;
	}
	return v;
}

void yyGUITextInput::_calculate_rects() {
	f32 width = _get_text_width_in_pixels(m_textCursorPositionInChars);

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

	if (m_isSelected)
	{
		m_selectionRect.y = m_buildRectInPixels.y;
		m_selectionRect.w = m_buildRectInPixels.w;

		s32 s1 = m_selectionStart;
		s32 s2 = m_selectionEnd;

		//printf("ST %i : SE %i\n", m_selectionStart, m_selectionEnd);

		if (m_selectionEnd < m_selectionStart)
		{
			s1 = m_selectionEnd;
			s2 = m_selectionStart;
		}

		f32 start = 0.f;
		if(m_selectionStart < m_selectionEnd)
			start = _get_text_width_in_pixels(m_selectionStart);
		else
			start = _get_text_width_in_pixels(m_selectionEnd);

		m_selectionRect.x = FLT_MAX;
		m_selectionRect.z = -FLT_MAX;
		f32 w = 0.f;
		f32 w1 = 0.f;
		for (s32 i = s1; i < s2; ++i)
		{
			auto ch = m_textElement->m_text[i];
			w += m_textElement->m_font->GetGlyph(ch)->width;
			
			f32 w2 = w1 + m_buildRectInPixels.x + m_horScroll + start;
			f32 w3 = w + m_buildRectInPixels.x + m_horScroll + start;
			if (w2 < m_selectionRect.x) m_selectionRect.x = w2;
			if (w3 > m_selectionRect.z) m_selectionRect.z = w3;

			w1 = w;
		}
		if (m_selectionRect.x < m_buildRectInPixels.x) m_selectionRect.x = m_buildRectInPixels.x;
		if (m_selectionRect.z > m_buildRectInPixels.z) m_selectionRect.z = m_buildRectInPixels.z;
	}
}

void yyGUITextInput::OnDraw(){
	if (!m_visible) return;


	g_engine->m_videoAPI->DrawRectangle(m_buildRectInPixels, m_bgColorCurrent, m_bgColorCurrent);
	if (m_isSelected)
	{
		g_engine->m_videoAPI->DrawRectangle(m_selectionRect, m_selectColor, m_selectColor);
	}
	if (m_textElement->m_text.size())
	{
		m_textElement->OnDraw();
	}
	else
	{
		if (m_defaultTextElement)
		{
			if (m_defaultTextElement->m_text.size())
			{
				m_defaultTextElement->OnDraw();
			}
		}
	}
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
	
	if (m_defaultTextElement)
	{
		m_defaultTextElement->SetBuildRect(m_buildRectInPixels);
		m_defaultTextElement->Rebuild();
	}

	m_textElement->SetBuildRect(m_buildRectInPixels);
	m_textElement->Rebuild();

	m_textCursorElement->SetBuildRect(v4f(m_buildRectInPixels.x, m_buildRectInPixels.y + 1.f, m_buildRectInPixels.x + 1, m_buildRectInPixels.w - 1.f));
	m_textCursorElement->Rebuild();
}

void yyGUITextInput::SetFont(yyGUIFont* newFont) {
	m_textElement->SetFont(newFont);
	if (m_defaultTextElement)
	{
		m_defaultTextElement->SetFont(newFont);
	}
}

void yyGUITextInput::UseDefaultText(const wchar_t* text, const yyColor& c) {
	if (!m_defaultTextElement)
	{
		m_defaultTextElement = yyGUICreateText(v2f(m_buildRectInPixels.x, m_buildRectInPixels.y), m_textElement->m_font, text, m_textElement->m_drawGroup);
		yyGUIRemoveElement(m_defaultTextElement);
	}
	else
	{
		m_defaultTextElement->SetText(text);
	}
	m_defaultTextElement->m_color = c;
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
