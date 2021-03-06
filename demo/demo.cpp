﻿#include "demo.h"
#include "demo_example.h"
#include "demo_example_sprite.h"
#include "demo_example_MDL.h"
#include "demo_example_gui.h"
#include "demo_example_lines.h"
#include "demo_example_billboards.h"

#include "yy_window.h"
#include "yy_input.h"
#include "yy_gui.h"
#include "yy_image.h"

Demo* g_demo = 0;

// you can implement what you want (write to file, write to game console)
void log_onError(const char* message){
	fprintf(stderr, message);
}

void log_onInfo(const char* message){
	fprintf(stdout, message);
}

void window_callbackOnSize(yyWindow* window) {
}

void window_onCLose(yyWindow* window){
	yyQuit(); // change yySystemState - set yySystemState::Quit
}

Demo::Demo(){
	m_inputContext = 0;
	m_engineContext = 0;
	m_window = 0;
	m_dt = 0.f;

	g_demo = this;
	m_gpu = 0;
	m_defaultFont = 0;
	m_activeExample = 0;
	m_selectedExample = -1;

	for (u32 i = 0; i < (u32)yyCursorType::_count; ++i)
	{
		m_cursors[i] = 0;
	}
}

Demo::~Demo(){
	for (u32 i = 0; i < (u32)yyCursorType::_count; ++i)
	{
		if (m_cursors[i])delete m_cursors[i];
	}
	if (m_activeExample) m_activeExample->Shutdown();

	for (u32 i = 0, sz = m_examples.size(); i < sz; ++i)
	{
		delete m_examples[i];
	}

	if (m_window) yyDestroy(m_window);
	if (m_engineContext) yyDestroy(m_engineContext);
	if (m_inputContext) yyDestroy(m_inputContext);
}


