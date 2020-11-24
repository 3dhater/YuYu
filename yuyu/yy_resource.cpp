﻿#include "yy.h"

#include "yy_resource.h"
#include "yy_ptr.h"

#include <cassert>
#include <string>
#include "engine.h"

extern Engine * g_engine;


yyResourceManager::yyResourceManager()
{
	yyLogWriteInfo("Init resource manager\n");

	yyImageLoader loader;
	loader.ext = ".dds";
	loader.image_loader_callback = ImageLoader_DDS;
	m_imageLoaders.push_back(loader);

	loader.ext = ".png";
	loader.image_loader_callback = ImageLoader_PNG;
	m_imageLoaders.push_back(loader);
}

yyResourceManager::~yyResourceManager()
{
	yyLogWriteInfo("Destroy resource manager\n");
}

extern "C"
{

YY_API void YY_C_DECL yyLoadImageAsync(const char* fn, s32 id)
{
	g_engine->m_workerCommands.put(BackgroundWorkerCommands(BackgroundWorkerCommands::LoadImage, fn, id));
}

YY_API yyImage* YY_C_DECL yyLoadImage(const char* fileName)
{
	assert(fileName);
	std::filesystem::path p(fileName);
	if( !std::filesystem::exists(p) )
	{
		YY_PRINT_FAILED;
		return nullptr;
	}
	if( !p.has_extension() )
	{
		YY_PRINT_FAILED;
		return nullptr;
	}

	for( auto & loader : g_engine->m_resourceManager->m_imageLoaders )
	{
		auto e1 = p.extension().u8string();
		std::transform(e1.begin(), e1.end(), e1.begin(),
		    [](unsigned char c){ return std::tolower(c); });

		if(std::filesystem::path(loader.ext.data()) == e1)
			return loader.image_loader_callback(p);
	}
	return nullptr;
}
YY_API void YY_C_DECL yyDeleteImage(yyImage* image)
{
	assert(image);
	delete image;
}

/*YY_API yyResource YY_C_DECL yyGetTextureFromFile(const char* fn)
{
	yyPtr<yyImage> image = yyLoadImage(fn);
	
	yyResource newResource;
	
	if(!image.m_data)
		return newResource;

	g_engine->m_videoAPI->
}*/

}
