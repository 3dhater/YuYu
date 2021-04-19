#ifndef _YUYU_GUI_GROUPH_
#define _YUYU_GUI_GROUPH_

class yyGUIGroup: public yyGUIElement
{
	void _setVisible(yyGUIElement*, bool);
public:
	yyGUIGroup();
	virtual ~yyGUIGroup();
	virtual void OnUpdate(f32 dt);
	virtual void OnDraw();
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