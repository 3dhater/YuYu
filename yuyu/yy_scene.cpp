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

YY_API yySceneObjectBase* YY_C_DECL yySceneGetRootObject()
{
	return g_engine->m_sceneRootObject;
}
YY_API yyCamera* YY_C_DECL yySceneGetActiveCamera()
{
	return g_engine->m_sceneActiveCamera;
}
YY_API void YY_C_DECL yySceneSetActiveCamera(yyCamera* c)
{
	g_engine->m_sceneActiveCamera = c;
}

YY_API yySprite* YY_C_DECL yyCreateSprite(const v4f& r, yyResource* texture, bool pivotOnCenter)
{
	yySprite* newSprite = yyCreate<yySprite>();

	auto rect = r;
	if(pivotOnCenter)
		rect = yySpriteMovePivotOnCenter(r);

	auto vAPI = yyGetVideoDriverAPI();
	auto model = yyCreate<yyModel>();
	auto meshBuffer = yyCreate<yyMeshBuffer>();

	meshBuffer->m_iCount = 6;
	meshBuffer->m_vCount = 4;
	meshBuffer->m_stride = sizeof(yyVertexGUI);
	meshBuffer->m_vertexType = yyVertexType::GUI;
	meshBuffer->m_vertices = (u8*)yyMemAlloc(meshBuffer->m_vCount * meshBuffer->m_stride);
	meshBuffer->m_indices  = (u8*)yyMemAlloc(meshBuffer->m_iCount * sizeof(u16));

	u16* inds = (u16*)meshBuffer->m_indices;

	yyVertexGUI * vertex = (yyVertexGUI*)meshBuffer->m_vertices;
	vertex->m_position.set(rect.x, rect.w, 0.f);
	vertex->m_tcoords.set(0.f,1.f);
	vertex++;
	vertex->m_position.set(rect.x, rect.y, 0.f);
	vertex->m_tcoords.set(0.f,0.f);
	vertex++;
	vertex->m_position.set(rect.z, rect.y, 0.f);
	vertex->m_tcoords.set(1.f,0.f);
	vertex++;
	vertex->m_position.set(rect.z, rect.w, 0.f);
	vertex->m_tcoords.set(1.f,1.f);
	vertex++;

	inds[0] = 0;
	inds[1] = 1;
	inds[2] = 2;
	inds[3] = 0;
	inds[4] = 2;
	inds[5] = 3;

	model->m_meshBuffers.push_back(meshBuffer);
	newSprite->m_model = vAPI->CreateModel(model);
	newSprite->m_texture = texture;

	yyDestroy(model);

	return newSprite;
}

}
