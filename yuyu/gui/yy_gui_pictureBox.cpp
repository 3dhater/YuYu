﻿#include "yy.h"
#include "yy_gui.h"
#include "math/vec.h"

#include "yy_model.h"
#include "yy_input.h"

#include "../engine.h"

extern Engine * g_engine;

yyGUIPictureBox::yyGUIPictureBox()
	:
	m_texture(nullptr),
	m_pictureBoxModel(nullptr),
	m_onClick(nullptr)
{
	m_type = yyGUIElementType::PictureBox;
}

yyGUIPictureBox::~yyGUIPictureBox()
{
	auto vAPI = yyGetVideoDriverAPI();
	if( this->m_pictureBoxModel )
		vAPI->UnloadModel(this->m_pictureBoxModel);
	
	if( this->m_texture )
		vAPI->UnloadTexture(this->m_texture);
}

YY_API yyGUIPictureBox* YY_C_DECL yyGUICreatePictureBox(const v4f& rect, yyResource* texture, s32 id)
{
	yyGUIPictureBox* element = yyCreate<yyGUIPictureBox>();
	element->m_rect = rect;
	element->m_texture = texture;
	element->m_id = id;


	auto vAPI = yyGetVideoDriverAPI();

	auto model = yyCreate<yyModel>();


	model->m_iCount = 6;
	model->m_vCount = 4;
	model->m_stride = sizeof(yyVertexGUI);
	model->m_vertexType = yyVertexType::GUI;
	model->m_vertices = (u8*)yyMemAlloc(model->m_vCount * model->m_stride);
	model->m_indices = (u8*)yyMemAlloc(model->m_iCount * sizeof(u16));
	u16* inds = (u16*)model->m_indices;

	yyVertexGUI * vertex = (yyVertexGUI*)model->m_vertices;
	vertex->m_position.set(rect.x, rect.w);
	vertex->m_tcoords.set(0.f, 1.f);
	vertex++;
	vertex->m_position.set(rect.x, rect.y);
	vertex->m_tcoords.set(0.f, 0.f);
	vertex++;
	vertex->m_position.set(rect.z, rect.y);
	vertex->m_tcoords.set(1.f, 0.f);
	vertex++;
	vertex->m_position.set(rect.z, rect.w);
	vertex->m_tcoords.set(1.f, 1.f);
	vertex++;

	inds[0] = 0;
	inds[1] = 1;
	inds[2] = 2;
	inds[3] = 0;
	inds[4] = 2;
	inds[5] = 3;

	element->m_pictureBoxModel = vAPI->CreateModel(model);
	yyDestroy(model);

	g_engine->addGuiElement(element);

	return element;
}