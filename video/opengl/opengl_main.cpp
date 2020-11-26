#define YY_EXPORTS
#include "yy.h"

#include "OpenGL.h"

yyVideoDriverAPI g_api;

//yyResource g_defaultRes;

#ifdef YY_PLATFORM_WINDOWS
extern wglSwapIntervalEXT_t gwglSwapIntervalEXT;
#endif

u32 GetAPIVersion()
{
	return yyVideoDriverAPIVersion;
}

OpenGL * g_openGL = nullptr;

bool Init(yyWindow* window)
{
	if(g_openGL)
		return true;

	g_openGL = new OpenGL;
	if(!g_openGL->Init(window))
	{
		delete g_openGL;
		g_openGL = nullptr;
		return false;
	}
	return true;
}
void Destroy()
{
	if(g_openGL)
	{
		delete g_openGL;
		g_openGL = nullptr;
	}
}

//void UseClearColor(bool use)
//{
//	g_openGL->m_useClearColor = use;
//}
//void UseClearDepth(bool use)
//{
//	g_openGL->m_useClearDepth = use;
//}

void SetClearColor(f32 r, f32 g, f32 b, f32 a)
{
	gglClearColor( r, g, b, a );
}

//void BeginDraw()
//{
//	GLbitfield mask = 0; //GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT;
//	if(g_openGL->m_useClearColor) mask |= GL_COLOR_BUFFER_BIT;
//	if(g_openGL->m_useClearDepth) mask |= GL_DEPTH_BUFFER_BIT;
//	if( mask ) gglClear(mask);
//}
// `if` is really expensive instruction...
void BeginDrawClearAll()
{
	gglClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}
void BeginDrawClearColor()
{
	gglClear(GL_COLOR_BUFFER_BIT);
}
void BeginDrawClearDepth()
{
	gglClear(GL_DEPTH_BUFFER_BIT);
}
void BeginDrawNotClear()
{
}
void EndDraw()
{
#ifdef YY_PLATFORM_WINDOWS
	SwapBuffers( g_openGL->m_windowDC );
#else
#error For Windows
#endif
}

yyResource* CreateTexture(yyImage* image, bool useLinearFilter)
{
	yyResource * newRes = new yyResource;
	newRes->m_type = yyResourceType::Texture;
	newRes->m_index = g_openGL->m_freeTextureCellIndex;

	// need ?
	//if(g_openGL->m_textures[g_openGL->m_freeTextureCellIndex].m_texture)
	//	delete g_openGL->m_textures[g_openGL->m_freeTextureCellIndex].m_texture;

	g_openGL->m_textures->m_data[g_openGL->m_freeTextureCellIndex].m_data = new OpenGLTexture;
	
	if(g_openGL->initTexture(image, g_openGL->m_textures->m_data[g_openGL->m_freeTextureCellIndex].m_data, useLinearFilter))
	{
		++g_openGL->m_freeTextureCellIndex;
		if(g_openGL->m_freeTextureCellIndex == YY_MAX_TEXTURES)
			g_openGL->m_freeTextureCellIndex = 0;

		if(g_openGL->m_textures->m_data[g_openGL->m_freeTextureCellIndex].m_data)
		{
			for(u32 i = 0; i < YY_MAX_TEXTURES; ++i)
			{
				if(!g_openGL->m_textures->m_data[i].m_data)
				{
					g_openGL->m_freeTextureCellIndex = i;
					break;
				}
			}
		}
		return newRes;
	}

	if(newRes)
		delete newRes;
	if(g_openGL->m_textures->m_data[g_openGL->m_freeTextureCellIndex].m_data)
	{
		delete g_openGL->m_textures->m_data[g_openGL->m_freeTextureCellIndex].m_data;
		g_openGL->m_textures->m_data[g_openGL->m_freeTextureCellIndex].m_data = nullptr;
	}
	return nullptr;
}

yyResource* GetTexture(const char* fileName, bool useLinearFilter)
{
	if(!fileName) return nullptr;
	std::filesystem::path p(fileName);

	auto node = g_openGL->m_textureCache.head();
	if(node)
	{
		for(size_t i = 0, sz = g_openGL->m_textureCache.size(); i < sz; ++i)
		{
			if( node->m_data )
			{
				if( node->m_data->m_path == p )
				{
					++node->m_data->m_refCount;
					return node->m_data->m_resource;
				}
			}
			node = node->m_right;
		}
	}

	auto image = yyLoadImage(fileName);
	if(image)
	{
		auto res = CreateTexture(image, useLinearFilter);
		yyDeleteImage(image);

		TextureCacheNode * cacheNode = new TextureCacheNode;
		cacheNode->m_refCount = 1;
		cacheNode->m_path = fileName;
		cacheNode->m_resource = res;
		//g_openGL->m_textureCache.push_back(cacheNode);
		g_openGL->m_textureCache.push_back(cacheNode);
		return res;
	}
	return nullptr;
}

