#ifndef __KK_KRGUI_TEXTURE_H__
#define __KK_KRGUI_TEXTURE_H__

namespace Kr
{
	namespace Gui
	{
		struct Texture
		{
			Texture()
			{
				texture_value = 0;
				texture_address = 0; // address for texture object / or texture ID for OpenGL
				width = 0;
				height = 0;
				can_destroy = true;
			}
			unsigned long long texture_value   ;
			unsigned long long texture_address ; // address for texture object / or texture ID for OpenGL

			int width  ;
			int height ;

			bool can_destroy ;
		};
	}
}

#endif