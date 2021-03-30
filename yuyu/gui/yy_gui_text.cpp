#include "yy.h"
#include "yy_gui.h"
#include "math/vec.h"

#include "yy_model.h"
#include "yy_input.h"

#include "../engine.h"

extern Engine * g_engine;

yyGUIText::yyGUIText(){
	m_font = 0;
	m_type = yyGUIElementType::Text;
	m_buffer = 0;
	SetBufferSize(1024);
	m_onMouseInRect = 0;
	m_onClick = 0;
}

yyGUIText::~yyGUIText(){
	if (m_buffer)
	{
		yyMemFree(m_buffer);
		m_buffer = 0;
	}
	Clear();
}

void yyGUIText::OnUpdate(f32 dt){
	if (!m_visible) return;
	yyGUIElement::CheckCursorInRect();
	if (m_ignoreInput) return;

	if (m_isInActiveAreaRect)
	{

		if (m_onMouseInRect)
			m_onMouseInRect(this,m_id);

		if (g_engine->m_inputContext->m_isLMBDown)
		{
			if (m_onClick)
				m_onClick(this, m_id);
		}
	}
}

void yyGUIText::OnDraw(){
	if (!m_visible) return;
	for (u16 k = 0, ksz = m_drawNodes.m_size; k < ksz; ++k)
	{
		auto & dn = m_drawNodes.m_data[k];
		if (dn.m_texture)
			g_engine->m_videoAPI->SetTexture(0, dn.m_texture);
		if (dn.m_model)
			g_engine->m_videoAPI->SetModel(dn.m_model);
		g_engine->m_videoAPI->Draw();
	}
}

void yyGUIText::SetBufferSize(u32 newSize){
	if (m_bufferSize == newSize)
		return;
	if (m_buffer)
	{
		yyMemFree(m_buffer);
		m_buffer = 0;
	}
	m_buffer = (wchar_t*)yyMemAlloc(newSize * sizeof(wchar_t));
	m_bufferSize = newSize;
}

#define YY_MAX_FONT_TEXTURES 100
struct _yyGui_text_model {
	_yyGui_text_model() {
		m_model = 0;
		m_isUsing = false;
		m_numOfSymbols = 0;
	}
	~_yyGui_text_model() {
		if (m_model)
			yyDestroy(m_model);
	}
	void Reserve(u32 numSymbols) {
		if (m_numOfSymbols == numSymbols)
			return;
		m_numOfSymbols = numSymbols;

		if (m_model->m_indices)  yyMemFree(m_model->m_indices);
		if (m_model->m_vertices) yyMemFree(m_model->m_vertices);

		m_model->m_vertices = (u8*)yyMemAlloc(sizeof(yyVertexGUI) * 4 * numSymbols);
		m_model->m_indices = (u8*)yyMemAlloc(numSymbols * 2 * 3 * sizeof(u16));
	}

	//v4f(TXN, TYN, TXP, TYP)
	void AddChar(const v4f& rect, yyGUIFontGlyph* glyph){
		yyVertexGUI v1, v2, v3, v4;
		
		v1.m_position.set(rect.x, rect.w);
		v2.m_position.set(rect.x, rect.y);
		v3.m_position.set(rect.z, rect.y);
		v4.m_position.set(rect.z, rect.w);

		v1.m_tcoords = glyph->lb;
		v2.m_tcoords = glyph->lt;
		v3.m_tcoords = glyph->rt;
		v4.m_tcoords = glyph->rb;

		auto v = (yyVertexGUI*)m_model->m_vertices;
		v += m_model->m_vCount;
		*v = v1; 
		v++;
		*v = v3; 
		v++;
		*v = v4; 
		v++;
		*v = v2; 
		v++;

		u16 * i = (u16*)m_model->m_indices;
		i += m_model->m_iCount;
		*i = m_model->m_vCount;    ++i;
		*i = m_model->m_vCount +1; ++i;
		*i = m_model->m_vCount +2; ++i;
		*i = m_model->m_vCount ; ++i;
		*i = m_model->m_vCount +3; ++i;
		*i = m_model->m_vCount +1; ++i;

		m_model->m_vCount += 4;
		m_model->m_iCount += 6;
	}
	yyModel* m_model;
	bool m_isUsing;   // создавать ли на её основе GPU ресурс
	u32 m_numOfSymbols;
};
_yyGui_text_model g_text_models[YY_MAX_FONT_TEXTURES];

