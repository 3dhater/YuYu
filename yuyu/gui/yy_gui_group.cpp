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
}

yyGUIGroup::~yyGUIGroup(){
}

void yyGUIGroup::Rebuild() {
	yyGUIElement::CallOnRebuildSetRects();
}


YY_API yyGUIGroup* YY_C_DECL yyGUICreateGroup(const v4f& rect, s32 id, yyGUIDrawGroup* drawGroup){
	yyGUIGroup* element = yyCreate<yyGUIGroup>();
	element->SetDrawGroup(drawGroup);
	element->m_activeAreaRect = rect;
	element->m_clipRect = rect;
	element->m_buildingRect = rect;
	element->m_activeAreaRect_global = element->m_activeAreaRect;
	element->m_clipRect_global = element->m_clipRect;
	element->m_buildingRect_global = element->m_buildingRect;
	element->m_id = id;
	return element;
}

void yyGUIGroup::OnUpdate(f32 dt){
	if (!m_visible) return;

	bool isIgnoreUpdateInputBefore = g_engine->m_guiIgnoreUpdateInput;
	bool isMouseInActiveAreaRect = m_isInActiveAreaRect;

	yyGUIElement::CheckCursorInRect();
	if (m_ignoreInput) return;

	g_engine->m_guiIgnoreUpdateInput = isIgnoreUpdateInputBefore;

	if (m_isInActiveAreaRect)
	{
		if (m_onMouseInRect)
			m_onMouseInRect(this, m_id);
	}

	if (m_onClick && g_engine->m_inputContext->m_isLMBDown)
	{
		if (m_isInActiveAreaRect)
			m_onClick(this, m_id);
	}

	if (!m_isInActiveAreaRect && isMouseInActiveAreaRect)
	{
		if (m_onMouseLeave)
			m_onMouseLeave(this, m_id);
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