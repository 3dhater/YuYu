#include "yy.h"

#include <iostream>
#include <fstream>
#include <filesystem>

#include "yy_image.h"
#include "yy_fs.h"

#include <png.h>
#include <zlib.h>

static void __cdecl user_error_fn( png_structp png_ptr, png_const_charp msg ){
	fprintf(stderr, "%s\n", msg);
	longjmp(png_jmpbuf(png_ptr), 1);
}
static void __cdecl user_warning_fn( png_structp /*png_ptr*/, png_const_charp msg ){
	fprintf(stderr, "%s\n", msg);
}
class PNG
{
public:

	PNG():
		png_ptr( nullptr ),
		info_ptr( nullptr )
	{}

	~PNG(){
		if( info_ptr ){
			png_destroy_read_struct( &png_ptr, &info_ptr, NULL);
		}else if( png_ptr )
			png_destroy_read_struct( &png_ptr, NULL, NULL);
	}

	png_structp png_ptr;
	png_infop info_ptr;
};

void PNGAPI user_read_fn( png_structp png_ptr, png_bytep data, png_size_t length )
{
	std::ifstream * f = (std::ifstream*)png_get_io_ptr( png_ptr );
	f->read( (char*)data, (u32)length );
}

yyImage* ImageLoader_PNG(const char* p)
{
	//auto file_size = std::filesystem::file_size(p);
	auto file_size = yyFS::file_size(p);
	std::ifstream in(p, std::ios::binary);
	if(!in)
	{
		YY_PRINT_FAILED;
		return 0;
	}
	
	u8 buf[4];
	in.read( (char*)buf, 4 );
	if( png_sig_cmp( buf, (png_size_t)0, 4 ) ){
		YY_PRINT_FAILED;
		return 0;
	}

	PNG png;
	png.png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, user_error_fn, user_warning_fn );
	if( !png.png_ptr ){
		YY_PRINT_FAILED;
		return 0;
	}

	png.info_ptr = png_create_info_struct( png.png_ptr );
	if( !png.info_ptr ){
		YY_PRINT_FAILED;
		return 0;
	}

	if( setjmp( png_jmpbuf( png.png_ptr ) ) ){
		return 0;
	}

	png_set_read_fn( png.png_ptr, (void *)&in, user_read_fn);
	png_set_sig_bytes( png.png_ptr, 4 );
	png_read_info( png.png_ptr, png.info_ptr );

	u32 w, h;
	s32 bits;
	s32 color_type;
	png_get_IHDR( png.png_ptr, png.info_ptr, &w, &h, &bits, &color_type, NULL, NULL, NULL);

	if( color_type == PNG_COLOR_TYPE_PALETTE )
		png_set_palette_to_rgb( png.png_ptr );

	if( bits < 8 ){
		if( color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA )
			png_set_expand_gray_1_2_4_to_8( png.png_ptr );
	else
		png_set_packing( png.png_ptr );
	}

	if( png_get_valid( png.png_ptr, png.info_ptr, PNG_INFO_tRNS ) )
		png_set_tRNS_to_alpha( png.png_ptr );

	if( bits == 16 )
		png_set_strip_16( png.png_ptr );

	if( color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA )
		png_set_gray_to_rgb( png.png_ptr );

	s32 intent;
	const f64 screen_gamma = 2.2;

	if( png_get_sRGB( png.png_ptr, png.info_ptr, &intent ) )
		png_set_gamma( png.png_ptr, screen_gamma, 0.45455 );
	else{
		double image_gamma;
		if( png_get_gAMA( png.png_ptr, png.info_ptr, &image_gamma ) )
			png_set_gamma( png.png_ptr, screen_gamma, image_gamma );
		else
			png_set_gamma( png.png_ptr, screen_gamma, 0.45455);
	}

	png_read_update_info( png.png_ptr,  png.info_ptr );
	png_get_IHDR( png.png_ptr, png.info_ptr, &w, &h, &bits, &color_type,
		NULL, NULL, NULL);

	yyImage* image = yyCreate<yyImage>();
	image->m_width = w;
	image->m_height = h;
	if( color_type == PNG_COLOR_TYPE_RGB_ALPHA ){
		image->m_format = yyImageFormat::R8G8B8A8;
		image->m_pitch	= image->m_width * 4;
	}else{
		image->m_format = yyImageFormat::R8G8B8;
		image->m_pitch	= image->m_width * 3;
	}

	image->m_dataSize = image->m_pitch * image->m_height;
	image->m_data = (u8*)yyMemAlloc(image->m_dataSize);

	std::unique_ptr<png_bytep[]> row_pointers( new png_bytep[ image->m_height ] );
	for( u32 row = 0, p = 0; row < image->m_height; ++row ){
		row_pointers[ row ] = &image->m_data[ p ];
		p += image->m_pitch;
	}

	png_read_image( png.png_ptr, row_pointers.get() );
	png_read_end( png.png_ptr, NULL );

	return image;
}
