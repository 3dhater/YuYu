#ifndef __DEMO_EXAMPLE_SPRITE_H__
#define __DEMO_EXAMPLE_SPRITE_H__

class DemoExample_Sprite : public DemoExample
{
	yyResource* m_textureLevel;
	yySprite* m_spriteLevel;
	yySprite* m_spriteHero;
	v2f* m_spriteCameraPosition;
	v2f* m_spriteCameraScale;

public:
	DemoExample_Sprite();
	virtual ~DemoExample_Sprite();

	virtual bool Init();
	virtual void Shutdown();
	virtual const wchar_t* GetTitle();
	virtual const wchar_t* GetDescription();

	virtual bool DemoStep(f32 deltaTime);
};

#endif

