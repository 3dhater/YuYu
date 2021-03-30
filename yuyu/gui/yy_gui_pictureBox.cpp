#include "yy.h"
#include "yy_gui.h"
#include "math/vec.h"
#include "math/math.h"

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

yyGUIPictureBox::~yyGUIPictureBox(){
	auto vAPI = yyGetVideoDriverAPI();
	if( this->m_pictureBoxModel )
		vAPI->UnloadModel(this->m_pictureBoxModel);
	
	if( this->m_texture )
		vAPI->UnloadTexture(this->m_texture);
}

YY_API yyGUIPictureBox* YY_C_DECL yyGUICreatePictureBox(const v4f& rect, yyResource* texture, s32 id, yyGUIDrawGroup* drawGroup){
	yyGUIPictureBox* element = yyCreate<yyGUIPictureBox>();
	element->SetDrawGroup(drawGroup);
	element->m_activeAreaRect = rect;
	element->m_clipRect = rect;
	element->m_buildingRect = rect;
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
	vertex->m_position.set(element->m_buildingRect.x, element->m_buildingRect.w);
	vertex->m_tcoords.set(0.f, 1.f);
	vertex++;
	vertex->m_position.set(element->m_buildingRect.x, element->m_buildingRect.y);
	vertex->m_tcoords.set(0.f, 0.f);
	vertex++;
	vertex->m_position.set(element->m_buildingRect.z, element->m_buildingRect.y);
	vertex->m_tcoords.set(1.f, 0.f);
	vertex++;
	vertex->m_position.set(element->m_buildingRect.z, element->m_buildingRect.w);
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

	//g_engine->addGuiElement(element);

	return element;
}

void yyGUIPictureBox::OnUpdate(f32 dt){
	if (!m_visible) return;
	
	/*m_inRect = math::pointInRect(
		g_engine->m_inputContext->m_cursorCoordsForGUI.x,
		g_engine->m_inputContext->m_cursorCoordsForGUI.y,
		v4f(
			m_rect.x + m_offset.x,
			m_rect.y + m_offset.y,
			m_rect.z + m_offset.x,
			m_rect.w + m_offset.y
		)
	);*/
	yyGUIElement::CheckCursorInRect();

	if (m_ignoreInput) return;

	if (m_onClick && g_engine->m_inputContext->m_isLMBDown)
	{
		if (m_isInActiveAreaRect)
			m_onClick(this, m_id);
	}
}

void yyGUIPictureBox::OnDraw(){
	if (!m_visible) return;
	if (m_texture)
		g_engine->m_videoAPI->SetTexture(0, m_texture);
	if (m_pictureBoxModel)
		g_engine->m_videoAPI->SetModel(m_pictureBoxModel);
	g_engine->m_videoAPI->Draw();
}