bool Demo::Init(const char* videoDriver){
	m_inputContext  = yyCreate<yyInputContext>();
	m_engineContext = yyCreate<yyEngineContext>();
	m_engineContext->init(m_inputContext);

	// set callbacks if you want to read some information 
	yyLogSetErrorOutput(log_onError);
	yyLogSetInfoOutput(log_onInfo);
	yyLogSetWarningOutput(log_onError);

	for (u32 i = 0; i < (u32)yyCursorType::_count; ++i)
	{
		m_cursors[i] = new yyCursor((yyCursorType)i);

		switch ((yyCursorType)i)
		{
		default:
		case yyCursorType::Arrow: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/arrow.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case yyCursorType::Cross: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/prec.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case yyCursorType::Hand: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/link.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case yyCursorType::Help: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/helpsel.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case yyCursorType::IBeam: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/select.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case yyCursorType::No: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/unavail.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case yyCursorType::Size: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/move.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case yyCursorType::SizeNESW: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/nesw.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case yyCursorType::SizeNS: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/ns.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case yyCursorType::SizeNWSE: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/nwse.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case yyCursorType::SizeWE: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/ew.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case yyCursorType::UpArrow: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/up.cur", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		case yyCursorType::Wait: m_cursors[i]->m_handle = (HCURSOR)LoadImage(GetModuleHandle(0), L"../res/aero-no-tail/working.ani", IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE); break;
		}
		yySetCursor((yyCursorType)i, m_cursors[i]);
	}

	m_window = yyCreate<yyWindow>();
	u32 windowStyle = 0;
	if (!m_window->init(800, 600, windowStyle))
	{
		YY_PRINT_FAILED;
		return false;
	}

	// save pointer
	yySetMainWindow(m_window);
	m_window->m_onClose = window_onCLose;
	m_window->m_onSize = window_callbackOnSize;
	m_window->m_onMaximize = window_callbackOnSize;
	m_window->m_onRestore = window_callbackOnSize;

	// init video driver	
	if (!yyInitVideoDriver(videoDriver, m_window))
	{
		yyLogWriteWarning("Can't load video driver : %s\n", videoDriver);
		for (auto & entry : yy_fs::directory_iterator(yy_fs::current_path()))
		{
			auto path = entry.path();
			if (path.has_extension())
			{
				auto ex = path.extension();
				if (ex == ".dll" && yyIsValidVideoDriver(path.generic_string().c_str()))
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

	yySetDefaultTexture(yyGetTextureFromCache("../res/textures/editor/white.dds"));

	yyImage testImg;
	yyLoadImageGetInfo("../res/image.dds", &testImg);
	yyLoadImageGetInfo("../res/load.png", &testImg);
	yyLoadImageGetInfo("../res/1_32bit.bmp", &testImg);
	yyLoadImageGetInfo("../res/image.tga", &testImg);

	m_defaultFont = yyGUILoadFont("../res/fonts/Noto/notosans.txt");
	if (!m_defaultFont)
	{
		YY_PRINT_FAILED;
		return false;
	}

	AddExample(new DemoExample_Sprite);
	AddExample(new DemoExample_MDL);
	AddExample(new DemoExample_GUI);
	AddExample(new DemoExample_Lines);
	AddExample(new DemoExample_Billboards);


	if (m_examples.size())
	{
		SelectExamplePressDown();
	}

	return true;
}

void buttonExit_onClick(yyGUIElement* elem, s32 m_id){
//	yyQuit();
}

void buttonExit_onRelease(yyGUIElement* elem, s32 m_id){
	yyQuit();
}
void buttonExit_onMouseInRect(yyGUIElement* elem, s32 m_id) {
	yySetCursor(yyCursorType::Arrow, g_demo->m_cursors[(u32)yyCursorType::Hand]);
}

void Demo::MainLoop(){
	yyGUIText* gui_text_fps = yyGUICreateText(v2f(0.f, 0.f), m_defaultFont, 0, 0);
	
	yyGUIButton* gui_button_exit = yyGUICreateButton(
		v4f((f32)m_window->m_creationSize.x - 50.f, 0.f, (f32)m_window->m_creationSize.x, 25.f),
		//v4f(0.f, m_window->m_creationSize.y-50, 50, m_window->m_creationSize.y),
		yyGetTextureFromCache("../res/exit.png"), 0, 0);
	gui_button_exit->SetMouseHoverTexture(yyGetTextureFromCache("../res/exit_hov.png"));
	gui_button_exit->SetMouseClickTexture(yyGetTextureFromCache("../res/exit_clk.png"));
	gui_button_exit->m_onClick = buttonExit_onClick;
	gui_button_exit->m_onMouseInRect = buttonExit_onMouseInRect;
	gui_button_exit->m_onRelease = buttonExit_onRelease;
	gui_button_exit->m_isAnimated = true;
	gui_button_exit->m_align = yyGUIElement::Align::AlignRightTop;
	//gui_button_exit->m_useAsCheckbox = true;
	//gui_button_exit->m_isChecked = true;

	
	f32 fps_timer = 0.f;
	u32 fps = 0;
	u32 fps_counter = 0;

	yyEvent currentEvent;

	while (yyRun(&m_dt))
	{
		yySetCursor(yyCursorType::Arrow, g_demo->m_cursors[(u32)yyCursorType::Arrow]);

		++fps_counter;
		fps_timer += m_dt;

		if (fps_timer > 1.f)
		{
			fps_timer = 0.f;
			fps = fps_counter;
			fps_counter = 0;
		}

		if (gui_text_fps)
			gui_text_fps->SetText(L"FPS: %u", fps);

		yyGUIUpdate(m_dt);

		while (yyPollEvent(currentEvent))
		{
			switch (currentEvent.m_type)
			{
			default:
			case yyEventType::Engine:
				break;
			case yyEventType::System:
				break;
			case yyEventType::Window: {
				if (currentEvent.m_event_window.m_event == yyEvent_Window::size_changed) {
					yyGetVideoDriverAPI()->UpdateMainRenderTarget(m_window->m_currentSize, 
					v2f((f32)m_window->m_currentSize.x, (f32)m_window->m_currentSize.y));
					yyGUIRebuild();
				}
			}break;
			}
		}

		yyUpdateAsyncLoader();

		switch (*m_engineContext->m_state)
		{
		default:
			break;
		case yySystemState::Run:
		{
			m_gpu->BeginDraw();
			m_gpu->ClearAll();
			m_gpu->UseDepth(true);

			if (m_activeExample)
			{
				if (!m_activeExample->DemoStep(m_dt))
				{
					StopDemo();
				}
			}
			else if(m_selectedExample != -1)
			{
				m_gpu->DrawRectangle(v4f(5.f,10.f, 30.f,25.f), ColorRed, ColorGreenYellow);

				if (m_inputContext->IsKeyPressed(yyKey::K_UP))
					this->SelectExamplePressUp();
				if (m_inputContext->IsKeyHit(yyKey::K_DOWN)) 
				{
					/*{
						auto img = yyLoadImage("../res/GA3E/level1_ground.png");
						yyDestroy(img);
					}*/
					/*{
						auto t = yyCreateTextureFromFile("../res/GA3E/level1_ground.png");
						t->Load();
						yyDestroy(t);
					}*/
					/*{
						auto t = yyGetTextureFromCache("../res/GA3E/level1_ground.png");
						yyRemoveTextureFromCache(t);
						yyDestroy(t);
					}*/
					/*{
						auto t = yyGetTextureFromCache("../res/GA3E/level1_ground.png");
						t->Load();
						t->Load();
						t->Unload();
						t->Unload();
						yyRemoveTextureFromCache(t);
						yyDestroy(t);
					}*/

					this->SelectExamplePressDown();
				}
				if (m_inputContext->IsKeyHold(yyKey::K_ENTER))
				{
					StartDemo();
				}
			}


			yyGUIDrawAll(m_dt);
			m_gpu->EndDraw();
			m_gpu->SwapBuffers();

		}break;
		}
	}
}

void Demo_TextTitle_onMouseInRect(yyGUIElement* elem, s32 m_id){
	g_demo->m_selectedExample = m_id;
	g_demo->_SelectExampleUpdateColors();
}

void Demo_TextTitle_onClick(yyGUIElement* elem, s32 m_id) {
	g_demo->StartDemo();
}

void Demo_TextTitle_onDraw_toRed(yyGUIElement* elem, s32 m_id) {
	f32 sp = 5.f * g_demo->m_dt;
	elem->m_color.m_data[1] -= sp;
	elem->m_color.m_data[2] -= sp;
	if (elem->m_color.m_data[1] < 0.f)
		elem->m_color.m_data[1] = 0.f;
	if (elem->m_color.m_data[2] < 0.f)
		elem->m_color.m_data[2] = 0.f;
	elem->m_offset.x += 1.f;
	if (elem->m_offset.x > 5.f)
		elem->m_offset.x = 5.f;
	if (elem->m_color.m_data[1] == 0.f && elem->m_color.m_data[2] == 0.f)
	{
		elem->m_onDraw = 0;
	}
}
void Demo_TextTitle_onDraw_toWhite(yyGUIElement* elem, s32 m_id) {
	f32 sp = 5.f * g_demo->m_dt;
	elem->m_color.m_data[1] += sp;
	elem->m_color.m_data[2] += sp;
	if (elem->m_color.m_data[1] > 1.f)
		elem->m_color.m_data[1] = 1.f;
	if (elem->m_color.m_data[2] > 1.f)
		elem->m_color.m_data[2] = 1.f;
	elem->m_offset.x -= 1.f;
	if (elem->m_offset.x < 0.f)
		elem->m_offset.x = 0.f;
	if (elem->m_color.m_data[1] == 1.f && elem->m_color.m_data[2] == 1.f)
	{
		elem->m_onDraw = 0;
	}
}

void Demo::AddExample(DemoExample* e){
	static v2f gui_text_position = v2f(0.f, 20.f);
	e->m_guiTextTitle = yyGUICreateText(gui_text_position, m_defaultFont, e->GetTitle(), 0);
	e->m_guiTextTitle->m_onMouseInRect = Demo_TextTitle_onMouseInRect;
	e->m_guiTextTitle->m_onClick = Demo_TextTitle_onClick;
	e->m_guiTextTitle->m_id = m_examples.size();
	gui_text_position.y += 10.f;
	m_examples.push_back(e);
}

void Demo::SelectExamplePressDown(){
	++m_selectedExample;
	if (m_selectedExample == m_examples.size())
		m_selectedExample = 0;
	_SelectExampleUpdateColors();
}

void Demo::SelectExamplePressUp(){
	--m_selectedExample;
	if (m_selectedExample == -1)
		m_selectedExample = m_examples.size() - 1;
	_SelectExampleUpdateColors();
}

void Demo::_SelectExampleUpdateColors(){
	for (u16 i = 0, sz = m_examples.size(); i < sz; ++i)
	{
		/*if(i == m_selectedExample)
			m_examples[i]->m_guiTextTitle->m_color = ColorRed;
		else
			m_examples[i]->m_guiTextTitle->m_color = ColorWhite;*/
		if (i == m_selectedExample)
			m_examples[i]->m_guiTextTitle->m_onDraw = Demo_TextTitle_onDraw_toRed;
		else
			m_examples[i]->m_guiTextTitle->m_onDraw = Demo_TextTitle_onDraw_toWhite;
	}
}

void Demo::StopDemo(){
	m_activeExample->Shutdown();
	m_activeExample = 0;
	_showMainMenuGUI();
}

void Demo::StartDemo(){
	m_activeExample = m_examples[m_selectedExample];
	yyStringA stra;
	stra += m_activeExample->GetTitle();
	yyLogWriteInfo("Start Demo: %s\n", stra.data());
	if (!m_activeExample->Init())
	{
		yyLogWriteWarning("Can't start demo [%s]\n", stra.data());

		m_activeExample->Shutdown();
		m_activeExample = 0;
	}
	else
	{
		_hideMainMenuGUI();
	}
}

void Demo::_hideMainMenuGUI(){
	for (u16 i = 0, sz = m_examples.size(); i < sz; ++i)
	{
		m_examples[i]->m_guiTextTitle->SetVisible(false);
	}
}

void Demo::_showMainMenuGUI(){
	for (u16 i = 0, sz = m_examples.size(); i < sz; ++i)
	{
		m_examples[i]->m_guiTextTitle->SetVisible(true);
	}
}