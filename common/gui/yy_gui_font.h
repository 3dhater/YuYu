#ifndef _YUYU_GUI_FONT_H_
#define _YUYU_GUI_FONT_H_

#include "containers\array.h"

struct yyGUIFontGlyph
{
	yyGUIFontGlyph()
	{
		textureID = 0;
		width = 0;
		height = 0;
		underhang = 0;
		overhang = 0;
		symbol = 0;
	}

	int textureID;
	float width;
	float height;
	float underhang;
	float overhang;
	char16_t symbol;

	// texture coords, left-top left-bottom etc.
	v2f lt, lb, rt, rb;
};

class yyGUIFont : public yyGUIElement
{
public:
	yyGUIFont();
	virtual ~yyGUIFont();
	virtual void OnUpdate(f32 dt) {}
	virtual void OnDraw(f32 dt) {}
	virtual void Rebuild();
	
	u16 GetTextureCount() {
		return m_textures.size();
	}

	yyArray<yyGUIFontGlyph*> m_glyphs;

	virtual yyGUIFontGlyph* GetGlyph(wchar_t ch);
	
	// from engine texture cache. do not delete
	yyArraySmall<yyResource*> m_textures;

	virtual f32 GetTextLength(const wchar_t* str);

	f32 m_maxHeight;
};

#endif