#ifndef __DEMO_EXAMPLE_BILL_H__
#define __DEMO_EXAMPLE_BILL_H__

#include "scene\orbit_camera.h"
#include "scene\sprite.h"
#include "yy_material.h"
class DemoExample_Billboards : public DemoExample
{
	yyOrbitCamera* m_editorCamera;

	yyResource* m_textureLevel;
	yySprite* m_spriteLevel;

public:
	DemoExample_Billboards();
	virtual ~DemoExample_Billboards();

	virtual bool Init();
	virtual void Shutdown();
	virtual const wchar_t* GetTitle();
	virtual const wchar_t* GetDescription();

	virtual bool DemoStep(f32 deltaTime);
};

#endif

