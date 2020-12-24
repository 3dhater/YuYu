#include "yy.h"

#include "yy_image.h"
#include "dds.h"

#include "io/file_io.h"

#include <iostream>
#include <fstream>
//#include <filesystem>

#include "yy_ptr.h"
#include "yy_fs.h"

//bool ImageLoader_DDS_export(yyImage* img, const char* fileName, const char* extName)
//{
//	char filePath[256];
//	sprintf(filePath, "%s.%s", fileName, extName);
//	
//	yyFileIO f;
//	if(!f.open(filePath, "wb+"))
//	{
//		YY_PRINT_FAILED;
//		return false;
//	}
//
//	DDS_HEADER hdr;
//	hdr.height = img->m_height;
//	hdr.width = img->m_width;
//	hdr.pitchOrLinearSize = img->m_dataSize;
//	
//	const unsigned int dds_magic = 0x20534444;
//	f.writeUnsignedInt(dds_magic);
//	f.writeBytes(&hdr, hdr.size);
//	f.writeBytes(img->m_data, img->m_dataSize);
//	return true;
//}

yyImage* ImageLoader_DDS(const char* p)
{
	//auto file_size = std::filesystem::file_size(p);
	auto file_size = yyFS::file_size(p);

	if( file_size < sizeof(u32) + sizeof(DDS_HEADER) )
	{
		YY_PRINT_FAILED;
		return 0;
	}

	/*u8* file_data = new u8[(u32)file_size];
	if(!file_data)
	{
		YY_PRINT_FAILED;
		return 0;
	}*/

	//yyPtr<u8> _u8_destroyer(file_data);

	yyPtr<yyImage> image = yyCreate<yyImage>();
	if (!image.m_data)
	{
		YY_PRINT_FAILED;
		return 0;
	}

	image.m_data->m_data = (u8*)yyMemAlloc(file_size);
	if (!image.m_data->m_data)
	{
		YY_PRINT_FAILED;
		return 0;
	}

	std::ifstream in(p, std::ios::binary);
	if(!in)
	{
		YY_PRINT_FAILED;
		return 0;
	}


	in.read((char*)image.m_data->m_data, file_size);
	if( in.gcount() < (std::streamsize)file_size )
	{
		YY_PRINT_FAILED;
		return 0;
	}

	auto magic = *reinterpret_cast<const u32*>(image.m_data->m_data);
    if(magic != DDS_MAGIC)
    {
        YY_PRINT_FAILED;
		return 0;
    }

	auto hdr = reinterpret_cast<const DDS_HEADER*>(image.m_data->m_data + sizeof(u32));
	if( hdr->size != sizeof(DDS_HEADER) )
    {
        YY_PRINT_FAILED;
		return 0;
    }

	if( hdr->ddspf.size != sizeof(DDS_PIXELFORMAT) )
    {
        YY_PRINT_FAILED;
		return 0;
    }

	if( (hdr->ddspf.flags & DDS_FOURCC) &&
        (YY_MAKEFOURCC('D', 'X', '1', '0') == hdr->ddspf.fourCC))
    {
		YY_PRINT_FAILED;
		return 0;
	}

	auto offset = sizeof(u32) + sizeof(DDS_HEADER);
    u8 * bitData = image.m_data->m_data + offset;
    u64 bitSize  = file_size - offset;
	image.m_data->m_bitDataOffset = offset;
	image.m_data->m_fileSize = file_size;

	auto format = GetDDSImageFormat(hdr->ddspf);
	if(format == yyImageFormat::Unknown)
    {
		YY_PRINT_FAILED;
		return 0;
    }

	if(hdr->flags & DDS_HEADER_FLAGS_VOLUME)
    {
		YY_PRINT_FAILED;
		return 0;
    }

	if(hdr->caps2 & DDS_CUBEMAP)
	{
		YY_PRINT_FAILED;
		return 0;
	}

	// Дальше всё для OpenGL

	image.m_data->m_mipCount = hdr->mipMapCount;
    if(0 == image.m_data->m_mipCount)
        image.m_data->m_mipCount = 1;

	image.m_data->m_width  = hdr->width;
	image.m_data->m_height = hdr->height;
	image.m_data->m_pitch  = hdr->pitchOrLinearSize;

	if( image.m_data->m_mipCount > 1 )
	{
		image.m_data->m_dataSize = hdr->pitchOrLinearSize * 2;
	}
	else
	{
		if(format == yyImageFormat::R8G8B8A8 || format == yyImageFormat::A8R8G8B8)
		{
			image.m_data->m_pitch  = 4 * image.m_data->m_width;
			image.m_data->m_dataSize = image.m_data->m_pitch * image.m_data->m_height;
		
			std::memcpy(image.m_data->m_data, bitData, image.m_data->m_dataSize);
		}
		else
		{
			image.m_data->m_dataSize = hdr->pitchOrLinearSize;
		}
	}

	

	image.m_data->m_format = format;

	auto ret = image.m_data;

	image.m_data = nullptr;

	return ret;
}
