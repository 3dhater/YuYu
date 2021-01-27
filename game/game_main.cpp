#include "yy.h"
#include "yy_window.h"

#include "yy_ptr.h"

#include "yy_async.h"
#include "yy_resource.h"
#include "yy_material.h"

#include "strings/string.h"

#include <cstdio>
#include <vector>
#include <string>

#include "yy_fs.h"
#include "yy_image.h"
#include "yy_model.h"
#include "yy_gui.h"
#include "yy_input.h"

#include "scene/common.h"
#include "scene/camera.h"
#include "scene/sprite.h"

// for auto create\delete
struct yyEngineContext
{
	yyEngineContext()
	{
		m_state = nullptr;
	}
	~yyEngineContext()
	{
		yyStop(); // destroy main class, free memory
	}

	void init(yyInputContext* input)
	{
		m_state = yyStart(input); // allocate memory for main class inside yuyu.dll
	}

	yySystemState * m_state;
};

void window_onCLose(yyWindow* window)
{
	yyQuit(); // change yySystemState - set yySystemState::Quit
}

// you can implement what you want (write to file, write to game console)
void log_onError(const char* message)
{
	fprintf(stderr,message);
}
void log_onInfo(const char* message)
{
	fprintf(stdout,message);
}

yyVideoDriverAPI* g_videoDriver = nullptr;
void asyncLoadEventHandler(u32 userIndex, void* data)
{
	switch (userIndex)
	{
	default:
		break;
	//case 1: g_img1 = (yyImage*)data; printf("1\n"); break;
	//case 2: g_img2 = (yyImage*)data; printf("2\n"); break;
	case 1:
	{
	}break;
	case 2:
	{
	}break;
	}
}

void pictureBox_load_onClick(yyGUIElement* elem, s32 m_id)
{
	yyLoadImageAsync("../res/grass.dds",1);
	yyLoadImageAsync("../res/grass.png",2);
}

yyInputContext* g_inputContex = nullptr;
void window_callbackMouse(yyWindow* w, s32 wheel, s32 x, s32 y, u32 click)
{
	g_inputContex->m_cursorCoords.x = (f32)x;
	g_inputContex->m_cursorCoords.y = (f32)y;

	if( click & yyWindow_mouseClickMask_LMB_DOWN )
	{
		g_inputContex->m_isLMBDown = true;
		g_inputContex->m_isLMBHold = true;
	}
	if( click & yyWindow_mouseClickMask_LMB_UP )
	{
		g_inputContex->m_isLMBHold = false;
	}
}
void updateInputContext() // call before all callbacks
{
	g_inputContex->m_isLMBDown = false;
}

void window_callbackKeyboard(yyWindow*, bool isPress, u32 key, char16_t character)
{
	if(isPress)
	{
		if(key < 256)
			g_inputContex->m_key_hold[key] = 1;
	}
	else
	{
		if(key < 256)
			g_inputContex->m_key_hold[key] = 0;
	}
}

