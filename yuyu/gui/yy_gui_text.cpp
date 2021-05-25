#include "yy.h"
#include "yy_gui.h"
#include "math/vec.h"

#include "yy_model.h"
#include "yy_input.h"

#include "../engine.h"

extern yyEngine * g_engine;

void _yyGui_text_model_AddChar(const v4f& rect, yyGUIFontGlyph* glyph, yyModel* model) {
	yyVertexGUI v1, v2, v3, v4;

	v1.m_position.set(rect.x, rect.w);
	v2.m_position.set(rect.x, rect.y);
	v3.m_position.set(rect.z, rect.y);
	v4.m_position.set(rect.z, rect.w);

	v1.m_tcoords = glyph->lb;
	v2.m_tcoords = glyph->lt;
	v3.m_tcoords = glyph->rt;
	v4.m_tcoords = glyph->rb;

	auto v = (yyVertexGUI*)model->m_vertices;
	v += model->m_vCount;
	*v = v1;
	v++;
	*v = v3;
	v++;
	*v = v4;
	v++;
	*v = v2;
	v++;

	u16 * i = (u16*)model->m_indices;
	i += model->m_iCount;
	*i = model->m_vCount;    ++i;
	*i = model->m_vCount + 1; ++i;
	*i = model->m_vCount + 2; ++i;
	*i = model->m_vCount; ++i;
	*i = model->m_vCount + 3; ++i;
	*i = model->m_vCount + 1; ++i;

	model->m_vCount += 4;
	model->m_iCount += 6;
}

yyGUIText::yyGUIText(){
	m_drawNodes = 0;
	m_textureCount = 0;
	m_font = 0;
	m_type = yyGUIElementType::Text;
	m_buffer = 0;
	m_bufferSize = 0;
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
	
	if (m_drawNodes) delete[] m_drawNodes;

	/*for (int i = 0; i < YY_MAX_FONT_TEXTURES; ++i)
	{
		if (m_text_models[i].m_model) {
			yyMegaAllocator::Destroy(m_text_models[i].m_model);
		}
	}*/
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

void yyGUIText::OnDraw(f32 dt){
	if (!m_visible) return;
	g_engine->m_videoAPI->SetGUIShaderData(this); 
	for (u16 i = 0; i < m_textureCount; ++i)
	{
		auto dn = &m_drawNodes[i];
		if (!dn->m_isDraw)
			continue;
		if (!dn->m_textureGPU)
			continue;
		if (!dn->m_modelGPU)
			continue;
		
		g_engine->m_videoAPI->SetTexture(0, dn->m_textureGPU);
		g_engine->m_videoAPI->SetModel(dn->m_modelGPU);
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


void yyGUIText::SetText(const wchar_t* format, ...){
	assert(m_font);
	Clear();

	va_list arg;
	va_start(arg, format);
	_vsnwprintf(m_buffer, m_bufferSize, format, arg);
	va_end(arg);

	m_text = m_buffer;

	auto len = util::str_len(m_buffer);
	if (!len)
		return;

	v2f text_pointer;
	text_pointer.x = m_buildRectInPixels.x;
	text_pointer.y = m_buildRectInPixels.y;

	v2f textLen;

	f32 glyph_max_height = 0.f;
	for (size_t i = 0; i < len; ++i)
	{
		wchar_t ch = m_buffer[i];

		auto glyph = m_font->GetGlyph(ch);
		if (!glyph)
			continue;

		auto dn = &m_drawNodes[glyph->textureID];
		dn->m_isDraw = true;
		if (len > dn->m_numOfSymbols)
		{
			dn->m_numOfSymbols = len;
			if (dn->m_modelSource->m_indices)  yyMemFree(dn->m_modelSource->m_indices);
			if (dn->m_modelSource->m_vertices) yyMemFree(dn->m_modelSource->m_vertices);
			dn->m_modelSource->m_vertices = (u8*)yyMemAlloc(sizeof(yyVertexGUI) * 4 * len);
			dn->m_modelSource->m_indices = (u8*)yyMemAlloc(len * 2 * 3 * sizeof(u16));
		}
		//auto & model = m_text_models[glyph->textureID];
		//model.m_isUsing = true;
		//model.Reserve(len);

		auto TXN = text_pointer.x;
		auto TXP = TXN + glyph->width;
		auto TYN = text_pointer.y;
		auto TYP = TYN + glyph->height;


		if (glyph->height > glyph_max_height)
			glyph_max_height = glyph->height;
		
		_yyGui_text_model_AddChar(v4f(TXN, TYN, TXP, TYP), glyph, dn->m_modelSource);
		//model.AddChar(v4f(TXN, TYN, TXP, TYP), glyph);

		text_pointer.x += glyph->width;
		textLen.x += glyph->width;
	}
	textLen.y = glyph_max_height;

	m_buildRectInPixels.z = m_buildRectInPixels.x + textLen.x;
	m_buildRectInPixels.w = m_buildRectInPixels.y + textLen.y;
	m_sensorRectInPixels = m_buildRectInPixels;
	m_buildRectInPixelsCreation.z = textLen.x;
	m_buildRectInPixelsCreation.w = textLen.y;

	//this->SetBuildRect(m_buildRectInPixels);

	for (u16 i = 0; i < m_textureCount; ++i)
	{
		auto dn = &m_drawNodes[i];
		if (!dn->m_isDraw)
			continue;

		if (dn->m_modelGPU->IsLoaded())
			dn->m_modelGPU->Unload();

		dn->m_modelGPU->Load();
	}
}

void yyGUIText::Clear(){
	for (u16 i = 0; i < m_textureCount; ++i)
	{
		if(m_drawNodes[i].m_modelGPU->IsLoaded())
			m_drawNodes[i].m_modelGPU->Unload();
		m_drawNodes[i].m_isDraw = false;
		m_drawNodes[i].m_modelSource->m_stride = sizeof(yyVertexGUI);
		m_drawNodes[i].m_modelSource->m_vertexType = yyVertexType::GUI;
		m_drawNodes[i].m_modelSource->m_iCount = 0;
		m_drawNodes[i].m_modelSource->m_vCount = 0;
	}
}

void yyGUIText::Rebuild() {
	yyGUIElement::CallOnRebuildSetRects();
	SetText(L"%s", m_text.data());
}

void yyGUIText::SetFont(yyGUIFont* newFont) {
	assert(newFont);
	m_font = newFont;
	m_textureCount = m_font->GetTextureCount();
	assert(m_textureCount > 0);

	if (m_drawNodes) delete[] m_drawNodes;

	m_drawNodes = new yyGUITextDrawNode[m_textureCount];
	for (u16 i = 0; i < m_textureCount; ++i)
	{
		m_drawNodes[i].m_modelGPU = yyCreateModel(m_drawNodes[i].m_modelSource);
		m_drawNodes[i].m_textureGPU = m_font->m_textures[i];
	}
}

YY_API yyGUIText* YY_C_DECL yyGUICreateText(const v2f& position, yyGUIFont* font, const wchar_t* text, yyGUIDrawGroup* drawGroup){
	assert(font);
	yyGUIText* element = yyCreate<yyGUIText>();
	element->SetDrawGroup(drawGroup);
	element->SetFont(font);
	element->m_position = position;
	element->SetBuildRect(v4f(position.x, position.y, position.x, position.y));
	element->m_buildRectInPixelsCreation.x = position.x;
	element->m_buildRectInPixelsCreation.y = position.y;

	yyStringW wstr = text;

	if (wstr.size())
		element->SetText(text);

//	g_engine->addGuiElement(element);

	return element;
}
