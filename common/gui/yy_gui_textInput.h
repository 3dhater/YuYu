﻿#ifndef _YUYU_GUI_TEXTINPUT_H_
#define _YUYU_GUI_TEXTINPUT_H_

class yyGUITextInput : public yyGUIElement
{
	float m_textCursorTimer;
	float m_textCursorTimerLimit;
	bool m_drawTextCursor;
	size_t m_textCursorPositionInChars;
	size_t m_textCursorPositionWhenClick;
	void _calculate_rects();
	void _calculate_text_cursor_position_from_mouse();
	f32 _get_text_width_in_pixels(size_t char_index);
	f32 m_horScroll;
	bool m_isSelected;
	bool m_isActivated;
	
	u8 m_clickCount;
	void _end_edit(bool isEnter);

	
	s32 m_selectionStart;
	s32 m_selectionEnd;
	v4f m_selectionRect;

	u32 m_charLimit;
public:
	yyGUITextInput();
	virtual ~yyGUITextInput();
	virtual void OnUpdate(f32 dt);
	virtual void OnDraw(f32 dt);
	virtual void Rebuild();

	virtual void SetFont(yyGUIFont*);
	virtual void SetText(const wchar_t* text, ...);
	virtual void Clear();
	virtual void SetBufferSize(u32 newSize);
	
	virtual void SelectAll();
	virtual void DeselectAll();
	virtual void DeleteAll();
	virtual void DeleteSelected();
	virtual void CutToClipboard();
	virtual void CopyToClipboard();
	virtual void PasteFromClipboard();
	
	virtual void SetLimit(s32); // default 1000

	virtual void Activate();
	virtual bool IsActivated();

	yyGUIText* m_textElement;
	yyGUIText* m_defaultTextElement;
	//yyGUIPictureBox* m_bgElement;

	yyGUIPictureBox* m_textCursorElement;
	/*yyStringW m_text;
	yyGUIFont* m_font;
	u16 m_textureCount;
	v2f m_position;
	wchar_t* m_buffer;
	u32 m_bufferSize;
	yyGUITextDrawNode* m_drawNodes;*/

	bool m_IBeamOnlyWhenActivated; // false

	yyColor m_bgColorCurrent;
	yyColor m_bgColor;
	yyColor m_bgColorHover;
	yyColor m_bgColorActive;
	yyColor m_selectColor;

	yyGUICallback m_onClickLMB;
	yyGUICallback m_onClickMMB;
	yyGUICallback m_onClickRMB;
	yyGUICallback m_onClickX1MB;
	yyGUICallback m_onClickX2MB;

	bool(*m_onCharacter)(wchar_t);
	yyGUICallback m_onEscape;
	yyGUICallback m_onEnter;

	virtual void UseDefaultText(const wchar_t*, const yyColor&);
};

#endif