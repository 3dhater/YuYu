#define YY_EXPORTS
#include "yy.h"
#include "yy_color.h"
#include "yy_window.h"
#include "yy_image.h"
#include "yy_material.h"
#include "yy_ptr.h"
#include "yy_model.h"
#include "yy_gui.h"

#include "OpenGL.h"
#include "OpenGL_texture.h"
#include "OpenGL_model.h"
#include "OpenGL_shader_GUI.h"
#include "OpenGL_shader_sprite.h"
#include "OpenGL_shader_Line3D.h"
#include "OpenGL_shader_standart.h"
#include "OpenGL_shader_terrain.h"
#include "OpenGL_shader_depth.h"
#include "OpenGL_shader_simple.h"
#include "OpenGL_shader_ScreenQuad.h"

#include "scene/common.h"
#include "scene/sprite.h"

yyVideoDriverAPI g_api;
bool g_useDepth = true;
void LoadModel(yyResource* r);

#ifdef YY_DEBUG
bool g_drawBegin = false;
#endif

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
	assert(window);
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

void SetClearColor(f32 r, f32 g, f32 b, f32 a)
{
	gglClearColor( r, g, b, a );
}


OpenGLModel* CreateOpenGLModel(yyModel* model)
{
	assert(model);
	auto newModel = yyCreate<OpenGLModel>();
	if(g_openGL->initModel(model, newModel))
		return newModel;
	yyDestroy(newModel);
	return nullptr;
}
void DeleteTexture(yyResource* r)
{
	yyDestroy(g_openGL->m_textures[r->m_index]);
	g_openGL->m_textures[r->m_index] = nullptr;
	g_openGL->m_freeTextureResourceIndex.push_back(r->m_index);
	yyDestroy(r);
}
void UnloadTexture(yyResource* r)
{
	assert(r);
	assert((r->m_type == yyResourceType::Texture) || (r->m_type == yyResourceType::RenderTargetTexture));
	if(r->m_refCount == 0)
		return;

	--r->m_refCount;
	if(!r->m_refCount)
	{
		r->m_isLoaded = false;
		if (g_openGL->m_textures[r->m_index])
		{
			yyDestroy(g_openGL->m_textures[r->m_index]);
			g_openGL->m_textures[r->m_index] = nullptr;
		}
	}
}
OpenGLTexture* CreateOpenGLTexture(yyImage* image, bool useLinearFilter, bool useComparisonFilter)
{
	assert(image);
	auto newTexture = yyCreate<OpenGLTexture>();
	if(g_openGL->initTexture(image, newTexture, useLinearFilter, useComparisonFilter))
		return newTexture;
	yyDestroy(newTexture);
	return nullptr;
}
void LoadTexture(yyResource* r)
{
	assert(r);
	assert((r->m_type == yyResourceType::Texture)|| (r->m_type == yyResourceType::RenderTargetTexture));
	++r->m_refCount;
	if(r->m_refCount==1)
	{
		r->m_isLoaded = true;

		// возможно g_openGL->m_textures[r->m_index] может быть занят, по этому лучше добавить проверку
		if(r->m_source)
		{
			g_openGL->m_textures[r->m_index] = CreateOpenGLTexture((yyImage*)r->m_source, 
				(r->m_flags & yyResource::flags::texture_useLinearFilter) == 1,
				(r->m_flags & yyResource::flags::texture_useComparisonFilter) == 1);
			return;
		}

		if(r->m_file.size())
		{
			yyPtr<yyImage> image = yyLoadImage(r->m_file.c_str());
			g_openGL->m_textures[r->m_index] = CreateOpenGLTexture(image.m_data, 
				(r->m_flags & yyResource::flags::texture_useLinearFilter) == 1,
				(r->m_flags & yyResource::flags::texture_useComparisonFilter) == 1);
			return;
		} 
	}
}
yyResource* CreateTextureFromFile(const char* fileName, bool useLinearFilter, bool useComparisonFilter, bool load)
{
	assert(fileName);
	yyResource * newRes = yyCreate<yyResource>();
	newRes->m_type = yyResourceType::Texture;
	newRes->m_source = nullptr;
	newRes->m_refCount = 0;
	if (useLinearFilter)
		newRes->m_flags |= yyResource::flags::texture_useLinearFilter;
	if (useComparisonFilter)
		newRes->m_flags |= yyResource::flags::texture_useComparisonFilter;
	newRes->m_file = fileName;

	if (g_openGL->m_freeTextureResourceIndex.head())
	{
		newRes->m_index = g_openGL->m_freeTextureResourceIndex.head()->m_data;
		g_openGL->m_freeTextureResourceIndex.erase_node(g_openGL->m_freeTextureResourceIndex.head());
	}
	else
	{
		newRes->m_index = g_openGL->m_textures.size();
		g_openGL->m_textures.push_back(nullptr);
	}


	if(load)
		LoadTexture(newRes);
	return newRes;
}
yyResource* CreateTexture(yyImage* image, bool useLinearFilter, bool useComparisonFilter)
{
	assert(image);
	yyResource * newRes = yyCreate<yyResource>();
	newRes->m_type = yyResourceType::Texture;
	newRes->m_source = image;
	newRes->m_refCount = 1;
	if(useLinearFilter)
		newRes->m_flags |= yyResource::flags::texture_useLinearFilter;
	if (useComparisonFilter)
		newRes->m_flags |= yyResource::flags::texture_useComparisonFilter;
	
	bool isNewIndex = false;
	if (g_openGL->m_freeTextureResourceIndex.head())
	{
		newRes->m_index = g_openGL->m_freeTextureResourceIndex.head()->m_data;
		g_openGL->m_freeTextureResourceIndex.erase_node(g_openGL->m_freeTextureResourceIndex.head());
	}
	else
	{
		newRes->m_index = g_openGL->m_textures.size();
		isNewIndex = true;
	}


	auto newTexture = CreateOpenGLTexture(image, useLinearFilter, useComparisonFilter);
	if(newTexture)
	{
		if (isNewIndex)
			g_openGL->m_textures.push_back(newTexture);
		else
			g_openGL->m_textures[newRes->m_index] = newTexture;

		newRes->m_isLoaded = true;
		return newRes;
	}

	if(newRes)
		yyDestroy( newRes );
	return nullptr;
}

