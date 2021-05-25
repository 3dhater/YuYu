#ifndef _YUYU_GUI_TEXT_H_
#define _YUYU_GUI_TEXT_H_

struct yyGUITextDrawNode 
{
	yyGUITextDrawNode() 	{
		m_textureGPU = 0;
		m_modelGPU = 0;
		m_numOfSymbols = 0;
		m_isDraw = false;
		m_modelSource = yyMegaAllocator::CreateModel();
	}
	~yyGUITextDrawNode() {
		if (m_modelGPU) yyMegaAllocator::Destroy(m_modelGPU);
		if (m_modelSource) yyMegaAllocator::Destroy(m_modelSource);
	}
	yyResource* m_textureGPU;
	yyResource* m_modelGPU;

	bool m_isDraw;
	yyModel* m_modelSource;
	u32 m_numOfSymbols;
};

class yyGUIText: public yyGUIElement
{
public:
	yyGUIText();
	virtual ~yyGUIText();
	virtual void OnUpdate(f32 dt);
	virtual void OnDraw(f32 dt);
	virtual void Rebuild();

	virtual void SetFont(yyGUIFont*);
	virtual void SetText(const wchar_t* text, ...);
	virtual void Clear();
	virtual void SetBufferSize(u32 newSize);

	yyStringW m_text;
	yyGUIFont* m_font;
	u16 m_textureCount;

	v2f m_position;
	wchar_t* m_buffer;
	u32 m_bufferSize;

	yyGUITextDrawNode* m_drawNodes;

	yyGUICallback m_onMouseInRect;
	yyGUICallback m_onClick; // LMB down
};

#endif