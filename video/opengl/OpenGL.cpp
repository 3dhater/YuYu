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
#include "OpenGL_shader_Line3D.h"

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
{
	for( u32 i = 0; i < (u32)yyVideoDriverTextureSlot::Count; ++i)
	{
		m_currentTextures[i] = nullptr;
	}
}

OpenGL::~OpenGL()
{
	yyLogWriteInfo("Destroy video driver...\n");

	if(m_shader_gui) yyDestroy(m_shader_gui);
	if(m_shader_sprite) yyDestroy(m_shader_sprite);
	if(m_shader_line3d) yyDestroy(m_shader_line3d);

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
#error For Windows
#endif
	return res;
}

bool OpenGL::Init(yyWindow* window)
{
	yyLogWriteInfo("Init video driver - OpenGL...\n");
	m_window = window;

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
	m_windowDC   = m_window->m_dc;
	if(!m_windowDC)
	{
		YY_PRINT_FAILED;
		return false;
	}

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
	}

	int v_maj = 3;
	int v_min = 2;
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
#error For Windows
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
	gglViewport(0, 0, m_window->m_size.x, m_window->m_size.y);

	UpdateGUIProjectionMatrix(m_window->m_clientSize);

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

	m_shader_line3d = yyCreate<OpenGLShaderLine3D>();
	if(!m_shader_line3d->init())
	{
		yyLogWriteError("Can't create 3d line shader...");
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

	return true;
}

bool OpenGL::initTexture(yyImage* image, OpenGLTexture* newTexture, bool useLinearFilter)
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
			gglCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 
				0, size, image->m_data + offset);

			offset += size;
			width  /= 2;
			height /= 2;
		}
	}
	gglGenerateMipmap(GL_TEXTURE_2D);
	gglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	gglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	gglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, useLinearFilter ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
	gglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, useLinearFilter ? GL_LINEAR : GL_NEAREST);
	return true;
}

bool OpenGL::initModel(yyModel* model, OpenGLModel* openglModel)
{
	for(u16 i = 0, sz = model->m_meshBuffers.size(); i < sz; ++i)
	{
		auto meshBuffer = model->m_meshBuffers.m_data[i];

		OpenGLMeshBuffer* openGLMEshBuffer = yyCreate<OpenGLMeshBuffer>();

		gglGenVertexArrays(1, &openGLMEshBuffer->m_VAO);
		gglBindVertexArray(openGLMEshBuffer->m_VAO);
		gglGenBuffers(1, &openGLMEshBuffer->m_vBuffer);
		gglBindBuffer(GL_ARRAY_BUFFER, openGLMEshBuffer->m_vBuffer);
		gglBufferData(GL_ARRAY_BUFFER, meshBuffer->m_vCount * meshBuffer->m_stride, meshBuffer->m_vertices, GL_DYNAMIC_DRAW);

		// POSITION
		gglEnableVertexAttribArray(0);
		gglVertexAttribPointer(0, 3, GL_FLOAT, false, meshBuffer->m_stride, 0); 

		// TexCoords
		gglEnableVertexAttribArray(1);
		gglBindBuffer(GL_ARRAY_BUFFER, openGLMEshBuffer->m_vBuffer);
		gglVertexAttribPointer(1, 2, GL_FLOAT, false, meshBuffer->m_stride, (unsigned char*)NULL + (3 * sizeof(float)));
	
		//// Normals
		//gglEnableVertexAttribArray(2);
		//gglBindBuffer(GL_ARRAY_BUFFER, m_vBuffer);
		//gglVertexAttribPointer(2, 3, GL_FLOAT, false, smesh->m_stride, (unsigned char*)NULL + (5 * sizeof(float)));
	
		//// binormal
		//gglEnableVertexAttribArray(3);
		//gglBindBuffer(GL_ARRAY_BUFFER, m_vBuffer);
		//gglVertexAttribPointer(3, 3, GL_FLOAT, false, smesh->m_stride, (unsigned char*)NULL + (8 * sizeof(float)));

		//// tangent
		//gglEnableVertexAttribArray(4);
		//gglBindBuffer(GL_ARRAY_BUFFER, m_vBuffer);
		//gglVertexAttribPointer(4, 3, GL_FLOAT, false, smesh->m_stride, (unsigned char*)NULL + (11 * sizeof(float)));

		gglGenBuffers(1, &openGLMEshBuffer->m_iBuffer);
		gglBindBuffer(GL_ELEMENT_ARRAY_BUFFER, openGLMEshBuffer->m_iBuffer);
		gglBufferData(GL_ELEMENT_ARRAY_BUFFER, meshBuffer->m_iCount * sizeof(u16), meshBuffer->m_indices, GL_DYNAMIC_DRAW);

		openGLMEshBuffer->m_iCount = meshBuffer->m_iCount;

		openglModel->m_meshBuffers.push_back(openGLMEshBuffer);

		gglBindVertexArray(0);
		gglBindBuffer(GL_ARRAY_BUFFER,0);
		gglBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	}

	return true;
}

