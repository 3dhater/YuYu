#ifndef _YUYU_SCENE_SPRITE_H_
#define _YUYU_SCENE_SPRITE_H_


void yySprite_update(void * impl);
struct yySprite
{
	yySprite()
	{
		m_objectBase.m_objectType = yySceneObjectBase::ObjectType::Sprite;
		m_objectBase.m_implementationPtr = this;
		m_objectBase.m_updateImplementation = yySprite_update;
	}
	~yySprite(){}

	yySceneObjectBase m_objectBase;

	yyResource* m_texture = nullptr;
	yyResource* m_model   = nullptr;
};

YY_FORCE_INLINE void yySprite_update(void * impl)
{
	//auto sprite = (yySprite*)impl;
}

#endif