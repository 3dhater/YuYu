#include "yy.h"
#include "scene/common.h"
#include "scene/sprite.h"
#include "yy_model.h"

#include "engine.h"
extern Engine * g_engine;

YY_FORCE_INLINE v4f yySpriteMovePivotOnCenter(const v4f& rect)
{
	f32 w = (rect.z - rect.x) * 0.5f;
	f32 h = (rect.w - rect.y) * 0.5f;
	return v4f(
		rect.x - w,
		rect.y - h,
		rect.z - w,
		rect.w - h
	);
}

extern "C"
{

YY_API yySprite* YY_C_DECL yyCreateSprite(const v4f& r, yyResource* texture, bool pivotOnCenter)
{
	yySprite* newSprite = yyCreate<yySprite>();

	auto rect = r;
	if(pivotOnCenter)
		rect = yySpriteMovePivotOnCenter(r);

	auto vAPI = yyGetVideoDriverAPI();
	auto model = yyCreate<yyModel>();

	model->m_iCount = 6;
	model->m_vCount = 4;
	model->m_stride = sizeof(yyVertexGUI);
	model->m_vertexType = yyVertexType::GUI;
	model->m_vertices = (u8*)yyMemAlloc(model->m_vCount * model->m_stride);
	model->m_indices  = (u8*)yyMemAlloc(model->m_iCount * sizeof(u16));

	u16* inds = (u16*)model->m_indices;

	yyVertexGUI * vertex = (yyVertexGUI*)model->m_vertices;
	vertex->m_position.set(rect.x, rect.w);
	vertex->m_tcoords.set(0.f,1.f);
	vertex++;
	vertex->m_position.set(rect.x, rect.y);
	vertex->m_tcoords.set(0.f,0.f);
	vertex++;
	vertex->m_position.set(rect.z, rect.y);
	vertex->m_tcoords.set(1.f,0.f);
	vertex++;
	vertex->m_position.set(rect.z, rect.w);
	vertex->m_tcoords.set(1.f,1.f);
	vertex++;

	inds[0] = 0;
	inds[1] = 1;
	inds[2] = 2;
	inds[3] = 0;
	inds[4] = 2;
	inds[5] = 3;

	newSprite->m_model = vAPI->CreateModel(model);
	newSprite->m_texture = texture;

	yyDestroy(model);

	return newSprite;
}

}
