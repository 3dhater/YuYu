#ifndef _YY_IMAGE_H__
#define _YY_IMAGE_H__

#include "yy_color.h"

enum class yyImageFormat : u32
{
	R8G8B8,			// u8 u8 u8
	R8G8B8A8,		// u8 u8 u8 u8
	A8R8G8B8,		// u8 u8 u8 u8
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