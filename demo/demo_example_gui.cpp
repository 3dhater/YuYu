#include "demo.h"
#include "demo_example.h"
#include "demo_example_gui.h"

#include "yy_input.h"
#include "yy_gui.h"

extern Demo* g_demo;

yyGUIDrawGroup* g_drawGroupToTop = 0;
void DemoExample_GUI_ButtonGr1_onClick(yyGUIElement* elem, s32 m_id) {
	g_drawGroupToTop = elem->m_drawGroup;
}

void DemoExample_GUI_ButtonGr2_onClick(yyGUIElement* elem, s32 m_id) {
	g_drawGroupToTop = elem->m_drawGroup;
}

void DemoExample_GUI_button1_onClick(yyGUIElement* elem, s32 m_id) {
	yyLogWriteInfo("Button 1\n");
}
void DemoExample_GUI_button2_onClick(yyGUIElement* elem, s32 m_id) {
	yyLogWriteInfo("Button 2\n");
}

DemoExample_GUI::DemoExample_GUI(){
	m_text_hello = 0;
	m_drawGroup1 = 0;
	m_drawGroup2 = 0;
	m_buttonGr1 = 0;
	m_buttonGr2 = 0;
	m_textGr1 = 0;
	m_textGr2 = 0;
	m_button1 = 0;
	m_button2 = 0;
	m_text1 = 0;
	m_textInput = 0;
}
DemoExample_GUI::~DemoExample_GUI(){
	Shutdown();
}

bool DemoExample_GUI::Init(){
	m_text_hello = yyGUICreateText(v2f(0.f, 20.f), g_demo->m_defaultFont, L"Draw Group test:", 0);

	m_drawGroup1 = yyGUICreateDrawGroup();
	m_drawGroup2 = yyGUICreateDrawGroup();

	auto textRect = m_text_hello->m_buildRectInPixels;

	m_buttonGr1 = yyGUICreateButton(v4f(textRect.z, 20.f, textRect.z + 60.f, 40.f), yyGetTextureFromCache("../res/textures/editor/red.dds"), -1, m_drawGroup1);
	m_buttonGr2 = yyGUICreateButton(v4f(textRect.z + 15.f, 25.f, textRect.z + 35.f, 45.f), yyGetTextureFromCache("../res/textures/editor/white.dds"), -1, m_drawGroup2);

	m_buttonGr1->m_onClick = DemoExample_GUI_ButtonGr1_onClick;
	m_buttonGr2->m_onClick = DemoExample_GUI_ButtonGr2_onClick;

	m_textGr1 = yyGUICreateText(v2f(0.f, 0.f), g_demo->m_defaultFont, L"Group 1", m_drawGroup1);
	m_textGr2 = yyGUICreateText(v2f(0.f, 0.f), g_demo->m_defaultFont, L"Group 2", m_drawGroup2);
	m_textGr1->IgnoreInput(true);
	m_textGr2->IgnoreInput(true);
	m_textGr1->m_color = ColorWhite;
	m_textGr2->m_color = ColorRed;

	m_textGr1->SetParent(m_buttonGr1);
	m_textGr2->SetParent(m_buttonGr2);
	
	m_button1 = yyGUICreateButton(v4f(m_buttonGr1->m_buildRectInPixels.z, 20.f, m_buttonGr1->m_buildRectInPixels.z + 80.f, 80.f),
		yyGetTextureFromCache("../res/textures/editor/red.dds"), -1, 0);
	m_button1->m_onClick = DemoExample_GUI_button1_onClick;
	m_button1->m_align = m_button1->AlignCenter;
	
	m_button2 = yyGUICreateButton(v4f(5.f, 5.f, 70.f, 70.f), 0, -1, 0);
	m_button2->m_onClick = DemoExample_GUI_button2_onClick;
	m_button2->SetParent(m_button1);
	m_button2->SetText(L"XXX", g_demo->m_defaultFont, true);
	m_button2->SetColor(ColorBlue, 0);
	m_button2->SetColor(ColorLightBlue, 1);
	m_button2->SetColor(ColorCyan, 2);
	m_button2->m_isAnimated = true;
	/*m_text1 = yyGUICreateText(v2f(0.f, 0.f), g_demo->m_defaultFont, L"Button in button", 0);
	m_text1->SetParent(m_button2);
	m_text1->m_color = ColorBlue;
	m_text1->IgnoreInput(true);*/

	m_textInput = yyGUICreateTextInput(v4f(200.f, 0.f, 320.f, 16.f), g_demo->m_defaultFont, L"Text input bla bla", 0);
	m_textInput->m_align = m_textInput->AlignCenter;
	m_textInput->UseDefaultText(L"Enter text", yyColor(0.4f));

	yyGUIRebuild();

	return true;
}


void DemoExample_GUI::Shutdown(){
	if (m_textInput)
	{
		yyGUIDeleteElement(m_textInput);
		m_textInput = 0;
	}
	if (m_button1)
	{
		yyGUIDeleteElement(m_button1);
		m_button1 = 0;
	}
	if (m_button2)
	{
		yyGUIDeleteElement(m_button2);
		m_button2 = 0;
	}
	if (m_text1)
	{
		yyGUIDeleteElement(m_text1);
		m_text1 = 0;
	}

	// yyGUIDeleteDrawGroup will delete all GUI elements in this group
	if (m_drawGroup1)
	{
		yyGUIDeleteDrawGroup(m_drawGroup1);
		m_drawGroup1 = 0;
	}

	if (m_drawGroup2)
	{
		yyGUIDeleteDrawGroup(m_drawGroup2);
		m_drawGroup2 = 0;
	}

	if (m_text_hello)
	{
		yyGUIDeleteElement(m_text_hello);
		m_text_hello = 0;
	}
}

const wchar_t* DemoExample_GUI::GetTitle(){
	return L"GUI";
}

const wchar_t* DemoExample_GUI::GetDescription(){
	return L"Everything about GUI";
}

bool DemoExample_GUI::DemoStep(f32 deltaTime){
	bool isGuiInputFocus = yyGUIIsInputFocus();

	if (g_drawGroupToTop)
	{
		yyGUIDrawGroupMoveFront(g_drawGroupToTop);
		g_drawGroupToTop = 0;
	}
	
	if (isGuiInputFocus)
		return true;

	return g_demo->m_inputContext->IsKeyHit(yyKey::K_ESCAPE) == false;
}