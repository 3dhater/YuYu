#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include "yy.h"
#include "yy_window.h"

#include "yy_ptr.h"
#include "math/math.h"

#include "yy_async.h"
#include "yy_resource.h"

#include "strings/string.h"

#include <cstdio>
#include <vector>
#include <string>

#include "yy_fs.h"
#include "yy_image.h"
#include "yy_model.h"
#include "yy_gui.h"
#include "yy_input.h"

#include "camera.h"
#include "scene/sprite.h"

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
	window->Hide();
	yyQuit(); // change yySystemState - set yySystemState::Quit
}
void log_onError(const char* message)
{
	fprintf(stderr, message);
}
void log_onInfo(const char* message)
{
	fprintf(stdout, message);
}

yyVideoDriverAPI* g_videoDriver = nullptr;
yyInputContext* g_inputContex = nullptr;
void window_callbackMouse(yyWindow* w, s32 wheel, s32 x, s32 y, u32 click)
{
	g_inputContex->m_cursorCoords.x = (f32)x;
	g_inputContex->m_cursorCoords.y = (f32)y;

	g_inputContex->m_mouseDelta.x = (f32)x - g_inputContex->m_cursorCoordsOld.x;
	g_inputContex->m_mouseDelta.y = (f32)y - g_inputContex->m_cursorCoordsOld.y;

	g_inputContex->m_cursorCoordsOld = g_inputContex->m_cursorCoords;

	if (click & yyWindow_mouseClickMask_LMB_DOWN)
	{
		g_inputContex->m_isLMBDown = true;
		g_inputContex->m_isLMBHold = true;
	}
	if (click & yyWindow_mouseClickMask_LMB_UP)
	{
		g_inputContex->m_isLMBHold = false;
		g_inputContex->m_isLMBUp = true;
	}
	if (click & yyWindow_mouseClickMask_LMB_DOUBLE)
	{
		g_inputContex->m_isLMBDbl = true;
	}

	if (click & yyWindow_mouseClickMask_RMB_DOWN)
	{
		g_inputContex->m_isRMBDown = true;
		g_inputContex->m_isRMBHold = true;
	}
	if (click & yyWindow_mouseClickMask_RMB_UP)
	{
		g_inputContex->m_isRMBHold = false;
	}
}
void updateInputContext() // call before all callbacks
{
	g_inputContex->m_isLMBDbl = false;
	g_inputContex->m_isLMBDown = false;
	g_inputContex->m_isRMBDown = false;
	g_inputContex->m_isLMBUp = false;
	g_inputContex->m_mouseDelta.x = 0.f;
	g_inputContex->m_mouseDelta.y = 0.f;
}

