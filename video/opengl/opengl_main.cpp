#define YY_EXPORTS
#include "yy.h"
#include "yy_color.h"
#include "yy_image.h"
#include "yy_ptr.h"
#include "yy_model.h"

#include "OpenGL.h"
#include "OpenGL_texture.h"
#include "OpenGL_model.h"
#include "OpenGL_shader_GUI.h"
#include "OpenGL_shader_sprite.h"
#include "OpenGL_shader_Line3D.h"
#include "OpenGL_shader_standart.h"

#include "scene/common.h"
#include "scene/sprite.h"

yyVideoDriverAPI g_api;

void LoadModel(yyResource* r);

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
OpenGLModel* CreateOpenGLModel(yyModel* model)
{
	assert(model);
	auto newModel = yyCreate<OpenGLModel>();
	if(g_openGL->initModel(model, newModel))
		return newModel;
	yyDestroy(newModel);
	return nullptr;
}
void UnloadTexture(yyResource* r)
{
	assert(r);
#ifdef YY_DEBUG
	if(r->m_type != yyResourceType::Texture)
	{
		YY_PRINT_FAILED;
	}
#endif
	if(r->m_refCount == 0)
		return;

	--r->m_refCount;
	if(!r->m_refCount)
	{
		yyDestroy(g_openGL->m_textures[r->m_index]);
		g_openGL->m_textures[r->m_index] = nullptr;
	}
}
OpenGLTexture* CreateOpenGLTexture(yyImage* image, bool useLinearFilter)
{
	assert(image);
	auto newTexture = yyCreate<OpenGLTexture>();
	if(g_openGL->initTexture(image, newTexture, useLinearFilter))
		return newTexture;
	yyDestroy(newTexture);
	return nullptr;
}
void LoadTexture(yyResource* r)
{
	assert(r);
#ifdef YY_DEBUG
	if(r->m_type != yyResourceType::Texture)
	{
		YY_PRINT_FAILED;
	}
#endif
	++r->m_refCount;
	if(r->m_refCount == 1)
	{
		if(r->m_source)
		{
			g_openGL->m_textures[r->m_index] = CreateOpenGLTexture((yyImage*)r->m_source, 
				r->m_flags & yyResource::flags::texture_useLinearFilter );
			return;
		}

		if(r->m_file.size())
		{
			yyPtr<yyImage> image = yyLoadImage(r->m_file.c_str());
			g_openGL->m_textures[r->m_index] = CreateOpenGLTexture(image.m_data, 
				r->m_flags & yyResource::flags::texture_useLinearFilter );
			return;
		} 
	}
}
yyResource* CreateTextureFromFile(const char* fileName, bool useLinearFilter, bool load)
{
	assert(fileName);
	yyResource * newRes = yyCreate<yyResource>();
	newRes->m_type = yyResourceType::Texture;
	newRes->m_source = nullptr;
	newRes->m_index = g_openGL->m_textures.size();
	newRes->m_refCount = 0;
	if(useLinearFilter)
		newRes->m_flags |= yyResource::flags::texture_useLinearFilter;
	newRes->m_file = fileName;

	g_openGL->m_textures.push_back(nullptr);

	if(load)
		LoadTexture(newRes);
	return newRes;
}
yyResource* CreateTexture(yyImage* image, bool useLinearFilter)
{
	assert(image);
	yyResource * newRes = yyCreate<yyResource>();
	newRes->m_type = yyResourceType::Texture;
	newRes->m_source = image;
	newRes->m_index = g_openGL->m_textures.size();
	newRes->m_refCount = 1;
	if(useLinearFilter)
		newRes->m_flags |= yyResource::flags::texture_useLinearFilter;

	auto newTexture = CreateOpenGLTexture(image, useLinearFilter);
	if(newTexture)
	{
		g_openGL->m_textures.push_back(newTexture);
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
	v ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}


yyResource* CreateModelFromFile(const char* fileName, bool load)
{
	assert(fileName);
	yyResource * newRes = yyCreate<yyResource>();
	newRes->m_type = yyResourceType::Model;
	newRes->m_source = nullptr;
	newRes->m_index = g_openGL->m_models.size();
	newRes->m_refCount = 0;
	newRes->m_file = fileName;

	g_openGL->m_models.push_back(nullptr);

	if(load)
		LoadModel(newRes);
	return newRes;
}
yyResource* CreateModel(yyModel* model)
{
	assert(model);
	yyResource * newRes = yyCreate<yyResource>();
	newRes->m_type = yyResourceType::Model;
	newRes->m_index = g_openGL->m_models.size();
	newRes->m_source = model;
	newRes->m_refCount = 1;

	auto newModel = CreateOpenGLModel(model);
	if(newModel)
	{
		g_openGL->m_models.push_back(newModel);
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
	if(r->m_type != yyResourceType::Model)
	{
		YY_PRINT_FAILED;
	}
#endif
	if(r->m_refCount == 0)
		return;

	--r->m_refCount;
	if(!r->m_refCount)
	{
		yyDestroy(g_openGL->m_models[r->m_index]);
		g_openGL->m_models[r->m_index] = nullptr;
	}
}
void LoadModel(yyResource* r)
{
	assert(r);
#ifdef YY_DEBUG
	if(r->m_type != yyResourceType::Model)
	{
		YY_PRINT_FAILED;
	}
#endif
	++r->m_refCount;
	if(r->m_refCount == 1)
	{
		if(r->m_source)
		{
			g_openGL->m_models[r->m_index] = CreateOpenGLModel((yyModel*)r->m_source );
			return;
		}

		if(r->m_file.size())
		{
			yyPtr<yyModel> model = yyLoadModel(r->m_file.c_str());
			g_openGL->m_models[r->m_index] = CreateOpenGLModel(model.m_data);
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

void SetTexture(yyVideoDriverAPI::TextureSlot slot, yyResource* res)
{
	g_openGL->m_currentTextures[(u32)slot] = g_openGL->m_textures[ res->m_index ];
}
void SetModel(yyResource* res)
{
	g_openGL->m_currentModel = g_openGL->m_models[res->m_index];
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
	}
	else
	{
		glUseProgram( g_openGL->m_shader_std->m_program );
		glUniformMatrix4fv(g_openGL->m_shader_std->m_uniform_WVP, 1, GL_FALSE, g_openGL->m_matrixWorldViewProjection.getPtr() );
		if(g_openGL->m_currentTextures[0])
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,g_openGL->m_currentTextures[0]->m_texture);
		}
	}

	for(u16 i = 0, sz = g_openGL->m_currentModel->m_meshBuffers.size(); i < sz; ++i)
	{
		auto meshBuffer = g_openGL->m_currentModel->m_meshBuffers[i];
		gglBindVertexArray(meshBuffer->m_VAO);
		gglDrawElements(GL_TRIANGLES, meshBuffer->m_iCount, GL_UNSIGNED_SHORT, 0);
	}
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
		gglActiveTexture(GL_TEXTURE0);
		gglBindTexture(GL_TEXTURE_2D,g_openGL->m_currentTextures[0]->m_texture);
	}
	SetModel(sprite->m_model);
	auto meshBuffer = g_openGL->m_currentModel->m_meshBuffers[0];
	gglBindVertexArray(meshBuffer->m_VAO);
	gglDrawElements(GL_TRIANGLES, meshBuffer->m_iCount, GL_UNSIGNED_SHORT, 0);
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
	default:
		YY_PRINT_FAILED;
		break;
	}
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

extern "C"
{
	YY_API yyVideoDriverAPI* YY_C_DECL GetAPI()
	{
		g_api.GetAPIVersion = GetAPIVersion;
		g_api.Init          = Init;
		g_api.Destroy       = Destroy;
	
		g_api.SetClearColor = SetClearColor;
		//g_api.BeginDraw		= BeginDraw;
		g_api.BeginDrawClearAll	= BeginDrawClearAll;
		g_api.BeginDrawClearColor	= BeginDrawClearColor;
		g_api.BeginDrawClearDepth	= BeginDrawClearDepth;
		g_api.BeginDrawNotClear	= BeginDrawNotClear;
		g_api.EndDraw		= EndDraw;

		g_api.CreateTexture = CreateTexture;
		g_api.CreateTextureFromFile = CreateTextureFromFile;
		g_api.UnloadTexture = UnloadTexture;
		g_api.LoadTexture = LoadTexture;

		g_api.UseVSync = UseVSync;
		g_api.UseDepth = UseDepth;

		g_api.CreateModel = CreateModel;
		g_api.CreateModelFromFile = CreateModelFromFile;
		g_api.LoadModel = LoadModel;
		g_api.UnloadModel = UnloadModel;

		g_api.BeginDrawGUI = BeginDrawGUI;
		g_api.EndDrawGUI = EndDrawGUI;
		g_api.SetTexture = SetTexture;
		g_api.SetModel = SetModel;
		
		g_api.Draw = Draw;
		g_api.DrawSprite = DrawSprite;
		g_api.DrawLine3D = DrawLine3D;
		g_api.SetMatrix = SetMatrix;
		g_api.GetSpriteCameraPosition = GetSpriteCameraPosition;
		g_api.GetSpriteCameraScale = GetSpriteCameraScale;

		g_api.GetTextureSize = GetTextureSize;

		return &g_api;
	}
}
