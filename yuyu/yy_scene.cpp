#include "yy.h"
#include "scene/common.h"
#include "scene/sprite.h"
#include "scene/sprite2.h"
#include "yy_model.h"

#include "engine.h"
extern Engine * g_engine;

extern "C"
{

YY_API yySprite* YY_C_DECL yyCreateSprite(const v4f& r, yyResource* texture, u8 pivotPosition){
	YY_DEBUG_PRINT_FUNC;
	auto rect = yySpriteGetNewPivot(r, pivotPosition);
	yySprite* newSprite = yyCreate<yySprite>();
	auto vAPI = yyGetVideoDriverAPI();
	auto model = yySpriteCreateNew(rect, v2f(0.f,0.f), v2f(1.f,1.f));
	newSprite->m_model = yyCreateModel(model);
	newSprite->m_model->Load();
	newSprite->m_texture = texture;
	if (!newSprite->m_texture->IsLoaded())
		newSprite->m_texture->Load();
	yyMegaAllocator::Destroy(model);

	yyLogWriteInfo("Create sprite\n");

	return newSprite;
}

YY_API yySprite2* YY_C_DECL yyCreateSprite2(yyResource* texture){
	YY_DEBUG_PRINT_FUNC;
	yySprite2* newSprite = yyCreate<yySprite2>();
	newSprite->m_texture = texture;
	if (!newSprite->m_texture->IsLoaded())
		newSprite->m_texture->Load();
	yyLogWriteInfo("Create sprite2\n");
	return newSprite;
}

}
