#include "yy.h"
#include "yy_gui.h"
#include "math/vec.h"
#include "io/file_io.h"

#include "yy_model.h"
#include "yy_input.h"

#include "../engine.h"

extern yyEngine * g_engine;

yyGUIFont::yyGUIFont(){
	m_maxHeight = 0.f;
	m_glyphs.reserve(0xffff);
	for (u32 i = 0; i < 0xffff; ++i)
	{
		m_glyphs.push_back(nullptr);
	}
}

yyGUIFont::~yyGUIFont(){
	for (u32 i = 0, sz = m_glyphs.size(); i < sz; ++i)
	{
		if (m_glyphs[i])
		{
			yyDestroy(m_glyphs[i]);
		}
	}
}

yyGUIFontGlyph* yyGUIFont::GetGlyph(wchar_t ch){
	auto glyph = m_glyphs[(u32)ch];
	if (!glyph)
	{
		glyph = yyCreate<yyGUIFontGlyph>();
		m_glyphs[(u32)ch] = glyph;
	}

	return glyph;
}


void yyGUIFont::Rebuild(){}

YY_API yyGUIFont* YY_C_DECL yyGUILoadFont(const char* fileName){
	yy_fs::path p = fileName;
	if (!yy_fs::exists(p))
	{
		yyLogWriteWarning("Font file [%s] not exist\n", fileName);
		YY_PRINT_FAILED;
		return nullptr;
	}

	yy_fs::path parentPath = p.parent_path();

	yyFileIO file;
	file.open(fileName, "rb");

	if (!file.isOpen())
	{
		yyLogWriteWarning("Can't open file [%s]\n", fileName);
		YY_PRINT_FAILED;
		return nullptr;
	}

	if (!file.isUTF16LE())
	{
		yyLogWriteWarning("Font file [%s] is not UTF16-LE\n", fileName);
		YY_PRINT_FAILED;
		return nullptr;
	}

	std::wstring str;
	file.readWordFromUTF16LE(str);

	int num_of_textures = util::to_int(str.data(), (int)str.size());
	if (num_of_textures < 1)
	{
		yyLogWriteWarning("Bad font file [%s]\n", fileName);
		YY_PRINT_FAILED;
		return nullptr;
	}

	std::vector<std::wstring> texture_name_array;
	texture_name_array.reserve(num_of_textures);
	for (int i = 0; i < num_of_textures; ++i)
	{
		file.get_lineUTF16LE(str);

		std::wstring wstr;
		wstr = parentPath.string_type.data();
		wstr += L"\\";
		wstr += str;

		texture_name_array.emplace_back(wstr);
	}

	// skip comment
	// надо переделать. комментарий по сути обязателен.
	file.get_lineUTF16LE(str);

	struct CharInfo
	{
		CharInfo()
		{
			underhang = 0;
			overhang = 0;
			textureSlot = 0;
		}

		wchar_t symbol;
		v4f rect;
		float underhang;
		float overhang;
		int   textureSlot;
	};

	std::vector<CharInfo> char_info_array;
	while (!file.isEOF())
	{
		CharInfo ci;

		bool is_new_line = false;
		int integer = 0;

		// symbol
		file.readWordFromUTF16LE(str, false, false, false, false, false, false, &is_new_line);
		if (str.size()) ci.symbol = str[0];
		if (is_new_line) continue;

		// leftTopX
		file.readWordFromUTF16LE(str, false, false, false, false, false, false, &is_new_line);
		if (str.size()) ci.rect.x = (float)util::to_int(str.data(), (int)str.size());
		if (is_new_line) continue;


		// leftTopY
		file.readWordFromUTF16LE(str, false, false, false, false, false, false, &is_new_line);
		if (str.size()) ci.rect.y = (float)util::to_int(str.data(), (int)str.size());
		if (is_new_line) continue;

		// rightBottomX
		file.readWordFromUTF16LE(str, false, false, false, false, false, false, &is_new_line);
		if (str.size()) ci.rect.z = (float)util::to_int(str.data(), (int)str.size());
		if (is_new_line) continue;

		// rightBottomY
		file.readWordFromUTF16LE(str, false, false, false, false, false, false, &is_new_line);
		if (str.size()) ci.rect.w = (float)util::to_int(str.data(), (int)str.size());
		if (is_new_line) continue;

		// underhang
		file.readWordFromUTF16LE(str, false, false, false, false, false, false, &is_new_line);
		if (str.size()) ci.underhang = (float)util::to_int(str.data(), (int)str.size());
		if (is_new_line) continue;

		// overhang
		file.readWordFromUTF16LE(str, false, false, false, false, false, false, &is_new_line);
		if (str.size()) ci.overhang = (float)util::to_int(str.data(), (int)str.size());
		if (is_new_line) continue;

		// textureId
		file.readWordFromUTF16LE(str, false, false, false, false, false, false, &is_new_line);
		if (str.size()) ci.textureSlot = util::to_int(str.data(), (int)str.size());

		char_info_array.emplace_back(ci);
	}


	if (char_info_array.size())
	{
		yyGUIFont* newFont = yyCreate<yyGUIFont>();

		auto oldMipMaps = yyIsUseMipMaps();
		auto oldTextureFilter = yyGetTextureFilter();
		
		yyUseMipMaps(false);
		yySetTextureFilter(yyTextureFilter::PPP);

		for (auto & tf : texture_name_array)
		{
			yyStringA stra;
			stra = tf.data();

			auto t = yyGetTextureFromCache(stra.data());
			t->Load();
			newFont->m_textures.push_back(t);
		}

		yyUseMipMaps(oldMipMaps);
		yySetTextureFilter(oldTextureFilter);

		v2f textureSize;
		newFont->m_textures[0]->GetTextureSize(&textureSize);

		f32 uvPerPixel_x = 1.0f / textureSize.x;
		f32 uvPerPixel_y = 1.0f / textureSize.y;
		
		for (auto & ci : char_info_array)
		{
			auto glyph = newFont->GetGlyph(ci.symbol);
			glyph->symbol = ci.symbol;
			glyph->lb.set(ci.rect.x * uvPerPixel_x, (ci.rect.w ) * uvPerPixel_y);
			glyph->lt.set(ci.rect.x * uvPerPixel_x, (ci.rect.y) * uvPerPixel_y);
			glyph->rb.set((ci.rect.z) * uvPerPixel_x, (ci.rect.w) * uvPerPixel_y);
			glyph->rt.set((ci.rect.z) * uvPerPixel_x, (ci.rect.y) * uvPerPixel_y);

			glyph->width = ci.rect.z - ci.rect.x ;
			glyph->height = ci.rect.w - ci.rect.y;

			if (glyph->height > newFont->m_maxHeight)
				newFont->m_maxHeight = glyph->height;

			glyph->overhang = ci.overhang;
			glyph->underhang = ci.underhang;
			glyph->textureID = ci.textureSlot;
			
		//	printf("[%c] [%f]\n", ci.symbol, glyph->height);
		}

		//g_engine->addGuiElement(newFont);
		g_engine->m_mainGUIDrawGroup->AddElement(newFont);

		return newFont;
	}
	return nullptr;
}

f32 yyGUIFont::GetTextLength(const wchar_t* str) {
	assert(str);
	f32 len = 0.f;
	auto str_len = util::str_len(str);
	for (size_t i = 0; i < str_len; ++i)
	{
		len += GetGlyph(str[i])->width;
	}
	return len;
}