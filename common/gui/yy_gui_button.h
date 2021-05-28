#ifndef _YUYU_GUI_BUTTON_H_
#define _YUYU_GUI_BUTTON_H_

#include "../yy_color.h"

class yyGUIButton : public yyGUIElement
{
	yyVideoDriverAPI* m_gpu;
public:
	yyGUIButton();
	virtual ~yyGUIButton();
	virtual void OnUpdate(f32 dt);
	virtual void OnDraw(f32 dt);
	virtual void Rebuild();

	virtual void SetVisible(bool v);
	virtual void SetOffset(const v2f& o);

	virtual void SetMouseHoverTexture(yyResource*, v4f* uv = 0);
	virtual void SetMouseClickTexture(yyResource*, v4f* uv = 0);


	virtual void SetColor(const yyColor&, s32 pictureBox);
	virtual void SetOpacity(f32, s32 pictureBox);
	virtual yyColor& GetColor(s32 pictureBox);
	virtual f32 GetOpacity(s32 pictureBox);

	yyGUIPictureBox* m_basePB;
	yyGUIPictureBox* m_mouseHoverPB;
	yyGUIPictureBox* m_mouseClickPB;
	yyResource* m_baseTexture;

	yyGUICallback m_onClick; // LMB down
	yyGUICallback m_onRelease; // LMB up
	yyGUICallback m_onMouseEnter;
	yyGUICallback m_onMouseLeave;

	//bool m_mouseInRect;
	bool m_isClicked;
	bool m_isChecked;

	bool m_useAsCheckbox;

	bool m_textResizeButton;
	yyGUIText* m_textElement;
	yyStringW m_text;
	yyColor m_textColor;
	yyColor m_textColorHover;
	yyColor m_textColorPress;
	virtual void SetText(const wchar_t*, yyGUIFont*, bool resizeButton);
	virtual void SetTextColor(const yyColor&);// nneed to remove

	// fade in\out for m_mouseHoverPB
	bool m_isAnimated;

	v4f m_uvRect;
};

#endif