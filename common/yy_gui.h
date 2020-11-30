#ifndef _YUYU_GUI_H_
#define _YUYU_GUI_H_

#include "math/vec.h"

using yyGUICallback = void(*)(yyGUIElement* elem, s32 m_id);

enum class yyGUIElementType  : u32
{
	Unknown,
	PictureBox
};

class yyGUIElement
{
public:
	yyGUIElement(){}
	virtual ~yyGUIElement(){}
	
	s32 m_id = -1;

	v4f m_rect;

	bool m_visible = true;

	yyGUIElementType m_type = yyGUIElementType::Unknown;
};

class yyGUIPictureBox : public yyGUIElement
{
public:
	yyGUIPictureBox();
	virtual ~yyGUIPictureBox();

	yyResource* m_texture = nullptr;
	yyResource* m_pictureBoxModel = nullptr; // yyResourceType::Model

	yyGUICallback m_onClick = nullptr; // LMB down
};

extern "C"
{
	YY_API void YY_C_DECL yyGUIUpdate(f32 deltaTime);
	YY_API void YY_C_DECL yyGUIDrawAll();
	YY_API yyGUIPictureBox* YY_C_DECL yyGUICreatePictureBox(const v4f& rect, yyResource* texture, s32 id);
}

#endif