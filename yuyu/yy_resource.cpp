#include "yy.h"

#include "yy_resource.h"
#include "yyResourceImpl.h"
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
	YY_API void YY_C_DECL yyLoadImageGetInfo(const char* fileName, yyImage* img) {
		assert(fileName);
		if (!yy_fs::exists(fileName))
		{
			yyLogWriteWarning("File %s not found\n", fileName);
			YY_PRINT_FAILED;
			return;
		}

		yyStringA str(fileName);
		util::stringToLower(str);
		auto ext = util::stringGetExtension(str, true);
		if (!ext.size())
		{
			YY_PRINT_FAILED;
			return;
		}

		for (auto & loader : g_engine->m_imageLoaders)
		{
			if (loader.ext == ext)
				loader.image_loader_getInfo_callback(fileName, img);
		}
		return;
	}

YY_API yyImage* YY_C_DECL yyLoadImage(const char* fileName){
	assert(fileName);
	if(!yy_fs::exists(fileName))
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

YY_API void YY_C_DECL yyRemoveMDLFromCache(yyMDL* mdl) {
	auto curr = g_engine->m_modelCache.head();
	if (!curr)
		return;

	auto last = curr->m_left;

	while (true)
	{
		if (curr->m_data.m_resource == mdl)
		{
			g_engine->m_modelCache.erase_by_node(curr);
			return;
		}

		if (curr == last)
			break;

		curr = curr->m_right;
	}
}

YY_API yyMDL* YY_C_DECL yyGetMDLFromCache(const char* fileName){
	YY_DEBUG_PRINT_FUNC;
	assert(fileName);
	if (!yy_fs::exists(fileName))
	{
		yyLogWriteWarning("File %s not found\n", fileName);
		YY_PRINT_FAILED;
		return nullptr;
	}

	yy_fs::path p = fileName;
	for (auto & node : g_engine->m_modelCache)
	{
		if (node.m_path == p)
		{
			++node.m_resource->m_refCount;
			return node.m_resource;
		}
	}

	yyMDL* model = yyCreateMDLFromFile(fileName);
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

YY_API yyMDL* YY_C_DECL yyCreateMDLFromFile(const char* fileName){
	YY_DEBUG_PRINT_FUNC;
	assert(fileName);
	yyLogWriteInfo("Load model: %s\n", fileName);
	if(!yy_fs::exists(fileName))
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


	//// попробую использовать такой способ обхода цикла
	//// без u16 i = 0, sz = newMDL->m_layers.size()
	//u16 size = newMDL->m_layers.size();
	//for (u16 i = 0; i < size; ++i)
	//{
	//	auto layer = newMDL->m_layers[i];
	//	for (u16 o = 0; o < YY_MDL_LAYER_NUM_OF_TEXTURES; ++o)
	//	{
	//		if (layer->m_texturePath[o].size())
	//		{
	//			layer->m_textureGPU[o] = yyGetTexture(layer->m_texturePath[o].data(), useLinearFilterForTextures, false, loadTextures);
	//		}
	//	}

	//	//layer->m_meshGPU = g_engine->m_videoAPI->CreateModel(layer->m_model);
	//	layer->m_meshGPU = yyCreateModel(layer->m_model);
	//}

	return newMDL;
}


YY_API void YY_C_DECL yySetTextureFilter(yyTextureFilter f) {
	g_engine->m_textureFilter = f;
}

YY_API yyTextureFilter YY_C_DECL yyGetTextureFilter() {
	return g_engine->m_textureFilter;
}

YY_API void YY_C_DECL yyUseMipMaps(bool v) {
	g_engine->m_useMipmaps = v;
}

YY_API bool YY_C_DECL yyIsUseMipMaps() {
	return g_engine->m_useMipmaps;
}

YY_API void YY_C_DECL yySetTextureAddressMode(yyTextureAddressMode m) {
	g_engine->m_textureAddressMode = m;
}
YY_API void YY_C_DECL yySetTextureComparisonFunc(yyTextureComparisonFunc f) {
	g_engine->m_textureComparisonFunc = f;
}
YY_API yyTextureComparisonFunc YY_C_DECL yyGetTextureComparisonFunc() {
	return g_engine->m_textureComparisonFunc;
}
YY_API yyTextureAddressMode YY_C_DECL yyGetTextureAddressMode() {
	return g_engine->m_textureAddressMode;
}
YY_API void YY_C_DECL yySetTextureAnisotropicLevel(s32 v) {
	g_engine->m_textureAnisotropicLevel = v;
}
YY_API s32 YY_C_DECL yyGetTextureAnisotropicLevel() {
	return g_engine->m_textureAnisotropicLevel;
}

YY_API yyResource* YY_C_DECL yyCreateModel(yyModel* model) {
	//auto res = yyCreate<yyResourceImpl>();
	auto res = (yyResourceImpl*)yyMegaAllocator::CreateResource();
	res->InitModelResourse(model);
	return res;
}

YY_API yyResource* YY_C_DECL yyCreateTexture(yyImage* image) {
	//auto res = yyCreate<yyResourceImpl>();
	auto res = (yyResourceImpl*)yyMegaAllocator::CreateResource();
	res->InitTextureResourse(image, 0);
	return res;
}
YY_API yyResource* YY_C_DECL yyCreateRenderTargetTexture(const v2f& size) {
	//auto res = yyCreate<yyResourceImpl>();
	auto res = (yyResourceImpl*)yyMegaAllocator::CreateResource();
	res->InitTextureRenderTargetResourse(size);
	res->Load(false);
	return res;
}

YY_API yyResource* YY_C_DECL yyCreateTextureFromFile(const char* fileName) {
	assert(fileName);
	//auto res = yyCreate<yyResourceImpl>();
	auto res = (yyResourceImpl*)yyMegaAllocator::CreateResource();
	res->InitTextureResourse(0, fileName);
	return res;
}

YY_API yyResource* YY_C_DECL yyGetTextureFromCache(const char* fileName){
	assert(fileName);
	yy_fs::path p = fileName;
	for (auto & node : g_engine->m_textureCache)
	{
		if (node.m_path == p)
		{
			//node.m_resource->Load(async); // ++refCount;
			node.m_resource->AddRef();
			return node.m_resource;
		}
	}

	yyLogWriteInfo("Load texture: %s\n", fileName);

	auto res = yyCreateTextureFromFile(fileName);

	if (res)
	{
		CacheNode<yyResource> cache_node;
		cache_node.m_resource = res;
		cache_node.m_path = p;
		g_engine->m_textureCache.push_back(cache_node);

		yyResourceImpl* impl = (yyResourceImpl*)res;
		impl->m_flags |= impl->flag_fromCache;
	}
	else
	{
		YY_PRINT_FAILED;
		return nullptr;
	}
	return res;
}

YY_API void YY_C_DECL yyRemoveTextureFromCache(yyResource* r) {
	auto curr = g_engine->m_textureCache.head();
	if (!curr)
		return;
	
	auto last = curr->m_left;

	while(true)
	{
		if (curr->m_data.m_resource == r)
		{
			g_engine->m_textureCache.erase_by_node(curr);
			return;
		}

		if (curr == last)
			break;

		curr = curr->m_right;
	}
}

YY_API void YY_C_DECL yyDeleteTexture(yyResource* r, bool onlyUnloaded) {
	if (onlyUnloaded)
	{
		if (!r->IsLoaded())
		{
			yyRemoveTextureFromCache(r);
			yyMegaAllocator::Destroy(r);
		}
	}
	else
	{
		yyRemoveTextureFromCache(r);
		yyMegaAllocator::Destroy(r);
	}
}

}


