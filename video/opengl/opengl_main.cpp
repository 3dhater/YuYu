#define YY_EXPORTS
#include "yy.h"

#include "OpenGL.h"
#include "OpenGL_texture.h"
#include "OpenGL_model.h"
#include "OpenGL_shader_GUI.h"

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

	g_openGL = yyCreate<OpenGL>();
	if(!g_openGL->Init(window))
	{
		yyDestroy(g_openGL);
		g_openGL = nullptr;
		return false;
	}
	return true;
}
void Destroy()
{
	if(g_openGL)
	{
		yyDestroy(g_openGL);
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
	yyResource * newRes = yyCreate<yyResource>();
	newRes->m_type = yyResourceType::Texture;
	newRes->m_index = g_openGL->m_freeTextureCellIndex;

	// need ?
	//if(g_openGL->m_textures[g_openGL->m_freeTextureCellIndex].m_texture)
	//	delete g_openGL->m_textures[g_openGL->m_freeTextureCellIndex].m_texture;

	g_openGL->m_textures->m_cells[g_openGL->m_freeTextureCellIndex].m_data = yyCreate<OpenGLTexture>();
	
	if(g_openGL->initTexture(image, g_openGL->m_textures->m_cells[g_openGL->m_freeTextureCellIndex].m_data, useLinearFilter))
	{
		++g_openGL->m_freeTextureCellIndex;
		if(g_openGL->m_freeTextureCellIndex == YY_MAX_TEXTURES)
			g_openGL->m_freeTextureCellIndex = 0;

		if(g_openGL->m_textures->m_cells[g_openGL->m_freeTextureCellIndex].m_data)
		{
			for(u32 i = 0; i < YY_MAX_TEXTURES; ++i)
			{
				if(!g_openGL->m_textures->m_cells[i].m_data)
				{
					g_openGL->m_freeTextureCellIndex = i;
					break;
				}
			}
		}
		return newRes;
	}

	if(newRes)
		yyDestroy( newRes );
	if(g_openGL->m_textures->m_cells[g_openGL->m_freeTextureCellIndex].m_data)
	{
		yyDestroy( g_openGL->m_textures->m_cells[g_openGL->m_freeTextureCellIndex].m_data );
		g_openGL->m_textures->m_cells[g_openGL->m_freeTextureCellIndex].m_data = nullptr;
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

		CacheNode * cacheNode = yyCreate<CacheNode>();
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
					if(g_openGL->m_textures->m_cells[index].m_data)
					{
						yyDestroy( g_openGL->m_textures->m_cells[index].m_data );
						g_openGL->m_textures->m_cells[index].m_data = nullptr;
					}
					node->m_data->m_path.clear();
					yyDestroy( res );
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
	if(g_openGL->m_textures->m_cells[res->m_index].m_data)
	{
		yyDestroy( g_openGL->m_textures->m_cells[res->m_index].m_data );
		g_openGL->m_textures->m_cells[res->m_index].m_data = nullptr;
	}
	if(res->m_index < g_openGL->m_freeTextureCellIndex)
		g_openGL->m_freeTextureCellIndex = res->m_index;
	yyDestroy( res );
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
	yyResource * newRes = yyCreate<yyResource>();
	newRes->m_type = yyResourceType::Model;
	newRes->m_index = g_openGL->m_freeModelsCellIndex;

	// need ?
	//if(g_openGL->m_textures[g_openGL->m_freeTextureCellIndex].m_texture)
	//	delete g_openGL->m_textures[g_openGL->m_freeTextureCellIndex].m_texture;

	g_openGL->m_models->m_cells[g_openGL->m_freeModelsCellIndex].m_data = yyCreate<OpenGLModel>();
	
	if(g_openGL->initModel(model, g_openGL->m_models->m_cells[g_openGL->m_freeModelsCellIndex].m_data))
	{
		++g_openGL->m_freeModelsCellIndex;
		if(g_openGL->m_freeModelsCellIndex == YY_MAX_MODELS)
			g_openGL->m_freeModelsCellIndex = 0;

		if(g_openGL->m_models->m_cells[g_openGL->m_freeModelsCellIndex].m_data)
		{
			for(u32 i = 0; i < YY_MAX_MODELS; ++i)
			{
				if(!g_openGL->m_models->m_cells[i].m_data)
				{
					g_openGL->m_freeModelsCellIndex = i;
					break;
				}
			}
		}
		return newRes;
	}

	if(newRes)
		yyDestroy( newRes );
	if(g_openGL->m_models->m_cells[g_openGL->m_freeModelsCellIndex].m_data)
	{
		yyDestroy( g_openGL->m_models->m_cells[g_openGL->m_freeModelsCellIndex].m_data );
		g_openGL->m_models->m_cells[g_openGL->m_freeModelsCellIndex].m_data = nullptr;
	}
	return nullptr;
}

void ReleaseModel(yyResource* res)
{
	if(res->m_type == yyResourceType::Model)
	{
		res->m_type = yyResourceType::None; // now this yyResource is empty
		auto node = g_openGL->m_modelCache.head();
		for( size_t i = 0, sz = g_openGL->m_modelCache.size(); i < sz; ++i )
		{
			if(node->m_data->m_resource->m_index == res->m_index)
			{
				--node->m_data->m_refCount;
				if(!node->m_data->m_refCount)
				{
					auto index = node->m_data->m_resource->m_index;
					if(g_openGL->m_models->m_cells[index].m_data)
					{
						yyDestroy( g_openGL->m_models->m_cells[index].m_data );
						g_openGL->m_models->m_cells[index].m_data = nullptr;
					}
					node->m_data->m_path.clear();
					yyDestroy( res );
					g_openGL->m_modelCache.erase_first(node->m_data);

					if(index < g_openGL->m_freeModelsCellIndex)
						g_openGL->m_freeModelsCellIndex = index;
				}
				return;
			}
			node = node->m_right;
		}
	}
	if(g_openGL->m_models->m_cells[res->m_index].m_data)
	{
		yyDestroy( g_openGL->m_models->m_cells[res->m_index].m_data );
		g_openGL->m_models->m_cells[res->m_index].m_data = nullptr;
	}
	if(res->m_index < g_openGL->m_freeModelsCellIndex)
		g_openGL->m_freeModelsCellIndex = res->m_index;
	yyDestroy( res );
}

GLenum last_active_texture;
GLint last_program;
GLint last_texture;
GLint last_sampler;
GLint last_array_buffer;
GLint last_vertex_array_object;
GLint last_polygon_mode[2];
GLint last_viewport[4];
GLint last_scissor_box[4];
GLenum last_blend_src_rgb;
GLenum last_blend_dst_rgb;
GLenum last_blend_src_alpha;
GLenum last_blend_dst_alpha;
GLenum last_blend_equation_rgb;
GLenum last_blend_equation_alpha;
GLboolean last_enable_blend;
GLboolean last_enable_cull_face;
GLboolean last_enable_depth_test;
GLboolean last_enable_scissor_test;
void BeginDrawGUI()
{
	gglGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&last_active_texture);
	gglActiveTexture(GL_TEXTURE0);
	gglGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	gglGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
#ifdef GL_SAMPLER_BINDING
	gglGetIntegerv(GL_SAMPLER_BINDING, &last_sampler);
#endif
	gglGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	gglGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array_object);
#ifdef GL_POLYGON_MODE
	gglGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
#endif
	gglGetIntegerv(GL_VIEWPORT, last_viewport);
	gglGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
	gglGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
	gglGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
	gglGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
	gglGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
	gglGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
	gglGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
	last_enable_blend = gglIsEnabled(GL_BLEND);
	last_enable_cull_face = gglIsEnabled(GL_CULL_FACE);
	last_enable_depth_test = gglIsEnabled(GL_DEPTH_TEST);
	last_enable_scissor_test = gglIsEnabled(GL_SCISSOR_TEST);

	/*gglEnable(GL_BLEND);
	gglBlendEquation(GL_FUNC_ADD);
	gglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);*/
	gglDisable(GL_CULL_FACE);
	gglDisable(GL_DEPTH_TEST);
	gglEnable(GL_SCISSOR_TEST);
#ifdef GL_POLYGON_MODE
	gglPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

	gglUseProgram(g_openGL->m_gui_shader->m_program);
	gglUniformMatrix4fv(g_openGL->m_gui_shader->m_uniform_ProjMtx, 1, GL_FALSE, g_openGL->m_guiProjectionMatrix.getPtr() );
	
	g_openGL->m_isGUI = true;
}
void EndDrawGUI()
{
	g_openGL->m_isGUI = false;

	// Restore modified GL state
	gglUseProgram(last_program);
	gglBindTexture(GL_TEXTURE_2D, last_texture);
#ifdef GL_SAMPLER_BINDING
	gglBindSampler(0, last_sampler);
#endif
	gglActiveTexture(last_active_texture);
	gglBindVertexArray(last_vertex_array_object);
	gglBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	gglBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
	gglBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
	if (last_enable_blend) gglEnable(GL_BLEND); else gglDisable(GL_BLEND);
	if (last_enable_cull_face) gglEnable(GL_CULL_FACE); else gglDisable(GL_CULL_FACE);
	if (last_enable_depth_test) gglEnable(GL_DEPTH_TEST); else gglDisable(GL_DEPTH_TEST);
	if (last_enable_scissor_test) gglEnable(GL_SCISSOR_TEST); else gglDisable(GL_SCISSOR_TEST);
#ifdef GL_POLYGON_MODE
	gglPolygonMode(GL_FRONT_AND_BACK, (GLenum)last_polygon_mode[0]);
#endif
	gglViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
	gglScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}

