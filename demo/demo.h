#ifndef __DEMO_H__
#define __DEMO_H__

#include "yy.h"
#include "containers\array.h"

class DemoExample;
class Demo
{
	yyEngineContext* m_engineContext;

	yyVideoDriverAPI* m_gpu;

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
	void StopDemo();

	yyInputContext * m_inputContext;
	yyWindow*        m_window;
	yyGUIFont*        m_defaultFont;

	friend void window_callbackMouse(yyWindow* w, s32 wheel, s32 x, s32 y, u32 click);
	friend void updateInputContext();
	friend void window_callbackKeyboard(yyWindow*, bool isPress, u32 key, char16_t character);
};

#endif

