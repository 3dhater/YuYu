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
		yyMegaAllocator::Destroy(this->m_pictureBoxModel);
	
	// do not delete texture here
	// because it can be from cache
	/*if( this->m_texture )
		yyMegaAllocator::Destroy(this->m_texture);*/
}

yyResource* yyGUIPictureBox::DropTexture() {
	auto t = this->m_texture;
	this->m_texture = 0;
	return t;
}

void yyGUIPictureBox::Rebuild() {
	yyGUIElement::CallOnRebuildSetRects();
	auto vAPI = yyGetVideoDriverAPI();

	if (this->m_pictureBoxModel)
		yyMegaAllocator::Destroy(this->m_pictureBoxModel);

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
		v2f tsz;
		m_texture->GetTextureSize(&tsz);
		f32 mulX = 1.f / (f32)tsz.x;
		f32 mulY = 1.f / (f32)tsz.y;

		lt.x = m_uvRect.x * mulX;
		lt.y = m_uvRect.y * mulY;
		rb.x = m_uvRect.z * mulX;
		rb.y = m_uvRect.w * mulY;
	}

	yyVertexGUI * vertex = (yyVertexGUI*)model->m_vertices;
	vertex->m_position.set(m_buildRectInPixels.x, m_buildRectInPixels.w);
	vertex->m_tcoords.set(lt.x, rb.y);
	vertex++;
	vertex->m_position.set(m_buildRectInPixels.x, m_buildRectInPixels.y);
	vertex->m_tcoords.set(lt.x, lt.y);
	vertex++;
	vertex->m_position.set(m_buildRectInPixels.z, m_buildRectInPixels.y);
	vertex->m_tcoords.set(rb.x, lt.y);
	vertex++;
	vertex->m_position.set(m_buildRectInPixels.z, m_buildRectInPixels.w);
	vertex->m_tcoords.set(rb.x, rb.y);
	vertex++;

	inds[0] = 0;
	inds[1] = 1;
	inds[2] = 2;
	inds[3] = 0;
	inds[4] = 2;
	inds[5] = 3;

	m_pictureBoxModel = yyCreateModel(model);
	m_pictureBoxModel->Load();
	yyDestroy(model);
}


YY_API yyGUIPictureBox* YY_C_DECL yyGUICreatePictureBox(const v4f& rect, yyResource* texture, s32 id, yyGUIDrawGroup* drawGroup, v4i* uv){
	assert(texture);
	yyGUIPictureBox* element = yyCreate<yyGUIPictureBox>();
	element->SetDrawGroup(drawGroup);
	element->SetBuildRect(rect);
	element->m_texture = texture;
	element->m_id = id;

	if (!element->m_texture->IsLoaded())
		element->m_texture->Load();

	if (uv)
	{
		element->m_uvRect = *uv;
	}
	else
	{
		auto gpu = yyGetVideoDriverAPI();
		v2f s;
		texture->GetTextureSize(&s);
		element->m_uvRect.z = (s32)s.x;
		element->m_uvRect.w = (s32)s.y;
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