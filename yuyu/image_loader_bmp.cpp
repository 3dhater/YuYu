#include "yy.h"

#include <iostream>
#include <fstream>
#include <filesystem>

#include "yy_image.h"
#include "yy_fs.h"

// http://www.opengl-tutorial.org/beginners-tutorials/tutorial-5-a-textured-cube/#loading-bmp-images-yourself
yyImage* ImageLoader_BMP(const char* p)
{
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int width, height;
	unsigned int imageSize;   // = width*height*3

	auto file_size = yyFS::file_size(p);
	std::ifstream in(p, std::ios::binary);
	if(!in)
	{
		YY_PRINT_FAILED;
		return 0;
	}
	
	FILE * file = fopen(p, "rb");
	if (!file) 
	{
		yyLogWriteWarning("Image could not be opened\n"); 
		YY_PRINT_FAILED;
		return 0;
	}

	if (fread(header, 1, 54, file) != 54) 	{ // If not 54 bytes read : problem
		yyLogWriteWarning("Not a correct BMP file\n");
		YY_PRINT_FAILED;
		return 0;
	}

	if (header[0] != 'B' || header[1] != 'M') {
		yyLogWriteWarning("Not a correct BMP file\n");
		YY_PRINT_FAILED;
		return 0;
	}

	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	if (imageSize == 0)    imageSize = width*height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)      dataPos = 54; // The BMP header is done that way


	yyImage* image = yyCreate<yyImage>();
	image->m_width = width;
	image->m_height = height;
	image->m_format = yyImageFormat::R8G8B8;
	image->m_pitch	= image->m_width * 3;

	image->m_dataSize = image->m_pitch * image->m_height;
	image->m_data = (u8*)yyMemAlloc(image->m_dataSize);

	fread(image->m_data, 1, imageSize, file);
	fclose(file);

	image->convertToR8G8B8A8();
	image->flipVertical();
	image->flipPixel();

	return image;
}
