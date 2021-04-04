#ifndef _YUYU_GUI_GROUPH_
#define _YUYU_GUI_GROUPH_

class yyGUIGroup: public yyGUIElement
{
public:
	yyGUIGroup();
	virtual ~yyGUIGroup();
	virtual void OnUpdate(f32 dt);
	virtual void OnDraw();
	virtual void Rebuild();

	yyGUICallback m_onClick; // LMB down
	yyGUICallback m_onMouseInRect;
	yyGUICallback m_onMouseLeave;

	virtual void AddElement(yyGUIElement*);
};

#endif