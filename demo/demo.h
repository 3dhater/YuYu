#ifndef __DEMO_H__
#define __DEMO_H__

#include "yy.h"

// for auto create\delete
struct yyEngineContext
{
	yyEngineContext()
	{
		m_state = nullptr;
	}
	~yyEngineContext()
	{
		yyStop(); // destroy main class, free memory
	}

	void init(yyInputContext* input)
	{
		m_state = yyStart(input); // allocate memory for main class inside yuyu.dll
	}

	yySystemState * m_state;
};

class Demo
{
	yyInputContext * m_inputContext;
	yyEngineContext* m_engineContext;
	yyWindow*        m_window;

	yyVideoDriverAPI* m_gpu;
	yyGUIFont*        m_defaultFont;
public:
	Demo();
	~Demo();

	bool Init(const char* videoDriver);
	void MainLoop();

	friend void window_callbackMouse(yyWindow* w, s32 wheel, s32 x, s32 y, u32 click);
	friend void updateInputContext();
	friend void window_callbackKeyboard(yyWindow*, bool isPress, u32 key, char16_t character);
};

#endif

