/*
	This file contains software technology from Irrlicht Engine
	Copyright(C) 2002 - 2012 Nikolaus Gebhardt
*/

#include "yy.h"

#include <iostream>
#include <fstream>
#include <filesystem>

#include "yy_image.h"
#include "yy_fs.h"

#pragma pack(push,2)
struct BitmapHeader {
	u16		bfType;
	u32		bfSize;
	u16		bfReserved1;
	u16		bfReserved2;
	u32		bfOffBits;
};
#pragma pack(pop)
struct rgbquad {
	u8 Blue;
	u8 Green;
	u8 Red;
	u8 Reserved;
};

struct ciexyzTriple {
	v3i		ciexyzRed;
	v3i		ciexyzGreen;
	v3i		ciexyzBlue;
};
struct BitmapInfoHeader_v5 {
	u32			bV5Size;		//	размер header в файле
	s32			bV5Width;		//	ширина
	s32			bV5Height;		//	высота
	u16			bV5Planes;		//	хз что это, всегда 1
	u16			bV5BitCount;	//	биты
	u32			bV5Compression;	//	1 - RLE 8bit, 2 - RLE 4bit, 3 или что-то, видимо, специальные обозначения у разработчиков 2D редакторов.
	u32			bV5SizeImage;	//	размер массива пикселей/индексов
	s32			bV5XPelsPerMeter;// размер в чём-то, видимо для печати или вывода ещё кудато
	s32			bV5YPelsPerMeter;//	для обычного использования в ПК не играет никакой роли
	u32			bV5ClrUsed;		//	обычно тут ноль
	u32			bV5ClrImportant;//	и тут ноль
	u32			bV5RedMask;		//	для определения позиции цветов
	u32			bV5GreenMask;	//	в форматах типа x1r5g5b5
	u32			bV5BlueMask;
	u32			bV5AlphaMask;
	u32			bV5CSType;		//	далее информация для более специализированного
	ciexyzTriple bV5Endpoints;	//	использования.
	u32			bV5GammaRed;	//	для передачи простой картинки достаточно того
	u32			bV5GammaGreen;	//	что указано выше. А эта часть нужна для, например,
	u32			bV5GammaBlue;	//	тех кто делает видео плеер, видео редактор
	u32			bV5Intent;		//	что-то типа этого. Как бы универсальное решение
	u32			bV5ProfileData;	//	от Microsoft
	u32			bV5ProfileSize;
	u32			bV5Reserved;
};

//	irrlicht
void decompress4BitRLE(u8*& rleData, u8*& inds, u32 size, u32 width, u32 height);
void decompress8BitRLE(u8*& rleData, u8*& inds, u32 size, u32 width, u32 height);

void ImageLoaderGetInfo_BMP(const char* p, yyImage* img) {
	FILE * file = fopen(p, "rb");
	if (!file)
	{
		yyLogWriteWarning("BMP: Image could not be opened\n");
		YY_PRINT_FAILED;
		return;
	}
	BitmapHeader header;
	BitmapInfoHeader_v5 info;
	if (fread(&header, 1, sizeof(BitmapHeader), file) != sizeof(BitmapHeader))
	{
		yyLogWriteWarning("BMP: Not a correct BMP file\n");
		YY_PRINT_FAILED;
		fclose(file);
		return;
	}
	fread(&info, 1, sizeof(BitmapInfoHeader_v5), file);
	if (info.bV5Size < 40U)
	{
		yyLogWriteWarning("BMP: Bad header size\n");
		YY_PRINT_FAILED;
		fclose(file);
		return;
	}
	fclose(file);
	img->m_width = static_cast<u32>(info.bV5Width);
	img->m_height = static_cast<u32>(info.bV5Height);
	img->m_bits = info.bV5BitCount;
}

