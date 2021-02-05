#include "demo.h"

#include "yy_window.h"
#include "yy_input.h"
#include "yy_gui.h"

Demo* g_demo = 0;

// you can implement what you want (write to file, write to game console)
void log_onError(const char* message)
{
	fprintf(stderr, message);
}
void log_onInfo(const char* message)
{
	fprintf(stdout, message);
}
void window_callbackMouse(yyWindow* w, s32 wheel, s32 x, s32 y, u32 click)
{
	g_demo->m_inputContext->m_cursorCoords.x = (f32)x;
	g_demo->m_inputContext->m_cursorCoords.y = (f32)y;

	g_demo->m_inputContext->m_mouseDelta.x = (f32)x - g_demo->m_inputContext->m_cursorCoordsOld.x;
	g_demo->m_inputContext->m_mouseDelta.y = (f32)y - g_demo->m_inputContext->m_cursorCoordsOld.y;

	g_demo->m_inputContext->m_cursorCoordsOld = g_demo->m_inputContext->m_cursorCoords;

	if (click & yyWindow_mouseClickMask_LMB_DOWN)
	{
		g_demo->m_inputContext->m_isLMBDown = true;
		g_demo->m_inputContext->m_isLMBHold = true;
	}
	if (click & yyWindow_mouseClickMask_LMB_UP)
	{
		g_demo->m_inputContext->m_isLMBHold = false;
		g_demo->m_inputContext->m_isLMBUp = true;
	}
	if (click & yyWindow_mouseClickMask_LMB_DOUBLE)
	{
		g_demo->m_inputContext->m_isLMBDbl = true;
	}

	if (click & yyWindow_mouseClickMask_RMB_DOWN)
	{
		g_demo->m_inputContext->m_isRMBDown = true;
		g_demo->m_inputContext->m_isRMBHold = true;
	}
	if (click & yyWindow_mouseClickMask_RMB_UP)
	{
		g_demo->m_inputContext->m_isRMBHold = false;
	}
}
void updateInputContext() // call before all callbacks
{
	g_demo->m_inputContext->m_isLMBDbl = false;
	g_demo->m_inputContext->m_isLMBDown = false;
	g_demo->m_inputContext->m_isRMBDown = false;
	g_demo->m_inputContext->m_isLMBUp = false;
	g_demo->m_inputContext->m_mouseDelta.x = 0.f;
	g_demo->m_inputContext->m_mouseDelta.y = 0.f;
}
void window_callbackKeyboard(yyWindow*, bool isPress, u32 key, char16_t character)
{
	if (isPress)
	{
		if (key < 256)
			g_demo->m_inputContext->m_key_hold[key] = 1;
	}
	else
	{
		if (key < 256)
			g_demo->m_inputContext->m_key_hold[key] = 0;
	}
}
void window_onCLose(yyWindow* window)
{
	yyQuit(); // change yySystemState - set yySystemState::Quit
}

Demo::Demo()
{
	m_inputContext = 0;
	m_engineContext = 0;
	m_window = 0;

	g_demo = this;
	m_gpu = 0;
	m_defaultFont = 0;
}

Demo::~Demo()
{
	if (m_window) yyDestroy(m_window);
	if (m_engineContext) yyDestroy(m_engineContext);
	if (m_inputContext) yyDestroy(m_inputContext);
}

bool Demo::Init(const char* videoDriver)
{
	m_inputContext  = yyCreate<yyInputContext>();
	m_engineContext = yyCreate<yyEngineContext>();
	m_engineContext->init(m_inputContext);

	// set callbacks if you want to read some information 
	yyLogSetErrorOutput(log_onError);
	yyLogSetInfoOutput(log_onInfo);
	yyLogSetWarningOutput(log_onError);

	m_window = yyCreate<yyWindow>();
	if (!m_window->init(800, 600, 0))
	{
		YY_PRINT_FAILED;
		return false;
	}

	// save pointer
	yySetMainWindow(m_window);
	m_window->m_onClose = window_onCLose;
	m_window->m_onMouseButton = window_callbackMouse;
	m_window->m_onKeyboard = window_callbackKeyboard;

	// init video driver	
	if (!yyInitVideoDriver(videoDriver, m_window))
	{
		yyLogWriteWarning("Can't load video driver : %s\n", videoDriver);
		for (auto & entry : yyFS::directory_iterator(yyFS::current_path()))
		{
			auto path = entry.path();
			if (path.has_extension())
			{
				auto ex = path.extension();
				if (ex == ".yyvd")
				{
					yyLogWriteWarning("Trying to load video driver : %s\n", path.generic_string().c_str());

					if (yyInitVideoDriver(path.generic_string().c_str(), m_window))
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
		return false;
	}

vidOk:

	m_gpu = yyGetVideoDriverAPI();
	m_gpu->SetClearColor(0.3f, 0.3f, 0.74f, 1.f);
	m_window->SetTitle(m_gpu->GetVideoDriverName());

	m_defaultFont = yyGUILoadFont("../res/fonts/Noto/notosans.txt");
	if (!m_defaultFont)
	{
		YY_PRINT_FAILED;
		return false;
	}


	return true;
}

void Demo::MainLoop()
{
	yyGUIText* gui_text_fps = yyGUICreateText(v2f(0.f, 0.f), m_defaultFont, 0);

	f32 deltaTime = 0.f;
	f32 fps_timer = 0.f;
	u32 fps = 0;
	u32 fps_counter = 0;
	bool run = true;
	while (run)
	{
		static u64 t1 = 0;
		u64 t2 = yyGetTime();
		f32 m_tick = f32(t2 - t1);
		t1 = t2;
		deltaTime = m_tick / 1000.f;

		++fps_counter;
		fps_timer += deltaTime;

		if (fps_timer > 1.f)
		{
			fps_timer = 0.f;
			fps = fps_counter;
			fps_counter = 0;
		}

		updateInputContext();

#ifdef YY_PLATFORM_WINDOWS
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			GetMessage(&msg, NULL, 0, 0);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
#else
#error For windows
#endif
		if (gui_text_fps)
			gui_text_fps->SetText(L"FPS: %u", fps);

		yyGUIUpdate(deltaTime);
		yyUpdateAsyncLoader();

		switch (*m_engineContext->m_state)
		{
		default:
			run = false;
			break;
		case yySystemState::Run:
		{
			m_gpu->BeginDraw();
			m_gpu->ClearAll();
			m_gpu->UseDepth(true);



			yyGUIDrawAll();
			m_gpu->EndDraw();
			m_gpu->SwapBuffers();

		}break;
		}
	}
}