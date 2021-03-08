#include "yy.h"
#include "yy_window.h"
#include "yy_image.h"
#include "yy_gui.h"
#include "yy_model.h"

#include "OpenGL.h"
#include "OpenGL_texture.h"
#include "OpenGL_model.h"
#include "OpenGL_shader_GUI.h"
#include "OpenGL_shader_sprite.h"
#include "OpenGL_shader_sprite2.h"
#include "OpenGL_shader_Line3D.h"
#include "OpenGL_shader_standart.h"
#include "OpenGL_shader_terrain.h"
#include "OpenGL_shader_depth.h"
#include "OpenGL_shader_simple.h"
#include "OpenGL_shader_ScreenQuad.h"

#include "math/mat.h"

void OpenGL::UpdateGUIProjectionMatrix(const v2i& windowSize)
{
	gglViewport(0, 0, (GLsizei)windowSize.x, (GLsizei)windowSize.y);
	float L = 0;
	float R = (float)windowSize.x;
	float T = 0;
	float B = (float)windowSize.y;

	/*float ortho_projection[4][4] =
	{
		{ 2.0f/(R-L),   0.0f,         0.0f,   0.0f },
		{ 0.0f,         2.0f/(T-B),   0.0f,   0.0f },
		{ 0.0f,         0.0f,        -1.0f,   0.0f },
		{ (R+L)/(L-R),  (T+B)/(B-T),  0.0f,   1.0f },
	};*/
	m_guiProjectionMatrix.m_data[0] = v4f(2.0f/(R-L),   0.0f,         0.0f,   0.0f);
	m_guiProjectionMatrix.m_data[1] = v4f(0.0f,         2.0f/(T-B),   0.0f,   0.0f);
	m_guiProjectionMatrix.m_data[2] = v4f(0.0f,         0.0f,        -1.0f,   0.0f);
	m_guiProjectionMatrix.m_data[3] = v4f((R+L)/(L-R),  (T+B)/(B-T),  0.0f,   1.0f);

	//m_guiProjectionMatrix = Mat4();
}