void UseVSync(bool v)
{
#ifdef YY_PLATFORM_WINDOWS
	gwglSwapIntervalEXT(v ? 1 : 0);
#else
#error For Windows
#endif
}
void UseDepth(bool v)
{
	if (g_useDepth == v)
		return;
	g_useDepth = v;
	v ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}
void UseBlend(bool v)
{
	if (v)
	{
		glEnable(GL_BLEND);
		glBlendEquation(GL_FUNC_ADD);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
	{
		glDisable(GL_BLEND);
	}
}

yyResource* CreateRenderTargetTexture(const v2f& size, bool useLinearFilter, bool useComparisonFilter)
{
	yyResource * newRes = yyCreate<yyResource>();
	newRes->m_type = yyResourceType::RenderTargetTexture;
	newRes->m_source = 0;
	newRes->m_index = g_openGL->m_textures.size();
	newRes->m_refCount = 1;
	if (useLinearFilter)
		newRes->m_flags |= yyResource::flags::texture_useLinearFilter;
	if (useComparisonFilter)
		newRes->m_flags |= yyResource::flags::texture_useComparisonFilter;

	auto newTexture = yyCreate<OpenGLTexture>();
	g_openGL->initFBO(newTexture, size, useLinearFilter, useComparisonFilter);
	if (newTexture)
	{
		g_openGL->m_textures.push_back(newTexture);
		newRes->m_isLoaded = true;
		return newRes;
	}

	if (newRes)
		yyDestroy(newRes);
	return nullptr;
}

yyResource* CreateModel(yyModel* model)
{
	assert(model);
	yyResource * newRes = yyCreate<yyResource>();
	newRes->m_type = yyResourceType::Model;
	newRes->m_source = model;
	newRes->m_refCount = 1;
	newRes->m_aabb = model->m_aabb;

	bool isNewIndex = false;
	if (g_openGL->m_freeModelResourceIndex.head())
	{
		newRes->m_index = g_openGL->m_freeModelResourceIndex.head()->m_data;
		g_openGL->m_freeModelResourceIndex.erase_node(g_openGL->m_freeModelResourceIndex.head());
	}
	else
	{
		newRes->m_index = g_openGL->m_models.size();
		isNewIndex = true;
	}

	auto newModel = CreateOpenGLModel(model);
	if(newModel)
	{
		if (isNewIndex)
			g_openGL->m_models.push_back(newModel);
		else
			g_openGL->m_models[newRes->m_index] = newModel;

		newRes->m_isLoaded = true;
		return newRes;
	}

	if(newRes)
		yyDestroy( newRes );
	return nullptr;
}
void UnloadModel(yyResource* r)
{
	assert(r);
#ifdef YY_DEBUG
	if(r->m_type != yyResourceType::Model) YY_PRINT_FAILED;
#endif
	if(r->m_refCount == 0)
		return;

	--r->m_refCount;
	if(!r->m_refCount)
	{
		r->m_isLoaded = false;
		if (g_openGL->m_models[r->m_index])
		{
			yyDestroy(g_openGL->m_models[r->m_index]);
			g_openGL->m_models[r->m_index] = nullptr;
			// ресурс создаётся один на модель и на всегда
			// для полного удаления вместе с ресурсом нужна другая функция - DeleteModel
		}
	}
}
void DeleteModel(yyResource* r)
{
	assert(r);
	assert(r->m_type == yyResourceType::Model);

	// нужно ли использовать это в таком случае? m_refCount 
	// если нужно удалить, то надо удалить, иначе надо вызывать UnloadModel
	//if (r->m_refCount == 0) return;
	/*--r->m_refCount;
	if (!r->m_refCount)
	{
		r->m_isLoaded = false;
		if (g_openGL->m_models[r->m_index])
		{
			yyDestroy(g_openGL->m_models[r->m_index]);
			g_openGL->m_models[r->m_index] = nullptr;
		}
	}*/
	yyDestroy(g_openGL->m_models[r->m_index]);
	g_openGL->m_models[r->m_index] = nullptr;
	// чтобы не переполнять g_openGL->m_models при создании новых ресурсов,
	// лучше вставлять их в свободные ячейки.
	// чтобы не перебирать их, наверно, лучше иметь список свободных ячеек
	// соответственно, при создании ресурса, перед указанием индекса, нужно проверить этот список
	g_openGL->m_freeModelResourceIndex.push_back(r->m_index);
	yyDestroy(r); // удаление ресурса
}
void LoadModel(yyResource* r)
{
	assert(r);
#ifdef YY_DEBUG
	if(r->m_type != yyResourceType::Model)		YY_PRINT_FAILED;
#endif
	++r->m_refCount;
	if (r->m_refCount == 1)
	{
		r->m_isLoaded = true;
		if(r->m_source)
		{
			g_openGL->m_models[r->m_index] = CreateOpenGLModel((yyModel*)r->m_source );
			return;
		}
	}
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

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
#ifdef GL_POLYGON_MODE
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

	glUseProgram(g_openGL->m_shader_gui->m_program);
	glUniformMatrix4fv(g_openGL->m_shader_gui->m_uniform_ProjMtx, 1, GL_FALSE, g_openGL->m_guiProjectionMatrix.getPtr() );
	
	g_openGL->m_isGUI = true;
}
void EndDrawGUI()
{
	g_openGL->m_isGUI = false;

	// Restore modified GL state
	glUseProgram(last_program);
	glBindTexture(GL_TEXTURE_2D, last_texture);
#ifdef GL_SAMPLER_BINDING
	glBindSampler(0, last_sampler);
#endif
	glActiveTexture(last_active_texture);
	glBindVertexArray(last_vertex_array_object);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
	glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
	if (last_enable_blend) gglEnable(GL_BLEND); else gglDisable(GL_BLEND);
	if (last_enable_cull_face) gglEnable(GL_CULL_FACE); else gglDisable(GL_CULL_FACE);
	if (last_enable_depth_test) gglEnable(GL_DEPTH_TEST); else gglDisable(GL_DEPTH_TEST);
	if (last_enable_scissor_test) gglEnable(GL_SCISSOR_TEST); else gglDisable(GL_SCISSOR_TEST);
#ifdef GL_POLYGON_MODE
	glPolygonMode(GL_FRONT_AND_BACK, (GLenum)last_polygon_mode[0]);
#endif
	glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
	glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}
void SetRenderTarget(yyResource* rtt)
{
	if (rtt)
	{
		if (rtt->m_type == yyResourceType::RenderTargetTexture)
			glBindFramebuffer(GL_FRAMEBUFFER, g_openGL->m_textures[rtt->m_index]->m_FBO);
		else
			yyLogWriteWarning("SetRenderTarget: yyResourceType is not RenderTargetTexture");
	}
	else
	{
		glBindFramebuffer(GL_FRAMEBUFFER, g_openGL->m_mainTarget->m_FBO);
	}
}
void SetViewport(f32 x, f32 y, f32 width, f32 height)
{
	glViewport(x, y, width, height);
}
void SetTexture(yyVideoDriverAPI::TextureSlot slot, yyResource* res)
{
	if(res)
		g_openGL->m_currentTextures[(u32)slot] = g_openGL->m_textures[res->m_index];
	else
		g_openGL->m_currentTextures[(u32)slot] = nullptr;
}
void SetModel(yyResource* res)
{
	if (res)
		g_openGL->m_currentModel = g_openGL->m_models[res->m_index];
	else
		g_openGL->m_currentModel = nullptr;
}
void Draw()
{
	if( !g_openGL->m_currentModel )
		return;

	if(g_openGL->m_isGUI)
	{
		if(g_openGL->m_currentTextures[0])
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,g_openGL->m_currentTextures[0]->m_texture);
		}
	}
	else
	{
		auto material = g_openGL->m_currentMaterial;
		if (!material)
			material = &g_openGL->m_currentModel->m_material;

		if (material->m_wireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if (material->m_cullBackFace)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);


		switch (material->m_type)
		{
		case yyMaterialType::Terrain:
			glUseProgram(g_openGL->m_shader_terrain->m_program);
			glUniformMatrix4fv(g_openGL->m_shader_terrain->m_uniform_WVP, 1, GL_FALSE, g_openGL->m_matrixWorldViewProjection.getPtr());
			glUniformMatrix4fv(g_openGL->m_shader_terrain->m_uniform_W, 1, GL_FALSE, g_openGL->m_matrixWorld.getPtr());
			glUniformMatrix4fv(g_openGL->m_shader_terrain->m_uniform_LightView, 1, GL_FALSE, g_openGL->m_matrixLightView.getPtr());
			glUniformMatrix4fv(g_openGL->m_shader_terrain->m_uniform_LightProjection, 1, GL_FALSE, g_openGL->m_matrixLightProjection.getPtr());
			glUniform3fv(g_openGL->m_shader_terrain->m_uniform_sunDir, 1, material->m_sunDir.data());
			glUniform3fv(g_openGL->m_shader_terrain->m_uniform_ambientColor, 1, material->m_ambientColor.data());
			glUniform1f(g_openGL->m_shader_terrain->m_uniform_selfLight, material->m_selfLight);
			if (g_openGL->m_currentTextures[0]) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, g_openGL->m_currentTextures[0]->m_texture);
			}
			if (g_openGL->m_currentTextures[1]) {
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, g_openGL->m_currentTextures[1]->m_texture);
			}
			break;
		case yyMaterialType::Simple:
		{
			switch (g_openGL->m_currentModel->m_vertexType)
			{
			case yyVertexType::Model:
			{
				glUseProgram(g_openGL->m_shader_simple->m_program);
				glUniformMatrix4fv(g_openGL->m_shader_simple->m_uniform_WVP, 1, GL_FALSE, g_openGL->m_matrixWorldViewProjection.getPtr());
				glUniform4fv(g_openGL->m_shader_simple->m_uniform_BaseColor, 1, &material->m_baseColor.m_data[0]);
			}break;
			case yyVertexType::AnimatedModel:
			{
				glUseProgram(g_openGL->m_shader_simpleAnimated->m_program);
				glUniformMatrix4fv(g_openGL->m_shader_simpleAnimated->m_uniform_WVP, 1, GL_FALSE, g_openGL->m_matrixWorldViewProjection.getPtr());
				glUniformMatrix4fv(g_openGL->m_shader_simpleAnimated->m_uniform_World, 1, GL_FALSE, g_openGL->m_matrixWorld.getPtr());
				glUniformMatrix4fv(g_openGL->m_shader_simpleAnimated->m_uniform_Bones, 255, GL_FALSE, g_openGL->m_matrixBones[0].getPtr());
				glUniform4fv(g_openGL->m_shader_simpleAnimated->m_uniform_BaseColor, 1, &material->m_baseColor.m_data[0]);
			}break;
			}
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);
			if (g_openGL->m_currentTextures[0]) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, g_openGL->m_currentTextures[0]->m_texture);
			}
		}break;
		case yyMaterialType::Default:
			glUseProgram(g_openGL->m_shader_std->m_program);
			glUniformMatrix4fv(g_openGL->m_shader_std->m_uniform_WVP, 1, GL_FALSE, g_openGL->m_matrixWorldViewProjection.getPtr());
			glUniformMatrix4fv(g_openGL->m_shader_std->m_uniform_W, 1, GL_FALSE, g_openGL->m_matrixWorld.getPtr());
			glUniformMatrix4fv(g_openGL->m_shader_std->m_uniform_LightView, 1, GL_FALSE, g_openGL->m_matrixLightView.getPtr());
			glUniformMatrix4fv(g_openGL->m_shader_std->m_uniform_LightProjection, 1, GL_FALSE, g_openGL->m_matrixLightProjection.getPtr());
			glUniform3fv(g_openGL->m_shader_std->m_uniform_sunDir, 1, material->m_sunDir.data());
			glUniform3fv(g_openGL->m_shader_std->m_uniform_ambientColor, 1, material->m_ambientColor.data());
			glUniform1f(g_openGL->m_shader_std->m_uniform_selfLight, material->m_selfLight);
			if (g_openGL->m_currentTextures[0]) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, g_openGL->m_currentTextures[0]->m_texture);
			}
			if (g_openGL->m_currentTextures[1]) {
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, g_openGL->m_currentTextures[1]->m_texture);
			}
			break;
		case yyMaterialType::Depth:
			glUseProgram(g_openGL->m_shader_depth->m_program);
			glUniformMatrix4fv(g_openGL->m_shader_depth->m_uniform_World, 1, GL_FALSE, g_openGL->m_matrixWorld.getPtr());
			glUniformMatrix4fv(g_openGL->m_shader_depth->m_uniform_LightView, 1, GL_FALSE, g_openGL->m_matrixLightView.getPtr());
			glUniformMatrix4fv(g_openGL->m_shader_depth->m_uniform_LightProjection, 1, GL_FALSE, g_openGL->m_matrixLightProjection.getPtr());
			if (g_openGL->m_currentTextures[0]) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, g_openGL->m_currentTextures[0]->m_texture);
			}
			break;
		default:
			glUseProgram(g_openGL->m_shader_std->m_program);
			glUniformMatrix4fv(g_openGL->m_shader_std->m_uniform_WVP, 1, GL_FALSE, g_openGL->m_matrixWorldViewProjection.getPtr());
			glUniformMatrix4fv(g_openGL->m_shader_std->m_uniform_W, 1, GL_FALSE, g_openGL->m_matrixWorld.getPtr());
			glUniformMatrix4fv(g_openGL->m_shader_std->m_uniform_LightView, 1, GL_FALSE, g_openGL->m_matrixLightView.getPtr());
			glUniformMatrix4fv(g_openGL->m_shader_std->m_uniform_LightProjection, 1, GL_FALSE, g_openGL->m_matrixLightProjection.getPtr());
			glUniform3fv(g_openGL->m_shader_std->m_uniform_sunDir, 1, material->m_sunDir.data());
			glUniform3fv(g_openGL->m_shader_std->m_uniform_ambientColor, 1, material->m_ambientColor.data());
			glUniform1f(g_openGL->m_shader_std->m_uniform_selfLight, material->m_selfLight);
			if (g_openGL->m_currentTextures[0]) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, g_openGL->m_currentTextures[0]->m_texture);
			}
			if (g_openGL->m_currentTextures[1]) {
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, g_openGL->m_currentTextures[1]->m_texture);
			}
			break;
		}

	}
	glBindVertexArray(g_openGL->m_currentModel->m_VAO);
	glDrawElements(GL_TRIANGLES, g_openGL->m_currentModel->m_iCount, g_openGL->m_currentModel->m_indexType, 0);
}
void DrawSprite(yySprite* sprite)
{
	assert(sprite);
	glUseProgram( g_openGL->m_shader_sprite->m_program );
	glUniformMatrix4fv(g_openGL->m_shader_sprite->m_uniform_ProjMtx, 1, GL_FALSE, g_openGL->m_guiProjectionMatrix.getPtr() );
	glUniformMatrix4fv(g_openGL->m_shader_sprite->m_uniform_WorldMtx, 1, GL_FALSE, sprite->m_objectBase.m_globalMatrix.getPtr() );
	glUniform2fv(g_openGL->m_shader_sprite->m_uniform_CameraPosition, 1, &g_openGL->m_spriteCameraPosition.x);
	glUniform2fv(g_openGL->m_shader_sprite->m_uniform_CameraScale, 1, &g_openGL->m_spriteCameraScale.x);
	glUniform2fv(g_openGL->m_shader_sprite->m_uniform_uv1, 1, &sprite->m_tcoords_1.x);
	glUniform2fv(g_openGL->m_shader_sprite->m_uniform_uv2, 1, &sprite->m_tcoords_2.x);

	int flags = 0;
	if(sprite->m_currentState)
	{
		if(sprite->m_currentState->m_invertX)
			flags |= 1;
		if(sprite->m_currentState->m_invertY)
			flags |= 2;
	}
	
	glUniform1i(g_openGL->m_shader_sprite->m_uniform_flags, flags);

	if(sprite->m_texture)
	{
		SetTexture(yyVideoDriverAPI::TextureSlot::Texture0, sprite->m_texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,g_openGL->m_currentTextures[0]->m_texture);
	}
	SetModel(sprite->m_model);
	glBindVertexArray(g_openGL->m_currentModel->m_VAO);
	glDrawElements(GL_TRIANGLES, g_openGL->m_currentModel->m_iCount, GL_UNSIGNED_SHORT, 0);
}
void DrawLine2D(const v3f& _p1, const v3f& _p2, const yyColor& color)
{
	v4f p1 = _p1;
	v4f p2 = _p2;
	glUseProgram(g_openGL->m_shader_line3d->m_program);
	glUniformMatrix4fv(g_openGL->m_shader_line3d->m_uniform_ProjMtx, 1, GL_FALSE, g_openGL->m_guiProjectionMatrix.getPtr());
	glUniform4fv(g_openGL->m_shader_line3d->m_uniform_P1, 1, &p1.x);
	glUniform4fv(g_openGL->m_shader_line3d->m_uniform_P2, 1, &p2.x);
	glUniform4fv(g_openGL->m_shader_line3d->m_uniform_Color, 1, color.data());

	glBindVertexArray(g_openGL->m_shader_line3d->m_VAO);
	glDrawArrays(GL_LINES, 0, 2);
}
void DrawLine3D(const v4f& _p1, const v4f& _p2, const yyColor& color)
{
	glUseProgram( g_openGL->m_shader_line3d->m_program );
	glUniformMatrix4fv(g_openGL->m_shader_line3d->m_uniform_ProjMtx, 1, GL_FALSE, g_openGL->m_matrixViewProjection.getPtr() );
	glUniform4fv(g_openGL->m_shader_line3d->m_uniform_P1, 1, _p1.cdata());
	glUniform4fv(g_openGL->m_shader_line3d->m_uniform_P2, 1, _p2.cdata());
	glUniform4fv(g_openGL->m_shader_line3d->m_uniform_Color, 1, color.data());

	glBindVertexArray(g_openGL->m_shader_line3d->m_VAO);
	glDrawArrays(GL_LINES, 0, 2);
}
void SetMatrix(yyVideoDriverAPI::MatrixType mt, const Mat4& mat)
{
	switch(mt)
	{
	case yyVideoDriverAPI::World:
		g_openGL->m_matrixWorld = mat;
		break;
	case yyVideoDriverAPI::View:
		g_openGL->m_matrixView = mat;
		break;
	case yyVideoDriverAPI::Projection:
		g_openGL->m_matrixProjection = mat;
		break;
	case yyVideoDriverAPI::ViewProjection:
		g_openGL->m_matrixViewProjection = mat;
		break;
	case yyVideoDriverAPI::WorldViewProjection:
		g_openGL->m_matrixWorldViewProjection = mat;
		break;
	case yyVideoDriverAPI::LightView:
		g_openGL->m_matrixLightView = mat;
		break;
	case yyVideoDriverAPI::LightProjection:
		g_openGL->m_matrixLightProjection = mat;
		break;
	default:
		YY_PRINT_FAILED;
		break;
	}
}
void SetBoneMatrix(u32 boneIndex, const Mat4& mat)
{
	g_openGL->m_matrixBones[boneIndex] = mat;
}
v2f* GetSpriteCameraPosition()
{
	return &g_openGL->m_spriteCameraPosition;
}
v2f* GetSpriteCameraScale()
{
	return &g_openGL->m_spriteCameraScale;
}

