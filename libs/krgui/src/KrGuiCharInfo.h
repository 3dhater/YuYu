#ifndef __KK_KRGUI_CHARINFO_H__
#define __KK_KRGUI_CHARINFO_H__

namespace Kr
{
	namespace Gui
	{
		struct CharInfo
		{
			CharInfo()
			{
				underhang = 0;
				overhang = 0;
				textureSlot = 0;
			}

			wchar_t symbol;
			Vec4f rect;
			float underhang;
			float overhang;
			int   textureSlot;
		};
	}
}

#endif