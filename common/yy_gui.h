#ifndef _YUYU_GUI_H_
#define _YUYU_GUI_H_

#include "math/vec.h"

typedef void(*yyGUICallback)(yyGUIElement* elem, s32 m_id);

enum class yyGUIElementType  : u32
{
	Unknown,
	PictureBox
};

class yyGUIElement
{
public:
	yyGUIElement()
	:
		m_id(-1),
		m_visible(true),
		m_type(yyGUIElementType::Unknown)
	{}
	virtual ~yyGUIElement(){}
	
	s32 m_id;
	bool m_visible;
	yyGUIElementType m_type;
	v4f m_rect;
};

class yyGUIPictureBox : public yyGUIElement
{
public:
	yyGUIPictureBox();
	virtual ~yyGUIPictureBox();

	yyResource* m_texture;
	yyResource* m_pictureBoxModel; // yyResourceType::Model

	yyGUICallback m_onClick; // LMB down
};

extern "C"
{
	YY_API void YY_C_DECL yyGUIUpdate(f32 deltaTime);
	YY_API void YY_C_DECL yyGUIDrawAll();
	YY_API yyGUIPictureBox* YY_C_DECL yyGUICreatePictureBox(const v4f& rect, yyResource* texture, s32 id);
}

#endif