#ifdef YY_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define WGL_NUMBER_PIXEL_FORMATS_ARB            0x2000
#define WGL_DRAW_TO_WINDOW_ARB                  0x2001
#define WGL_DRAW_TO_BITMAP_ARB                  0x2002
#define WGL_ACCELERATION_ARB                    0x2003
#define WGL_NEED_PALETTE_ARB                    0x2004
#define WGL_NEED_SYSTEM_PALETTE_ARB             0x2005
#define WGL_SWAP_LAYER_BUFFERS_ARB              0x2006
#define WGL_SWAP_METHOD_ARB                     0x2007
#define WGL_NUMBER_OVERLAYS_ARB                 0x2008
#define WGL_NUMBER_UNDERLAYS_ARB                0x2009
#define WGL_TRANSPARENT_ARB                     0x200A
#define WGL_TRANSPARENT_RED_VALUE_ARB           0x2037
#define WGL_TRANSPARENT_GREEN_VALUE_ARB         0x2038
#define WGL_TRANSPARENT_BLUE_VALUE_ARB          0x2039
#define WGL_TRANSPARENT_ALPHA_VALUE_ARB         0x203A
#define WGL_TRANSPARENT_INDEX_VALUE_ARB         0x203B
#define WGL_SHARE_DEPTH_ARB                     0x200C
#define WGL_SHARE_STENCIL_ARB                   0x200D
#define WGL_SHARE_ACCUM_ARB                     0x200E
#define WGL_SUPPORT_GDI_ARB                     0x200F
#define WGL_SUPPORT_OPENGL_ARB                  0x2010
#define WGL_DOUBLE_BUFFER_ARB                   0x2011
#define WGL_STEREO_ARB                          0x2012
#define WGL_PIXEL_TYPE_ARB                      0x2013
#define WGL_COLOR_BITS_ARB                      0x2014
#define WGL_RED_BITS_ARB                        0x2015
#define WGL_RED_SHIFT_ARB                       0x2016
#define WGL_GREEN_BITS_ARB                      0x2017
#define WGL_GREEN_SHIFT_ARB                     0x2018
#define WGL_BLUE_BITS_ARB                       0x2019
#define WGL_BLUE_SHIFT_ARB                      0x201A
#define WGL_ALPHA_BITS_ARB                      0x201B
#define WGL_ALPHA_SHIFT_ARB                     0x201C
#define WGL_ACCUM_BITS_ARB                      0x201D
#define WGL_ACCUM_RED_BITS_ARB                  0x201E
#define WGL_ACCUM_GREEN_BITS_ARB                0x201F
#define WGL_ACCUM_BLUE_BITS_ARB                 0x2020
#define WGL_ACCUM_ALPHA_BITS_ARB                0x2021
#define WGL_DEPTH_BITS_ARB                      0x2022
#define WGL_STENCIL_BITS_ARB                    0x2023
#define WGL_AUX_BUFFERS_ARB                     0x2024
#define WGL_NO_ACCELERATION_ARB                 0x2025
#define WGL_GENERIC_ACCELERATION_ARB            0x2026
#define WGL_FULL_ACCELERATION_ARB               0x2027
#define WGL_SWAP_EXCHANGE_ARB                   0x2028
#define WGL_SWAP_COPY_ARB                       0x2029
#define WGL_SWAP_UNDEFINED_ARB                  0x202A
#define WGL_TYPE_RGBA_ARB                       0x202B
#define WGL_TYPE_COLORINDEX_ARB                 0x202C
#define WGL_SAMPLE_BUFFERS_ARB               0x2041
#define WGL_SAMPLES_ARB                      0x2042
#define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB             0x2093
#define WGL_CONTEXT_FLAGS_ARB                   0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB            0x9126

wglChoosePixelFormatARB_t gwglChoosePixelFormatARB = nullptr;
wglCreateContextAttribsARB_t gwglCreateContextAttribsARB = nullptr;
wglSwapIntervalEXT_t gwglSwapIntervalEXT = nullptr;
wglCreateContext_t gwglCreateContext = nullptr;
wglMakeCurrent_t gwglMakeCurrent = nullptr;
wglGetProcAddress_t gwglGetProcAddress = nullptr;
wglDeleteContext_t gwglDeleteContext = nullptr;
#else
#error For Windows
#endif

OpenGL::OpenGL()
	:
	//m_window(nullptr),
	m_shader_gui(nullptr),
	m_shader_sprite(nullptr),
	m_shader_sprite2(nullptr),
	m_shader_line3d(nullptr),
	m_shader_std(nullptr),
	m_shader_terrain(nullptr),
	m_shader_depth(nullptr),
	m_shader_simple(nullptr),
	m_shader_simpleAnimated(nullptr),
	m_shader_screenQuad(nullptr),

	m_isGUI(false),
	m_spriteCameraScale(v2f(1.f,1.f))
{
#ifdef YY_PLATFORM_WINDOWS
	m_OpenGL_lib = nullptr;
	m_windowDC = nullptr;
	m_renderingContext = nullptr;
#endif
	for( u32 i = 0; i < yyVideoDriverMaxTextures; ++i)
	{
		m_currentTextures[i] = nullptr;
	}
	m_currentMaterial = nullptr;
	m_currentModel = nullptr;
	m_mainTarget = 0;
	m_mainTargetSurface = 0;
}

