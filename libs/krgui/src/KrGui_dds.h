#ifndef __KK_KRGUI_DDS_H__
#define __KK_KRGUI_DDS_H__

namespace Kr
{
	namespace Gui
	{
		namespace _internal
		{
			struct dds_pixelformat
			{
				dds_pixelformat()
				{
					size = sizeof(dds_pixelformat);
					flags = 0x00000041; //DDS_RGBA
					fourCC = 0;
					RGBBitCount = 32;
					RBitMask = 0x000000ff;
					GBitMask = 0x0000ff00;
					BBitMask = 0x00ff0000;
					ABitMask = 0xff000000;
				}

				unsigned int size;
				unsigned int flags;
				unsigned int fourCC;
				unsigned int RGBBitCount;
				unsigned int RBitMask;
				unsigned int GBitMask;
				unsigned int BBitMask;
				unsigned int ABitMask;
			};

			struct dds_header
			{
				dds_header()
				{
					size = sizeof(dds_header);     // offset 0
													// DDS_HEIGHT | DDS_WIDTH
					flags = 0x00000002 | 0x00000004;    // offset 4
					height = 0;   // offset 8
					width = 0;    // offset 12
					pitchOrLinearSize = 0; // offset 16
					depth = 0;    // offset 20
					mipMapCount = 1;  // offset 24
					caps = 0;
					caps2 = 0;
					caps3 = 0;
					caps4 = 0;
					reserved2 = 0;
				}

				unsigned int size;
				unsigned int flags;
				unsigned int height;
				unsigned int width;
				unsigned int pitchOrLinearSize;
				unsigned int depth;
				unsigned int mipMapCount;
				unsigned int reserved1[11]; 
				dds_pixelformat ddspf;
				unsigned int caps;
				unsigned int caps2;
				unsigned int caps3;
				unsigned int caps4;
				unsigned int reserved2;
			};
		}
	}
}

#endif