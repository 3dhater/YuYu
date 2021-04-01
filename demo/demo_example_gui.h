#ifndef __DEMO_EXAMPLE_GUI_H__
#define __DEMO_EXAMPLE_GUI_H__

class DemoExample_GUI : public DemoExample
{
	yyGUIText* m_text_hello;
	yyGUIDrawGroup* m_drawGroup1;
	yyGUIDrawGroup* m_drawGroup2;
	yyGUIButton* m_buttonGr1;
	yyGUIButton* m_buttonGr2;
	yyGUIButton* m_button1;
	yyGUIButton* m_button2;
	yyGUIText* m_textGr1;
	yyGUIText* m_textGr2;
	yyGUIText* m_text1;

public:
	DemoExample_GUI();
	virtual ~DemoExample_GUI();

	virtual bool Init();
	virtual void Shutdown();
	virtual const wchar_t* GetTitle();
	virtual const wchar_t* GetDescription();

	virtual bool DemoStep(f32 deltaTime);
};

#endif

