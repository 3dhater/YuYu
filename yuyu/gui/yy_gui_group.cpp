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
	YY_DEBUG_PRINT_FUNC;
	m_type = yyGUIElementType::Group;
	m_onMouseInRect = 0;
	m_onMouseLeave = 0;
	m_onClick = 0;
	//m_useAutomaticSizeChange = false;
}

yyGUIGroup::~yyGUIGroup(){
	YY_DEBUG_PRINT_FUNC;
}

void yyGUIGroup::Rebuild() {
	yyGUIElement::CallOnRebuildSetRects();
}


YY_API yyGUIGroup* YY_C_DECL yyGUICreateGroup(const v4f& rect, s32 id, yyGUIDrawGroup* drawGroup){
	YY_DEBUG_PRINT_FUNC;
	yyGUIGroup* element = yyCreate<yyGUIGroup>();
	element->SetDrawGroup(drawGroup);
	element->SetBuildRect(rect);
	element->m_id = id;
	//element->SetRectInZeroOne(rect); надо всё переделать
	return element;
}

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
	for (auto child : e->m_children)
	{
		child->SetVisible(v);

		if (child->m_children.head())
			_setVisible(child, v);
	}
}

void yyGUIGroup::SetVisible(bool v) {
	m_visible = v;
	_setVisible(this, v);
}