#ifndef __DEMO_EXAMPLE_H__
#define __DEMO_EXAMPLE_H__

// Base class for all examples
class DemoExample
{
public:
	DemoExample();
	virtual ~DemoExample();

	virtual bool Init() = 0;
	virtual void Shutdown() = 0;

	virtual const wchar_t* GetTitle() = 0;
	virtual const wchar_t* GetDescription() = 0;

	virtual bool DemoStep(f32 deltaTime) = 0;

	yyVideoDriverAPI* m_gpu;
	yyGUIText* m_guiTextTitle;
};

#endif

