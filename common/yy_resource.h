#ifndef _YY_RES_H_
#define _YY_RES_H_

#include "strings/string.h"

#include <filesystem>

// LOADER PROTOTYPES
yyImage* ImageLoader_DDS(const std::filesystem::path& p);
yyImage* ImageLoader_PNG(const std::filesystem::path& p);
yyModel* ModelLoader_TR3D(const std::filesystem::path& p); // from my old game

using ImageLoaderFunction_t = yyImage*(*)(const std::filesystem::path& p);
//using ImageLoaderExportFunction_t = bool(*)(yyImage* image, const char* fileName, const char* extName );

struct yyImageLoader
{
	yyStringA ext;
	ImageLoaderFunction_t image_loader_callback = nullptr;
//	ImageLoaderExportFunction_t image_export_callback = nullptr;
};

// for yyResource
enum class yyResourceType : u16
{
	None,
	Texture,
	Model,
};
struct yyResource
{
	yyResourceType m_type = yyResourceType::None;
	size_t m_index = 0; // index in video driver array (or in any other driver array) 
	u32 m_refCount=0;

	// for reload
	yyStringA m_file; // from file
	void * m_source = nullptr; //from yyImage* or yyModel*
	u32 m_flags = 0;
	enum flags{
		texture_useLinearFilter = BIT(0)
	};
};

#endif