#ifndef _YUYU_GUI_GROUPH_
#define _YUYU_GUI_GROUPH_


/* How to set without pixels, using only values from 0 to 1

m_gui_group = yyGUICreateGroup(v4f(), -1, 0);
m_gui_group->m_buildRect = m_creationRect;        // set 0 to 1 here
m_gui_group->SetRectsFromBuildRect();             // then call this
*/
class yyGUIGroup: public yyGUIElement
{
	void _setVisible(yyGUIElement*, bool);
public:
	yyGUIGroup();
	virtual ~yyGUIGroup();
	virtual void OnUpdate(f32 dt);
	virtual void OnDraw(f32 dt);
	virtual void Rebuild();

	yyGUICallback m_onClick; // LMB down
	yyGUICallback m_onMouseInRect;
	yyGUICallback m_onMouseLeave;

	// for example convert range this 0;800; to this 0.f;1.f;
	// then using this in rebuild for automatic size change
	// use m_useAutomaticSizeChange
	// SetRectInZeroOne calculate this range. call this when you
	//   changed m_activeAreaRect
	// if m_useAutomaticSizeChange then all rects will be same size (active area, rebuild, clip)
	/*v4f m_rectInZeroOne;
	bool m_useAutomaticSizeChange;
	virtual void SetRectInZeroOne(const v4f& rectInPixels);*/


	virtual void AddElement(yyGUIElement*);
	virtual void SetVisible(bool) override;

};

#endif