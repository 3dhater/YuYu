#include "yy.h"
#include "yy_window.h"

#include "yy_ptr.h"

#include "yy_async.h"
#include "yy_resource.h"

#include "strings/string.h"

#include <cstdio>
#include <vector>
#include <filesystem>
namespace fs = std::filesystem;
#include <string>

#include "yy_image.h"
#include "yy_gui.h"
#include "yy_input.h"

// for auto create\delete
struct yyEngineContext
{
	yyEngineContext(yyInputContext* input)
	{
		m_state = yyStart(input); // allocate memory for main class inside yuyu.dll
	}
	~yyEngineContext()
	{
		yyStop(); // destroy main class, free memory
	}

	yySystemState * m_state = nullptr;
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

yyImage * g_img1 = nullptr;
yyImage * g_img2 = nullptr;
void asyncLoadEventHandler(u32 userIndex, yyResource resource, void* rawData)
{
	switch (userIndex)
	{
	default:
		break;
	case 1: g_img1 = (yyImage*)rawData; printf("1\n"); break;
	case 2: g_img2 = (yyImage*)rawData; printf("2\n"); break;
	}
}

void pictureBox_onClick(yyGUIElement* elem, s32 m_id)
{
	printf("Click\n");
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

int main()
{
	// I don't want to use stack memory, so for class\struct I will create new objects using heap
	// use yyPtr if you want auto destroy objects
	
	yyPtr<yyInputContext> inputContext = yyCreate<yyInputContext>();
	g_inputContex = inputContext.m_data;

	yyPtr<yyEngineContext> engineContext = yyCreate<yyEngineContext>(inputContext.m_data);
	auto p_engineContext = engineContext.m_data;


	yySetAsyncLoadEventHandler(asyncLoadEventHandler);

	// set callbacks if you want to read some information 
	yyLogSetErrorOutput(log_onError);
	yyLogSetInfoOutput(log_onInfo);
	yyLogSetWarningOutput(log_onError);

	yyPtr<yyWindow> window = new yyWindow;
	auto p_window = window.m_data;

	if(!p_window->init(800,600))
	{
		YY_PRINT_FAILED;
		return 1;
	}

	p_window->m_onClose = window_onCLose;
	p_window->m_onMouseButton = window_callbackMouse;

	// init video driver
	const char * videoDriverType = "opengl.yyvd"; // for example read name from .ini
	if( !yyInitVideoDriver(videoDriverType, p_window) )
	{
		yyLogWriteWarning("Can't load video driver : %s\n", videoDriverType);

		// if failed, try to init other type
		std::vector<std::string> vidDrivers;
		for(const auto & entry : fs::directory_iterator(fs::current_path()))
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
	
	auto videoDriver = yyGetVideoDriverAPI();
	videoDriver->SetClearColor(0.3f,0.3f,0.4f,1.f);

	yyLoadImageAsync("../res/grass.dds",1);
	yyLoadImageAsync("../res/grass.png",2);
	
	//for(int i = 0; i < 10000; ++i)
	//{
	//	printf("%i\n",i);
	//	auto t = videoDriver->GetTexture("../res/grass.dds",true);
	//	videoDriver->ReleaseTexture(t);
	//}

	auto pictureBox = yyGUICreatePictureBox(v4f(0.f, 0.f, 512.f, 512.f), videoDriver->GetTexture("../res/grass.png",true), 1);
	pictureBox->m_onClick = pictureBox_onClick;

	bool run = true;
	while( run )
	{
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

		yyGUIUpdate(1.f);
		yyUpdateAsyncLoader();

		switch(*p_engineContext->m_state)
		{
		default:
			run = false;
			break;
		case yySystemState::Run:
		{
			

			videoDriver->BeginDrawClearAll();
			
			//videoDriver->BeginDrawGUI();
			yyGUIDrawAll();
			//videoDriver->EndDrawGUI();

			videoDriver->EndDraw();

		}break;
		}
	}

	if(g_img1) yyDeleteImage(g_img1);
	if(g_img2) yyDeleteImage(g_img2);

	return 0;
}