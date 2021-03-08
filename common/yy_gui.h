﻿#ifndef _YUYU_GUI_H_
#define _YUYU_GUI_H_

#include "math/vec.h"
#include "yy_color.h"

typedef void(*yyGUICallback)(yyGUIElement* elem, s32 m_id);

enum class yyGUIElementType  : u32
{
	Unknown,
	PictureBox,
	Button,
	Text
};

class yyGUIElement
{
protected:
	bool m_visible;
	bool m_inRect;
	bool m_ignoreInput;
public:

	yyGUIElement()
	:
		m_id(-1),
		m_visible(true),
		m_ignoreInput(false),
		m_type(yyGUIElementType::Unknown)
	{
		m_color = ColorWhite;
	}
	virtual ~yyGUIElement(){}
	
	virtual void SetOffset(const v2f& o) { m_offset = o; }
	virtual void SetVisible(bool v) { m_visible = v; }
	virtual void IgnoreInput(bool v) { m_ignoreInput = v; }
	bool IsVisible() { return m_visible; }
	bool IsInRect() { return m_inRect; }

	virtual void OnUpdate() = 0;
	virtual void OnDraw() = 0;

	s32 m_id;
	yyGUIElementType m_type;
	v4f m_rect;
	v2f m_offset;
	yyColor m_color;
};

#include "gui\yy_gui_font.h"

#include "gui\yy_gui_pictureBox.h"
#include "gui\yy_gui_text.h"

#include "gui\yy_gui_button.h"


extern "C"
{
	YY_API void YY_C_DECL yyGUIUpdate(f32 deltaTime);
	YY_API void YY_C_DECL yyGUIDrawAll();
	YY_API yyGUIPictureBox* YY_C_DECL yyGUICreatePictureBox(const v4f& rect, yyResource* texture, s32 id);
	YY_API yyGUIFont* YY_C_DECL yyGUILoadFont(const char* path);
	YY_API yyGUIText* YY_C_DECL yyGUICreateText(const v2f& position, yyGUIFont* font, const wchar_t* text);
	YY_API yyGUIButton* YY_C_DECL yyGUICreateButton(const v4f& rect, yyResource* baseTexture, s32 id);
	YY_API void YY_C_DECL yyGUIDeleteElement(yyGUIElement* elem);
	YY_API void YY_C_DECL yyGUIRemoveElement(yyGUIElement* elem); // without yyDestroy(elem);
	YY_API yyGUIElement* YY_C_DECL yyGUIGetElementInMouseFocus();
}

#endif