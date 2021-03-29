#ifndef _YUYU_GUI_BUTTON_H_
#define _YUYU_GUI_BUTTON_H_

class yyGUIButton : public yyGUIElement
{
	yyVideoDriverAPI* m_gpu;
public:
	yyGUIButton();
	virtual ~yyGUIButton();
	virtual void OnUpdate(f32 dt);
	virtual void OnDraw();
	virtual void SetVisible(bool v);
	virtual void SetOffset(const v2f& o);

	virtual void SetMouseHoverTexture(yyResource*);
	virtual void SetMouseClickTexture(yyResource*);

	yyGUIPictureBox* m_basePB;
	yyGUIPictureBox* m_mouseHoverPB;
	yyGUIPictureBox* m_mouseClickPB;
	//yyResource* m_baseTexture;

	yyGUICallback m_onClick; // LMB down
	yyGUICallback m_onRelease; // LMB up

	//bool m_mouseInRect;
	bool m_isClicked;
	bool m_isChecked;

	bool m_useAsCheckbox;

	// fade in\out for m_mouseHoverPB
	bool m_isAnimated;

};

#endif