int main(int argc, char* argv[])
{
	const char * videoDriverType = "d3d11.yyvd"; // for example read name from .ini
	yyStringA videoDriverTypeStr = videoDriverType;
	for (int i = 0; i < argc; ++i)
	{
		if (strcmp(argv[i],"-vid")==0)
		{
			++i;
			if (i < argc)
			{
				videoDriverTypeStr = argv[i];
			}
		}
	}

	// I don't want to use stack memory, so for class\struct I will create new objects using heap
	// use yyPtr if you want auto destroy objects
	
	yyPtr<yyInputContext> inputContext = yyCreate<yyInputContext>();
	g_inputContex = inputContext.m_data;

	yyPtr<yyEngineContext> engineContext = yyCreate<yyEngineContext>();
	engineContext.m_data->init(inputContext.m_data);
	auto p_engineContext = engineContext.m_data;


	yySetAsyncLoadEventHandler(asyncLoadEventHandler);

	// set callbacks if you want to read some information 
	yyLogSetErrorOutput(log_onError);
	yyLogSetInfoOutput(log_onInfo);
	yyLogSetWarningOutput(log_onError);

	yyPtr<yyWindow> window = yyCreate<yyWindow>();
	auto p_window = window.m_data;

	if(!p_window->init(800,600, 0))
	{
		YY_PRINT_FAILED;
		return 1;
	}

	// save pointer
	yySetMainWindow(window.m_data);
	window.m_data = yyGetMainWindow(); 

	p_window->m_onClose = window_onCLose;
	p_window->m_onMouseButton = window_callbackMouse;
	p_window->m_onKeyboard = window_callbackKeyboard;

	// init video driver	
	if( !yyInitVideoDriver(videoDriverTypeStr.c_str(), p_window) )
	{
		yyLogWriteWarning("Can't load video driver : %s\n", videoDriverTypeStr.c_str());

		// if failed, try to init other type
		std::vector<std::string> vidDrivers;
		
		for(auto & entry : yyFS::directory_iterator(yyFS::current_path() ) )
		{

			auto path = entry.path();
			if(path.has_extension())
			{
				auto ex = path.extension();
				if(ex == ".yyvd")
				{
					yyLogWriteWarning("Trying to load video driver : %s\n", path.generic_string().c_str());

					if( yyInitVideoDriver(path.generic_string().c_str(), p_window) )
					{
						goto vidOk;
					}
					else
					{
						yyLogWriteWarning("Can't load video driver : %s\n", path.generic_string().c_str());
					}
				}
			}
		}
		YY_PRINT_FAILED;
		return 1;
	}

vidOk:
	
	g_videoDriver = yyGetVideoDriverAPI();
	g_videoDriver->SetClearColor(0.3f,0.3f,0.74f,1.f);

	window.m_data->SetTitle(g_videoDriver->GetVideoDriverName());

	// CAMERA
	Mat4 proj;
	math::makePerspectiveRHMatrix(proj, 1.f, (f32)window.m_data->m_currentSize.x/(f32)window.m_data->m_currentSize.y, 0.1f,1000.f);
	g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::Projection, proj);
	Mat4 view;
	math::makeLookAtRHMatrix(view, v4f(10.f, 10.f, 10.f, 0.f), v4f(), v4f(0.f, 1.f, 0.f, 0.f));
	g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::View, view);
	g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::ViewProjection, proj * view); 
	// OR LIKE THIS
	yyCamera* camera = yyCreate<yyCamera>();
	camera->m_objectBase.m_localPosition.set(10.f,10.f,10.f,0.f);
	camera->m_aspect = (f32)window.m_data->m_currentSize.x/(f32)window.m_data->m_currentSize.y;
	camera->Update();
	g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::Projection, camera->m_projectionMatrix);
	g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::View, camera->m_viewMatrix);
	// for 3d line
	g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::ViewProjection, camera->m_projectionMatrix * camera->m_viewMatrix);

	auto modelGPU = g_videoDriver->CreateModelFromFile("../res/models/editor/te_sphere.tr3d", true);
	auto grassGPU = g_videoDriver->CreateTextureFromFile("../res/textures/grass_d.dds", true, false, true);

	yySprite* spriteLevel = yyCreateSprite(v4f(0.f,0.f,1160.f,224.f), g_videoDriver->CreateTextureFromFile("../res/GA3E/level1_ground.png",false, false, true), false);
	
	yySprite* spriteHero = yyCreateSprite(v4f(0.f,0.f,50.f,76.f), g_videoDriver->CreateTextureFromFile("../res/GA3E/hero0.png",false, false, true), true);
	spriteHero->SetMainFrame(123, 8, 174, 85);
	spriteHero->m_objectBase.m_localPosition.set(10.f, 20.f, 0.f, 0.f);
	auto stateIdleRight = spriteHero->AddState("IdleRight");
	stateIdleRight->m_isAnimation = true;
	//stateIdle->m_invertX = true;
	//stateIdle->m_invertY = true;
	stateIdleRight->AddAnimationFrame(0,0, 38,84);
	stateIdleRight->AddAnimationFrame(41,0, 79,84);
	stateIdleRight->SetFPS(2.f);
	auto stateIdleLeft = spriteHero->AddState("IdleLeft");
	stateIdleLeft->m_isAnimation = true;
	stateIdleLeft->m_invertX = true;
	stateIdleLeft->AddAnimationFrame(0,0, 38,84);
	stateIdleLeft->AddAnimationFrame(41,0, 79,84);
	stateIdleLeft->SetFPS(2.f);
	spriteHero->SetState(stateIdleRight);

	auto spriteCameraPosition = g_videoDriver->GetSpriteCameraPosition();
	auto spriteCameraScale    = g_videoDriver->GetSpriteCameraScale();
	spriteCameraScale->x = 1.7f;
	spriteCameraScale->y = 1.7f;
	
	auto rtt = g_videoDriver->CreateRenderTargetTexture(v2f(128.f, 128.f), false, true);
	auto gui_pictureBox = yyGUICreatePictureBox(v4f(0.f, 0.f, 256.f, 256.f), rtt, 1);
	
	f32 deltaTime = 0.f;
	bool run = true;
	while( run )
	{
		static u64 t1 = 0;
		u64 t2 = yyGetTime();
		f32 m_tick = f32(t2 - t1);
		t1 = t2;
		deltaTime = m_tick / 1000.f;

		updateInputContext();

#ifdef YY_PLATFORM_WINDOWS
		MSG msg;
		while( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
		{
			GetMessage( &msg, NULL, 0, 0 );
			TranslateMessage (&msg);
			DispatchMessage (&msg);
		}
#else
#error For windows
#endif


		yyGUIUpdate(deltaTime);
		yyUpdateAsyncLoader();

		switch(*p_engineContext->m_state)
		{
		default:
			run = false;
			break;
		case yySystemState::Run:
		{
			f32  spriteCameraMoveSpeed = 50.f;
			f32  heroMoveSpeed = 70.f;
			f32  spriteCameraScaleSpeed = 1.f;
			if(g_inputContex->isKeyHold(yyKey::K_NUM_4))
			{
				spriteCameraPosition->x -= spriteCameraMoveSpeed * deltaTime;
			}
			if(g_inputContex->isKeyHold(yyKey::K_NUM_6))
			{
				spriteCameraPosition->x += spriteCameraMoveSpeed * deltaTime;
			}
			if(g_inputContex->isKeyHold(yyKey::K_NUM_8))
			{
				spriteCameraPosition->y -= spriteCameraMoveSpeed * deltaTime;
			}
			if(g_inputContex->isKeyHold(yyKey::K_NUM_2))
			{
				spriteCameraPosition->y += spriteCameraMoveSpeed * deltaTime;
			}
			if(g_inputContex->isKeyHold(yyKey::K_NUM_7))
			{
				spriteCameraScale->x -= spriteCameraScaleSpeed * deltaTime;
			}
			if(g_inputContex->isKeyHold(yyKey::K_NUM_9))
			{
				spriteCameraScale->x += spriteCameraScaleSpeed * deltaTime;
			}
			if(g_inputContex->isKeyHold(yyKey::K_NUM_1))
			{
				spriteCameraScale->y -= spriteCameraScaleSpeed * deltaTime;
			}
			if(g_inputContex->isKeyHold(yyKey::K_NUM_3))
			{
				spriteCameraScale->y += spriteCameraScaleSpeed * deltaTime;
			}

			if(g_inputContex->isKeyHold(yyKey::K_LEFT))
			{
				spriteHero->m_objectBase.m_localPosition.x -= heroMoveSpeed * deltaTime;
				if(spriteHero->m_currentState != stateIdleLeft)
					spriteHero->SetState(stateIdleLeft);
			}
			if(g_inputContex->isKeyHold(yyKey::K_RIGHT))
			{
				spriteHero->m_objectBase.m_localPosition.x += heroMoveSpeed * deltaTime;
				if(spriteHero->m_currentState != stateIdleRight)
					spriteHero->SetState(stateIdleRight);
			}
			if(g_inputContex->isKeyHold(yyKey::K_UP))
			{
				spriteHero->m_objectBase.m_localPosition.y -= heroMoveSpeed * deltaTime;
			}
			if(g_inputContex->isKeyHold(yyKey::K_DOWN))
			{
				spriteHero->m_objectBase.m_localPosition.y += heroMoveSpeed * deltaTime;
			}
			if(g_inputContex->isKeyHold(yyKey::K_PGUP))
			{
				v4f r = spriteHero->m_objectBase.m_rotation;
				r.z += 1.f;
				spriteHero->m_objectBase.SetRotation(r);
			}
			if(g_inputContex->isKeyHold(yyKey::K_PGDOWN))
			{
				v4f r = spriteHero->m_objectBase.m_rotation;
				r.z -= 1.f;
				spriteHero->m_objectBase.SetRotation(r);
			}


			//g_videoDriver->SetViewport(0.f, 0.f, window.m_data->m_currentSize.x, window.m_data->m_currentSize.y);
			g_videoDriver->BeginDraw();
			g_videoDriver->ClearAll();
			
			g_videoDriver->UseDepth(true);

			g_videoDriver->DrawLine3D(v4f(-2.f, 0.f, 0.f, 0.f), v4f(2.f, 0.f, 0.f, 0.f), ColorRed);
			
			g_videoDriver->SetModel(modelGPU);
			g_videoDriver->SetTexture(yyVideoDriverAPI::TextureSlot::Texture0, grassGPU);
			g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::World, Mat4());
			g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::WorldViewProjection, camera->m_projectionMatrix * camera->m_viewMatrix * Mat4());
			yyMaterial material;
			g_videoDriver->SetMaterial(&material);
			g_videoDriver->Draw();

			/*g_videoDriver->SetRenderTarget(rtt);
			g_videoDriver->SetClearColor(0.3f, 0.f, 0.f, 1.f);
			g_videoDriver->SetViewport(0.f, 0.f, 128.f, 128.f);
			g_videoDriver->ClearAll();
			g_videoDriver->Draw();
			g_videoDriver->SetRenderTarget(0);
			g_videoDriver->SetViewport(0.f, 0.f, window.m_data->m_currentSize.x, window.m_data->m_currentSize.y);
			g_videoDriver->SetClearColor(0.3f, 0.3f, 0.74f, 1.f);*/

			g_videoDriver->UseDepth(false);
			g_videoDriver->DrawSprite(spriteLevel);
			spriteHero->m_objectBase.UpdateBase();
			spriteHero->Update(deltaTime);

			g_videoDriver->DrawSprite(spriteHero);
			

			yyGUIDrawAll();
			g_videoDriver->EndDraw();
			g_videoDriver->SwapBuffers();

		}break;
		}
	}

	if(spriteLevel)
		yyDestroy(spriteLevel);
	if(camera)
		yyDestroy(camera);

	return 0;
}