void GetTextureSize(yyResource* r, v2i* s)
{
	assert(r);
	assert(s);
	if(r->m_type != yyResourceType::Texture)
		return;
	OpenGLTexture* t = g_openGL->m_textures[ r->m_index ];
	if(!t)
		return;
	s->x = t->m_w;
	s->y = t->m_h;
}

void SetActiveWindow(yyWindow* w)
{
	g_openGL->SetActive(w);
}
void InitWindow(yyWindow* w)
{
	g_openGL->InitWindow(w);
}
void SetMaterial(yyMaterial* mat)
{
	g_openGL->m_currentMaterial = mat;
}
void MapModelForWriteVerts(yyResource* r, u8** v_ptr)
{
	assert(r);
	OpenGLModel* m = g_openGL->m_models[r->m_index];
	glBindBuffer(GL_ARRAY_BUFFER, m->m_vBuffer);
	*v_ptr = (u8*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
}
void UnmapModelForWriteVerts(yyResource* r)
{
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

yyVideoDriverObjectOpenGL g_yyVideoDriverObjectOpenGL;
void* GetVideoDriverObjects()
{
	return &g_yyVideoDriverObjectOpenGL;
}

const char* GetVideoDriverName()
{
	return "OpenGL 3.3";
}

void BeginDraw()
{
#ifdef YY_DEBUG
	if(g_drawBegin)
		yyLogWriteError("You forgot to call SwapBuffers()  Video driver: %s\n", GetVideoDriverName());
	assert(!g_drawBegin);
	g_drawBegin = true;
#endif
	glBindFramebuffer(GL_FRAMEBUFFER, g_openGL->m_mainTarget->m_FBO);
	glViewport(0, 0, g_openGL->m_mainTargetSize.x, g_openGL->m_mainTargetSize.y);
	UseDepth(true);
	
}
void ClearAll()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}
void ClearColor()
{
	glClear(GL_COLOR_BUFFER_BIT);
}
void ClearDepth()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}
void EndDraw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0); // window
	glViewport(0, 0, g_openGL->m_windowSize.x, g_openGL->m_windowSize.y);

	UseDepth(false);

	glUseProgram(g_openGL->m_shader_screenQuad->m_program);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_openGL->m_mainTarget->m_texture);
	glBindVertexArray(g_openGL->m_mainTargetSurface->m_VAO);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
	glDrawElements(GL_TRIANGLES, g_openGL->m_mainTargetSurface->m_iCount, g_openGL->m_mainTargetSurface->m_indexType, 0);
}

