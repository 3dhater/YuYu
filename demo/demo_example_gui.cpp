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

DemoExample_GUI::DemoExample_GUI(){
	m_text_hello = 0;
	m_drawGroup1 = 0;
	m_drawGroup2 = 0;
	m_buttonGr1 = 0;
	m_buttonGr2 = 0;
	m_textGr1 = 0;
	m_textGr2 = 0;
}
DemoExample_GUI::~DemoExample_GUI(){
	Shutdown();
}

bool DemoExample_GUI::Init(){
	m_text_hello = yyGUICreateText(v2f(0.f, 20.f), g_demo->m_defaultFont, L"Draw Group test:", 0);

	m_drawGroup1 = yyGUICreateDrawGroup();
	m_drawGroup2 = yyGUICreateDrawGroup();

	auto textRect = m_text_hello->m_buildingRect;

	m_buttonGr1 = yyGUICreateButton(v4f(textRect.z, 20.f, textRect.z + 60.f, 40.f), yyGetTextureResource("../res/textures/editor/red.dds", false, false, true), -1, m_drawGroup1);
	m_buttonGr2 = yyGUICreateButton(v4f(textRect.z + 15.f, 25.f, textRect.z + 75.f, 45.f), yyGetTextureResource("../res/textures/editor/white.dds", false, false, true), -1, m_drawGroup2);

	m_buttonGr1->m_onClick = DemoExample_GUI_ButtonGr1_onClick;
	m_buttonGr2->m_onClick = DemoExample_GUI_ButtonGr2_onClick;

	m_textGr1 = yyGUICreateText(v2f(m_buttonGr1->m_buildingRect.x, m_buttonGr1->m_buildingRect.y), g_demo->m_defaultFont, L"Group 1", m_drawGroup1);
	m_textGr2 = yyGUICreateText(v2f(m_buttonGr2->m_buildingRect.x, m_buttonGr2->m_buildingRect.y), g_demo->m_defaultFont, L"Group 2", m_drawGroup2);
	m_textGr1->m_color = ColorWhite;
	m_textGr2->m_color = ColorRed;

	return true;
}


void DemoExample_GUI::Shutdown(){
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
	if (g_drawGroupToTop)
	{
		yyGUIDrawGroupMoveFront(g_drawGroupToTop);
		g_drawGroupToTop = 0;
	}
	return g_demo->m_inputContext->isKeyHit(yyKey::K_ESCAPE) == false;
}