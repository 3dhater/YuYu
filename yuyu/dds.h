#ifndef __DDS_H__
#define __DDS_H__

constexpr u32 DDS_MAGIC = 0x20534444;

#define DDS_FOURCC      0x00000004  // DDPF_FOURCC
#define DDS_RGB         0x00000040  // DDPF_RGB
#define DDS_RGBA        0x00000041  // DDPF_RGB | DDPF_ALPHAPIXELS
#define DDS_LUMINANCE   0x00020000  // DDPF_LUMINANCE
#define DDS_LUMINANCEA  0x00020001  // DDPF_LUMINANCE | DDPF_ALPHAPIXELS
#define DDS_ALPHAPIXELS 0x00000001  // DDPF_ALPHAPIXELS
#define DDS_ALPHA       0x00000002  // DDPF_ALPHA
#define DDS_PAL8        0x00000020  // DDPF_PALETTEINDEXED8
#define DDS_PAL8A       0x00000021  // DDPF_PALETTEINDEXED8 | DDPF_ALPHAPIXELS
#define DDS_BUMPDUDV    0x00080000  // DDPF_BUMPDUDV

#define DDS_HEADER_FLAGS_TEXTURE        0x00001007  // DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT 
#define DDS_HEADER_FLAGS_MIPMAP         0x00020000  // DDSD_MIPMAPCOUNT
#define DDS_HEADER_FLAGS_VOLUME         0x00800000  // DDSD_DEPTH
#define DDS_HEADER_FLAGS_PITCH          0x00000008  // DDSD_PITCH
#define DDS_HEADER_FLAGS_LINEARSIZE     0x00080000  // DDSD_LINEARSIZE
#define DDS_CUBEMAP 0x00000200 // DDSCAPS2_CUBEMAP
#define DDS_FLAGS_VOLUME 0x00200000 // DDSCAPS2_VOLUME

struct DDS_PIXELFORMAT
{
	u32    size = sizeof(DDS_PIXELFORMAT);
	u32 flags = 0x00000041; //DDS_RGBA
	u32 fourCC = 0;
	u32 RGBBitCount = 32;
	u32 RBitMask = 0x000000ff;
	u32 GBitMask = 0x0000ff00;
	u32 BBitMask = 0x00ff0000;
	u32 ABitMask = 0xff000000;
};

struct DDS_HEADER
{
	u32 size = sizeof(DDS_HEADER);     // offset 0
	// DDS_HEIGHT | DDS_WIDTH
	u32 flags = 0x00000002 | 0x00000004;    // offset 4
	u32 height = 0;   // offset 8
	u32 width = 0;    // offset 12
	u32 pitchOrLinearSize = 0; // offset 16
	u32 depth = 0;    // offset 20
	u32 mipMapCount = 1;  // offset 24
	u32 reserved1[11]; 
	DDS_PIXELFORMAT ddspf;
	u32 caps = 0;
	u32 caps2 = 0;
	u32 caps3 = 0;
	u32 caps4 = 0;
	u32 reserved2 = 0;
};


#define ISBITMASK( r,g,b,a ) ( ddpf.RBitMask == r && ddpf.GBitMask == g && ddpf.BBitMask == b && ddpf.ABitMask == a )
inline yyImageFormat GetDDSImageFormat(const DDS_PIXELFORMAT& ddpf) noexcept
{
	if(ddpf.flags & DDS_RGB)
	{
		switch (ddpf.RGBBitCount)
		{
			case 32:
				if (ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000))
					return yyImageFormat::R8G8B8A8;

				if (ISBITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000))
					return yyImageFormat::Unknown;

				if (ISBITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0))
					return yyImageFormat::Unknown;

				if (ISBITMASK(0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000))
					return yyImageFormat::Unknown;

				if (ISBITMASK(0x0000ffff, 0xffff0000, 0, 0))
					return yyImageFormat::Unknown;

				if (ISBITMASK(0xffffffff, 0, 0, 0))
					return yyImageFormat::Unknown;
				break;

			case 24:
				break;

			case 16:
				if (ISBITMASK(0x7c00, 0x03e0, 0x001f, 0x8000))
					return yyImageFormat::Unknown;
				if (ISBITMASK(0xf800, 0x07e0, 0x001f, 0))
					return yyImageFormat::Unknown;
				if (ISBITMASK(0x0f00, 0x00f0, 0x000f, 0xf000))
					return yyImageFormat::Unknown;
				break;
		}
	}
	else if (ddpf.flags & DDS_LUMINANCE)
	{
		if (8 == ddpf.RGBBitCount)
		{
			if (ISBITMASK(0xff, 0, 0, 0))
				return yyImageFormat::Unknown;
			if (ISBITMASK(0x00ff, 0, 0, 0xff00))
				return yyImageFormat::Unknown;
		}

		if (16 == ddpf.RGBBitCount)
		{
			if (ISBITMASK(0xffff, 0, 0, 0))
				return yyImageFormat::Unknown; 
			if (ISBITMASK(0x00ff, 0, 0, 0xff00))
				return yyImageFormat::Unknown;
		}
	}
	else if (ddpf.flags & DDS_ALPHA)
	{
		if (8 == ddpf.RGBBitCount)
			return yyImageFormat::Unknown;
	}
	else if (ddpf.flags & DDS_BUMPDUDV)
	{
		if (16 == ddpf.RGBBitCount)
		{
			if (ISBITMASK(0x00ff, 0xff00, 0, 0))
				return yyImageFormat::Unknown; // D3DX10/11 writes this out as DX10 extension
		}

		if (32 == ddpf.RGBBitCount)
		{
			if (ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000))
				return yyImageFormat::R8G8B8A8;
			if (ISBITMASK(0x0000ffff, 0xffff0000, 0, 0))
				return yyImageFormat::Unknown;
		}
	}
	else if (ddpf.flags & DDS_FOURCC)
	{
		if (YY_MAKEFOURCC('D', 'X', 'T', '1') == ddpf.fourCC)
			return yyImageFormat::BC1;
		if (YY_MAKEFOURCC('D', 'X', 'T', '3') == ddpf.fourCC)
			return yyImageFormat::BC2;
		if (YY_MAKEFOURCC('D', 'X', 'T', '5') == ddpf.fourCC)
			return yyImageFormat::BC3;

		if (YY_MAKEFOURCC('D', 'X', 'T', '2') == ddpf.fourCC)
			return yyImageFormat::BC2;
		if (YY_MAKEFOURCC('D', 'X', 'T', '4') == ddpf.fourCC)
			return yyImageFormat::BC3;

	}

	return yyImageFormat::Unknown;
}


#endif