void SwapBuffers()
{
#ifdef YY_DEBUG
	g_drawBegin = false;
#endif
#ifdef YY_PLATFORM_WINDOWS
	SwapBuffers(g_openGL->m_windowDC);
#else
#error For Windows
#endif
}

void UpdateMainRenderTarget(const v2i& windowsSize, const v2f& bufferSize)
{
	g_openGL->m_windowSize = windowsSize;
	g_openGL->m_mainTargetSize = bufferSize;
	g_openGL->updateMainTarget();
}

void* GetTextureHandle(yyResource* res)
{
	return &g_openGL->m_textures[res->m_index]->m_texture;
}

void SetGUIShaderData(yyGUIElement* guielement)
{
	assert(guielement);
	glUniform2fv(g_openGL->m_shader_gui->m_uniform_Offset, 1, &guielement->m_offset.x);
	glUniform4fv(g_openGL->m_shader_gui->m_uniform_Color, 1, &guielement->m_color.m_data[0]);
}

extern "C"
{
	YY_API yyVideoDriverAPI* YY_C_DECL GetAPI()
	{
		g_api.BeginDraw = BeginDraw;
		g_api.BeginDrawGUI = BeginDrawGUI;
		g_api.ClearAll = ClearAll;
		g_api.ClearColor = ClearColor;
		g_api.ClearDepth = ClearDepth;
		g_api.CreateModel = CreateModel;
		g_api.CreateRenderTargetTexture = CreateRenderTargetTexture;
		g_api.CreateTexture = CreateTexture;
		g_api.CreateTextureFromFile = CreateTextureFromFile;
		g_api.DeleteModel = DeleteModel;
		g_api.DeleteTexture = DeleteTexture;
		g_api.Destroy = Destroy;
		g_api.Draw = Draw;
		g_api.DrawLine2D = DrawLine2D;
		g_api.DrawLine3D = DrawLine3D;
		g_api.DrawSprite = DrawSprite;
		g_api.EndDraw = EndDraw;
		g_api.EndDrawGUI = EndDrawGUI;
		g_api.GetAPIVersion = GetAPIVersion;
		g_api.GetSpriteCameraPosition = GetSpriteCameraPosition;
		g_api.GetSpriteCameraScale = GetSpriteCameraScale;
		g_api.GetTextureHandle = GetTextureHandle;
		g_api.GetTextureSize = GetTextureSize;
		g_api.GetVideoDriverName = GetVideoDriverName;
		g_api.GetVideoDriverObjects = GetVideoDriverObjects;
		g_api.Init = Init;
		g_api.LoadModel = LoadModel;
		g_api.LoadTexture = LoadTexture;
		g_api.MapModelForWriteVerts = MapModelForWriteVerts;
		g_api.SetBoneMatrix = SetBoneMatrix;
		g_api.SetClearColor = SetClearColor;
		g_api.SetGUIShaderData = SetGUIShaderData;
		g_api.SetMaterial = SetMaterial;
		g_api.SetMatrix = SetMatrix;
		g_api.SetModel = SetModel;
		g_api.SetRenderTarget = SetRenderTarget;
		g_api.SetTexture = SetTexture;
		g_api.SetViewport = SetViewport;
		g_api.SwapBuffers = SwapBuffers;
		g_api.UnloadModel = UnloadModel;
		g_api.UnloadTexture = UnloadTexture;
		g_api.UnmapModelForWriteVerts = UnmapModelForWriteVerts;
		g_api.UpdateMainRenderTarget = UpdateMainRenderTarget;
		g_api.UseBlend = UseBlend;
		g_api.UseDepth = UseDepth;
		g_api.UseVSync = UseVSync;

		return &g_api;
	}
}
