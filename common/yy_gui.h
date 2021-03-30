#ifndef _YUYU_GUI_H_
#define _YUYU_GUI_H_

#include "math/vec.h"
#include "yy_color.h"

typedef void(*yyGUICallback)(yyGUIElement* elem, s32 m_id);

enum class yyGUIElementType  : u32
{
	Unknown,
	PictureBox,
	Button,
	Text
};

class yyGUIElement
{
protected:
	bool m_visible;
	bool m_isInActiveAreaRect;
	bool m_ignoreInput;
public:

	yyGUIElement()
	:
		m_id(-1),
		m_visible(true),
		m_ignoreInput(false),
		m_type(yyGUIElementType::Unknown)
	{
		m_color = ColorWhite;
		m_drawGroup = 0;
	}
	virtual ~yyGUIElement(){}
	
	virtual void SetOffset(const v2f& o) { m_offset = o; }
	virtual void SetVisible(bool v) { m_visible = v; }
	virtual void IgnoreInput(bool v) { m_ignoreInput = v; }
	bool IsVisible() { return m_visible; }
	bool IsInRect() { return m_isInActiveAreaRect; }

	virtual void OnUpdate(f32 dt) = 0;
	virtual void OnDraw() = 0;

	void CheckCursorInRect();
	void SetDrawGroup(yyGUIDrawGroup* gr);

	s32 m_id;
	yyGUIElementType m_type;
	yyGUIDrawGroup* m_drawGroup;

	// element reacts when cursor in this rect
	v4f m_activeAreaRect;
	// for rebuild
	v4f m_buildingRect;
	// for scissor 
	v4f m_clipRect;

	v2f m_offset;
	yyColor m_color;
};

#include "gui\yy_gui_font.h"

#include "gui\yy_gui_pictureBox.h"
#include "gui\yy_gui_text.h"

#include "gui\yy_gui_button.h"

extern "C"
{
	// return true if cursor in gui element
	YY_API bool YY_C_DECL yyGUIUpdate(f32 deltaTime);

	YY_API void YY_C_DECL yyGUIDrawAll();
	YY_API yyGUIPictureBox* YY_C_DECL yyGUICreatePictureBox(const v4f& rect, yyResource* texture, s32 id, yyGUIDrawGroup* drawGroup);
	YY_API yyGUIFont* YY_C_DECL yyGUILoadFont(const char* path);
	YY_API yyGUIText* YY_C_DECL yyGUICreateText(const v2f& position, yyGUIFont* font, const wchar_t* text, yyGUIDrawGroup* drawGroup);
	YY_API yyGUIButton* YY_C_DECL yyGUICreateButton(const v4f& rect, yyResource* baseTexture, s32 id, yyGUIDrawGroup* drawGroup);
	YY_API void YY_C_DECL yyGUIDeleteElement(yyGUIElement* elem);
	YY_API void YY_C_DECL yyGUIRemoveElement(yyGUIElement* elem); // without yyDestroy(elem);
	YY_API yyGUIElement* YY_C_DECL yyGUIGetElementInMouseFocus();
	YY_API yyGUIDrawGroup* YY_C_DECL yyGUICreateDrawGroup();
	// delete with all elements
	YY_API void YY_C_DECL yyGUIDeleteDrawGroup(yyGUIDrawGroup*);
	YY_API void YY_C_DECL yyGUIDrawGroupMoveBack(yyGUIDrawGroup*);
	YY_API void YY_C_DECL yyGUIDrawGroupMoveFront(yyGUIDrawGroup*);
}

class yyGUIDrawGroup 
{
public:
	yyGUIDrawGroup() {
		m_isDraw  = true;
		m_isInput = true;
	};
	~yyGUIDrawGroup() {
		auto guiNode = m_guiElements.head();
		if (guiNode)
		{
			for (size_t i = 0, sz = m_guiElements.size(); i < sz; ++i)
			{
				yyDestroy(guiNode->m_data);
				guiNode = guiNode->m_right;
			}
		}
	};

	void AddElement(yyGUIElement*e) { m_guiElements.push_back(e); }
	void RemoveElement(yyGUIElement*e) { m_guiElements.erase_first(e); }
	yyList<yyGUIElement*>& GetElements() { return m_guiElements; }

	void SetDraw(bool v) { m_isDraw = v; }
	void SetInput(bool v) { m_isInput = v; }
	bool IsDraw() { return m_isDraw; }
	bool IsInput() { return m_isInput; }


	friend YY_API bool YY_C_DECL yyGUIUpdate(f32 deltaTime);
	friend YY_API void YY_C_DECL yyGUIDrawAll();

private:
	yyList<yyGUIElement*> m_guiElements;
	bool m_isDraw;
	bool m_isInput;
};



#endif