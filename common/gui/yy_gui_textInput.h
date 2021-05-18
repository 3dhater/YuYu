#ifndef _YUYU_GUI_TEXTINPUT_H_
#define _YUYU_GUI_TEXTINPUT_H_

class yyGUITextInput : public yyGUIElement
{
	float m_textCursorTimer;
	float m_textCursorTimerLimit;
	bool m_drawTextCursor;
	size_t m_textCursorPositionInChars;
	void _calculate_text_cursor_rect();
	void _calculate_text_cursor_position_in_chars();
	f32 _get_text_cursor_position_in_pixels();
	f32 m_horScroll;
	bool m_isSelected;
	void _delete_selected();

public:
	yyGUITextInput();
	virtual ~yyGUITextInput();
	virtual void OnUpdate(f32 dt);
	virtual void OnDraw();
	virtual void Rebuild();

	virtual void SetFont(yyGUIFont*);
	virtual void SetText(const wchar_t* text, ...);
	virtual void Clear();
	virtual void SetBufferSize(u32 newSize);

	yyGUIText* m_textElement;
	yyGUIPictureBox* m_bgElement;
	yyGUIPictureBox* m_textCursorElement;
	/*yyStringW m_text;
	yyGUIFont* m_font;
	u16 m_textureCount;
	v2f m_position;
	wchar_t* m_buffer;
	u32 m_bufferSize;
	yyGUITextDrawNode* m_drawNodes;*/

	yyColor m_bgColor;
	yyColor m_bgColorHover;
	yyColor m_bgColorActive;

	yyGUICallback m_onClickLMB;
	yyGUICallback m_onClickMMB;
	yyGUICallback m_onClickRMB;
	yyGUICallback m_onClickX1MB;
	yyGUICallback m_onClickX2MB;

	bool(*m_onCharacter)(wchar_t);
};

#endif