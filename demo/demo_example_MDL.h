#ifndef __DEMO_EXAMPLE_MDL_H__
#define __DEMO_EXAMPLE_MDL_H__

#include "scene\fly_camera.h"

class DemoExample_MDL : public DemoExample
{
public:
	DemoExample_MDL();
	virtual ~DemoExample_MDL();

	virtual bool Init();
	virtual void Shutdown();
	virtual const wchar_t* GetTitle();
	virtual const wchar_t* GetDescription();

	virtual bool DemoStep(f32 deltaTime);

	yyMDL* m_mdl_struct;

	yyFlyCamera* m_flyCamera;
};

#endif

