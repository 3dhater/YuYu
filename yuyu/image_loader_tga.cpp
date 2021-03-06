﻿//========================================================================
// GLFW - An OpenGL framework
// Platform:    Any
// API version: 2.7
// WWW:         http://www.glfw.org/
//------------------------------------------------------------------------
// Copyright (c) 2002-2006 Marcus Geelnard
// Copyright (c) 2006-2010 Camilla Berglund <elmindreda@elmindreda.org>
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would
//    be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not
//    be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source
//    distribution.
//
//========================================================================

//========================================================================
// Description:
//
// TGA format image file loader. This module supports version 1 Targa
// images, with these restrictions:
//  - Pixel format may only be 8, 24 or 32 bits
//  - Colormaps must be no longer than 256 entries
//
//=====================================================================

#include "yy.h"

#include <iostream>
#include <fstream>
#include <filesystem>

#include "yy_image.h"
#include "yy_fs.h"

typedef struct {
	int idlen;                 // 1 byte
	int cmaptype;              // 1 byte
	int imagetype;             // 1 byte
	int cmapfirstidx;          // 2 bytes
	int cmaplen;               // 2 bytes
	int cmapentrysize;         // 1 byte
	int xorigin;               // 2 bytes
	int yorigin;               // 2 bytes
	int width;                 // 2 bytes
	int height;                // 2 bytes
	int bitsperpixel;          // 1 byte
	int imageinfo;             // 1 byte
	int _alphabits;            // (derived from imageinfo)
	int _origin;               // (derived from imageinfo)
} _tga_header_t;

#define _TGA_CMAPTYPE_NONE      0
#define _TGA_CMAPTYPE_PRESENT   1

#define _TGA_IMAGETYPE_NONE     0
#define _TGA_IMAGETYPE_CMAP     1
#define _TGA_IMAGETYPE_TC       2
#define _TGA_IMAGETYPE_GRAY     3
#define _TGA_IMAGETYPE_CMAP_RLE 9
#define _TGA_IMAGETYPE_TC_RLE   10
#define _TGA_IMAGETYPE_GRAY_RLE 11

#define _TGA_IMAGEINFO_ALPHA_MASK   0x0f
#define _TGA_IMAGEINFO_ALPHA_SHIFT  0
#define _TGA_IMAGEINFO_ORIGIN_MASK  0x30
#define _TGA_IMAGEINFO_ORIGIN_SHIFT 4

#define _TGA_ORIGIN_BL 0
#define _TGA_ORIGIN_BR 1
#define _TGA_ORIGIN_UL 2
#define _TGA_ORIGIN_UR 3

static int ReadTGAHeader(FILE *f, _tga_header_t *h){
	unsigned char buf[18];
	int pos;

	// Read TGA file header from file
	pos = ftell(f);
	fread(buf, 1, 18, f);

	// Interpret header (endian independent parsing)
	h->idlen = (int)buf[0];
	h->cmaptype = (int)buf[1];
	h->imagetype = (int)buf[2];
	h->cmapfirstidx = (int)buf[3] | (((int)buf[4]) << 8);
	h->cmaplen = (int)buf[5] | (((int)buf[6]) << 8);
	h->cmapentrysize = (int)buf[7];
	h->xorigin = (int)buf[8] | (((int)buf[9]) << 8);
	h->yorigin = (int)buf[10] | (((int)buf[11]) << 8);
	h->width = (int)buf[12] | (((int)buf[13]) << 8);
	h->height = (int)buf[14] | (((int)buf[15]) << 8);
	h->bitsperpixel = (int)buf[16];
	h->imageinfo = (int)buf[17];

	// Extract alphabits and origin information
	h->_alphabits = (int)(h->imageinfo & _TGA_IMAGEINFO_ALPHA_MASK) >>
		_TGA_IMAGEINFO_ALPHA_SHIFT;
	h->_origin = (int)(h->imageinfo & _TGA_IMAGEINFO_ORIGIN_MASK) >>
		_TGA_IMAGEINFO_ORIGIN_SHIFT;

	// Validate TGA header (is this a TGA file?)
	if ((h->cmaptype == 0 || h->cmaptype == 1) &&
		((h->imagetype >= 1 && h->imagetype <= 3) ||
		(h->imagetype >= 9 && h->imagetype <= 11)) &&
			(h->bitsperpixel == 8 || h->bitsperpixel == 24 ||
				h->bitsperpixel == 32))
	{
		// Skip the ID field
		fseek(f, h->idlen, SEEK_CUR);

		// Indicate that the TGA header was valid
		return 1;
	}
	else
	{
		// Restore file position
		fseek(f, pos, SEEK_SET);

		// Indicate that the TGA header was invalid
		return 0;
	}
}

