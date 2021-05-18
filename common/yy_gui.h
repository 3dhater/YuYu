#ifndef _YUYU_GUI_H_
#define _YUYU_GUI_H_

#include "math/vec.h"
#include "yy_color.h"
#include "containers\list.h"

typedef void(*yyGUICallback)(yyGUIElement* elem, s32 m_id);

enum class yyGUIElementType  : u32
{
	Unknown,
	PictureBox,
	Button,
	Text,
	TextInput,
	Group
};

class yyGUIElement
{
protected:
	bool m_visible;
	bool m_isInActiveAreaRect;
	bool m_ignoreInput;
	//bool m_isInputFocus;
public:

	yyGUIElement()
	:
		m_id(-1),
		m_visible(true),
		m_ignoreInput(false),
		m_type(yyGUIElementType::Unknown)
	{
		m_window = yyGetMainWindow();
		m_color = ColorWhite;
		m_drawGroup = 0;
		m_onDraw = 0;
		m_onMouseInRect = 0;
		m_onRebuildSetRects = 0;
		m_userData = 0;
		m_ignoreSetCursorInGUI = false;
		//m_isInputFocus = false;

		m_align = AlignLeftTop;
		m_parent = 0;
		m_useProportionalScaling = false;
	}
	virtual ~yyGUIElement(){}
	
	virtual void SetOffset(const v2f& o) { m_offset = o; }
	virtual void SetVisible(bool v) { m_visible = v; }
	virtual void IgnoreInput(bool v) { m_ignoreInput = v; }
	bool IsVisible() { return m_visible; }
	bool IsInRect() { return m_isInActiveAreaRect; }
	//bool IsInputFocus() { return m_isInputFocus; }

	virtual void OnUpdate(f32 dt) = 0;
	virtual void OnDraw() = 0;
	virtual void Rebuild() = 0;

	void SetDrawGroup(yyGUIDrawGroup* gr);

	s32 m_id;
	yyGUIElementType m_type;
	yyGUIDrawGroup* m_drawGroup;
	
	// When you call yyGUICreate... and set 'rect' or 'position'
	//  you set this values in pixels, and engine must convert (SetBuildRect())
	//   this values to range from 0 to 1
	v4f m_buildRect;
	virtual void SetBuildRect(const v4f& rectInPixels);
	virtual void SetRectsFromBuildRect();

	// After creating element, every new element must call Rebuild()
	// In Rebuild, element must calculate rects in pixels, using
	//  information like parent position and aligment
	v4f m_buildRectInPixelsCreation; // save original size
	v4f m_buildRectInPixels;
	v4f m_sensorRectInPixels;
	v4f m_clipRectInPixels;

	// using m_buildRect to calculate new ...RectInPixels
	bool m_useProportionalScaling;

	//// element reacts when cursor in this rect
	//v4f m_activeAreaRect;
	//// for rebuild
	//v4f m_buildingRect;
	//// for scissor 
	//v4f m_clipRect;
	//v4f m_activeAreaRect_global;
	//v4f m_buildingRect_global;
	//v4f m_clipRect_global;

	v2f m_offset;
	yyColor m_color;

	void* m_userData;

	yyGUICallback m_onDraw;
	// if you want to calculate ___Rect by yourself
	yyGUICallback m_onRebuildSetRects;
	void CallOnRebuildSetRects();
	
	yyGUICallback m_onMouseInRect;

	enum Align {
		AlignLeftTop,
		AlignRightTop,
		AlignLeftBottom,
		AlignRightBottom,
		AlignCenter,
		AlignCenterTop,
	}m_align;
	yyGUIElement* m_parent;
	yyList<yyGUIElement*> m_children;
	void SetParent(yyGUIElement* parent) {
		m_parent = parent;
		if (parent) 
		{
			parent->m_children.push_back(this);
		}
		else
		{
			parent->m_children.erase_first(this);
		}
	}

	/*
		if(!m_ignoreSetCursorInGUI)
			g_engine->m_cursorInGUI = true;
	*/
	bool m_ignoreSetCursorInGUI;

	// do not call
	void CheckCursorInRect();

	yyWindow* m_window;
};

#include "gui\yy_gui_font.h"

#include "gui\yy_gui_group.h"
#include "gui\yy_gui_pictureBox.h"
#include "gui\yy_gui_text.h"
#include "gui\yy_gui_textInput.h"

#include "gui\yy_gui_button.h"

extern "C"
{
	// return true if cursor in gui element
	YY_API bool YY_C_DECL yyGUIUpdate(f32 deltaTime);
	YY_API bool YY_C_DECL yyGUIIsInputFocus();
	// call this when windows size changed or when you add children to some gui element
	YY_API void YY_C_DECL yyGUIRebuild();

	YY_API void YY_C_DECL yyGUIDrawAll();
	YY_API yyGUIPictureBox* YY_C_DECL yyGUICreatePictureBox(const v4f& rect, yyResource* texture, s32 id, yyGUIDrawGroup* drawGroup, v4i* uv = 0);
	// auto delete
	YY_API yyGUIFont* YY_C_DECL yyGUILoadFont(const char* path);
	YY_API yyGUIText* YY_C_DECL yyGUICreateText(const v2f& position, yyGUIFont* font, const wchar_t* text, yyGUIDrawGroup* drawGroup);
	YY_API yyGUITextInput* YY_C_DECL yyGUICreateTextInput(const v4f& rect, yyGUIFont* font, const wchar_t* text, yyGUIDrawGroup* drawGroup);
	// baseTexture can be 0
	YY_API yyGUIButton* YY_C_DECL yyGUICreateButton(const v4f& rect, yyResource* baseTexture, s32 id, yyGUIDrawGroup* drawGroup, v4i* uv = 0);
	YY_API void YY_C_DECL yyGUIDeleteElement(yyGUIElement* elem);
	YY_API void YY_C_DECL yyGUIRemoveElement(yyGUIElement* elem); // without yyDestroy(elem);
	YY_API yyGUIElement* YY_C_DECL yyGUIGetElementInMouseFocus();
	YY_API yyGUIDrawGroup* YY_C_DECL yyGUICreateDrawGroup();
	YY_API yyGUIGroup* YY_C_DECL yyGUICreateGroup(const v4f& rect, s32 id, yyGUIDrawGroup* drawGroup);
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