#ifndef _YUYU_GUI_TEXT_H_
#define _YUYU_GUI_TEXT_H_

struct yyGUITextDrawNode 
{
	yyGUITextDrawNode() 	{
		m_texture = 0;
		m_model = 0;
	}
	~yyGUITextDrawNode() {
		if (m_model) yyMegaAllocator::Destroy(m_model);
	}
	yyResource* m_texture;
	yyResource* m_model;
};

class yyGUIText: public yyGUIElement
{
public:
	yyGUIText();
	virtual ~yyGUIText();
	virtual void OnUpdate(f32 dt);
	virtual void OnDraw();
	virtual void Rebuild();

	virtual void SetText(const wchar_t* text, ...);
	virtual void Clear();
	virtual void SetBufferSize(u32 newSize);

	yyStringW m_text;
	yyGUIFont* m_font;
	v2f m_position;
	wchar_t* m_buffer;
	u32 m_bufferSize;

	yyArraySimple<yyGUITextDrawNode> m_drawNodes; // yyResourceType::Model

	yyGUICallback m_onMouseInRect;
	yyGUICallback m_onClick; // LMB down
};

#endif