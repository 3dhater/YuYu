#ifndef _YUYU_GUI_TEXT_H_
#define _YUYU_GUI_TEXT_H_

//struct _yyGui_text_model
//{
//	_yyGui_text_model();
//	~_yyGui_text_model();
//	void Reserve(u32 numSymbols);
//	void AddChar(const v4f& rect, yyGUIFontGlyph* glyph);
//	yyModel* m_model;
//	bool m_isUsing;   // создавать ли на её основе GPU ресурс
//	u32 m_numOfSymbols;
//};

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
	//_yyGui_text_model m_text_models[YY_MAX_FONT_TEXTURES];
public:
	yyGUIText();
	virtual ~yyGUIText();
	virtual void OnUpdate(f32 dt);
	virtual void OnDraw();
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