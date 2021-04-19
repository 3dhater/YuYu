#include "yy.h"
#include "yy_gui.h"
#include "math/vec.h"
#include "math/math.h"

#include "yy_input.h"

#include "../engine.h"

extern Engine * g_engine;

yyGUIGroup::yyGUIGroup()
	:
	m_onClick(nullptr)
{
	m_type = yyGUIElementType::Group;
	m_onMouseInRect = 0;
	m_onMouseLeave = 0;
	m_onClick = 0;
	//m_useAutomaticSizeChange = false;
}

yyGUIGroup::~yyGUIGroup(){
}

void yyGUIGroup::Rebuild() {
	/*if (m_useAutomaticSizeChange)
	{
		f32 wndSzX = 1.f / (f32)m_window->m_currentSize.x;
		f32 wndSzY = 1.f / (f32)m_window->m_currentSize.y;
		
		m_activeAreaRect.x = m_rectInZeroOne.x / wndSzX;
		m_activeAreaRect.z = m_rectInZeroOne.z / wndSzX;
		m_activeAreaRect.y = m_rectInZeroOne.y / wndSzY;
		m_activeAreaRect.w = m_rectInZeroOne.w / wndSzY;

		m_clipRect = m_activeAreaRect;
		m_buildingRect = m_activeAreaRect;
		m_activeAreaRect_global = m_activeAreaRect;
		m_clipRect_global = m_clipRect;
		m_buildingRect_global = m_buildingRect;
	}*/
	yyGUIElement::CallOnRebuildSetRects();
}


YY_API yyGUIGroup* YY_C_DECL yyGUICreateGroup(const v4f& rect, s32 id, yyGUIDrawGroup* drawGroup){
	yyGUIGroup* element = yyCreate<yyGUIGroup>();
	element->SetDrawGroup(drawGroup);
	element->SetBuildRect(rect);
	element->m_id = id;
	//element->SetRectInZeroOne(rect); надо всё переделать
	return element;
}

//void yyGUIGroup::SetRectInZeroOne(const v4f& rectInPixels) {
//	f32 wndSzX = 1.f / (f32)m_window->m_currentSize.x;
//	f32 wndSzY = 1.f / (f32)m_window->m_currentSize.y;
//
//	m_rectInZeroOne.x = rectInPixels.x * wndSzX;
//	m_rectInZeroOne.z = rectInPixels.z * wndSzX;
//	m_rectInZeroOne.y = rectInPixels.y * wndSzY;
//	m_rectInZeroOne.w = rectInPixels.w * wndSzY;
//}

void yyGUIGroup::OnUpdate(f32 dt){
	if (!m_visible) return;

	//bool isIgnoreUpdateInputBefore = g_engine->m_guiIgnoreUpdateInput;
	bool isMouseInActiveAreaRect = m_isInActiveAreaRect;

	yyGUIElement::CheckCursorInRect();
	if (m_isInActiveAreaRect)
	{
		if (m_onMouseInRect)
			m_onMouseInRect(this, m_id);
	}

	if (!m_isInActiveAreaRect && isMouseInActiveAreaRect)
	{
		if (m_onMouseLeave)
			m_onMouseLeave(this, m_id);
	}
	g_engine->m_guiIgnoreUpdateInput = false;

	if (m_ignoreInput) return;
	if (m_onClick && g_engine->m_inputContext->m_isLMBDown)
	{
		if (m_isInActiveAreaRect)
			m_onClick(this, m_id);
	}
}

void yyGUIGroup::OnDraw(){
	//if (!m_visible) return;
}

void yyGUIGroup::AddElement(yyGUIElement* elem) {
	elem->SetParent(this);
	this->m_children.erase_first(elem);
	this->m_children.push_back(elem);
}


void yyGUIGroup::_setVisible(yyGUIElement* e, bool v) {
	for (auto & child : e->m_children)
	{
		child.m_data->SetVisible(v);

		if (child.m_data->m_children.head())
			_setVisible(child.m_data, v);
	}
}

void yyGUIGroup::SetVisible(bool v) {
	m_visible = v;
	_setVisible(this, v);
}