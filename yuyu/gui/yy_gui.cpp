#include "yy.h"
#include "yy_gui.h"
#include "io/file_io.h"
#include "math/vec.h"
#include "math/math.h"

#include "yy_model.h"
#include "yy_input.h"

#include "../engine.h"

extern Engine * g_engine;

void yyGUIElement::CallOnRebuildSetRects() {
	if (m_onRebuildSetRects)
		m_onRebuildSetRects(this, m_id);
}

void yyGUIElement::SetDrawGroup(yyGUIDrawGroup* drawGroup){
	yyGUIDrawGroup* dg = drawGroup;
	if (!dg)
		dg = g_engine->m_mainGUIDrawGroup;
	this->m_drawGroup = dg;
	dg->AddElement(this);
}

void yyGUIElement::CheckCursorInRect() {
	if (g_engine->m_guiIgnoreUpdateInput)
	{
		m_isInActiveAreaRect = false;
		return;
	}

	m_isInActiveAreaRect = math::pointInRect(
		g_engine->m_inputContext->m_cursorCoordsForGUI.x,
		g_engine->m_inputContext->m_cursorCoordsForGUI.y,
		v4f(
			m_activeAreaRect_global.x + m_offset.x,
			m_activeAreaRect_global.y + m_offset.y,
			m_activeAreaRect_global.z + m_offset.x,
			m_activeAreaRect_global.w + m_offset.y
		)
	);
	if (m_isInActiveAreaRect)
	{
		g_engine->m_cursorInGUI = true;
		
		if (m_ignoreInput) return;

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
		if (guiElement->m_data->IsVisible())
		{
			guiElement->m_data->OnUpdate(dt);
		}
		guiElement = guiElement->m_left;
	}
}

