#ifndef _YUYU_GUI_H_
#define _YUYU_GUI_H_

#include "math/vec.h"

using yyGUICallback = void(*)(s32 m_id);

class yyGUIElement
{
public:
	yyGUIElement(){}
	virtual ~yyGUIElement(){}
	
	s32 m_id = -1;

	v4i m_rect;
	v2i m_offset;

	bool m_visible = true;

};

class yyGUIPictureBox : public yyGUIElement
{
public:
	yyGUIPictureBox();
	virtual ~yyGUIPictureBox();

	yyResource* m_texture = nullptr;
	yyResource* m_pictureBoxMesh = nullptr; // yyResourceType::Mesh
};

extern "C"
{

	YY_API void YY_C_DECL yyGUIDrawAll();
	YY_API yyGUIPictureBox* YY_C_DECL yyGUICreatePictureBox(const v4i& rect, yyResource* texture, s32 id);
}

#endif