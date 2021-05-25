#ifndef _YUYU_GUI_PICBOXH_
#define _YUYU_GUI_PICBOXH_

class yyGUIPictureBox : public yyGUIElement
{
public:
	yyGUIPictureBox();
	virtual ~yyGUIPictureBox();
	virtual void OnUpdate(f32 dt);
	virtual void OnDraw(f32 dt);
	virtual void Rebuild();

	yyResource* DropTexture();
	yyResource* m_texture;
	yyResource* m_pictureBoxModel; // yyResourceType::Model

	yyGUICallback m_onClick; // LMB down

	v4f m_uvRect;
};

#endif