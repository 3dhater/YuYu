#ifndef _YUYU_GUI_PICBOXH_
#define _YUYU_GUI_PICBOXH_

class yyGUIPictureBox : public yyGUIElement
{
public:
	yyGUIPictureBox();
	virtual ~yyGUIPictureBox();

	yyResource* m_texture;
	yyResource* m_pictureBoxModel; // yyResourceType::Model

	yyGUICallback m_onClick; // LMB down
};

#endif