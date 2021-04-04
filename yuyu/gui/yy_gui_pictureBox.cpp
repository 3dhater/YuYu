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
		vAPI->DeleteModel(this->m_pictureBoxModel);
	
	if( this->m_texture )
		vAPI->UnloadTexture(this->m_texture);
}

yyResource* yyGUIPictureBox::DropTexture() {
	auto t = this->m_texture;
	this->m_texture = 0;
	return t;
}

void yyGUIPictureBox::Rebuild() {
	auto vAPI = yyGetVideoDriverAPI();

	if (this->m_pictureBoxModel)
		vAPI->DeleteModel(this->m_pictureBoxModel);

	auto model = yyCreate<yyModel>();

	model->m_iCount = 6;
	model->m_vCount = 4;
	model->m_stride = sizeof(yyVertexGUI);
	model->m_vertexType = yyVertexType::GUI;
	model->m_vertices = (u8*)yyMemAlloc(model->m_vCount * model->m_stride);
	model->m_indices = (u8*)yyMemAlloc(model->m_iCount * sizeof(u16));
	u16* inds = (u16*)model->m_indices;

	v2f lt, rb;
	{
		v2i tsz;
		vAPI->GetTextureSize(m_texture, &tsz);
		f32 mulX = 1.f / (f32)tsz.x;
		f32 mulY = 1.f / (f32)tsz.y;

		lt.x = m_uvRect.x * mulX;
		lt.y = m_uvRect.y * mulY;
		rb.x = m_uvRect.z * mulX;
		rb.y = m_uvRect.w * mulY;
	}

	yyVertexGUI * vertex = (yyVertexGUI*)model->m_vertices;
	vertex->m_position.set(m_buildingRect_global.x, m_buildingRect_global.w);
	vertex->m_tcoords.set(lt.x, rb.y);
	vertex++;
	vertex->m_position.set(m_buildingRect_global.x, m_buildingRect_global.y);
	vertex->m_tcoords.set(lt.x, lt.y);
	vertex++;
	vertex->m_position.set(m_buildingRect_global.z, m_buildingRect_global.y);
	vertex->m_tcoords.set(rb.x, lt.y);
	vertex++;
	vertex->m_position.set(m_buildingRect_global.z, m_buildingRect_global.w);
	vertex->m_tcoords.set(rb.x, rb.y);
	vertex++;

	inds[0] = 0;
	inds[1] = 1;
	inds[2] = 2;
	inds[3] = 0;
	inds[4] = 2;
	inds[5] = 3;

	m_pictureBoxModel = vAPI->CreateModel(model);
	yyDestroy(model);
}


YY_API yyGUIPictureBox* YY_C_DECL yyGUICreatePictureBox(const v4f& rect, yyResource* texture, s32 id, yyGUIDrawGroup* drawGroup, v4i* uv){
	assert(texture);
	yyGUIPictureBox* element = yyCreate<yyGUIPictureBox>();
	element->SetDrawGroup(drawGroup);
	element->m_activeAreaRect = rect;
	element->m_clipRect = rect;
	element->m_buildingRect = rect;
	element->m_activeAreaRect_global = element->m_activeAreaRect;
	element->m_clipRect_global = element->m_clipRect;
	element->m_buildingRect_global = element->m_buildingRect;
	element->m_texture = texture;
	element->m_id = id;

	if (uv)
	{
		element->m_uvRect = *uv;
	}
	else
	{
		auto gpu = yyGetVideoDriverAPI();
		v2i s;
		gpu->GetTextureSize(texture, &s);
		element->m_uvRect.z = s.x;
		element->m_uvRect.w = s.y;
	}
	element->Rebuild();

	return element;
}

void yyGUIPictureBox::OnUpdate(f32 dt){
	if (!m_visible) return;
	
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
	g_engine->m_videoAPI->SetGUIShaderData(this);
	if (m_texture)
		g_engine->m_videoAPI->SetTexture(0, m_texture);
	if (m_pictureBoxModel)
		g_engine->m_videoAPI->SetModel(m_pictureBoxModel);
	g_engine->m_videoAPI->Draw();
}