static void ReadTGA_RLE(unsigned char *buf, int size, int bpp,FILE *f){
	int repcount, bytes, k, n;
	unsigned char pixel[4];
	char c;

	// Dummy check
	if (bpp > 4)
	{
		return;
	}

	while (size > 0)
	{
		// Get repetition count
		fread(&c, 1, 1, f);
		repcount = (unsigned int)c;
		bytes = ((repcount & 127) + 1) * bpp;
		if (size < bytes)
		{
			bytes = size;
		}

		// Run-Length packet?
		if (repcount & 128)
		{
			fread(pixel, bpp, 1, f);
			for (n = 0; n < (repcount & 127) + 1; n++)
			{
				for (k = 0; k < bpp; k++)
				{
					*buf++ = pixel[k];
				}
			}
		}
		else
		{
			// It's a Raw packet
			fread(buf, bytes, 1, f);
			buf += bytes;
		}

		size -= bytes;
	}
}

void ImageLoaderGetInfo_TGA(const char* p, yyImage* img) {
	_tga_header_t h;
	//unsigned char *cmap, *pix, tmp, *src, *dst;
	//int cmapsize, pixsize, pixsize2;
	//int bpp, bpp2, k, m, n, swapx, swapy;

	FILE* f = fopen(p, "rb");
	if (!f)
	{
		YY_PRINT_FAILED;
		return;
	}

	// Read TGA header
	if (!ReadTGAHeader(f, &h))
	{
		fclose(f);
		YY_PRINT_FAILED;
		return;
	}

	img->m_width = h.width;
	img->m_height = h.height;

	fclose(f);
}

