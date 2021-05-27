#ifndef _YUYU_GUI_CHECKBOX_H_
#define _YUYU_GUI_CHECKBOX_H_

enum class yyGUICheckBoxType : u32
{
	Type1
};

class yyGUICheckBox : public yyGUIElement
{
	yyGUICheckBoxType m_checkBoxType;
public:
	yyGUICheckBox();
	virtual ~yyGUICheckBox();
	virtual void OnUpdate(f32 dt);
	virtual void OnDraw(f32 dt);
	virtual void Rebuild();

	yyGUIPictureBox* m_basePB;
	yyGUIPictureBox* m_hoverPB;
	yyGUIPictureBox* m_checkPB;
	yyGUIText* m_text;
	f32 m_textPositionAdd; // only for implementation, don't use it
	v2f m_textOffset;


	yyGUIFont* m_font;

	bool m_isAnimated;
	bool m_isChecked;

	yyGUICallback m_onClick; // LMB down
	yyGUICallback m_onRelease; // LMB up
};

#endif