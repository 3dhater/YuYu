#ifndef __KK_KRGUI_FONTGLYPH_H__
#define __KK_KRGUI_FONTGLYPH_H__

namespace Kr
{
	namespace Gui
	{
		struct FontGlyph
		{
			FontGlyph()
			{
				textureID = 0;
				width = 0;
				height = 0;
				underhang = 0;
				overhang = 0;
				symbol = 0;
			}
			int textureID ;
			float width  ;
			float height ;

			//float bias = 0; // when next char move on previous char (like little `j` in some fonts)
			float underhang ;
			float overhang ;

			char16_t symbol ;
		
			// texture coords, left-top left-bottom etc.
			Vec2f lt, lb, rt, rb;
		};
	}
}

#endif