yyImage* ImageLoader_BMP(const char* p){
	FILE * file = fopen(p, "rb");
	if (!file) 
	{
		yyLogWriteWarning("BMP: Image could not be opened\n"); 
		YY_PRINT_FAILED;
		return 0;
	}

	BitmapHeader header;
	BitmapInfoHeader_v5 info;

	if (fread(&header, 1, sizeof(BitmapHeader), file) != sizeof(BitmapHeader)) 	
	{ 
		yyLogWriteWarning("BMP: Not a correct BMP file\n");
		YY_PRINT_FAILED;
		fclose(file);
		return 0;
	}

	if (header.bfType != 19778 )
	{
		yyLogWriteWarning("BMP: Not a correct BMP file\n");
		YY_PRINT_FAILED;
		fclose(file);
		return 0;
	}

	fread(&info, 1, sizeof(BitmapInfoHeader_v5), file);
	if (info.bV5Size < 40U) 
	{
		yyLogWriteWarning("BMP: Bad header size\n");
		YY_PRINT_FAILED;
		fclose(file);
		return 0;
	}

	if (!info.bV5Width) 
	{
		yyLogWriteWarning("BMP: Bad width\n");
		YY_PRINT_FAILED;
		fclose(file);
		return 0;
	}
	
	if (!info.bV5Height)
	{
		yyLogWriteWarning("BMP: Bad height\n");
		YY_PRINT_FAILED;
		fclose(file);
		return 0;
	}

	if (info.bV5BitCount != 1u &&
		info.bV5BitCount != 4u &&
		info.bV5BitCount != 8u &&
		info.bV5BitCount != 16u &&
		info.bV5BitCount != 24u &&
		info.bV5BitCount != 32u) 
	{
		yyLogWriteWarning("BMP: Bad bit count\n");
		YY_PRINT_FAILED;
		fclose(file);
		return 0;
	}

	yyPtr<yyImage> image = yyCreate<yyImage>();
	
	image.m_data->m_width = static_cast<u32>(info.bV5Width);
	image.m_data->m_height = static_cast<u32>(info.bV5Height);
	image.m_data->m_bits = info.bV5BitCount;

	bool flipPixel = false;

	if (image.m_data->m_bits == 24u)
	{
		image.m_data->m_format = yyImageFormat::R8G8B8;
		image.m_data->m_pitch = image.m_data->m_width * 3u;
		image.m_data->m_dataSize = image.m_data->m_pitch * image.m_data->m_height;
		
		image.m_data->m_data = (u8*)yyMemAlloc(image.m_data->m_dataSize);

		fseek(file, 54, SEEK_SET);
		fread(image.m_data->m_data, 1, image.m_data->m_dataSize, file);
		flipPixel = true;
	}
	else if (image.m_data->m_bits == 32u)
	{
		image.m_data->m_pitch = image.m_data->m_width * 4u;
		u32 offset = header.bfOffBits;

		if (info.bV5Compression == 3) // BI_BITFIELDS
		{
			fseek(file, offset, SEEK_SET);
			image.m_data->m_format = yyImageFormat::R8G8B8A8;

			image.m_data->m_dataSize = image.m_data->m_pitch * image.m_data->m_height;
			image.m_data->m_data = (u8*)yyMemAlloc(image.m_data->m_dataSize);

			fread(image.m_data->m_data, 1, image.m_data->m_dataSize, file);
			image.m_data->flipPixel();
		}
		else
		{
			yyLogWriteWarning("BMP: unsupported format\n");
			YY_PRINT_FAILED;
			fclose(file);
			return 0;
		}
	}
	else if (image.m_data->m_bits == 16u)
	{
		if (info.bV5Size != 40U && info.bV5Size != 56u) 
		{
			yyLogWriteWarning("BMP: unsupported format\n");
			YY_PRINT_FAILED;
			fclose(file);
			return 0;
		}

		image.m_data->m_pitch = image.m_data->m_width * 2u;

		if (info.bV5Size == 40U) 
		{ // x1r5g5b5
			fseek(file, 54u, SEEK_SET);
			image.m_data->m_format = yyImageFormat::X1R5G5B5;
		}
		else 
		{
			if (info.bV5RedMask == 3840U &&
				info.bV5GreenMask == 240U &&
				info.bV5BlueMask == 15u) 
			{
				if (info.bV5AlphaMask) 
				{
					image.m_data->m_format = yyImageFormat::A4R4G4B4;
				}
				else 
				{
					image.m_data->m_format = yyImageFormat::X4R4G4B4;
				}
			}
			else if (info.bV5RedMask == 63488u &&
				info.bV5GreenMask == 2016u &&
				info.bV5BlueMask == 31u) 
			{
				image.m_data->m_format = yyImageFormat::R5G6B5;
			}
			else if (info.bV5RedMask == 31744u &&
				info.bV5GreenMask == 992u &&
				info.bV5BlueMask == 31u) 
			{
				if (info.bV5AlphaMask)
					image.m_data->m_format = yyImageFormat::A1R5G5B5;
			}
			else 
			{
				yyLogWriteWarning("BMP: unsupported format\n");
				YY_PRINT_FAILED;
				fclose(file);
				return 0;
			}
			fseek(file, 70U, SEEK_SET);
		}
		image.m_data->m_dataSize = image.m_data->m_pitch * image.m_data->m_height;
		image.m_data->m_data = (u8*)yyMemAlloc(image.m_data->m_dataSize);

		fread(image.m_data->m_data, 1, image.m_data->m_dataSize, file);
	}
	else if (image.m_data->m_bits == 8u)
	{
		image.m_data->m_format = yyImageFormat::R8G8B8A8;
		image.m_data->m_pitch = image.m_data->m_width * 4u;
		image.m_data->m_dataSize = image.m_data->m_pitch * image.m_data->m_height;
		fseek(file, 54u, SEEK_SET);
		u32 tableSize = header.bfOffBits - 54u;
		std::unique_ptr<u8[]> table(new u8[tableSize]);
		fread(table.get(), tableSize, 1, file);
		rgbquad * quadTable = reinterpret_cast<rgbquad*>(table.get());
		fseek(file, header.bfOffBits, SEEK_SET);
		image.m_data->m_data = (u8*)yyMemAlloc(image.m_data->m_dataSize);
		u32 indSize = image.m_data->m_width * image.m_data->m_height;
		u8 * _data = new u8[indSize];
		std::unique_ptr<u8[]> inds(_data);
		if (!info.bV5Compression) 
		{
			fread(inds.get(), indSize, 1, file);
		}
		else 
		{
			u8 * _data2 = new u8[info.bV5SizeImage];
			std::unique_ptr<u8[]> rle(_data2);
			fread(rle.get(), info.bV5SizeImage, 1, file);
			decompress8BitRLE(_data2, _data, info.bV5SizeImage, image.m_data->m_width, image.m_data->m_height);
		}
		u8 * u8_ptr = inds.get();
		for (u32 i = 0u, count = 0u; i < indSize; ) 
		{
			image.m_data->m_data[count] = quadTable[u8_ptr[i]].Red;
			image.m_data->m_data[count + 1u] = quadTable[u8_ptr[i]].Green;
			image.m_data->m_data[count + 2u] = quadTable[u8_ptr[i]].Blue;
			image.m_data->m_data[count + 3u] = 255u;
			count += 4u;
			i += 1u;
		}
	}
	else if (image.m_data->m_bits == 4u)
	{
		image.m_data->m_format = yyImageFormat::R8G8B8A8;
		image.m_data->m_pitch = image.m_data->m_width * 4u;
		image.m_data->m_dataSize = image.m_data->m_pitch * image.m_data->m_height;
		fseek(file, 54u, SEEK_SET);
		u32 tableSize = header.bfOffBits - 54u;
		std::unique_ptr<u8[]> table(new u8[tableSize]);
		fread(table.get(), tableSize, 1, file);
		rgbquad * quadTable = reinterpret_cast<rgbquad*>(table.get());
		fseek(file, header.bfOffBits, SEEK_SET);
		u32 indSize = image.m_data->m_width * image.m_data->m_height / 2;
		image.m_data->m_data = (u8*)yyMemAlloc(image.m_data->m_dataSize);
		std::unique_ptr<u8[]> inds(new u8[indSize]);
		u8 * u8_ptr = inds.get();
		if (!info.bV5Compression)
		{
			fread(inds.get(), indSize, 1, file);
		}
		else 
		{
			std::unique_ptr<u8[]> rle(new u8[info.bV5SizeImage]);
			fread(rle.get(), info.bV5SizeImage, 1, file);
			u8 * rlePtr = rle.get();
			decompress4BitRLE(rlePtr, u8_ptr, indSize, image.m_data->m_width, image.m_data->m_height);
		}
		for (u32 i = 0u, count = 0u; i < indSize; ) 
		{
			u8 color = u8_ptr[i];
			u8 part1 = color & 15u;
			u8 part2 = (color & 240U) >> 4u;

			image.m_data->m_data[count] = quadTable[part2].Red;
			image.m_data->m_data[count + 1u] = quadTable[part2].Green;
			image.m_data->m_data[count + 2u] = quadTable[part2].Blue;
			image.m_data->m_data[count + 3u] = 255u;
			image.m_data->m_data[count + 4u] = quadTable[part1].Red;
			image.m_data->m_data[count + 5u] = quadTable[part1].Green;
			image.m_data->m_data[count + 6u] = quadTable[part1].Blue;
			image.m_data->m_data[count + 7u] = 255u;
			count += 8u;
			i += 1u;
		}
	}
	else if (image.m_data->m_bits == 1u) 
	{
		image.m_data->m_format = yyImageFormat::One_bit;
		image.m_data->m_pitch = image.m_data->m_width;
		image.m_data->m_dataSize = info.bV5SizeImage;
		fseek(file, header.bfOffBits, SEEK_SET);
		image.m_data->m_data = (u8*)yyMemAlloc(image.m_data->m_dataSize);
		fread(image.m_data->m_data, image.m_data->m_dataSize, 1, file);
	}
	else 
	{
		YY_PRINT_FAILED;
		fclose(file);
		return 0;
	}
	fclose(file);

	image.m_data->convertToR8G8B8A8();
	image.m_data->flipVertical();
	if(flipPixel)
		image.m_data->flipPixel();

	auto ret = image.m_data;
	image.m_data = 0;
	return ret;
}