yyImage* ImageLoader_TGA(const char* p){
	_tga_header_t h;
	unsigned char *cmap, *pix, tmp, *src, *dst;
	int cmapsize, pixsize, pixsize2;
	int bpp, bpp2, k, m, n, swapx, swapy;

	FILE* f = fopen(p, "rb");
	if (!f)
	{
		YY_PRINT_FAILED;
		return 0;
	}

	// Read TGA header
	if (!ReadTGAHeader(f, &h))
	{
		fclose(f);
		YY_PRINT_FAILED;
		return 0;
	}

	// Is there a colormap?
	cmapsize = (h.cmaptype == _TGA_CMAPTYPE_PRESENT ? 1 : 0) * h.cmaplen *
		((h.cmapentrysize + 7) / 8);
	if (cmapsize > 0)
	{
		// Is it a colormap that we can handle?
		if ((h.cmapentrysize != 24 && h.cmapentrysize != 32) ||
			h.cmaplen == 0 || h.cmaplen > 256)
		{
			fclose(f);
			return 0;
		}

		// Allocate memory for colormap
		cmap = (unsigned char *)yyMemAlloc(cmapsize);
		if (cmap == NULL)
		{
			fclose(f);
			return 0;
		}

		// Read colormap from file
		fread(cmap, cmapsize, 1, f);
	}
	else
	{
		cmap = NULL;
	}

	// Size of pixel data
	pixsize = h.width * h.height * ((h.bitsperpixel + 7) / 8);

	// Bytes per pixel (pixel data - unexpanded)
	bpp = (h.bitsperpixel + 7) / 8;

	// Bytes per pixel (expanded pixels - not colormap indeces)
	if (cmap)
	{
		bpp2 = (h.cmapentrysize + 7) / 8;
	}
	else
	{
		bpp2 = bpp;
	}

	// For colormaped images, the RGB/RGBA image data may use more memory
	// than the stored pixel data
	pixsize2 = h.width * h.height * bpp2;

	// Allocate memory for pixel data
	pix = (unsigned char *)yyMemAlloc(pixsize2);
	if (pix == NULL)
	{
		if (cmap)
		{
			free(cmap);
		}
		return 0;
	}

	// Read pixel data from file
	if (h.imagetype >= _TGA_IMAGETYPE_CMAP_RLE)
	{
		ReadTGA_RLE(pix, pixsize, bpp, f);
	}
	else
	{
		fread(pix, pixsize, 1, f);
	}
	
	fclose(f);

	// If the image origin is not what we want, re-arrange the pixels
	switch (h._origin)
	{
	default:
	case _TGA_ORIGIN_UL:
		swapx = 0;
		swapy = 1;
		break;

	case _TGA_ORIGIN_BL:
		swapx = 0;
		swapy = 0;
		break;

	case _TGA_ORIGIN_UR:
		swapx = 1;
		swapy = 1;
		break;

	case _TGA_ORIGIN_BR:
		swapx = 1;
		swapy = 0;
		break;
	}

	if (swapy)
	{
		src = pix;
		dst = &pix[(h.height - 1)*h.width*bpp];
		for (n = 0; n < h.height / 2; n++)
		{
			for (m = 0; m < h.width; m++)
			{
				for (k = 0; k < bpp; k++)
				{
					tmp = *src;
					*src++ = *dst;
					*dst++ = tmp;
				}
			}
			dst -= 2 * h.width*bpp;
		}
	}

	if (swapx)
	{
		src = pix;
		dst = &pix[(h.width - 1)*bpp];
		for (n = 0; n < h.height; n++)
		{
			for (m = 0; m < h.width / 2; m++)
			{
				for (k = 0; k < bpp; k++)
				{
					tmp = *src;
					*src++ = *dst;
					*dst++ = tmp;
				}
				dst -= 2 * bpp;
			}
			src += ((h.width + 1) / 2)*bpp;
			dst += ((3 * h.width + 1) / 2)*bpp;
		}
	}

	// Convert BGR/BGRA to RGB/RGBA, and optionally colormap indeces to
	// RGB/RGBA values
	if (cmap)
	{
		// Convert colormap pixel format (BGR -> RGB or BGRA -> RGBA)
		if (bpp2 == 3 || bpp2 == 4)
		{
			for (n = 0; n < h.cmaplen; n++)
			{
				tmp = cmap[n*bpp2];
				cmap[n*bpp2] = cmap[n*bpp2 + 2];
				cmap[n*bpp2 + 2] = tmp;
			}
		}

		// Convert pixel data to RGB/RGBA data
		for (m = h.width * h.height - 1; m >= 0; m--)
		{
			n = pix[m];
			for (k = 0; k < bpp2; k++)
			{
				pix[m*bpp2 + k] = cmap[n*bpp2 + k];
			}
		}

		// Free memory for colormap (it's not needed anymore)
		yyMemFree(cmap);
	}
	else
	{
		// Convert image pixel format (BGR -> RGB or BGRA -> RGBA)
		if (bpp2 == 3 || bpp2 == 4)
		{
			src = pix;
			dst = &pix[2];
			for (n = 0; n < h.height * h.width; n++)
			{
				tmp = *src;
				*src = *dst;
				*dst = tmp;
				src += bpp2;
				dst += bpp2;
			}
		}
	}

	yyImage* image = yyCreate<yyImage>();
	image->m_width = h.width;
	image->m_height = h.height;
	image->m_bits = bpp2 * 8;
	image->m_data = pix;

	if(image->m_bits == 32 )
	{
		image->m_format = yyImageFormat::R8G8B8A8;
		image->m_pitch	= image->m_width * 4;
	}
	else if (image->m_bits == 24)
	{
		image->m_format = yyImageFormat::R8G8B8;
		image->m_pitch	= image->m_width * 3;
	}

	image->m_dataSize = image->m_pitch * image->m_height;
	image->convertToR8G8B8A8();
	image->flipVertical();

	return image;
}
