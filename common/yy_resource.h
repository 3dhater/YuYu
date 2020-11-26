#ifndef _YY_RES_H_
#define _YY_RES_H_

//#include "yy_image.h"
#include <filesystem>

// IMAGE LOADER PROTOTYPES
yyImage* ImageLoader_DDS(const std::filesystem::path& p);
yyImage* ImageLoader_PNG(const std::filesystem::path& p);

using ImageLoaderFunction_t = yyImage*(*)(const std::filesystem::path& p);
//using ImageLoaderExportFunction_t = bool(*)(yyImage* image, const char* fileName, const char* extName );

struct yyImageLoader
{
	std::string ext;
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
	u16 m_index = 0; // index in video driver array (or in any other driver array) 
};

//class yyResourceManager
//{
//public:
//	yyResourceManager();
//	~yyResourceManager();
//
//	//yyVideoDriverAPI* m_videoDriverAPI = nullptr;
//
//	std::vector<yyImageLoader> m_imageLoaders;
//};

#endif