//	irrlicht
void decompress4BitRLE(u8*& rleData, u8*& inds, u32 size, u32 width, u32 height) {
	u32 lineWidth = (width + 1u) / 2u/*+pitch*/;
	u8* p = rleData;
	u8* newBmp = inds;
	u8* d = newBmp;
	u8* destEnd = newBmp + lineWidth*height;
	s32 line = 0;
	s32 shift = 4;

	while (rleData - p < (s32)size && d < destEnd) {
		if (*p == 0) {
			++p;

			switch (*p) {
			case 0: // end of line
				++p;
				++line;
				d = newBmp + (line*lineWidth);
				shift = 4;
				break;
			case 1: // end of bmp
				return;
			case 2:
			{
				++p;
				s32 x = (u8)*p; ++p;
				s32 y = (u8)*p; ++p;
				d += x / 2 + y*lineWidth;
				shift = x % 2 == 0 ? 4 : 0;
			}
			break;
			default:
			{
				// absolute mode
				s32 count = (u8)*p; ++p;
				s32 readAdditional = ((2 - ((count) % 2)) % 2);
				s32 readShift = 4;
				s32 i;

				for (i = 0; i<count; ++i)
				{
					s32 color = (((u8)*p) >> readShift) & 0x0f;
					readShift -= 4;
					if (readShift < 0)
					{
						++*p;
						readShift = 4;
					}

					u8 mask = static_cast<u8>(0x0f << shift);
					*d = static_cast<u8>((*d & (~mask)) | ((color << shift) & mask));

					shift -= 4;
					if (shift < 0)
					{
						shift = 4;
						++d;
					}

				}

				for (i = 0; i<readAdditional; ++i)
					++p;
			}
			}
		}
		else
		{
			s32 count = (u8)*p; ++p;
			s32 color1 = (u8)*p; color1 = color1 & 0x0f;
			s32 color2 = (u8)*p; color2 = (color2 >> 4) & 0x0f;
			++p;

			for (s32 i = 0; i<count; ++i)
			{
				u8 mask = static_cast<u8>(0x0f << shift);
				u8 toSet = static_cast<u8>((shift == 0 ? color1 : color2) << shift);
				*d = static_cast<u8>((*d & (~mask)) | (toSet & mask));

				shift -= 4;
				if (shift < 0)
				{
					shift = 4;
					++d;
				}
			}
		}
	}
}

void decompress8BitRLE(u8*& rleData, u8*& inds, u32 size, u32 width, u32 height) {
	u8* p = rleData;
	u8* newBmp = inds;
	u8* d = newBmp;
	u8* destEnd = newBmp + (width*height);
	s32 line = 0;

	while (rleData - p < (s32)size && d < destEnd)
	{
		if (*p == 0)
		{
			++p;

			switch (*p)
			{
			case 0: // end of line
				++p;
				++line;
				d = newBmp + (line*(width));
				break;
			case 1: // end of bmp
				return;
			case 2:
				++p; d += (u8)*p;  // delta
				++p; d += ((u8)*p)*(width);
				++p;
				break;
			default:
			{
				// absolute mode
				s32 count = (u8)*p; ++p;
				s32 readAdditional = ((2 - (count % 2)) % 2);
				s32 i;

				for (i = 0; i<count; ++i)
				{
					*d = *p;
					++p;
					++d;
				}

				for (i = 0; i<readAdditional; ++i)
					++p;
			}
			}
		}
		else
		{
			s32 count = (u8)*p; ++p;
			u8 color = *p; ++p;
			for (s32 i = 0; i<count; ++i)
			{
				*d = color;
				++d;
			}
		}
	}
}