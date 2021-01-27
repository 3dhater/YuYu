#ifndef _YY_RES_H_
#define _YY_RES_H_

#include "strings/string.h"
#include "math\aabb.h"

#include "yy_fs.h"
//#include <filesystem>

// LOADER PROTOTYPES
// don't forget to add this in Engine::m_imageLoaders
yyImage* ImageLoader_DDS(const char* p);
yyImage* ImageLoader_PNG(const char* p);
yyImage* ImageLoader_TGA(const char* p);
yyImage* ImageLoader_BMP(const char* p); 
yyModel* ModelLoader_TR3D(const char* p); // from my old game

typedef yyImage*(*ImageLoaderFunction_t)(const char* p);
typedef yyModel*(*ModelLoaderFunction_t)(const char* p);
//using ImageLoaderExportFunction_t = bool(*)(yyImage* image, const char* fileName, const char* extName );

struct yyImageLoader
{
	yyImageLoader()
		:
		image_loader_callback(nullptr)
	{}

	yyStringA ext;
	ImageLoaderFunction_t image_loader_callback;
};

struct yyModelLoader
{
	yyModelLoader()
		:
		model_loader_callback(nullptr)
	{}

	yyStringA ext;
	ModelLoaderFunction_t model_loader_callback;
};

// for yyResource
enum class yyResourceType : u16
{
	None,
	Texture,
	RenderTargetTexture,
	Model,
};
struct yyResource
{
	yyResource()
		:
		m_isLoaded(false),
		m_type(yyResourceType::None),
		m_index(0),
		m_refCount(0),
		m_source(nullptr),
		m_flags(0)
	{
		m_aabb.m_min = m_aabb.m_max = v4f();
	}

	bool m_isLoaded;

	yyResourceType m_type;
	size_t m_index; // index in video driver array (or in any other driver array) 
	u32 m_refCount; // нельзя использовать напрямую. использовать только в videoDriver и yuyu.dll

	Aabb m_aabb; // for models/ must be

	// for reload
	yyStringA m_file; // from file
	void * m_source; //from yyImage* or yyModel*
	u32 m_flags;
	enum flags{
		texture_useLinearFilter = BIT(0),
		texture_useComparisonFilter = BIT(1)
	};
};

#endif