#ifndef _YUYU_GUI_TEXT_H_
#define _YUYU_GUI_TEXT_H_

struct yyGUITextDrawNode 
{
	yyGUITextDrawNode() 	{
		m_textureGPU = 0;
		m_modelGPU = 0;
	}
	~yyGUITextDrawNode() {
		if (m_modelGPU) yyMegaAllocator::Destroy(m_modelGPU);
	}
	yyResource* m_textureGPU;
	yyResource* m_modelGPU;
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