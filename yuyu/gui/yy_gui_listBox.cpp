#include "yy.h"
#include "yy_gui.h"
#include "math/vec.h"
#include "math/math.h"

#include "yy_model.h"
#include "yy_input.h"

#include "../engine.h"

extern yyEngine * g_engine;

yyGUIListBox::yyGUIListBox() {
	m_type = yyGUIElementType::ListBox;
	m_isSelectable = true;
	m_isAnimatedScroll = true;
	m_font = 0;
	m_y_scroll = 0.f;
	m_bgColor.set(0.2f);
	m_bgColorHover.set(0.25f);
	m_bgColorCurrent = m_bgColor;
	m_contentHeight = 0.f;
	m_itemColorHover.set(0.35f);
	m_itemColorSelected = ColorBlue;
	m_y_scrollTarget = 0.f;
	m_animatedScrollLerp = 0.15f;
}

yyGUIListBox::~yyGUIListBox() {
	for (u32 i = 0, sz = m_items.size(); i < sz; ++i)
	{
		yyDestroy(m_items[i]);
	}
}

void yyGUIListBox::OnUpdate(f32 dt) {
	if (!m_visible) return;
	yyGUIElement::CheckCursorInRect();
	if (m_ignoreInput) return;

	m_bgColorCurrent = m_bgColor;

	if (m_isInActiveAreaRect && (!g_engine->m_guiElementInMouseFocus))
	{
		m_bgColorCurrent = m_bgColorHover;

		if (g_engine->m_inputContext->m_wheelDelta)
		{
			if (!m_isAnimatedScroll)
			{
				if (g_engine->m_inputContext->m_wheelDelta > 0.f)
					m_y_scroll += m_y_scroll_speed;
				else
					m_y_scroll -= m_y_scroll_speed;

				if (m_y_scroll <= m_y_scrollLimit)
					m_y_scroll = m_y_scrollLimit;
				if (m_y_scroll > 0.f)
					m_y_scroll = 0.f;

				/*for (u32 i = 0, sz = m_items.size(); i < sz; ++i)
				{
					auto item = m_items[i];
					item->m_textElement->m_offset.y = m_y_scroll;
				}*/
			}
			else
			{
				if (g_engine->m_inputContext->m_wheelDelta > 0.f)
					m_y_scrollTarget += m_y_scroll_speed;
				else
					m_y_scrollTarget -= m_y_scroll_speed;

				if (m_y_scrollTarget <= m_y_scrollLimit)
					m_y_scrollTarget = m_y_scrollLimit;
				if (m_y_scrollTarget > 0.f)
					m_y_scrollTarget = 0.f;
			}

			//Rebuild();
		}
	}

	
}

void yyGUIListBox::OnDraw(f32 dt) {
	if (!m_visible) return;

	if (m_isAnimatedScroll)
	{
		if (m_y_scroll != m_y_scrollTarget)
		{
			m_y_scroll = math::lerp(m_y_scroll, m_y_scrollTarget, m_animatedScrollLerp);
			if (m_y_scroll <= m_y_scrollLimit)
				m_y_scroll = m_y_scrollLimit;
			if (m_y_scroll > 0.f)
				m_y_scroll = 0.f;

			if (std::abs(m_y_scrollTarget - m_y_scroll) < 0.01f)
				m_y_scroll = m_y_scrollTarget;
		}
	}

	for (u32 i = 0, sz = m_items.size(); i < sz; ++i)
	{
		auto item = m_items[i];
		item->m_textElement->m_offset.y = m_y_scroll;
	}

	g_engine->m_videoAPI->DrawRectangle(m_buildRectInPixels, m_bgColorCurrent, m_bgColorCurrent);

	for (u32 i = 0, sz = m_items.size(); i < sz; ++i)
	{
		auto item = m_items[i];

		if (item->m_selected)
		{
			v4f r = item->m_rect;
			r.y += m_y_scroll;
			r.w += m_y_scroll;
			g_engine->m_videoAPI->DrawRectangle(r, m_itemColorSelected, m_itemColorSelected);
		}

		item->m_textElement->OnDraw(dt);
	}
}

void yyGUIListBox::Rebuild() {
	yyGUIElement::CallOnRebuildSetRects();

	m_contentHeight = 0.f;
	for (u32 i = 0, sz = m_items.size(); i < sz; ++i)
	{
		auto item = m_items[i];

		f32 y2 = m_buildRectInPixels.y + m_contentHeight;// +m_y_scroll;


		item->m_rect = v4f(
			m_buildRectInPixels.x,
			y2,
			m_buildRectInPixels.z,
			y2 + m_font->m_maxHeight);

		item->m_textElement->SetBuildRect(item->m_rect);
		item->m_textElement->Rebuild();
		 
		m_contentHeight += m_font->m_maxHeight;
	}

	m_y_scrollLimit = -m_contentHeight;
	m_y_scrollLimit += m_buildRectInPixels.w - m_buildRectInPixels.y;
}

yyGUIListBoxItem* yyGUIListBox::AddItem(const wchar_t* text) {
	assert(text);
	yyGUIListBoxItem* newItem = yyCreate2<yyGUIListBoxItem>(m_font, text);
	m_items.push_back(newItem);
	return newItem;
}

void yyGUIListBox::DeleteItem(yyGUIListBoxItem* item) {
	assert(item);
	m_items.erase_first(item);
}

YY_API yyGUIListBox* YY_C_DECL yyGUICreateListBox(const v4f& rect, yyGUIFont* font, yyGUIDrawGroup* drawGroup) {
	assert(font);
	yyGUIListBox* element = yyCreate<yyGUIListBox>();
	element->m_font = font;
	element->m_y_scroll_speed = font->m_maxHeight;
	element->SetDrawGroup(drawGroup);
	element->SetBuildRect(rect);
	return element;
}

yyGUIListBoxItem::yyGUIListBoxItem(yyGUIFont* f, const wchar_t* t) {
	m_textElement = yyGUICreateText(v2f(), f, t, 0);
	yyGUIRemoveElement(m_textElement);
	m_selected = false;
	m_isMouseHover = false;
}

yyGUIListBoxItem::~yyGUIListBoxItem() {
	if (m_textElement) yyDestroy(m_textElement);
}

void yyGUIListBoxItem::SetText(const wchar_t* t) {
	assert(t);
	m_textElement->SetText(t);
}
const wchar_t* yyGUIListBoxItem::GetText() {
	return m_textElement->m_text.data();
}