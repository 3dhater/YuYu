#include "yy.h"
#include "yy_gui.h"
#include "io/file_io.h"
#include "math/vec.h"
#include "math/math.h"

#include "yy_model.h"
#include "yy_input.h"

#include "../engine.h"

extern Engine * g_engine;

void yyGUIElement::SetDrawGroup(yyGUIDrawGroup* drawGroup){
	yyGUIDrawGroup* dg = drawGroup;
	if (!dg)
		dg = g_engine->m_mainGUIDrawGroup;
	this->m_drawGroup = dg;
	dg->AddElement(this);
}

void yyGUIElement::CheckCursorInRect() {
	m_isInActiveAreaRect = math::pointInRect(
		g_engine->m_inputContext->m_cursorCoordsForGUI.x,
		g_engine->m_inputContext->m_cursorCoordsForGUI.y,
		v4f(
			m_activeAreaRect.x + m_offset.x,
			m_activeAreaRect.y + m_offset.y,
			m_activeAreaRect.z + m_offset.x,
			m_activeAreaRect.w + m_offset.y
		)
	);
	if (m_isInActiveAreaRect)
	{
		g_engine->m_cursorInGUI = true;
		g_engine->m_guiIgnoreUpdateInput = true;
	}
}

YY_API void YY_C_DECL yyGUIDeleteElement(yyGUIElement* elem){
	assert(elem);
	//g_engine->m_guiElements.erase_first(elem);
	elem->m_drawGroup->RemoveElement(elem);
	yyDestroy(elem);
}

YY_API void YY_C_DECL yyGUIRemoveElement(yyGUIElement* elem){
	assert(elem);
	elem->m_drawGroup->RemoveElement(elem);
	//g_engine->m_guiElements.erase_first(elem);
}

YY_API yyGUIDrawGroup* YY_C_DECL yyGUICreateDrawGroup() {
	yyGUIDrawGroup* new_dg = yyCreate<yyGUIDrawGroup>();
	if (!g_engine->m_mainGUIDrawGroup)
		g_engine->m_mainGUIDrawGroup = new_dg;
	else
		g_engine->m_GUIDrawGroups.push_back(new_dg);
	return new_dg;
}

YY_API void YY_C_DECL yyGUIDeleteDrawGroup(yyGUIDrawGroup* dg) {
	assert(dg);
	yyDestroy(dg);
	if (dg == g_engine->m_mainGUIDrawGroup)
	{
		g_engine->m_mainGUIDrawGroup = 0;
		return;
	}
	g_engine->m_GUIDrawGroups.erase_first(dg);
}

YY_API void YY_C_DECL yyGUIDrawGroupMoveBack(yyGUIDrawGroup* dg) {
	assert(dg);
	g_engine->m_GUIDrawGroups.erase_first(dg);
	g_engine->m_GUIDrawGroups.push_back(dg);
}

YY_API void YY_C_DECL yyGUIDrawGroupMoveFront(yyGUIDrawGroup* dg) {
	assert(dg);
	g_engine->m_GUIDrawGroups.erase_first(dg);
	g_engine->m_GUIDrawGroups.push_front(dg);
}

void Engine::GUIUpdateDrawGroup(yyGUIDrawGroup* dg, f32 dt) {
	auto & elements = dg->GetElements();
	auto guiElement = elements.head();
	for (size_t i = 0, sz = elements.size(); i < sz; ++i)
	{
		if (!g_engine->m_guiIgnoreUpdateInput)
			guiElement->m_data->OnUpdate(dt);
		guiElement = guiElement->m_right;
	}
}

void Engine::GUIDrawDrawGroup(yyGUIDrawGroup* dg) {
	auto & elements = dg->GetElements();
	auto guiElement = elements.head();
	for (size_t i = 0, sz = elements.size(); i < sz; ++i)
	{
		g_engine->m_videoAPI->SetGUIShaderData(guiElement->m_data);
		if (guiElement->m_data->m_onDraw)
			guiElement->m_data->m_onDraw(guiElement->m_data, guiElement->m_data->m_id);
		guiElement->m_data->OnDraw();
		guiElement = guiElement->m_right;
	}
}

YY_API bool YY_C_DECL yyGUIUpdate(f32 deltaTime){
	g_engine->m_cursorInGUI = false;
	g_engine->m_guiIgnoreUpdateInput = false;
	
	auto dg = g_engine->m_GUIDrawGroups.head();
	for(size_t i = 0, sz = g_engine->m_GUIDrawGroups.size(); i < sz; ++i)
	{
		if(dg->m_data->m_isInput)
			g_engine->GUIUpdateDrawGroup(dg->m_data, deltaTime);
		dg = dg->m_right;
	}
	
	g_engine->GUIUpdateDrawGroup(g_engine->m_mainGUIDrawGroup, deltaTime);

	if (g_engine->m_inputContext->m_isLMBUp)
	{
		g_engine->m_guiElementInMouseFocus = 0;
	}
	return g_engine->m_cursorInGUI;
}

YY_API void YY_C_DECL yyGUIDrawAll(){
	g_engine->m_videoAPI->BeginDrawGUI();
	g_engine->GUIDrawDrawGroup(g_engine->m_mainGUIDrawGroup);

	auto dg = g_engine->m_GUIDrawGroups.tail();
	for (size_t i = 0, sz = g_engine->m_GUIDrawGroups.size(); i < sz; ++i)
	{
		if (dg->m_data->m_isDraw)
			g_engine->GUIDrawDrawGroup(dg->m_data);
		dg = dg->m_left;
	}

	g_engine->m_videoAPI->EndDrawGUI();
}

YY_API yyGUIElement* YY_C_DECL yyGUIGetElementInMouseFocus(){
	return g_engine->m_guiElementInMouseFocus;
}