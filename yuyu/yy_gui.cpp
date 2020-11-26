#include "yy.h"
#include "yy_gui.h"
#include "math/vec.h"

#include "engine.h"

extern Engine * g_engine;

yyGUIPictureBox::yyGUIPictureBox()
{
}

yyGUIPictureBox::~yyGUIPictureBox()
{
}

YY_API yyGUIPictureBox* YY_C_DECL yyGUICreatePictureBox(const v4i& rect, yyResource* texture, s32 id)
{
	yyGUIPictureBox* element = new yyGUIPictureBox;
	element->m_rect = rect;
	element->m_texture = texture;
	element->m_id = id;

	g_engine->addGuiElement(element);
	//g_engine->m_videoAPI->CreateMesh();

	return element;
}

YY_API void YY_C_DECL yyGUIDrawAll()
{
}

