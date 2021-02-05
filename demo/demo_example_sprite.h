#ifndef __DEMO_EXAMPLE_SPRITE_H__
#define __DEMO_EXAMPLE_SPRITE_H__

class DemoExample_Sprite : public DemoExample
{
	yySprite* m_spriteLevel;
	v2f* m_spriteCameraPosition;
	v2f* m_spriteCameraScale;

public:
	DemoExample_Sprite();
	virtual ~DemoExample_Sprite();

	virtual bool Init();
	virtual void Shutdown();
	virtual const wchar_t* GetTitle();
	virtual const wchar_t* GetDescription();

	virtual void DemoStep(f32 deltaTime);
};

#endif