OpenGL::~OpenGL()
{
	yyLogWriteInfo("Destroy video driver...\n");

	if (m_mainTarget) yyDestroy(m_mainTarget);
	if (m_mainTargetSurface) yyDestroy(m_mainTargetSurface);

	
	if (m_shader_screenQuad) yyDestroy(m_shader_screenQuad);
	if (m_shader_gui) yyDestroy(m_shader_gui);
	if (m_shader_sprite) yyDestroy(m_shader_sprite);
	if (m_shader_sprite2) yyDestroy(m_shader_sprite2);
	if (m_shader_line3d) yyDestroy(m_shader_line3d);
	if (m_shader_std) yyDestroy(m_shader_std);
	if (m_shader_terrain) yyDestroy(m_shader_terrain);
	if (m_shader_depth) yyDestroy(m_shader_depth);
	if (m_shader_simple) yyDestroy(m_shader_simple);
	if (m_shader_simpleAnimated) yyDestroy(m_shader_simpleAnimated);

	for(size_t i = 0, sz = m_textures.size(); i < sz; ++i)
	{
		if( m_textures[i] )
			yyDestroy( m_textures[i] );
	}

	for(size_t i = 0, sz = m_models.size(); i < sz; ++i)
	{
		if( m_models[i] )
			yyDestroy( m_models[i] );
	}

#ifdef YY_PLATFORM_WINDOWS
	if(m_OpenGL_lib)
		FreeLibrary(m_OpenGL_lib);
#else
#error For Windows
#endif
}

void* OpenGL::get_proc(const char *proc)
{
	void * res = nullptr;

#ifdef YY_PLATFORM_WINDOWS
	res = gwglGetProcAddress(proc);
	if (!res)
		res = GetProcAddress(m_OpenGL_lib, proc);
#else
#error Need implementation
#endif
	return res;
}

