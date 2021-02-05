﻿#ifndef __DEMO_H__
#define __DEMO_H__

#include "yy.h"
#include "containers\array.h"

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

class DemoExample;
class Demo
{
	yyEngineContext* m_engineContext;
	yyWindow*        m_window;

	yyVideoDriverAPI* m_gpu;
	yyGUIFont*        m_defaultFont;

	s32 m_selectedExample;
	yyArray<DemoExample*> m_examples;
	DemoExample* m_activeExample;

	void _SelectExampleUpdateColors();
	void _hideMainMenuGUI();
	void _showMainMenuGUI();
public:
	Demo();
	~Demo();

	bool Init(const char* videoDriver);
	void MainLoop();

	void AddExample(DemoExample*);
	void SelectExamplePressDown();
	void SelectExamplePressUp();
	void StartDemo();

	yyInputContext * m_inputContext;

	friend void window_callbackMouse(yyWindow* w, s32 wheel, s32 x, s32 y, u32 click);
	friend void updateInputContext();
	friend void window_callbackKeyboard(yyWindow*, bool isPress, u32 key, char16_t character);
};

#endif

