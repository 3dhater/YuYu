#include "yy.h"

#include "yy_resource.h"
#include "yy_ptr.h"
#include "yy_image.h"
#include "yy_model.h"
#include "yy_fs.h"

#include <cassert>
#include <string>
#include "engine.h"

extern Engine * g_engine;

extern "C"
{

YY_API void YY_C_DECL yyLoadImageAsync(const char* fn, s32 id)
{
	g_engine->m_workerCommands.put(BackgroundWorkerCommands(BackgroundWorkerCommands::LoadImage, fn, id));
}

YY_API yyImage* YY_C_DECL yyLoadImage(const char* fileName)
{
	assert(fileName);
	if(!yyFS::exists(fileName))
	{
		yyLogWriteWarning("File %s not found\n", fileName);
		YY_PRINT_FAILED;
		return nullptr;
	}

	yyStringA str(fileName);
	util::stringToLower(str);
	auto ext = util::stringGetExtension(str, true);
	if( !ext.size() )
	{
		YY_PRINT_FAILED;
		return nullptr;
	}

	for( auto & loader : g_engine->m_imageLoaders )
	{
		if(loader.ext == ext)
			return loader.image_loader_callback(fileName);
	}
	return nullptr;
}
YY_API void YY_C_DECL yyDeleteImage(yyImage* image)
{
	assert(image);
	yyDestroy( image );
}

YY_API yyModel* YY_C_DECL yyGetModel(const char* fileName)
{
	assert(fileName);
	if (!yyFS::exists(fileName))
	{
		yyLogWriteWarning("File %s not found\n", fileName);
		YY_PRINT_FAILED;
		return nullptr;
	}

	yyFS::path p = fileName;
	for (auto & node : g_engine->m_modelCache)
	{
		if (node.m_path == p)
			return node.m_resource;
	}

	yyModel* model = yyLoadModel(fileName);
	if (!model)
	{
		YY_PRINT_FAILED;
		return nullptr;
	}

	CacheNode<yyModel> cache_node;
	cache_node.m_resource = model;
	cache_node.m_path = p;
	g_engine->m_modelCache.push_back(cache_node);
	return model;
}

YY_API yyModel* YY_C_DECL yyLoadModel(const char* fileName)
{
	assert(fileName);
	if(!yyFS::exists(fileName))
	{
		yyLogWriteWarning("File %s not found\n", fileName);
		YY_PRINT_FAILED;
		return nullptr;
	}
	yyStringA str(fileName);
	util::stringToLower(str);
	auto ext = util::stringGetExtension(str, true);
	if( !ext.size() )
	{
		YY_PRINT_FAILED;
		return nullptr;
	}

	for( auto & loader : g_engine->m_modelLoaders )
	{
		if(loader.ext == ext)
			return loader.model_loader_callback(fileName);
	}
	return nullptr;
}

YY_API void YY_C_DECL yyDeleteModel(yyModel* m)
{
	assert(m);

	for (size_t i = 0, sz = g_engine->m_modelCache.size(); i < sz; ++i)
	{
		auto & node = g_engine->m_modelCache[i];
		if (m != node.m_resource)
			continue;

		g_engine->m_modelCache.erase(g_engine->m_modelCache.begin() + i);
		break;
	}

	yyDestroy( m );
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
