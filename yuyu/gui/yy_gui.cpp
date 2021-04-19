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

void yyGUIElement::SetBuildRect(const v4f& rectInPixels) {
	if (m_parent)
	{
		f32 wndSzX = 1.f / m_parent->m_buildRectInPixels.z - m_parent->m_buildRectInPixels.x;
		f32 wndSzY = 1.f / m_parent->m_buildRectInPixels.w - m_parent->m_buildRectInPixels.y;

		m_buildRect.x = rectInPixels.x * wndSzX;
		m_buildRect.z = rectInPixels.z * wndSzX;
		m_buildRect.y = rectInPixels.y * wndSzY;
		m_buildRect.w = rectInPixels.w * wndSzY;
	}
	else
	{
		f32 wndSzX = 1.f / (f32)m_window->m_creationSize.x;
		f32 wndSzY = 1.f / (f32)m_window->m_creationSize.y;
	
		m_buildRect.x = rectInPixels.x * wndSzX;
		m_buildRect.z = rectInPixels.z * wndSzX;
		m_buildRect.y = rectInPixels.y * wndSzY;
		m_buildRect.w = rectInPixels.w * wndSzY;
	}
	m_buildRectInPixelsCreation = rectInPixels;
	m_buildRectInPixels  = rectInPixels;
	m_clipRectInPixels   = rectInPixels;
	m_sensorRectInPixels = rectInPixels;
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
			m_sensorRectInPixels.x + m_offset.x,
			m_sensorRectInPixels.y + m_offset.y,
			m_sensorRectInPixels.z + m_offset.x,
			m_sensorRectInPixels.w + m_offset.y
		)
	);
	if (m_isInActiveAreaRect)
	{
		if(!m_ignoreSetCursorInGUI)
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

void yyGUIElement::SetRectsFromBuildRect() {
	f32 windowSizeX_1 = 1.f / m_window->m_currentSize.x;
	f32 windowSizeY_1 = 1.f / m_window->m_currentSize.y;

	if (m_buildRect.x > 0.f) m_buildRectInPixelsCreation.x = m_buildRect.x / windowSizeX_1;
	if (m_buildRect.z > 0.f) m_buildRectInPixelsCreation.z = m_buildRect.z / windowSizeX_1;
	if (m_buildRect.y > 0.f) m_buildRectInPixelsCreation.y = m_buildRect.y / windowSizeY_1;
	if (m_buildRect.w > 0.f) m_buildRectInPixelsCreation.w = m_buildRect.w / windowSizeY_1;
	m_buildRectInPixels = m_buildRectInPixelsCreation;
	m_sensorRectInPixels = m_buildRectInPixelsCreation;
	m_clipRectInPixels = m_buildRectInPixelsCreation;
}

void Engine::GUIRebuildElement(yyGUIElement* e) {
	v4f parentRectInPixels;
	f32 parentRectSizeDiff_X = 0.f;
	f32 parentRectSizeDiff_Y = 0.f;
	
	v2f parentCreationCenter;
	v2f parentCurrentCenter;

	if (e->m_parent)
	{
		parentRectInPixels = e->m_parent->m_buildRectInPixels;
		
		parentCreationCenter.x = e->m_parent->m_buildRectInPixelsCreation.x +
			((e->m_parent->m_buildRectInPixelsCreation.z -
				e->m_parent->m_buildRectInPixelsCreation.x)*0.5f);
		parentCreationCenter.y = e->m_parent->m_buildRectInPixelsCreation.y +
			((e->m_parent->m_buildRectInPixelsCreation.w -
				e->m_parent->m_buildRectInPixelsCreation.y)*0.5f);

		parentCurrentCenter.x = e->m_parent->m_buildRectInPixels.x +
			((e->m_parent->m_buildRectInPixels.z -
				e->m_parent->m_buildRectInPixels.x)*0.5f);
		parentCurrentCenter.y = e->m_parent->m_buildRectInPixels.y +
			((e->m_parent->m_buildRectInPixels.w -
				e->m_parent->m_buildRectInPixels.y)*0.5f);
	}
	else
	{
		parentRectInPixels.x = 0.f;
		parentRectInPixels.y = 0.f;
		parentRectInPixels.z = e->m_window->m_currentSize.x;
		parentRectInPixels.w = e->m_window->m_currentSize.y;

		parentCreationCenter.x = std::floor((f32)e->m_window->m_creationSize.x * 0.5f);
		parentCreationCenter.y = std::floor((f32)e->m_window->m_creationSize.y * 0.5f);

		parentCurrentCenter.x = (f32)e->m_window->m_currentSize.x * 0.5f;
		parentCurrentCenter.y = (f32)e->m_window->m_currentSize.y * 0.5f;
	}

	f32 parentRectSizeX_1 = 1.f / (parentRectInPixels.z - parentRectInPixels.x);
	f32 parentRectSizeY_1 = 1.f / (parentRectInPixels.w - parentRectInPixels.y);
	parentRectSizeDiff_X = parentCurrentCenter.x - parentCreationCenter.x;
	parentRectSizeDiff_Y = parentCurrentCenter.y - parentCreationCenter.y;

	//printf("%i %i\n", windowCenterDiff.x, windowCenterDiff.y);

	switch (e->m_align)
	{
	default:
		YY_PRINT_FAILED;
		break;
	case yyGUIElement::Align::AlignLeftTop: {
		if (e->m_parent)
		{
			// i think not correct
			e->m_buildRectInPixels = e->m_buildRectInPixelsCreation;
			e->m_buildRectInPixels.x += e->m_parent->m_buildRectInPixels.x;
			e->m_buildRectInPixels.y += e->m_parent->m_buildRectInPixels.y;
			e->m_buildRectInPixels.z += e->m_parent->m_buildRectInPixels.x;
			e->m_buildRectInPixels.w += e->m_parent->m_buildRectInPixels.y;
		}
		else
		{
			if (e->m_useProportionalScaling)
			{
				if (e->m_buildRect.x != 0.f) e->m_buildRectInPixels.x = e->m_buildRect.x / parentRectSizeX_1;
				if (e->m_buildRect.z != 0.f) e->m_buildRectInPixels.z = e->m_buildRect.z / parentRectSizeX_1;
				if (e->m_buildRect.y != 0.f) e->m_buildRectInPixels.y = e->m_buildRect.y / parentRectSizeY_1;
				if (e->m_buildRect.w != 0.f) e->m_buildRectInPixels.w = e->m_buildRect.w / parentRectSizeY_1;
			}
		}
		e->m_clipRectInPixels = e->m_buildRectInPixels;
		e->m_sensorRectInPixels = e->m_buildRectInPixels;
	}break;
	case yyGUIElement::Align::AlignRightTop: {
		if (e->m_parent)
		{
			// maybe not correct
			e->m_buildRectInPixels = e->m_parent->m_buildRectInPixels;
			e->m_buildRectInPixels.x += e->m_buildRect.x / parentRectSizeX_1;
			e->m_buildRectInPixels.z += e->m_buildRect.z / parentRectSizeX_1;
		}
		else
		{
			if (e->m_useProportionalScaling)
			{
				if (e->m_buildRect.x != 0.f) e->m_buildRectInPixels.x = e->m_buildRect.x / parentRectSizeX_1;
				if (e->m_buildRect.z != 0.f) e->m_buildRectInPixels.z = e->m_buildRect.z / parentRectSizeX_1;
				if (e->m_buildRect.y != 0.f) e->m_buildRectInPixels.y = e->m_buildRect.y / parentRectSizeY_1;
				if (e->m_buildRect.w != 0.f) e->m_buildRectInPixels.w = e->m_buildRect.w / parentRectSizeY_1;
			}
			else
			{
				e->m_buildRectInPixels.x = e->m_buildRectInPixelsCreation.x + parentRectSizeDiff_X + parentRectSizeDiff_X;
				e->m_buildRectInPixels.z = e->m_buildRectInPixelsCreation.z + parentRectSizeDiff_X + parentRectSizeDiff_X;
			}
		}

		e->m_clipRectInPixels   = e->m_buildRectInPixels;
		e->m_sensorRectInPixels = e->m_buildRectInPixels;
	}break;
	case yyGUIElement::Align::AlignLeftBottom: {
		if (e->m_parent)
		{
			e->m_buildRectInPixels = e->m_parent->m_buildRectInPixels;
			e->m_buildRectInPixels.y += e->m_buildRect.y / parentRectSizeY_1;
			e->m_buildRectInPixels.w += e->m_buildRect.w / parentRectSizeY_1;
		}
		else
		{
			if (e->m_useProportionalScaling)
			{
				if (e->m_buildRect.x != 0.f) e->m_buildRectInPixels.x = e->m_buildRect.x / parentRectSizeX_1;
				if (e->m_buildRect.z != 0.f) e->m_buildRectInPixels.z = e->m_buildRect.z / parentRectSizeX_1;
				if (e->m_buildRect.y != 0.f) e->m_buildRectInPixels.y = e->m_buildRect.y / parentRectSizeY_1;
				if (e->m_buildRect.w != 0.f) e->m_buildRectInPixels.w = e->m_buildRect.w / parentRectSizeY_1;
			}
			else
			{
				e->m_buildRectInPixels.y = e->m_buildRectInPixelsCreation.y + parentRectSizeDiff_Y + parentRectSizeDiff_Y;
				e->m_buildRectInPixels.w = e->m_buildRectInPixelsCreation.w + parentRectSizeDiff_Y + parentRectSizeDiff_Y;
			}
		}

		e->m_clipRectInPixels = e->m_buildRectInPixels;
		e->m_sensorRectInPixels = e->m_buildRectInPixels;
	}break;
	case yyGUIElement::Align::AlignRightBottom: {
		if (e->m_parent)
		{
			e->m_buildRectInPixels = e->m_parent->m_buildRectInPixels;
			e->m_buildRectInPixels.y += e->m_buildRect.y / parentRectSizeY_1;
			e->m_buildRectInPixels.w += e->m_buildRect.w / parentRectSizeY_1;
			e->m_buildRectInPixels.x += e->m_buildRect.x / parentRectSizeX_1;
			e->m_buildRectInPixels.z += e->m_buildRect.z / parentRectSizeX_1;
		}
		else
		{
			if (e->m_useProportionalScaling)
			{
				if (e->m_buildRect.x != 0.f) e->m_buildRectInPixels.x = e->m_buildRect.x / parentRectSizeX_1;
				if (e->m_buildRect.z != 0.f) e->m_buildRectInPixels.z = e->m_buildRect.z / parentRectSizeX_1;
				if (e->m_buildRect.y != 0.f) e->m_buildRectInPixels.y = e->m_buildRect.y / parentRectSizeY_1;
				if (e->m_buildRect.w != 0.f) e->m_buildRectInPixels.w = e->m_buildRect.w / parentRectSizeY_1;
			}
			else
			{
				e->m_buildRectInPixels.x = e->m_buildRectInPixelsCreation.x + parentRectSizeDiff_X + parentRectSizeDiff_X;
				e->m_buildRectInPixels.z = e->m_buildRectInPixelsCreation.z + parentRectSizeDiff_X + parentRectSizeDiff_X;
				e->m_buildRectInPixels.y = e->m_buildRectInPixelsCreation.y + parentRectSizeDiff_Y + parentRectSizeDiff_Y;
				e->m_buildRectInPixels.w = e->m_buildRectInPixelsCreation.w + parentRectSizeDiff_Y + parentRectSizeDiff_Y;
			}
		}

		e->m_clipRectInPixels = e->m_buildRectInPixels;
		e->m_sensorRectInPixels = e->m_buildRectInPixels;
	}break;
	case yyGUIElement::Align::AlignCenter: {
		if (e->m_useProportionalScaling)
		{
			if (e->m_buildRect.x != 0.f) e->m_buildRectInPixels.x = e->m_buildRect.x / parentRectSizeX_1;
			if (e->m_buildRect.z != 0.f) e->m_buildRectInPixels.z = e->m_buildRect.z / parentRectSizeX_1;
			if (e->m_buildRect.y != 0.f) e->m_buildRectInPixels.y = e->m_buildRect.y / parentRectSizeY_1;
			if (e->m_buildRect.w != 0.f) e->m_buildRectInPixels.w = e->m_buildRect.w / parentRectSizeY_1;
		}
		else
		{
			e->m_buildRectInPixels.x = e->m_buildRectInPixelsCreation.x + parentRectSizeDiff_X;
			e->m_buildRectInPixels.z = e->m_buildRectInPixelsCreation.z + parentRectSizeDiff_X;
			e->m_buildRectInPixels.y = e->m_buildRectInPixelsCreation.y + parentRectSizeDiff_Y;
			e->m_buildRectInPixels.w = e->m_buildRectInPixelsCreation.w + parentRectSizeDiff_Y;
		}

		e->m_clipRectInPixels = e->m_buildRectInPixels;
		e->m_sensorRectInPixels = e->m_buildRectInPixels;
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