//void Game_GSOpenGL::OnWindowSizeChange(s32 newSizeX, s32 newSizeY)
//{
//	gglViewport(0, 0, (GLsizei)newSizeX, (GLsizei)newSizeY);
//}
//
//bool Game_GSOpenGL::_createdefaultShaders()
//{
//	m_shader_gui = Game_Create<Game_ShaderOpenGL_GUI>();
//	if( !m_shader_gui->Init() )
//	{
//		Game_Destroy(m_shader_gui);
//		m_shader_gui = 0;
//		GAME_PRINT_FAILED;
//		return false;
//	}
//
//	m_shader_line3d = Game_Create<Game_ShaderOpenGL_Line3D>();
//	if( !m_shader_line3d->Init() )
//	{
//		Game_Destroy(m_shader_line3d);
//		m_shader_line3d = 0;
//		GAME_PRINT_FAILED;
//		return false;
//	}
//
//	m_shader_modelSimple = Game_Create<Game_ShaderOpenGL_ModelSimple>();
//	if( !m_shader_modelSimple->Init() )
//	{
//		Game_Destroy(m_shader_modelSimple);
//		m_shader_modelSimple = 0;
//		GAME_PRINT_FAILED;
//		return false;
//	}
//
//	m_shader_modelStandart = Game_Create<Game_ShaderOpenGL_ModelStandart>();
//	if( !m_shader_modelStandart->Init() )
//	{
//		Game_Destroy(m_shader_modelStandart);
//		m_shader_modelStandart = 0;
//		GAME_PRINT_FAILED;
//		return false;
//	}
//
//	m_shader_outputRect = Game_Create<Game_ShaderOpenGL_OutputRectShader>();
//	if( !m_shader_outputRect->Init() )
//	{
//		Game_Destroy(m_shader_outputRect);
//		m_shader_outputRect = 0;
//		GAME_PRINT_FAILED;
//		return false;
//	}
//
//	return true;
//}
//
//void Game_GSOpenGL::SetClearColor(f32 r, f32 g, f32 b, f32 a)
//{
//	m_clearColor_r = r;
//	m_clearColor_g = g;
//	m_clearColor_b = b;
//	m_clearColor_a = a;
//	gglClearColor( r, g, b, a ); 
//}
//
//void Game_GSOpenGL::BeginDraw(bool clear)
//{
//	if( clear )
//	{
//		GLbitfield mask = GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT;
//		gglClear(mask);
//	}
//}
//void Game_GSOpenGL::DrawScreenQuad(Game_Texture* t)
//{
//	//if(!t)
//	//	t = m_defaultTexture;
//	//m_shader_outputRect->SetActive();
//
//	/*unsigned long long * t_ptr = (unsigned long long *)t->GetHandle();
//	auto tid = (GLuint)*t_ptr;
//	gglBindTexture(GL_TEXTURE_2D, tid);
//	gglActiveTexture(GL_TEXTURE0);*/
//	gglBindVertexArray(m_rectangle_VAO);
//	gglDrawArrays(GL_TRIANGLES, 0, 6);
//}
//void Game_GSOpenGL::EndDraw()
//{
//	gglBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//#ifdef GAME_PLATFORM_WINDOWS
//	SwapBuffers( m_windowDC );
//#else
//#error For Windows
//#endif
//}
//
//void Game_GSOpenGL::SetDepth(bool v)
//{
//	if( v )
//	{
//		glEnable(GL_DEPTH_TEST);
//		glDepthMask(GL_TRUE);
//	}
//	else
//	{
//		glDisable(GL_DEPTH_TEST);
//	}
//}
//
//void Game_GSOpenGL::SetBackFaceCulling(bool v)
//{
//	if( v )
//	{
//		glEnable(GL_CULL_FACE);
//		glCullFace(GL_BACK);
//	}
//	else
//	{
//		glDisable(GL_CULL_FACE);
//	}
//}
//
//void Game_GSOpenGL::SetViewport( s32 x, s32 y, s32 w, s32 h )
//{
//	gglViewport(x, y, w, h);
//}
//
//void Game_GSOpenGL::SetCompFunc(Game_GSCompFunc f)
//{
//	switch (f)
//	{
//	case Game_GSCompFunc::Never:
//		gglDepthFunc(GL_NEVER);
//		break;
//	case Game_GSCompFunc::Less:
//		gglDepthFunc(GL_LESS);
//		break;
//	case Game_GSCompFunc::Equal:
//		gglDepthFunc(GL_EQUAL);
//		break;
//	case Game_GSCompFunc::Lequal:
//		gglDepthFunc(GL_LEQUAL);
//		break;
//	case Game_GSCompFunc::Greater:
//		gglDepthFunc(GL_GREATER);
//		break;
//	case Game_GSCompFunc::Notequal:
//		gglDepthFunc(GL_NOTEQUAL);
//		break;
//	case Game_GSCompFunc::Gequal:
//		gglDepthFunc(GL_GEQUAL);
//		break;
//	case Game_GSCompFunc::Always:
//		gglDepthFunc(GL_ALWAYS);
//		break;
//	default:
//		break;
//	}
//}
//
//void Game_GSOpenGL::UseScissor( bool v )
//{
//	if(v)
//		gglEnable(GL_SCISSOR_TEST);
//	else
//		gglDisable(GL_SCISSOR_TEST);
//}
//
//void Game_GSOpenGL::SetScissor( int x1, int y1, int x2, int y2 )
//{
//	gglScissor( x1, y1, x2, y2 );
//}
//
//Game_Texture* Game_GSOpenGL::CreateTexture(Game_Image* image, bool useFilter)
//{
//	Game_Texture* new_texture = _create_texture(image, useFilter);
//	if(!new_texture)
//	{
//		Game_LogWriteWarning("Can't create texture...");
//	}
//	return new_texture;
//}
//
//Game_Texture* Game_GSOpenGL::_create_texture(Game_Image* image, bool linear)
//{
//	Game_Texture_OpenGL* new_texture = Game_Create<Game_Texture_OpenGL>();
//	new_texture->m_h = image->m_height;
//	new_texture->m_w = image->m_width;
//
//	gglGenTextures(1, &new_texture->m_texture);
//	gglBindTexture(GL_TEXTURE_2D, new_texture->m_texture);
//
//	if( image->m_format == Game_ImageFormat::R8G8B8A8 )
//	{
//		gglTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->m_width, image->m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->m_data);
//	}
//	else
//	{
//		u32 format = 0;
//		u32 blockSize = 16;
//
//		switch(image->m_format)
//		{
//		case Game_ImageFormat::BC1:
//			blockSize = 8;
//			format = 0x83F1;
//			break;
//		case Game_ImageFormat::BC2:
//			format = 0x83F2;
//			break;
//		case Game_ImageFormat::BC3:
//			format = 0x83F3;
//			break;
//		default:
//			break;
//		}
//
//		u32 offset = 0;
//
//		auto width   = image->m_width;
//		auto height  = image->m_width;
//		/* load the mipmaps */
//		for(u32 level = 0; level < image->m_mipCount && (width || height); ++level)
//		{
//			u32 size = ((width+3)/4)*((height+3)/4)*blockSize;
//			gglCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height, 
//				0, size, image->m_data + offset);
//
//			offset += size;
//			width  /= 2;
//			height /= 2;
//		}
//	}
//	gglGenerateMipmap(GL_TEXTURE_2D);
//	gglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	gglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	gglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, linear ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST);
//	gglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, linear ? GL_LINEAR : GL_NEAREST);
//	return new_texture;
//}
//
//Game_Texture* Game_GSOpenGL::_create_FBO(const v2i& rttSize, bool linear)
//{
//	Game_Texture_OpenGL* new_texture = Game_Create<Game_Texture_OpenGL>();
//	new_texture->m_w = rttSize.x;
//	new_texture->m_h = rttSize.y;
//	gglGenFramebuffers(1, &new_texture->m_FBO);
//	gglBindFramebuffer(GL_FRAMEBUFFER, new_texture->m_FBO);
//	
//	gglGenTextures(1, &new_texture->m_texture);
//	gglBindTexture(GL_TEXTURE_2D, new_texture->m_texture);
//	gglTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rttSize.x, rttSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
//	gglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, linear ? GL_LINEAR : GL_NEAREST );
//	gglTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, linear ? GL_LINEAR : GL_NEAREST );
//	gglBindTexture(GL_TEXTURE_2D, 0);
//
//	gglGenRenderbuffers(1, &new_texture->m_depthRBO);
//	gglBindRenderbuffer(GL_RENDERBUFFER, new_texture->m_depthRBO);
//	gglRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, rttSize.x, rttSize.y);
//	gglFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, new_texture->m_depthRBO);
//	gglBindRenderbuffer(GL_RENDERBUFFER, 0);
//
//	gglFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, new_texture->m_texture, 0);
//	//gglFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, new_texture->m_depth, 0);
//	gglBindFramebuffer(GL_FRAMEBUFFER, 0);
//	return new_texture;
//}
//
//void Game_GSOpenGL::SetTarget( Game_Texture* t)
//{
//	if(t)
//	{
//		Game_Texture_OpenGL* oglt = (Game_Texture_OpenGL*)t;
//		gglBindFramebuffer(GL_FRAMEBUFFER, oglt->m_FBO);
//	}
//	else
//	{
//		gglBindFramebuffer(GL_FRAMEBUFFER, 0);
//	}
//}
//
//Game_Texture* Game_GSOpenGL::CreateRTT(const v2i& rttSize, bool useFilter)
//{
//	Game_Texture* new_texture = _create_FBO(rttSize, useFilter);
//	if(!new_texture)
//	{
//		Game_LogWriteWarning("Can't create FBO...");
//	}
//	return new_texture;
//}
//
//Game_Texture* Game_GSOpenGL::GetDefaultTexture()
//{
//	return m_defaultTexture;
//}
//
//void Game_GSOpenGL::DrawGUICommand(Game_GUIDrawCommand* cmd)
//{
//	auto clsz = m_window->GetClientSize();
//
//	if(!cmd->texture)
//		return;
//
//	gglScissor((GLint)cmd->clipRect.x, (GLint)(clsz.y - cmd->clipRect.w), (GLsizei)(cmd->clipRect.z - cmd->clipRect.x), (GLsizei)(cmd->clipRect.w - cmd->clipRect.y));
//	gglBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd->verts.size() * sizeof(Game_GUIVertex), (const GLvoid*)&cmd->verts.data()[0], GL_STREAM_DRAW);
//	gglBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd->inds.size() * sizeof(unsigned short), (const GLvoid*)&cmd->inds.data()[0], GL_STREAM_DRAW);
//	unsigned long long * t_ptr = (unsigned long long *)cmd->texture->GetHandle();
//	if( t_ptr )
//	{
//		auto tid = (GLuint)*t_ptr;
//		gglBindTexture(GL_TEXTURE_2D, tid);
//	}
//	gglDrawElements(GL_TRIANGLES, (GLsizei)cmd->inds.size(), GL_UNSIGNED_SHORT, 0 );
//}
//
//GLenum last_active_texture;
//GLint last_program;
//GLint last_texture;
//GLint last_sampler;
//GLint last_array_buffer;
//GLint last_vertex_array_object;
//GLint last_polygon_mode[2];
//GLint last_viewport[4];
//GLint last_scissor_box[4];
//GLenum last_blend_src_rgb;
//GLenum last_blend_dst_rgb;
//GLenum last_blend_src_alpha;
//GLenum last_blend_dst_alpha;
//GLenum last_blend_equation_rgb;
//GLenum last_blend_equation_alpha;
//GLboolean last_enable_blend;
//GLboolean last_enable_cull_face;
//GLboolean last_enable_depth_test;
//GLboolean last_enable_scissor_test;
//GLuint vertex_array_object = 0;
//void Game_GSOpenGL::BeginDrawGUICommand()
//{
//	gglGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&last_active_texture);
//	gglActiveTexture(GL_TEXTURE0);
//	gglGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
//	gglGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
//#ifdef GL_SAMPLER_BINDING
//	gglGetIntegerv(GL_SAMPLER_BINDING, &last_sampler);
//#endif
//	gglGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
//	gglGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array_object);
//#ifdef GL_POLYGON_MODE
//	gglGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
//#endif
//	gglGetIntegerv(GL_VIEWPORT, last_viewport);
//	gglGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
//	gglGetIntegerv(GL_BLEND_SRC_RGB, (GLint*)&last_blend_src_rgb);
//	gglGetIntegerv(GL_BLEND_DST_RGB, (GLint*)&last_blend_dst_rgb);
//	gglGetIntegerv(GL_BLEND_SRC_ALPHA, (GLint*)&last_blend_src_alpha);
//	gglGetIntegerv(GL_BLEND_DST_ALPHA, (GLint*)&last_blend_dst_alpha);
//	gglGetIntegerv(GL_BLEND_EQUATION_RGB, (GLint*)&last_blend_equation_rgb);
//	gglGetIntegerv(GL_BLEND_EQUATION_ALPHA, (GLint*)&last_blend_equation_alpha);
//	last_enable_blend = gglIsEnabled(GL_BLEND);
//	last_enable_cull_face = gglIsEnabled(GL_CULL_FACE);
//	last_enable_depth_test = gglIsEnabled(GL_DEPTH_TEST);
//	last_enable_scissor_test = gglIsEnabled(GL_SCISSOR_TEST);
//
//	gglEnable(GL_BLEND);
//	gglBlendEquation(GL_FUNC_ADD);
//	gglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	gglDisable(GL_CULL_FACE);
//	gglDisable(GL_DEPTH_TEST);
//	gglEnable(GL_SCISSOR_TEST);
//#ifdef GL_POLYGON_MODE
//	gglPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//#endif
//
//	auto clsz = m_window->GetClientSize();
//	gglViewport(0, 0, (GLsizei)clsz.x, (GLsizei)clsz.y);
//	float L = 0;
//	float R = (float)clsz.x;
//	float T = 0;
//	float B = (float)clsz.y;
//
//	float ortho_projection[4][4] =
//	{
//		{ 2.0f/(R-L),   0.0f,         0.0f,   0.0f },
//		{ 0.0f,         2.0f/(T-B),   0.0f,   0.0f },
//		{ 0.0f,         0.0f,        -1.0f,   0.0f },
//		{ (R+L)/(L-R),  (T+B)/(B-T),  0.0f,   1.0f },
//	};
//
//	gglUseProgram(m_shader_gui->m_program);
//	gglUniform1i(m_shader_gui->m_uniforms[0].m_first, 0);
//	gglUniformMatrix4fv(m_shader_gui->m_uniforms[1].m_first, 1, GL_FALSE, &ortho_projection[0][0]);
//#ifdef GL_SAMPLER_BINDING
//	gglBindSampler(0, 0);
//#endif
//	gglGenVertexArrays(1, &vertex_array_object);
//	gglBindVertexArray(vertex_array_object);
//
//	gglBindBuffer(GL_ARRAY_BUFFER, m_shader_gui->m_VboHandle);
//	gglBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_shader_gui->m_ElementsHandle);
//	gglEnableVertexAttribArray(m_shader_gui->m_attrs[0].m_first);
//	gglEnableVertexAttribArray(m_shader_gui->m_attrs[1].m_first);
//	gglEnableVertexAttribArray(m_shader_gui->m_attrs[2].m_first);
//	gglVertexAttribPointer(m_shader_gui->m_attrs[0].m_first, 2, GL_FLOAT, GL_FALSE, sizeof(Game_GUIVertex), 0);
//	gglVertexAttribPointer(m_shader_gui->m_attrs[1].m_first, 2, GL_FLOAT, GL_FALSE, sizeof(Game_GUIVertex), (unsigned char*)NULL + (2 * sizeof(float)));
//	gglVertexAttribPointer(m_shader_gui->m_attrs[2].m_first, 4, GL_FLOAT, GL_FALSE, sizeof(Game_GUIVertex), (unsigned char*)NULL + (4 * sizeof(float)));
//}
//
//void Game_GSOpenGL::EndDrawGUICommand()
//{
//	gglDeleteVertexArrays(1, &vertex_array_object);
//
//	// ==============
//	// Restore modified GL state
//	gglUseProgram(last_program);
//	gglBindTexture(GL_TEXTURE_2D, last_texture);
//#ifdef GL_SAMPLER_BINDING
//	gglBindSampler(0, last_sampler);
//#endif
//	gglActiveTexture(last_active_texture);
//	gglBindVertexArray(last_vertex_array_object);
//	gglBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
//	gglBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
//	gglBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
//	if (last_enable_blend) gglEnable(GL_BLEND); else gglDisable(GL_BLEND);
//	if (last_enable_cull_face) gglEnable(GL_CULL_FACE); else gglDisable(GL_CULL_FACE);
//	if (last_enable_depth_test) gglEnable(GL_DEPTH_TEST); else gglDisable(GL_DEPTH_TEST);
//	if (last_enable_scissor_test) gglEnable(GL_SCISSOR_TEST); else gglDisable(GL_SCISSOR_TEST);
//#ifdef GL_POLYGON_MODE
//	gglPolygonMode(GL_FRONT_AND_BACK, (GLenum)last_polygon_mode[0]);
//#endif
//	gglViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
//	gglScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
//}
//
//void Game_GSOpenGL::DrawLine3D(const v4f& _p1, const v4f& _p2, const Game_Color& color)
//{
//	auto active_camera = Game_GetActiveCamera();
//	if(!active_camera)
//		return;
//
//	m_shader_line3d->SetActive();
//
//	auto P = active_camera->GetProjection();
//	auto V = active_camera->GetView();
//
//	auto WVP = P * V * Mat4();
//
//	auto p1 = _p1;
//	auto p2 = _p2;
//	gglUniformMatrix4fv(m_shader_line3d->GetUniform(0), 1, GL_FALSE, WVP.getPtr() );
//	gglUniform4fv(m_shader_line3d->GetUniform(1), 1, p1.data() );
//	gglUniform4fv(m_shader_line3d->GetUniform(2), 1, p2.data() );
//	gglUniform4fv(m_shader_line3d->GetUniform(3), 1, color.data() );
//
//	gglBindVertexArray(m_shader_line3d->m_VAO);
//	gglDrawArrays(GL_LINES, 0, 2);
//}
//
//Game_Model* Game_GSOpenGL::CreateModel(Game_Meshbuffer* soft)
//{
//	auto hard = Game_Create<Game_Model_OpenGL>();
//	hard->m_meshbuffer = soft;
//	hard->Init(soft);
//	hard->m_aabb = soft->GetAabb();
//	//return Game_Create<Game_Model_OpenGL>();
//	return hard;
//}
//void Game_GSOpenGL::DrawModel(Game_Model* model, const Mat4& world, Game_Material* material)
//{
//	if(!model)
//		return;
//	auto gl_model = (Game_Model_OpenGL*)model;
//
//	if(!material)
//		material = &m_defaultMaterial;
//
//	switch (material->m_type)
//	{
//	case Game_Material::EMT_MINIMUM:
//	{
//		m_shader_modelSimple->SetActive();
//		m_shader_modelSimple->OnShader(world, material);
//	}break;
//	case Game_Material::EMT_STANDART:
//	{
//		m_shader_modelStandart->SetActive();
//		m_shader_modelStandart->OnShader(world, material);
//	}break;
//	default:
//		GAME_PRINT_FAILED;
//		break;
//	}
//	
//	// ТАКЖЕ нужно следить за рендерингом GBuffer !!!
//	for(u32 i = 0, sz = gl_model->m_mesh.size(); i < sz; ++i)
//	{
//		auto gl_hMesh = (Game_HMesh_OpenGL*)gl_model->m_mesh.at(i);
//		gglBindVertexArray(gl_hMesh->m_VAO);
//		gglDrawElements(GL_TRIANGLES, gl_hMesh->m_iCount, GL_UNSIGNED_SHORT, 0);
//	}
//
//	if(material->m_drawAabb)
//		DrawAabb(model->GetAabb(), world, ColorWhite);
//}
//
//void Game_GSOpenGL::DrawAabb(const Aabb& aabb, const Mat4& world, const Game_Color& color)
//{
//	auto W = world;
//	auto position = world[3];
//	W[3] = v4f(0.f,0.f,0.f,1.f);
//
//	auto & p1 = aabb.m_min;
//	auto & p2 = aabb.m_max;
//
//	v4f v1 = p1;
//	v4f v2 = p2;
//	
//				
//	v4f v3( p1.x, p1.y, p2.z, 1.f );
//	v4f v4( p2.x, p1.y, p1.z, 1.f );
//	v4f v5( p1.x, p2.y, p1.z, 1.f );
//	v4f v6( p1.x, p2.y, p2.z, 1.f );
//	v4f v7( p2.x, p1.y, p2.z, 1.f );
//	v4f v8( p2.x, p2.y, p1.z, 1.f );
//
//	v1 = math::mul(v1, W) + position;
//	v2 = math::mul(v2, W) + position;
//	v3 = math::mul(v3, W) + position;
//	v4 = math::mul(v4, W) + position;
//	v5 = math::mul(v5, W) + position;
//	v6 = math::mul(v6, W) + position;
//	v7 = math::mul(v7, W) + position;
//	v8 = math::mul(v8, W) + position;
//				
//	DrawLine3D( v1, v4, color );
//	DrawLine3D( v5, v8, color );
//	DrawLine3D( v1, v5, color );
//	DrawLine3D( v4, v8, color );
//	DrawLine3D( v3, v7, color );
//	DrawLine3D( v6, v2, color );
//	DrawLine3D( v3, v6, color );
//	DrawLine3D( v7, v2, color );
//	DrawLine3D( v2, v8, color );
//	DrawLine3D( v4, v7, color );
//	DrawLine3D( v5, v6, color );
//	DrawLine3D( v1, v3, color );
//}