int main()
{

	yyPtr<yyInputContext> inputContext = yyCreate<yyInputContext>();
	g_inputContex = inputContext.m_data;

	yyPtr<yyEngineContext> engineContext = yyCreate<yyEngineContext>();
	engineContext.m_data->init(inputContext.m_data);
	auto p_engineContext = engineContext.m_data;


	yyLogSetErrorOutput(log_onError);
	yyLogSetInfoOutput(log_onInfo);
	yyLogSetWarningOutput(log_onError);

	yyPtr<yyWindow> window = yyCreate<yyWindow>();
	if (!window.m_data)
	{
		YY_PRINT_FAILED;
		return 1;
	}
	auto p_window = window.m_data;

	Camera camera;
	camera.init();

	RECT rc;
	GetWindowRect(GetDesktopWindow(), &rc);
	rc.left = 0; rc.top = 0; rc.right = 1200; rc.bottom = 800;
	if (!p_window->init(rc.right - rc.left, rc.bottom - rc.top, 0))
	{
		YY_PRINT_FAILED;
		return 1;
	}

	p_window->m_onClose = window_onCLose;
	p_window->m_onMouseButton = window_callbackMouse;
	yySetMainWindow(p_window);

	// init video driver
	const char * videoDriverType = "d3d11.yyvd";
	if (!yyInitVideoDriver(videoDriverType, p_window))
	{
		YY_PRINT_FAILED;
		return 0;
	}
	g_videoDriver = yyGetVideoDriverAPI();
	g_videoDriver->SetClearColor(0.3f, 0.3f, 0.4f, 1.f);
	g_videoDriver->UseVSync(true);

	yyVideoDriverObjectD3D11* vdo = (yyVideoDriverObjectD3D11*)g_videoDriver->GetVideoDriverObjects();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(p_window->m_hWnd);
	ImGui_ImplDX11_Init((ID3D11Device*)vdo->m_device, (ID3D11DeviceContext*)vdo->m_context);
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	window.m_data->SetFocus();
	
	f32 deltaTime = 0.f;
	bool run = true;
	while (run)
	{		
		static u64 t1 = 0u;
		u64 t2 = yyGetTime();
		f32 m_tick = f32(t2 - t1);
		t1 = t2;
		deltaTime = m_tick / 1000.f;
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
		
		

		// Start the Dear ImGui frame
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (g_inputContex->m_isRMBHold)
		{
			camera.rotate(v2f(-g_inputContex->m_mouseDelta.x, -g_inputContex->m_mouseDelta.y), deltaTime);
	//		printf("%f %f\n", io.MouseDelta.x, io.MouseDelta.y);

			if (io.KeysDownDuration[(int)yyKey::K_W] >= 0.0f)
				camera.moveForward(deltaTime);

			if (io.KeysDownDuration[(int)yyKey::K_S] >= 0.0f)
				camera.moveBackward(deltaTime);

			if (io.KeysDownDuration[(int)yyKey::K_A] >= 0.0f)
				camera.moveLeft(deltaTime);

			if (io.KeysDownDuration[(int)yyKey::K_D] >= 0.0f)
				camera.moveRight(deltaTime);

			if (io.KeysDownDuration[(int)yyKey::K_E] >= 0.0f)
				camera.moveUp(deltaTime);

			if (io.KeysDownDuration[(int)yyKey::K_Q] >= 0.0f)
				camera.moveDown(deltaTime);

			auto cursorX = std::floor((f32)window.m_data->m_currentSize.x / 2.f);
			auto cursorY = std::floor((f32)window.m_data->m_currentSize.y / 2.f);
			g_inputContex->m_cursorCoordsOld.set(cursorX, cursorY);

			yySetCursorPosition(cursorX, cursorY, window.m_data);
		}
	//	printf("%f\n", deltaTime);
		camera.update();
		g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::ViewProjection, camera.m_camera->m_viewProjectionMatrix);
		

		
		ImGui::Begin("Main menu", 0, ImGuiWindowFlags_AlwaysAutoResize);
		ImGuiTabBarFlags mainemnuTab_bar_flags = ImGuiTabBarFlags_None;
		if (ImGui::BeginTabBar("menutab", mainemnuTab_bar_flags))
		{
			if (ImGui::BeginTabItem("File"))
			{
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("View"))
			{
				if (ImGui::Button("Reset camera"))
				{
					camera.reset();
				}
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::End();

		ImGui::SetNextWindowSizeConstraints(ImVec2(300.f, 600.f), ImVec2(300.f, 600.f));
		ImGui::Begin("Parameters", 0, ImGuiWindowFlags_AlwaysAutoResize);
		if (ImGui::BeginTabBar("parameterstab"))
		{
			if (ImGui::BeginTabItem("Layers"))
			{
				ImGuiWindowFlags window_flags = ImGuiWindowFlags_HorizontalScrollbar;
				ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(255, 0, 0, 100));
				ImGui::BeginChild("ChildLayerList", ImVec2(ImGui::GetWindowContentRegionWidth(), 200), false, window_flags);
				for (int i = 0; i < 100; i++)
				{
					ImGui::Text("%04d: scrollable region", i);
				}
				ImGui::EndChild();
				ImGui::PopStyleColor();
				if (ImGui::Button("New"))
				{
					auto path = yyOpenFileDialog("Import model", "Import", "obj fbx", "Supported files");
					if (path)
					{
						//yyLogWriteInfo("OpenFileDialog: %s\n", path->to_stringA().data());

						yyDestroy(path);
					}
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Animations"))
			{
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::End();

		yyGUIUpdate(1.f);
		yyUpdateAsyncLoader();

		//WaitForSingleObject(GetCurrentThread(), 5);


		switch(*p_engineContext->m_state)
		{
		default:
			run = false;
			break;
		case yySystemState::Run:
		{
			g_videoDriver->BeginDraw();
			g_videoDriver->ClearAll();

			g_videoDriver->DrawLine3D(v4f(10.f, 0.f, 0.f, 0.f), v4f(-10.f, 0.f, 0.f, 0.f), ColorRed);
			g_videoDriver->DrawLine3D(v4f(0.f, 0.f, 10.f, 0.f), v4f(0.f, 0.f, -10.f, 0.f), ColorBlue);

			//g_videoDriver->UseDepth(true);

			///*g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::Projection, viewport.m_data->m_activeCamera->m_camera->m_projectionMatrix);
			//g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::View, viewport.m_data->m_activeCamera->m_camera->m_viewMatrix);
			//g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::ViewProjection, viewport.m_data->m_activeCamera->m_camera->m_viewProjectionMatrix);*/

			//g_videoDriver->DrawLine3D(v4f(-2.f, 0.f, 0.f, 0.f), v4f(2.f, 0.f, 0.f, 0.f), ColorRed);
			//g_videoDriver->DrawLine3D(v4f(0.f, 0.f, -2.f, 0.f), v4f(0.f, 0.f, 2.f, 0.f), ColorRed);


			//g_videoDriver->UseDepth(false);
			//
			//yyGUIDrawAll();


			g_videoDriver->EndDraw();
			ImGui::Render();
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			g_videoDriver->SwapBuffers();

		}break;
		}
	}

	/*ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();*/

	return 0;
}