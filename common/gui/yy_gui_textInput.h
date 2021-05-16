#ifndef _YUYU_GUI_TEXTINPUT_H_
#define _YUYU_GUI_TEXTINPUT_H_

class yyGUITextInput : public yyGUIElement
{
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
	/*yyStringW m_text;
	yyGUIFont* m_font;
	u16 m_textureCount;
	v2f m_position;
	wchar_t* m_buffer;
	u32 m_bufferSize;
	yyGUITextDrawNode* m_drawNodes;*/

	yyGUICallback m_onMouseInRect;
	yyGUICallback m_onClick; // LMB down
};

#endif