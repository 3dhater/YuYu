﻿#ifndef _YUYU_GUI_BUTTON_H_
#define _YUYU_GUI_BUTTON_H_

class yyGUIButton : public yyGUIElement
{
public:
	yyGUIButton();
	virtual ~yyGUIButton();
	virtual void OnUpdate();
	virtual void OnDraw();
	virtual void SetVisible(bool v);

	virtual void SetMouseHoverTexture(yyResource*);
	virtual void SetMouseClickTexture(yyResource*);

	yyGUIPictureBox* m_basePB;
	yyGUIPictureBox* m_mouseHoverPB;
	yyGUIPictureBox* m_mouseClickPB;
	//yyResource* m_baseTexture;

	yyGUICallback m_onClick; // LMB down
	yyGUICallback m_onRelease; // LMB up

	bool m_mouseInRect;
	bool m_isClicked;
	bool m_isChecked;

	bool m_useAsCheckbox;
};

#endif