void Engine::GUIDrawDrawGroup(yyGUIDrawGroup* dg) {
	auto & elements = dg->GetElements();
	auto guiElement = elements.head();
	for (size_t i = 0, sz = elements.size(); i < sz; ++i)
	{
		if (guiElement->m_data->IsVisible())
		{
			g_engine->m_videoAPI->SetGUIShaderData(guiElement->m_data);
			if (guiElement->m_data->m_onDraw)
				guiElement->m_data->m_onDraw(guiElement->m_data, guiElement->m_data->m_id);
			guiElement->m_data->OnDraw();
		}
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

void Engine::GUIRebuildElement(yyGUIElement* e) {
	v2i windowsSizeDiff = e->m_window->m_currentSize - e->m_window->m_creationSize;
	
	v2f windowsSizeHalf;// = e->m_window->m_currentSize * 0.5;
	windowsSizeHalf.x = (f32)e->m_window->m_currentSize.x * 0.5f;
	windowsSizeHalf.y = (f32)e->m_window->m_currentSize.y * 0.5f;
	
	v2f windowsCreationSizeHalf;// = e->m_window->m_creationSize * 0.5;
	windowsCreationSizeHalf.x = std::floor((f32)e->m_window->m_creationSize.x * 0.5f);
	windowsCreationSizeHalf.y = std::floor((f32)e->m_window->m_creationSize.y * 0.5f);

	v2i windowCenterDiff;
	windowCenterDiff.x = windowsSizeHalf.x - windowsCreationSizeHalf.x;
	windowCenterDiff.y = windowsSizeHalf.y - windowsCreationSizeHalf.y;
	//printf("%i %i\n", windowCenterDiff.x, windowCenterDiff.y);

	switch (e->m_align)
	{
	default:
		YY_PRINT_FAILED;
		break;
	case yyGUIElement::Align::AlignLeftTop: {
		if (e->m_parent)
		{
			e->m_buildingRect_global = e->m_buildingRect;
			e->m_buildingRect_global.x += e->m_parent->m_buildingRect_global.x;
			e->m_buildingRect_global.z += e->m_parent->m_buildingRect_global.x;
			e->m_buildingRect_global.y += e->m_parent->m_buildingRect_global.y;
			e->m_buildingRect_global.w += e->m_parent->m_buildingRect_global.y;

			e->m_activeAreaRect_global = e->m_activeAreaRect;
			e->m_activeAreaRect_global.x += e->m_parent->m_activeAreaRect_global.x;
			e->m_activeAreaRect_global.z += e->m_parent->m_activeAreaRect_global.x;
			e->m_activeAreaRect_global.y += e->m_parent->m_activeAreaRect_global.y;
			e->m_activeAreaRect_global.w += e->m_parent->m_activeAreaRect_global.y;

			e->m_clipRect_global = e->m_clipRect;
			e->m_clipRect_global.x += e->m_parent->m_clipRect_global.x; // ???
			e->m_clipRect_global.z += e->m_parent->m_clipRect_global.x; // ???
			e->m_clipRect_global.y += e->m_parent->m_clipRect_global.y; // ???
			e->m_clipRect_global.w += e->m_parent->m_clipRect_global.y; // ???
		}
	}break;
	case yyGUIElement::Align::AlignRightTop: {
		s32 x_diff = windowsSizeDiff.x;

		e->m_buildingRect_global = e->m_buildingRect;
		e->m_buildingRect_global.x += x_diff;
		e->m_buildingRect_global.z += x_diff;

		e->m_activeAreaRect_global = e->m_activeAreaRect;
		e->m_activeAreaRect_global.x += x_diff;
		e->m_activeAreaRect_global.z += x_diff;

		e->m_clipRect_global = e->m_clipRect;
		e->m_clipRect_global.x += x_diff; // ???
		e->m_clipRect_global.z += x_diff; // ???

		if (e->m_parent)
		{
		}
	}break;
	case yyGUIElement::Align::AlignLeftBottom: {
		s32 y_diff = windowsSizeDiff.y;
		e->m_buildingRect_global = e->m_buildingRect;
		e->m_buildingRect_global.y += y_diff;
		e->m_buildingRect_global.w += y_diff;

		e->m_activeAreaRect_global = e->m_activeAreaRect;
		e->m_activeAreaRect_global.y += y_diff;
		e->m_activeAreaRect_global.w += y_diff;

		e->m_clipRect_global = e->m_clipRect;
		e->m_clipRect_global.y += y_diff; // ???
		e->m_clipRect_global.w += y_diff; // ???

		if (e->m_parent)
		{
		}
	}break;
	case yyGUIElement::Align::AlignRightBottom: {
		s32 x_diff = windowsSizeDiff.x;
		s32 y_diff = windowsSizeDiff.y;
		e->m_buildingRect_global = e->m_buildingRect;
		e->m_buildingRect_global.x += x_diff;
		e->m_buildingRect_global.z += x_diff;
		e->m_buildingRect_global.y += y_diff;
		e->m_buildingRect_global.w += y_diff;

		e->m_activeAreaRect_global = e->m_activeAreaRect;
		e->m_activeAreaRect_global.x += x_diff;
		e->m_activeAreaRect_global.z += x_diff;
		e->m_activeAreaRect_global.y += y_diff;
		e->m_activeAreaRect_global.w += y_diff;

		e->m_clipRect_global = e->m_clipRect;
		e->m_clipRect_global.x += x_diff; // ???
		e->m_clipRect_global.z += x_diff; // ???
		e->m_clipRect_global.y += y_diff; // ???
		e->m_clipRect_global.w += y_diff; // ???
	}break;
	case yyGUIElement::Align::AlignCenter: {
		
		e->m_buildingRect_global = e->m_buildingRect;
		e->m_buildingRect_global.x += windowCenterDiff.x;
		e->m_buildingRect_global.z += windowCenterDiff.x;
		e->m_buildingRect_global.y += windowCenterDiff.y;
		e->m_buildingRect_global.w += windowCenterDiff.y;

		e->m_activeAreaRect_global = e->m_activeAreaRect;
		e->m_activeAreaRect_global.x += windowCenterDiff.x;
		e->m_activeAreaRect_global.z += windowCenterDiff.x;
		e->m_activeAreaRect_global.y += windowCenterDiff.y;
		e->m_activeAreaRect_global.w += windowCenterDiff.y;

		e->m_clipRect_global = e->m_clipRect;
		e->m_clipRect_global.x += windowCenterDiff.x; // ???
		e->m_clipRect_global.z += windowCenterDiff.x; // ???
		e->m_clipRect_global.y += windowCenterDiff.y; // ???
		e->m_clipRect_global.w += windowCenterDiff.y; // ???
	}break;
	}

	e->Rebuild();

	auto child = e->m_children.head();
	if (!child)
		return;
	for (size_t i2 = 0, sz2 = e->m_children.size(); i2 < sz2; ++i2)
	{
		GUIRebuildElement(child->m_data);
		child = child->m_right;
	}
}

void Engine::GUIRebuildDrawGroup(yyGUIDrawGroup* dg) {
	auto & elements = dg->GetElements();
	auto guiElement = elements.head();
	for (size_t i = 0, sz = elements.size(); i < sz; ++i)
	{
		// here i need to rebuild only elements without parents
		if (!guiElement->m_data->m_parent)
		{
			GUIRebuildElement(guiElement->m_data);

			/*auto child = guiElement->m_data->m_children.head();
			for (size_t i2 = 0, sz2 = guiElement->m_data->m_children.size(); i2 < sz2; ++i2)
			{
				GUIRebuildElement(child->m_data);
				child = child->m_right;
			}*/
		}

		guiElement = guiElement->m_right;
	}
}

YY_API void YY_C_DECL yyGUIRebuild(){
	g_engine->GUIRebuildDrawGroup(g_engine->m_mainGUIDrawGroup);
	auto dg = g_engine->m_GUIDrawGroups.tail();
	for (size_t i = 0, sz = g_engine->m_GUIDrawGroups.size(); i < sz; ++i)
	{
		g_engine->GUIRebuildDrawGroup(dg->m_data);
		dg = dg->m_left;
	}
}