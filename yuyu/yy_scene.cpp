﻿#include "yy.h"
#include "scene/common.h"
#include "scene/sprite.h"
#include "scene/sprite2.h"
#include "yy_model.h"

#include "engine.h"
extern Engine * g_engine;

extern "C"
{

YY_API yySprite* YY_C_DECL yyCreateSprite(const v4f& r, yyResource* texture, u8 pivotPosition)
{
	auto rect = yySpriteGetNewPivot(r, pivotPosition);
	yySprite* newSprite = yyCreate<yySprite>();
	auto vAPI = yyGetVideoDriverAPI();
	auto model = yySpriteCreateNew(rect, v2f(0.f,0.f), v2f(1.f,1.f));
	newSprite->m_model = vAPI->CreateModel(model);
	newSprite->m_texture = texture;
	yyDestroy(model);
	return newSprite;
}

YY_API yySprite2* YY_C_DECL yyCreateSprite2(yyResource* texture)
{
	yySprite2* newSprite = yyCreate<yySprite2>();
	newSprite->m_texture = texture;
	return newSprite;
}

}
