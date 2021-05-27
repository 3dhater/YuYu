#ifndef _YUYU_GUI_LISTBOX_H_
#define _YUYU_GUI_LISTBOX_H_

class yyGUIListBoxItem
{
	yyGUIText* m_textElement;
	bool m_selected;
	bool m_isMouseHover;
	v4f m_rect;
public:
	yyGUIListBoxItem(yyGUIFont*, const wchar_t* t);
	~yyGUIListBoxItem();

	bool IsSelected() { return m_selected; }
	void Select(bool v) { m_selected = v; }

	virtual void SetText(const wchar_t*);
	virtual const wchar_t* GetText();
	
	

	friend class yyGUIListBox;
};

class yyGUIListBox : public yyGUIElement
{
	yyArray<yyGUIListBoxItem*> m_items;
	f32 m_y_scroll;
	f32 m_y_scrollTarget;
	f32 m_contentHeight;
	f32 m_y_scrollLimit;
public:
	yyGUIListBox();
	virtual ~yyGUIListBox();
	virtual void OnUpdate(f32 dt);
	virtual void OnDraw(f32 dt);
	virtual void Rebuild();

	yyColor m_bgColorCurrent;
	yyColor m_bgColor;
	yyColor m_bgColorHover;

	yyColor m_itemColorHover;
	yyColor m_itemColorSelected;

	yyGUIFont* m_font;
	f32 m_y_scroll_speed; // default max font height

	bool m_isSelectable;
	bool m_isAnimatedScroll;
	f32  m_animatedScrollLerp; // 0...1 default 0.15f

	u32 GetItemsCount() { return m_items.size(); }
	yyGUIListBoxItem* GetItem(u32 index)
	{
		assert(index < m_items.size());
		return m_items[index];
	}

	virtual yyGUIListBoxItem* AddItem(const wchar_t* text);
	virtual void DeleteItem(yyGUIListBoxItem*);
};

#endif