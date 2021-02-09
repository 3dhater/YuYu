﻿#include "yy.h"

#include "yy_resource.h"
#include "yy_ptr.h"
#include "yy_image.h"
#include "yy_model.h"
#include "yy_fs.h"
#include "io/file_buffer.h"

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

YY_API yyMDL* YY_C_DECL yyGetModel(const char* fileName)
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
		{
			++node.m_resource->m_refCount;
			return node.m_resource;
		}
	}

	yyMDL* model = yyLoadModel(fileName);
	if (!model)
	{
		YY_PRINT_FAILED;
		return nullptr;
	}

	++model->m_refCount;

	CacheNode<yyMDL> cache_node;
	cache_node.m_resource = model;
	cache_node.m_path = p;
	g_engine->m_modelCache.push_back(cache_node);
	return model;
}

YY_API yyMDL* YY_C_DECL yyLoadModel(const char* fileName)
{
	assert(fileName);
	if(!yyFS::exists(fileName))
	{
		yyLogWriteWarning("File %s not found\n", fileName);
		YY_PRINT_FAILED;
		return nullptr;
	}
	
	yyFileBuffer fb;
	if(!fb.initFromFile(fileName))
		return nullptr;

	yyMDL* newMDL = yyCreate<yyMDL>();
	char yymdl[4];
	fb.read(yymdl, 4);
	if (
		yymdl[0] != 'y' ||
		yymdl[1] != 'm' ||
		yymdl[2] != 'd' ||
		yymdl[3] != 'l'
		)
		return nullptr;

	u32 version;
	fb.read(&version, sizeof(u32));
	
	if (version > YY_MDL_VERSION)
	{
		yyLogWriteWarning("Can't read MDL file: %s. Bad version. File version: %i Supported version: %i", fileName, version, YY_MDL_VERSION);
		return nullptr;
	}

	switch (version)
	{
	default:
	case 1:
		MDL_loadVersion1(&newMDL, &fb);
		break;
	}

	return newMDL;
}

/*
yyResource* CreateModelFromFile(const char* fileName, bool load)
{
assert(fileName);
yyResource * newRes = yyCreate<yyResource>();
newRes->m_type = yyResourceType::Model;
newRes->m_source = nullptr;
newRes->m_refCount = 0;
newRes->m_file = fileName;

if (g_openGL->m_freeModelResourceIndex.head())
{
newRes->m_index = g_openGL->m_freeModelResourceIndex.head()->m_data;
g_openGL->m_freeModelResourceIndex.erase_node(g_openGL->m_freeModelResourceIndex.head());
}
else
{
newRes->m_index = g_openGL->m_models.size();
g_openGL->m_models.push_back(nullptr);
}


if(load)
LoadModel(newRes);
return newRes;
}
*/
//YY_API yyResource* YY_C_DECL yyGetModelResource(const char* fileName, bool load)
//{
//	assert(fileName);
//	yyFS::path p = fileName;
//	for (auto & node : g_engine->m_modelGPUCache)
//	{
//		if (node.m_path == p)
//		{
//			if (node.m_resource->m_isLoaded)
//				++node.m_resource->m_refCount; // надо прибавлять только в случае если ресурс загружен
//			else
//			{
//				if (load)
//					g_engine->m_videoAPI->LoadModel(node.m_resource); // ++m_refCount inside
//			}
//			return node.m_resource;
//		}
//	}
//
//	auto res = g_engine->m_videoAPI->CreateModelFromFile(fileName, load);
//
//	if (res)
//	{
//		CacheNode<yyResource> cache_node;
//		cache_node.m_resource = res;
//		cache_node.m_path = p;
//		g_engine->m_modelGPUCache.push_back(cache_node);
//	}
//	else
//	{
//		YY_PRINT_FAILED;
//		return nullptr;
//	}
//	return res;
//}

YY_API void YY_C_DECL yyDeleteModel(yyMDL* m)
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

	--m->m_refCount;
	if(!m->m_refCount)
		yyDestroy( m );
}



}
