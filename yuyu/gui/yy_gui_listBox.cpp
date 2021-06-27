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
	m_isMultiSelect = false;
	m_isAnimatedScroll = true;
	m_font = 0;
	m_onSelect = 0;
	m_y_scroll = 0.f;
	m_bgColor.set(0.2f);
	m_bgColorHover.set(0.25f);
	m_bgColorCurrent = m_bgColor;
	m_contentHeight = 0.f;
	m_itemColorHover.set(0.35f);
	m_itemColorSelected = ColorBlue;
	m_itemColorSelectedHover.set(0.2f, 0.2f, 1.f, 1.f);
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
		}

		if (m_itemHover && g_engine->m_inputContext->m_isLMBDown)
		{
			if (m_isSelectable)
			{
				if (m_isMultiSelect)
				{
					m_itemHover->m_selected = m_itemHover->m_selected ? false : true;
				}
				else
				{
					for (u32 i = 0, sz = m_items.size(); i < sz; ++i)
					{
						auto item = m_items[i];
						item->m_selected = false;
					}
					m_itemHover->m_selected = m_itemHover->m_selected ? false : true;
				}
			}

			if (m_onSelect)
				m_onSelect(this, m_itemHover);
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

	g_engine->m_videoAPI->DrawRectangle(m_buildRectInPixels, m_bgColorCurrent, m_bgColorCurrent);

	m_itemHover = 0;

	for (u32 i = 0, sz = m_items.size(); i < sz; ++i)
	{
		auto item = m_items[i];

		v4f r = item->m_rect;
		r.y += m_y_scroll;
		r.w += m_y_scroll;

		item->m_isVisible = false;

		if (r.w >= m_buildRectInPixels.y && r.y <= m_buildRectInPixels.w)
		{
			item->m_isVisible = true;
		
			item->m_isMouseHover = false;
			if (m_isInActiveAreaRect)
			{
				if (g_engine->m_inputContext->m_cursorCoordsForGUI.y > r.y &&
					g_engine->m_inputContext->m_cursorCoordsForGUI.y <= r.w)
				{
					if (!m_itemHover)
					{
						item->m_isMouseHover = true;
						m_itemHover = item;
					}
				}
			}

			if (item->m_selected)
			{
				if (item->m_isMouseHover)
					g_engine->m_videoAPI->DrawRectangle(r, m_itemColorSelectedHover, m_itemColorSelectedHover);
				else
					g_engine->m_videoAPI->DrawRectangle(r, m_itemColorSelected, m_itemColorSelected);
			}
			else
			{
				if (item->m_isMouseHover)
					g_engine->m_videoAPI->DrawRectangle(r, m_itemColorHover, m_itemColorHover);
			}

			item->m_textElement->m_offset.y = m_y_scroll;
			item->m_textElement->OnDraw(dt);
		}
		/*else
		{
			wprintf(L"%s\n", item->GetText());
		}*/
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
	this->Rebuild();
	return newItem;
}

void yyGUIListBox::DeleteItem(yyGUIListBoxItem* item) {
	assert(item);
	m_items.erase_first(item);
}

void yyGUIListBox::SelectItem(yyGUIListBoxItem* item) {
	if (!m_isSelectable)
		return;
	if (m_isMultiSelect)
	{
		item->m_selected = item->m_selected ? false : true;
	}
	else
	{
		for (u32 i = 0, sz = m_items.size(); i < sz; ++i)
		{
			auto _item = m_items[i];
			_item->m_selected = false;
		}
		item->m_selected = item->m_selected ? false : true;
	}

	auto h = item->m_rect.w - item->m_rect.y;

	auto lbSz = m_buildRectInPixels.w - m_buildRectInPixels.y;
	
	/*printf("[%f %f : %f %f] %f %f\n", 
		item->m_rect.y, 
		item->m_rect.w, 
		item->m_rect.y + m_y_scroll + m_buildRectInPixels.y,
		item->m_rect.w + m_y_scroll + m_buildRectInPixels.y,
		m_buildRectInPixels.w, 
		m_buildRectInPixels.w-h
	);*/

	if (item->m_rect.y + m_y_scroll > m_buildRectInPixels.w - h)
	{
		auto v = item->m_rect.y - (m_buildRectInPixels.w - h);
		if (!m_isAnimatedScroll)
			m_y_scroll = -v;
		else
			m_y_scrollTarget = -v;
	}
	else if (item->m_rect.w + m_y_scroll < m_buildRectInPixels.y + h)
	{
		auto v = item->m_rect.w - (m_buildRectInPixels.y + h);

		if (!m_isAnimatedScroll)
			m_y_scroll = v;
		else
			m_y_scrollTarget = v;
	}
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
	m_userData = 0;
	m_id = -1;
	m_textElement = yyGUICreateText(v2f(), f, t, 0);
	yyGUIRemoveElement(m_textElement);
	m_selected = false;
	m_isMouseHover = false;
	m_isVisible = false;
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