// STL code is so difficult to understand
// I need to rewrite it - remove STL
void ReleaseTexture(yyResource* res)
{
	if(res->m_type == yyResourceType::Texture)
	{
		res->m_type = yyResourceType::None; // now this yyResource is empty
		auto node = g_openGL->m_textureCache.head();
		for( size_t i = 0, sz = g_openGL->m_textureCache.size(); i < sz; ++i )
		{
			if(node->m_data->m_resource->m_index == res->m_index)
			{
				--node->m_data->m_refCount;
				if(!node->m_data->m_refCount)
				{
					auto index = node->m_data->m_resource->m_index;
					if(g_openGL->m_textures->m_data[index].m_data)
					{
						delete g_openGL->m_textures->m_data[index].m_data;
						g_openGL->m_textures->m_data[index].m_data = nullptr;
					}
					node->m_data->m_path.clear();
					delete res;
					//g_openGL->m_textureCache.erase(N.second.m_path.generic_string());
					g_openGL->m_textureCache.erase_first(node->m_data);

					if(index < g_openGL->m_freeTextureCellIndex)
						g_openGL->m_freeTextureCellIndex = index;
				}
				return;
			}
			node = node->m_right;
		}
	}
	if(g_openGL->m_textures->m_data[res->m_index].m_data)
	{
		delete g_openGL->m_textures->m_data[res->m_index].m_data;
		g_openGL->m_textures->m_data[res->m_index].m_data = nullptr;
	}
	if(res->m_index < g_openGL->m_freeTextureCellIndex)
		g_openGL->m_freeTextureCellIndex = res->m_index;
	delete res;
}

void UseVSync(bool v)
{
#ifdef YY_PLATFORM_WINDOWS
	gwglSwapIntervalEXT(v ? 1 : 0);
#else
#error For Windows
#endif
}

yyResource* CreateModel(yyModel* model)
{
	yyResource * newRes = new yyResource;
	newRes->m_type = yyResourceType::Model;
	newRes->m_index = g_openGL->m_freeModelsCellIndex;

	// need ?
	//if(g_openGL->m_textures[g_openGL->m_freeTextureCellIndex].m_texture)
	//	delete g_openGL->m_textures[g_openGL->m_freeTextureCellIndex].m_texture;

	g_openGL->m_models->m_data[g_openGL->m_freeModelsCellIndex].m_data = new OpenGLModel;
	
	if(g_openGL->initModel(model, g_openGL->m_models->m_data[g_openGL->m_freeModelsCellIndex].m_data))
	{
		++g_openGL->m_freeModelsCellIndex;
		if(g_openGL->m_freeModelsCellIndex == YY_MAX_MODELS)
			g_openGL->m_freeModelsCellIndex = 0;

		if(g_openGL->m_models->m_data[g_openGL->m_freeModelsCellIndex].m_data)
		{
			for(u32 i = 0; i < YY_MAX_MODELS; ++i)
			{
				if(!g_openGL->m_models->m_data[i].m_data)
				{
					g_openGL->m_freeModelsCellIndex = i;
					break;
				}
			}
		}
		return newRes;
	}

	if(newRes)
		delete newRes;
	if(g_openGL->m_models->m_data[g_openGL->m_freeModelsCellIndex].m_data)
	{
		delete g_openGL->m_models->m_data[g_openGL->m_freeModelsCellIndex].m_data;
		g_openGL->m_models->m_data[g_openGL->m_freeModelsCellIndex].m_data = nullptr;
	}
	return nullptr;
}

void ReleaseModel(yyResource* res)
{
}

extern "C"
{
	YY_API yyVideoDriverAPI* YY_C_DECL GetAPI()
	{
		g_api.GetAPIVersion = GetAPIVersion;
		g_api.Init          = Init;
		g_api.Destroy       = Destroy;
	
		//g_api.UseClearColor = UseClearColor;
		//g_api.UseClearDepth = UseClearDepth;
		g_api.SetClearColor = SetClearColor;
		//g_api.BeginDraw		= BeginDraw;
		g_api.BeginDrawClearAll	= BeginDrawClearAll;
		g_api.BeginDrawClearColor	= BeginDrawClearColor;
		g_api.BeginDrawClearDepth	= BeginDrawClearDepth;
		g_api.BeginDrawNotClear	= BeginDrawNotClear;
		g_api.EndDraw		= EndDraw;

		g_api.CreateTexture = CreateTexture;
		g_api.GetTexture = GetTexture;
		g_api.ReleaseTexture = ReleaseTexture;
		g_api.UseVSync = UseVSync;
		g_api.CreateModel = CreateModel;
		g_api.ReleaseModel = ReleaseModel;

		return &g_api;
	}
}
