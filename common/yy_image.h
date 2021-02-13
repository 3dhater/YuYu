#ifndef _YY_IMAGE_H__
#define _YY_IMAGE_H__

#include "yy_color.h"

enum class yyImageFormat : u32
{
	One_bit,		// white or black

	R1,

	A1R5G5B5,		// 1 11111 11111 11111
	X1R5G5B5,		// x 11111 11111 11111

	A4R4G4B4,		// 1111 1111 1111 1111
	X4R4G4B4,		// xxxx 1111 1111 1111
	B4G4R4A4,

	R5G6B5,			// 11111 111111 11111
	B5G6R5,
	B5G5R5A1,

	A8,

	R8,
	R8G8,
	R8G8B8,			// u8 u8 u8
	R8G8B8A8,		// u8 u8 u8 u8
	R8G8B8G8,
	G8R8G8B8,
	B8G8R8A8,
	B8G8R8X8,
	X8R8G8B8,		// u8 u8 u8 u8
	A8R8G8B8,		// u8 u8 u8 u8

	R9G9B9E5,

	R10G10B10A2,

	R11G11B10,

	R16,
	R16G16,
	R16G16B16A16,

	R24G8,
	R24X8,
	X24G8,

	R32,
	R32G32,
	R32G8X24,
	R32G32B32,
	R32G32B32A32,

	BC1 = 0x83F1, // dds dxt1
	BC2 = 0x83F2, // dds dxt3
	BC3 = 0x83F3,	// dds dxt5
	Unknown
};

struct yyImage
{
	yyImage()
	:
		m_data(nullptr),
		m_bitDataOffset(0),
		m_dataSize(0),
		m_fileSize(0),
		m_width(0),
		m_height(0),
		m_bits(32),
		m_mipCount(1),
		m_pitch(0),
		m_format(yyImageFormat::R8G8B8A8)
	{}
	~yyImage()
	{
		if( m_data )
			yyMemFree(m_data);
	}
	
	struct rgb
	{
		u8 r, g, b;
	};
	struct rgba
	{
		u8 r, g, b, a;
	};

	void _convertToR8G8B8A8_from_X8R8G8B8()
	{
		for (u32 i = 0u; i < m_dataSize; ) 
		{
			s32 a = abs((s32)m_data[i] - 255);
			s32 r = abs((s32)m_data[i + 1u] - 255);
			s32 g = abs((s32)m_data[i + 2u] - 255);
			s32 b = abs((s32)m_data[i + 3u] - 255);
			a = 255;
			m_data[i] = (u8)r;
			m_data[i + 1u] = (u8)g;
			m_data[i + 2u] = (u8)b;
			m_data[i + 3u] = (u8)a;
			i += 4;
		}
		m_format = yyImageFormat::R8G8B8A8;
	}
	void _convertToR8G8B8A8_from_R8G8B8()
	{
		u32 new_pitch = m_width * 4;
		u32 new_dataSize = m_height * new_pitch;
		u8* new_data = (u8*)yyMemAlloc(new_dataSize);
		
		rgb* rgb_data = (rgb*)m_data;
		rgba* rgba_data = (rgba*)new_data;
		s32 num_of_pixels = m_width * m_height;
		for (s32 i = 0; i < num_of_pixels; ++i)
		{
			rgba_data->r = rgb_data->r;
			rgba_data->g = rgb_data->g;
			rgba_data->b = rgb_data->b;
			rgba_data->a = 255;
			
			rgba_data++;
			rgb_data++;
		}

		yyDestroy(m_data);

		m_data = new_data;
		m_pitch = new_pitch;
		m_dataSize = new_dataSize;
		m_format = yyImageFormat::R8G8B8A8;
	}
	void convertToR8G8B8A8()
	{
		assert(m_data);
		switch (m_format)
		{
		case yyImageFormat::R8G8B8:
			_convertToR8G8B8A8_from_R8G8B8();
			break;
		case yyImageFormat::X8R8G8B8:
			_convertToR8G8B8A8_from_X8R8G8B8();
			break;
		case yyImageFormat::R8G8B8A8:
			break;
		case yyImageFormat::A8R8G8B8:
			break;
		case yyImageFormat::BC1:
			break;
		case yyImageFormat::BC2:
			break;
		case yyImageFormat::BC3:
			break;
		case yyImageFormat::Unknown:
			break;
		default:
			YY_PRINT_FAILED;
			break;
		}
	}

	void flipVertical()
	{
		assert(m_data);
		u8 * line = nullptr;
		line = (u8*)yyMemAlloc(m_pitch);
		u8 * p_Up = &m_data[0u];
		u8 * p_Down = &m_data[m_pitch * m_height - m_pitch];
		for (u32 i = 0u; i < m_height / 2u; ++i)
		{
			memcpy(line, p_Up, m_pitch);
			memcpy(p_Up, p_Down, m_pitch);
			memcpy(p_Down, line, m_pitch);
			p_Up += m_pitch;
			p_Down -= m_pitch;
		}
		yyMemFree(line);
	}
	void flipPixel() 
	{
		for (u32 i = 0u; i < m_dataSize; ) 
		{
			u8 r = m_data[i];
			u8 g = m_data[i + 1u];
			u8 b = m_data[i + 2u];
			u8 a = m_data[i + 3u];
			m_data[i] = b;
			m_data[i + 1u] = g;
			m_data[i + 2u] = r;
			m_data[i + 3u] = a;
			i += 4;
		}
	}

	void fill( const yyColor& color )
	{
		u8* data = m_data;
		for( u32 h = 0; h < m_height; ++h )
		{
			for( u32 w = 0; w < m_width; ++w )
			{
				*data = color.getAsByteRed();   ++data;
				*data = color.getAsByteGreen(); ++data;
				*data = color.getAsByteBlue();  ++data;
				*data = color.getAsByteAlpha(); ++data;
			}
		}
	}

	// теперь m_data может хранить файл полностью. В случае DDS для DirectXTK.
	// для доступа к массиву с пикселями нужен m_bitDataOffset
	// если это обычные файлы (типа .PNG) то m_data указывает на обычный буфер а m_bitDataOffset = 0
	u8 * m_data;
	u32 m_bitDataOffset;
	u32 m_dataSize;
	u32 m_fileSize;
	u32	m_width;
	u32	m_height;
	u32	m_bits;
	u32	m_mipCount;
	u32	m_pitch;
	yyImageFormat m_format;

};

#endif