void OpenGL::SetActive(yyWindow* window)
{
#ifdef YY_PLATFORM_WINDOWS
	m_windowDC = window->m_dc;
	gwglMakeCurrent(m_windowDC, m_renderingContext);
#else
#error Need implementation
#endif
}
void OpenGL::InitWindow(yyWindow* window)
{
#ifdef YY_PLATFORM_WINDOWS
	auto dc = window->m_dc;
	int attributeListInt[] =
	{
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB,     32,
		WGL_DEPTH_BITS_ARB,     24,
		WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
		WGL_SWAP_METHOD_ARB,    WGL_SWAP_EXCHANGE_ARB,
		WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
		WGL_STENCIL_BITS_ARB,   8,
		0
	};
	int pixelFormat[1];
	unsigned int formatCount;
	auto result = gwglChoosePixelFormatARB(dc, attributeListInt, NULL, 1, pixelFormat, &formatCount);
	if (result != 1)
	{
		YY_PRINT_FAILED;
		return;
	}

	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
	result = SetPixelFormat(dc, pixelFormat[0], &pixelFormatDescriptor);
	if (result != 1)
	{
		YY_PRINT_FAILED;
		return;
	}
#else
#error Need implementation
#endif
}
bool OpenGL::Init(yyWindow* window)
{
	yyLogWriteInfo("Init video driver - OpenGL...\n");
	//m_window = window;

	m_windowSize.x = window->m_currentSize.x;
	m_windowSize.y = window->m_currentSize.y;
	m_mainTargetSize.x = window->m_currentSize.x;
	m_mainTargetSize.y = window->m_currentSize.y;

#ifdef YY_PLATFORM_WINDOWS
	m_OpenGL_lib = LoadLibrary(L"OpenGL32.dll");
	if(!m_OpenGL_lib)
	{
		YY_PRINT_FAILED;
		return false;
	}

	// вызывается 2 раза. сначала берутся адреса из opengl32.dll

	gwglCreateContext = (wglCreateContext_t)GetProcAddress(m_OpenGL_lib, "wglCreateContext");
	gwglMakeCurrent   = (wglMakeCurrent_t)GetProcAddress(m_OpenGL_lib, "wglMakeCurrent");
	gwglGetProcAddress = (wglGetProcAddress_t)GetProcAddress(m_OpenGL_lib, "wglGetProcAddress");
	gwglDeleteContext = (wglDeleteContext_t)GetProcAddress(m_OpenGL_lib, "wglDeleteContext");
	load_procs();

	WNDCLASSEX wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc   = DefWindowProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = GetModuleHandle(0);
	wc.hIcon         = 0;
	wc.hIconSm       = 0;
	wc.hCursor       = 0;
	wc.hbrBackground = 0;
	wc.lpszMenuName  = NULL;
	wc.lpszClassName = L"OPENGL";
	wc.cbSize        = sizeof(WNDCLASSEX);
	RegisterClassEx(&wc);
	auto tmp_hwnd = CreateWindowEx(WS_EX_APPWINDOW, wc.lpszClassName, L"q", WS_POPUP,
				0, 0, 128, 128, NULL, NULL, wc.hInstance, NULL);
	if(tmp_hwnd == NULL)
	{
		YY_PRINT_FAILED;
		return false;
	}
	ShowWindow(tmp_hwnd, SW_HIDE);
	HDC dc = GetDC( tmp_hwnd );
	if(!dc)
	{
		YY_PRINT_FAILED;
		return false;
	}
	PIXELFORMATDESCRIPTOR pf;
	auto error = SetPixelFormat(dc, 1, &pf);
	if(error != 1)
	{
		YY_PRINT_FAILED;
		return false;
	}
	auto rc = gwglCreateContext(dc);
	if(!rc)
	{
		YY_PRINT_FAILED;
		return false;
	}
	error = gwglMakeCurrent(dc, rc);
	if(error != 1)
	{
		YY_PRINT_FAILED;
		return false;
	}

	gwglGetProcAddress = (wglGetProcAddress_t)GetProcAddress(m_OpenGL_lib, "wglGetProcAddress");
	load_procs();
	gwglChoosePixelFormatARB = (wglChoosePixelFormatARB_t)gwglGetProcAddress("wglChoosePixelFormatARB");
	gwglCreateContextAttribsARB = (wglCreateContextAttribsARB_t)gwglGetProcAddress("wglCreateContextAttribsARB");
	gwglSwapIntervalEXT = (wglSwapIntervalEXT_t)gwglGetProcAddress("wglSwapIntervalEXT");

	int maxSamples = 0;
	gglGetIntegerv( GL_MAX_SAMPLES, &maxSamples );
	gwglMakeCurrent(NULL, NULL);
	gwglDeleteContext(rc);
	ReleaseDC(tmp_hwnd, dc);
	DestroyWindow(tmp_hwnd);
	m_windowDC   = window->m_dc;
	if(!m_windowDC)
	{
		YY_PRINT_FAILED;
		return false;
	}
	InitWindow(window);
	/*int attributeListInt[] = 
	{
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_ACCELERATION_ARB,   WGL_FULL_ACCELERATION_ARB,
		WGL_COLOR_BITS_ARB,     32,
		WGL_DEPTH_BITS_ARB,     24,
		WGL_DOUBLE_BUFFER_ARB,  GL_TRUE,
		WGL_SWAP_METHOD_ARB,    WGL_SWAP_EXCHANGE_ARB,
		WGL_PIXEL_TYPE_ARB,     WGL_TYPE_RGBA_ARB,
		WGL_STENCIL_BITS_ARB,   8,
		0
	};*/

	/*int pixelFormat[1];
	unsigned int formatCount;
	auto result = gwglChoosePixelFormatARB(m_windowDC, attributeListInt, NULL, 1, pixelFormat, &formatCount);
	if(result != 1)
	{
		YY_PRINT_FAILED;
		return false;
	}

	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
	result = SetPixelFormat(m_windowDC, pixelFormat[0], &pixelFormatDescriptor);
	if(result != 1)
	{
		YY_PRINT_FAILED;
		return false;
	}*/

	int result = 0;
	int v_maj = 3;
	int v_min = 3;
	while(true)
	{
		int attributeList[5];
		attributeList[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
		attributeList[1] = v_maj;
		attributeList[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
		attributeList[3] = v_min;
		attributeList[4] = 0;// Null terminate the attribute list.
		
		m_renderingContext = gwglCreateContextAttribsARB(m_windowDC, 0, attributeList);
		if(m_renderingContext)
		{
			result = TRUE;
			break;
		}
		else
		{
			--v_min;

			if( v_min < 0 )
			{
				if( v_maj == 3 )
				{
					result = FALSE;
					break;
				}

				v_min = 6;
				--v_maj;
			}
		}
	}

	if( result == FALSE )
	{
		YY_PRINT_FAILED;
		return false;
	}
	yyLogWriteInfo("Init OpenGL %i.%i\n", v_maj, v_min);
	gwglMakeCurrent(m_windowDC, m_renderingContext);
	result = gwglSwapIntervalEXT(1);
	if(result != 1)
	{
		YY_PRINT_FAILED;
		return false;
	}

#else
#error Need implementation
#endif

	const GLubyte* vendor = gglGetString(GL_VENDOR);
	const GLubyte* renderer = gglGetString(GL_RENDERER);
	if(vendor)
		yyLogWriteInfo("Vendor: %s\n", vendor);
	if(renderer)
		yyLogWriteInfo("Renderer: %s\n", renderer);
	gglClearDepth(1.0f);
	gglEnable(GL_DEPTH_TEST);
	gglFrontFace(GL_CW);
	gglViewport(0, 0, window->m_currentSize.x, window->m_currentSize.y);

	UpdateGUIProjectionMatrix(window->m_currentSize);

	//yyImage whiteImage;
	//whiteImage.m_width  = 2;
	//whiteImage.m_height = 2;
	//whiteImage.m_pitch  = whiteImage.m_width * 4; // RGBA - 4 bytes
	//whiteImage.m_dataSize = whiteImage.m_pitch * whiteImage.m_height;
	//whiteImage.m_data   = new u8[whiteImage.m_dataSize];
	//whiteImage.fill(ColorWhite);

	//m_defaultTexture = this->CreateTexture(&whiteImage,false);
	//if(!m_defaultTexture)
	//{
	//	Game_LogWriteError("Can't create default texture...");
	//	return false;
	//}
	//m_defaultMaterial.m_textureLayer[0].m_texture = m_defaultTexture;

	gglEnable(GL_BLEND);
	gglBlendEquation(GL_FUNC_ADD);
	gglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//_createdefaultShaders();
	m_shader_gui = yyCreate<OpenGLShaderGUI>();
	if(!m_shader_gui->init())
	{
		yyLogWriteError("Can't create gui shader...");
		YY_PRINT_FAILED;
		return false;
	}

	m_shader_sprite = yyCreate<OpenGLShaderSprite>();
	if(!m_shader_sprite->init())
	{
		yyLogWriteError("Can't create sprite shader...");
		YY_PRINT_FAILED;
		return false;
	}

	m_shader_sprite2 = yyCreate<OpenGLShaderSprite2>();
	if (!m_shader_sprite2->init())
	{
		yyLogWriteError("Can't create sprite2 shader...");
		YY_PRINT_FAILED;
		return false;
	}

	m_shader_line3d = yyCreate<OpenGLShaderLine3D>();
	if(!m_shader_line3d->init())
	{
		yyLogWriteError("Can't create 3d line shader...");
		YY_PRINT_FAILED;
		return false;
	}

	m_shader_std = yyCreate<OpenGLShaderStd>();
	if(!m_shader_std->init())
	{
		yyLogWriteError("Can't create standart shader...");
		YY_PRINT_FAILED;
		return false;
	}

	m_shader_terrain = yyCreate<OpenGLShaderTerrain>();
	if (!m_shader_terrain->init())
	{
		yyLogWriteError("Can't create terrain shader...");
		YY_PRINT_FAILED;
		return false;
	}

	m_shader_depth = yyCreate<OpenGLShaderDepth>();
	if (!m_shader_depth->init())
	{
		yyLogWriteError("Can't create depth shader...");
		YY_PRINT_FAILED;
		return false;
	}

	m_shader_simple = yyCreate<OpenGLShaderSimple>();
	if (!m_shader_simple->init())
	{
		yyLogWriteError("Can't create simple shader...");
		YY_PRINT_FAILED;
		return false;
	}

	m_shader_simpleAnimated = yyCreate<OpenGLShaderSimpleAnimated>();
	if (!m_shader_simpleAnimated->init())
	{
		yyLogWriteError("Can't create simple animated shader...");
		YY_PRINT_FAILED;
		return false;
	}

	m_shader_screenQuad = yyCreate<OpenGLShaderScreenQuad>();
	if (!m_shader_screenQuad->init())
	{
		yyLogWriteError("Can't create screen quad shader...");
		YY_PRINT_FAILED;
		return false;
	}

	/*m_forwardPlus = Game_Create<RendererForwardPlus>();
	if(!m_forwardPlus->Init())
	{
		GAME_PRINT_FAILED;
		return false;
	}*/

	/*gglGenVertexArrays(1, &m_rectangle_VAO);

	int windowW = cr.z - cr.x;
	int windowH = cr.w - cr.y;
	OnWindowSizeChange(windowW, windowH);*/

	if (!updateMainTarget())
	{
		YY_PRINT_FAILED;
		return false;
	}

	return true;
}

bool OpenGL::updateMainTarget()
{
	if (m_mainTarget) yyDestroy(m_mainTarget);
	if (m_mainTargetSurface) yyDestroy(m_mainTargetSurface);

	m_mainTarget = yyCreate<OpenGLTexture>();
	if (!initFBO(m_mainTarget, m_mainTargetSize, false, false))
	{
		yyLogWriteError("Can't create main render target...");
		YY_PRINT_FAILED;
		return false;
	}

	auto model = yyCreate<yyModel>();
	model->m_iCount = 6;
	model->m_vCount = 4;
	model->m_stride = sizeof(yyVertexGUI);
	model->m_vertexType = yyVertexType::GUI;
	model->m_vertices = (u8*)yyMemAlloc(model->m_vCount * model->m_stride);
	model->m_indices = (u8*)yyMemAlloc(model->m_iCount * sizeof(u16));
	u16* inds = (u16*)model->m_indices;

	yyVertexGUI * vertex = (yyVertexGUI*)model->m_vertices;
	vertex->m_position.set(-1.f, 1.f);
	vertex->m_tcoords.set(0.f, 1.f);
	vertex++;
	vertex->m_position.set(-1.f, -1.f);
	vertex->m_tcoords.set(0.f, 0.f);
	vertex++;
	vertex->m_position.set(1.f, -1.f);
	vertex->m_tcoords.set(1.f, 0.f);
	vertex++;
	vertex->m_position.set(1.f, 1.f);
	vertex->m_tcoords.set(1.f, 1.f);
	vertex++;

	inds[0] = 0;
	inds[1] = 1;
	inds[2] = 2;
	inds[3] = 0;
	inds[4] = 2;
	inds[5] = 3;

	m_mainTargetSurface = yyCreate<OpenGLModel>();
	if (!initModel(model, m_mainTargetSurface))
	{
		yyDestroy(model);
		yyLogWriteError("Can't create main render target surface...");
		YY_PRINT_FAILED;
		return false;
	}
	yyDestroy(model);
	return true;
}

bool OpenGL::initFBO(OpenGLTexture* new_texture, const v2f& size, bool useLinearFilter, bool useComparisonFilter)
{
	new_texture->m_w = size.x;
	new_texture->m_h = size.y;
	glGenFramebuffers(1, &new_texture->m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, new_texture->m_FBO);

	glGenTextures(1, &new_texture->m_texture);
	glBindTexture(GL_TEXTURE_2D, new_texture->m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	if (useComparisonFilter)
	{
		gglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		gglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, useLinearFilter ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, useLinearFilter ? GL_LINEAR : GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffers(1, &new_texture->m_depthRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, new_texture->m_depthRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.x, size.y);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, new_texture->m_depthRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, new_texture->m_texture, 0);
	//gglFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, new_texture->m_depth, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}
bool OpenGL::initTexture(yyImage* image, OpenGLTexture* newTexture, bool useLinearFilter, bool useComparisonFilter)
{
	newTexture->m_h = image->m_height;
	newTexture->m_w = image->m_width;

	gglGenTextures(1, &newTexture->m_texture);
	gglBindTexture(GL_TEXTURE_2D, newTexture->m_texture);

	if( image->m_format == yyImageFormat::R8G8B8A8 )
	{
		gglTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->m_width, image->m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->m_data);
	}
	else
	{
		u32 format = 0;
		u32 blockSize = 16;

		switch(image->m_format)
		{
		case yyImageFormat::BC1:
			blockSize = 8;
			format = 0x83F1;
			break;
		case yyImageFormat::BC2:
			format = 0x83F2;
			break;
		case yyImageFormat::BC3:
			format = 0x83F3;
			break;
		default:
			break;
		}

		u32 offset = 0;

		auto width   = image->m_width;
		auto height  = image->m_width;
		/* load the mipmaps */
		for(u32 level = 0; level < image->m_mipCount && (width || height); ++level)
		{
			u32 size = ((width+3)/4)*((height+3)/4)*blockSize;
			glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 
				0, size, image->m_data + image->m_bitDataOffset + offset);

			offset += size;
			width  /= 2;
			height /= 2;
		}
	}
	glGenerateMipmap(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	if (useComparisonFilter)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, useLinearFilter ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, useLinearFilter ? GL_LINEAR : GL_NEAREST);
	return true;
}

bool OpenGL::initModel(yyModel* model, OpenGLModel* openglModel)
{
	openglModel->m_material = model->m_material;
	openglModel->m_vertexType = model->m_vertexType;

	glGenVertexArrays(1, &openglModel->m_VAO);
	glBindVertexArray(openglModel->m_VAO);
	glGenBuffers(1, &openglModel->m_vBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, openglModel->m_vBuffer);
	glBufferData(GL_ARRAY_BUFFER, model->m_vCount * model->m_stride, model->m_vertices, GL_DYNAMIC_DRAW);

	if (model->m_vertexType == yyVertexType::GUI)
	{
		// POSITION
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, false, model->m_stride, 0);

		// TexCoords
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (2 * sizeof(float)));
	}
	else if (model->m_vertexType == yyVertexType::Model)
	{
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, model->m_stride, 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (3 * sizeof(float)));

		// Normals
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (5 * sizeof(float)));
	
		// binormal
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (8 * sizeof(float)));

		// tangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (11 * sizeof(float)));
	}
	else if (model->m_vertexType == yyVertexType::AnimatedModel)
	{
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, model->m_stride, 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (5 * sizeof(float)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (8 * sizeof(float)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (11 * sizeof(float)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (14 * sizeof(float)));
		glEnableVertexAttribArray(6);
		glVertexAttribIPointer(6, 4, GL_INT, model->m_stride, (unsigned char*)NULL + (18 * sizeof(float)));
	}

	

	glGenBuffers(1, &openglModel->m_iBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, openglModel->m_iBuffer);

	u32 index_sizeof = sizeof(u16);
	openglModel->m_indexType = GL_UNSIGNED_SHORT;
	if (model->m_indexType == yyMeshIndexType::u32)
	{
		openglModel->m_indexType = GL_UNSIGNED_INT;
		index_sizeof = sizeof(u32);
	}

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model->m_iCount * index_sizeof, model->m_indices, GL_DYNAMIC_DRAW);

	openglModel->m_iCount = model->m_iCount;

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);

	return true;
}