void SetTexture(yyVideoDriverTextureSlot slot, yyResource* res)
{
	g_openGL->m_currentTextures[(u32)slot] = g_openGL->m_textures->m_cells[ res->m_index ].m_data;
}
void SetModel(yyResource* res)
{
	g_openGL->m_currentModel = g_openGL->m_models->m_cells[res->m_index].m_data;
}
void Draw()
{
	if( !g_openGL->m_currentModel )
		return;

	if(g_openGL->m_isGUI)
	{
		if(g_openGL->m_currentTextures[0])
		{
			gglActiveTexture(GL_TEXTURE0);
			gglBindTexture(GL_TEXTURE_2D,g_openGL->m_currentTextures[0]->m_texture);
		}

		for(u16 i = 0, sz = g_openGL->m_currentModel->m_meshBuffers.size(); i < sz; ++i)
		{
			auto meshBuffer = g_openGL->m_currentModel->m_meshBuffers[i];
			gglBindVertexArray(meshBuffer->m_VAO);
			gglDrawElements(GL_TRIANGLES, meshBuffer->m_iCount, GL_UNSIGNED_SHORT, 0);
		}
	}
	else
	{
	}
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

		g_api.BeginDrawGUI = BeginDrawGUI;
		g_api.EndDrawGUI = EndDrawGUI;
		g_api.SetTexture = SetTexture;
		g_api.SetModel = SetModel;
		
		g_api.Draw = Draw;

		return &g_api;
	}
}
