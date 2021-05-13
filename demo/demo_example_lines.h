#ifndef __DEMO_EXAMPLE_LINES_H__
#define __DEMO_EXAMPLE_LINES_H__

#include "scene\orbit_camera.h"
#include "yy_material.h"
class DemoExample_Lines : public DemoExample
{
	yyResource* m_lineModel;
	yyResource* m_pointModel;
	yyMaterial m_lineModelMaterial;
	yyOrbitCamera* m_editorCamera;
public:
	DemoExample_Lines();
	virtual ~DemoExample_Lines();

	virtual bool Init();
	virtual void Shutdown();
	virtual const wchar_t* GetTitle();
	virtual const wchar_t* GetDescription();

	virtual bool DemoStep(f32 deltaTime);
};

#endif