void yyGUIText::SetText(const wchar_t* format, ...){
	assert(m_font);
	Clear();

	auto vAPI = yyGetVideoDriverAPI();

	va_list arg;
	va_start(arg, format);
	_vsnwprintf(m_buffer, m_bufferSize, format, arg);
	va_end(arg);

	auto len = util::str_len(m_buffer);
	if (!len)
		return;

	v2f text_pointer = m_position;
	m_buildingRect.x = m_position.x;
	m_buildingRect.y = m_position.y;

	f32 glyph_max_height = 0.f;
	for (size_t i = 0; i < len; ++i)
	{
		wchar_t ch = m_buffer[i];

		auto glyph = m_font->GetGlyph(ch);
		if (!glyph)
			continue;

		auto & model = g_text_models[glyph->textureID];
		model.m_isUsing = true;
		model.Reserve(len);

		auto TXN = text_pointer.x;
		auto TXP = TXN + glyph->width;
		auto TYN = text_pointer.y;
		auto TYP = TYN + glyph->height;

		if (glyph->height > glyph_max_height)
			glyph_max_height = glyph->height;

		model.AddChar(v4f(TXN, TYN, TXP, TYP), glyph);

		text_pointer.x += glyph->width;
	}

	m_buildingRect.z = text_pointer.x;
	m_buildingRect.w = text_pointer.y + glyph_max_height;
	m_activeAreaRect = m_buildingRect;
	m_clipRect = m_buildingRect;

	u32 array_index_counter = 0;
	for (int i = 0; i < YY_MAX_FONT_TEXTURES; ++i)
	{
		if (g_text_models[i].m_isUsing)
		{
			yyGUITextDrawNode * dn = 0;

			dn = m_drawNodes.get(array_index_counter);

			if (!dn)
			{
				yyGUITextDrawNode ndn;
				m_drawNodes.push_back(ndn);
				dn = m_drawNodes.get(m_drawNodes.m_size - 1);
			}
			else
				m_drawNodes.m_size++;

			dn->m_texture = m_font->m_textures[i];
			if (dn->m_model)
			{
				if(!dn->m_model->m_isLoaded)
					vAPI->LoadModel(dn->m_model);
			}
			else
			{
				dn->m_model = vAPI->CreateModel(g_text_models[i].m_model);
				/*printf("Verts:\n");
				for (int vi = 0; vi < g_text_models[i].m_model->m_vCount; ++vi)
				{
					yyVertexGUI* vg = (yyVertexGUI*)g_text_models[i].m_model->m_vertices;
					printf("\t[%f %f] [%f %f]\n", vg[vi].m_position.x, vg[vi].m_position.y, vg[vi].m_tcoords.x, vg[vi].m_tcoords.y);
				}
				printf("Inds:\n");
				for (int vi = 0; vi < g_text_models[i].m_model->m_iCount; ++vi)
				{
					u16* vg = (u16*)g_text_models[i].m_model->m_indices;
					printf("\t%u\n", vg[vi]);
				}*/
			}

			++array_index_counter;
		}
	}
}

void yyGUIText::Clear(){
	for (int i = 0; i < YY_MAX_FONT_TEXTURES; ++i)
	{
		if (!g_text_models[i].m_model) {
			g_text_models[i].m_model = yyCreate<yyModel>();
			g_text_models[i].m_model->m_stride = sizeof(yyVertexGUI);
			g_text_models[i].m_model->m_vertexType = yyVertexType::GUI;
		}
		g_text_models[i].m_model->m_iCount = 0;
		g_text_models[i].m_model->m_vCount = 0;
		g_text_models[i].m_isUsing = false;
	}

	auto vAPI = yyGetVideoDriverAPI();
	for (u16 i = 0; i < m_drawNodes.m_size; ++i)
	{
		auto & dn = m_drawNodes.m_data[i];
		if (dn.m_model)
		{
			if (dn.m_model->m_isLoaded)
				vAPI->UnloadModel(dn.m_model);
		}
	}
	m_drawNodes.clear();
}


YY_API yyGUIText* YY_C_DECL yyGUICreateText(const v2f& position, yyGUIFont* font, const wchar_t* text, yyGUIDrawGroup* drawGroup){
	yyGUIText* element = yyCreate<yyGUIText>();
	element->SetDrawGroup(drawGroup);
	element->m_font = font;
	element->m_position = position;
	
	yyStringW wstr = text;

	if (wstr.size())
		element->SetText(text);

//	g_engine->addGuiElement(element);

	return element;
}
