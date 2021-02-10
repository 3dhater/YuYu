#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_opengl3.h"

#include <GL/gl3w.h>

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
#include <iostream>

#include "yy_fs.h"
#include "yy_image.h"
#include "yy_model.h"
#include "yy_gui.h"
#include "yy_input.h"
#include "io/file_buffer.h"

#include "camera.h"
#include "scene/sprite.h"
#include "scene/mdl_object.h"

#include "MDLCreator_common.h"
#include "SceneObject.h"

//#include <assimp/Importer.hpp>      // C++ importer interface
//#include <assimp/scene.h>       // Output data structure
//#include <assimp/postprocess.h> // Post processing flags

yyVideoDriverAPI* g_videoDriver = nullptr;
yyInputContext* g_inputContex = nullptr;

yyStringA g_renameTextBuffer;
const u32 g_renameTextBufferSize = 128;
int g_shaderCombo_item_current = 0;

int g_selectedLayer = -1;
int g_selectedAnimation = -1;
int g_selectedHitbox = -1;
bool g_bindPose = false;


void SaveMDL(const char* fileName);
SceneObject* g_sceneObject = 0;

//Mat4 aiMatrixToGameMatrix(const aiMatrix4x4& a)
//{
//	Mat4 m;
//	auto dst = m.getPtr();
//	f32 * src = (f32*)&a.a1;
//	for (int i = 0; i < 16; ++i)
//	{
//		dst[i] = src[i];
//	}
//	return m;
//}
//aiNode* aiFindNode(const char* name, aiNode* node)
//{
//	if (strcmp(node->mName.data, name) == 0)
//	{
//		return node;
//	}
//	for (int i = 0; i < node->mNumChildren; ++i)
//	{
//		auto n = aiFindNode(name, node->mChildren[i]);
//		if (n)
//			return n;
//	}
//	return 0;
//}

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

yyStringA AnimationCheckName(yyMDLObject* object, yyStringA name)
{
	static int number = 0;
	yyStringA newName = name;

check_again:;

	int count = 0;
	for (u16 i = 0; i < object->m_mdl->m_animations.size(); ++i)
	{
		auto a = object->m_mdl->m_animations[i];
		if (a->m_name == newName)
			++count;
	}
	if (count > 0)
	{
		newName += number++;
		goto check_again;
	}
	return newName;
}
yyStringA ModelCheckName(yyMDLObject* object, yyStringA name)
{
	static int number = 0;
	yyStringA newName = name;

check_again:;

	int count = 0;
	for (u16 i = 0; i < object->m_mdl->m_layers.size(); ++i)
	{
		auto layer = object->m_mdl->m_layers[i];
		if (layer->m_model->m_name == newName)
		{
			++count;
		}
	}
	if (count > 0)
	{
		newName += number++;
		goto check_again;
	}
	return newName;
}

//void aiPrintNames(aiNode* node, int depth)
//{
//	for (int i = 0; i < depth; ++i)
//	{
//		printf("  ");
//	}
//	if (node->mName.data)
//		printf("%s", node->mName.data);
//	else
//		printf("[noname]");
//	printf("\n");
//	for (int i = 0; i < node->mNumChildren; ++i)
//	{
//		aiPrintNames(node->mChildren[i], depth + 1);
//	}
//}

// load model, create new layer
void newLayerAssimp(yyMDLObject* object, const char16_t* file)
{
	yyStringA stra;
	stra = file;
	ImportAssimp(object, stra.data());
	g_selectedLayer = object->m_mdl->m_layers.size() - 1;
	object->SetMDL(object->m_mdl);
	// update MDL aabb
}
void newLayer(yyMDLObject* object, const char16_t* file)
{
	yyStringA stra;
	stra = file;

	yyFS::path path = file;
	auto ext = path.extension();
	
	if (ext.string_type == L".smd")
	{
		ImportSMD(object, stra.data());
	}
	else if (ext.string_type == L".obj")
	{
		ImportOBJ(object, stra.data());
	}
	else
	{
	}

	g_selectedLayer = object->m_mdl->m_layers.size()-1;
	object->SetMDL(object->m_mdl);
	// update MDL aabb
}
void deleteLayer(yyMDLObject* object)
{
	auto layer = object->m_mdl->m_layers[g_selectedLayer];
	object->m_mdl->m_layers.erase(g_selectedLayer);
	g_sceneObject->m_layerInfo.erase(g_selectedLayer);
	if (layer->m_meshGPU)
	{
		yyGetVideoDriverAPI()->DeleteModel(layer->m_meshGPU);
		layer->m_meshGPU = nullptr;
	}
	yyDestroy(layer);
	// update MDL aabb

	if (object->m_mdl->m_layers.size() == g_selectedLayer)
		--g_selectedLayer;
}
void reloadTexture(yyMDLObject* object, int textureSlot, const wchar_t* path)
{
	auto layer = object->m_mdl->m_layers[g_selectedLayer];

	if (!yyFS::exists(yyFS::path(path)))
	{
		printf("Not exist\n");
		if (layer->m_textureGPU[textureSlot])
		{
			yyGetVideoDriverAPI()->DeleteTexture(layer->m_textureGPU[textureSlot]);
			layer->m_textureGPU[textureSlot] = 0;
		}
		return;
	}


	yyResource* texture = 0;
	texture = layer->m_textureGPU[textureSlot];
	
	if (texture)
	{
		yyGetVideoDriverAPI()->DeleteTexture(texture);
	}

	yyStringA str;
	str = path;
	texture = yyGetVideoDriverAPI()->CreateTextureFromFile(str.data(), true, false, true);

	layer->m_textureGPU[textureSlot] = texture;
}
int main(int argc, char* argv[])
{
	bool useOpenGL = false;
	for (int i = 0; i < argc; ++i)
	{
		if (strcmp(argv[i], "opengl") == 0)
		{
			useOpenGL = true;
		}
	}

	yyPtr<yyInputContext> inputContext = yyCreate<yyInputContext>();
	g_inputContex = inputContext.m_data;

	yyPtr<yyEngineContext> engineContext = yyCreate<yyEngineContext>();
	engineContext.m_data->init(inputContext.m_data);
	auto p_engineContext = engineContext.m_data;

	g_renameTextBuffer.reserve(g_renameTextBufferSize);

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
	//p_window->ToFullscreenMode();

	// init video driver
	const char * videoDriverType = "d3d11.yyvd";
	if(useOpenGL)
		videoDriverType = "opengl.yyvd";
	if (!yyInitVideoDriver(videoDriverType, p_window))
	{
		YY_PRINT_FAILED;
		return 0;
	}
	g_videoDriver = yyGetVideoDriverAPI();
	g_videoDriver->SetClearColor(0.3f, 0.3f, 0.4f, 1.f);
	g_videoDriver->UseVSync(true);

	{
		yyStringA t;
		t = "MDLCreator";
		t += " - ";
		t += g_videoDriver->GetVideoDriverName();
		p_window->SetTitle(t.data());
	}

	yyPtr<SceneObject> new_model = yyCreate<SceneObject>();
	g_sceneObject = new_model.m_data;
	g_sceneObject->init();

	yyVideoDriverObjectD3D11* vdo = (yyVideoDriverObjectD3D11*)g_videoDriver->GetVideoDriverObjects();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(p_window->m_hWnd);
	if (useOpenGL)
	{
		gl3wInit();
		const char* glsl_version = "#version 330";
		ImGui_ImplOpenGL3_Init(glsl_version);
	}
	else
		ImGui_ImplDX11_Init((ID3D11Device*)vdo->m_device, (ID3D11DeviceContext*)vdo->m_context);
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_FrameBg] = ImVec4(0.24f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.85f, 0.85f, 0.85f, 0.67f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.88f, 0.88f, 0.88f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.40f, 0.65f, 1.00f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.80f, 0.80f, 0.80f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.73f, 0.73f, 0.73f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.76f, 0.76f, 0.76f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.76f, 0.77f, 0.77f, 0.80f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.87f, 0.87f, 0.87f, 0.20f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.67f);
	colors[ImGuiCol_Tab] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.75f, 0.75f, 0.75f, 0.80f);
	colors[ImGuiCol_TabActive] = ImVec4(0.56f, 0.57f, 0.59f, 1.00f);

	
	window.m_data->SetFocus();
	//window.m_data->ToFullscreenMode();
	
	//yyPtr<yyMDLObject> mdlObject = yyCreate<yyMDLObject>();
	//mdlObject.m_data->m_mdl = yyCreate<yyMDL>();

	auto defaultTexture = yyGetTextureResource("../res/textures/editor/white.dds", false, false, true);

	f32 deltaTime = 0.f;
	bool run = true;
	while (run)
	{		
		static u64 t1 = 0u;
		u64 t2 = yyGetTime();
		f32 m_tick = f32(t2 - t1);
		t1 = t2;
		deltaTime = m_tick / 1000.f;
		if (deltaTime > 1.f)
			deltaTime = 1.f;
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
		if (useOpenGL)
		{
			ImGui_ImplOpenGL3_NewFrame();
		}
		else
		{
			ImGui_ImplDX11_NewFrame();
		}
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (g_inputContex->m_isRMBHold)
		{
	//		printf("%f %f\n", io.MouseDelta.x, io.MouseDelta.y);
			camera.rotate(v2f(-g_inputContex->m_mouseDelta.x, -g_inputContex->m_mouseDelta.y), deltaTime);

			if (io.KeysDownDuration[(int)yyKey::K_W] >= 0.0f)
				camera.moveForward(deltaTime, io.KeyShift);

			if (io.KeysDownDuration[(int)yyKey::K_S] >= 0.0f)
				camera.moveBackward(deltaTime, io.KeyShift);

			if (io.KeysDownDuration[(int)yyKey::K_A] >= 0.0f)
				camera.moveLeft(deltaTime, io.KeyShift);

			if (io.KeysDownDuration[(int)yyKey::K_D] >= 0.0f)
				camera.moveRight(deltaTime, io.KeyShift);

			if (io.KeysDownDuration[(int)yyKey::K_E] >= 0.0f)
				camera.moveUp(deltaTime, io.KeyShift);

			if (io.KeysDownDuration[(int)yyKey::K_Q] >= 0.0f)
				camera.moveDown(deltaTime, io.KeyShift);

			auto cursorX = std::floor((f32)window.m_data->m_currentSize.x / 2.f);
			auto cursorY = std::floor((f32)window.m_data->m_currentSize.y / 2.f);
			g_inputContex->m_cursorCoordsOld.set(cursorX, cursorY);

			yySetCursorPosition(cursorX, cursorY, window.m_data);
		}
	//	printf("%f\n", deltaTime);
		camera.update();
		g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::ViewProjection, camera.m_camera->m_viewProjectionMatrix);
		

		
		ImGui::Begin("Main menu", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoTitleBar);
		ImGuiTabBarFlags mainemnuTab_bar_flags = ImGuiTabBarFlags_None;
		if (ImGui::BeginTabBar("menutab", mainemnuTab_bar_flags))
		{
			if (ImGui::BeginTabItem("File"))
			{
				if (ImGui::Button("Save"))
				{
					auto filePath = yySaveFileDialog("Save file", "Save", "MDL");
					if (filePath)
					{
						SaveMDL(filePath->to_stringA().data());
						yyDestroy(filePath);
					}
				}
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
		ImGui::Begin("Parameters", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoTitleBar);
		if (ImGui::BeginTabBar("parameterstab"))
		{
			if (ImGui::BeginTabItem("Layers"))
			{
				ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(255, 0, 0, 100));
				ImGui::BeginChild("ChildLayerList", ImVec2(ImGui::GetWindowContentRegionWidth(), 200), false, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoMove);
				yyStringA stra;
				for (int n = 0, nsz = g_sceneObject->m_mdlObject->m_mdl->m_layers.size(); n < nsz; ++n)
				{
					auto layer = g_sceneObject->m_mdlObject->m_mdl->m_layers[n];
					stra = layer->m_model->m_name.data();

					if (ImGui::Selectable(stra.data(), g_selectedLayer == n))
					{
						g_selectedLayer = n;
				//		printf("Select %i\n", g_selectedLayer);
					}
				}
				ImGui::EndChild();
				ImGui::PopStyleColor();
				if (ImGui::Button("New"))
				{
					auto path = yyOpenFileDialog("Import model", "Import", "smd obj", "Supported models");
					if (path)
					{
						newLayer(g_sceneObject->m_mdlObject, path->data());
						yyDestroy(path);
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Assimp (only static)"))
				{
					/*auto path = yyOpenFileDialog("Import model", "Import", "smd obj", "Supported models");
					if (path)
					{
						newLayerAssimp(g_sceneObject->m_mdlObject, path->data());
						yyDestroy(path);
					}*/
				}

				if (g_selectedLayer != -1)
				{
					auto & currentLayerInfo = g_sceneObject->m_layerInfo[g_selectedLayer];
					
					ImGui::SameLine();
					if (ImGui::Button("Rename"))
					{
						g_renameTextBuffer = g_sceneObject->m_mdlObject->m_mdl->m_layers[g_selectedLayer]->m_model->m_name.data();
						ImGui::OpenPopup("Rename");
					}
					ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
					ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
					if (ImGui::BeginPopupModal("Rename", NULL, ImGuiWindowFlags_AlwaysAutoResize))
					{
						ImGui::InputText("new name", g_renameTextBuffer.data(), g_renameTextBufferSize);
						ImGui::Separator();

						if (ImGui::Button("OK", ImVec2(120, 0))) 
						{
							g_sceneObject->m_mdlObject->m_mdl->m_layers[g_selectedLayer]->m_model->m_name = ModelCheckName(g_sceneObject->m_mdlObject, g_renameTextBuffer.data());
							ImGui::CloseCurrentPopup(); 
						}
						ImGui::SetItemDefaultFocus();
						ImGui::SameLine();
						if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
						ImGui::EndPopup();
					}
					ImGui::SameLine();
					ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 0, 0, 100));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 255, 0, 255));
					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 255, 255));
					if (ImGui::Button("Delete"))
					{
						deleteLayer(g_sceneObject->m_mdlObject);
					}
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					if (ImGui::CollapsingHeader("Position"))
					{
						ImGui::DragFloat("X", &currentLayerInfo.m_offset.x, 0.005f);
						ImGui::DragFloat("Y", &currentLayerInfo.m_offset.y, 0.005f);
						ImGui::DragFloat("Z", &currentLayerInfo.m_offset.z, 0.005f);
						ImGui::Separator();
					}
					if (ImGui::CollapsingHeader("Shader") && g_selectedLayer != -1)
					{
						const char* shaderCombo_items[] = { "Simple" };
						ImGui::Combo("Type", &g_shaderCombo_item_current, shaderCombo_items, IM_ARRAYSIZE(shaderCombo_items));

						if (ImGui::CollapsingHeader("Texture 0"))
						{
							ImGui::InputText("path", currentLayerInfo.m_texturePathTextBuffer.data(), currentLayerInfo.m_texturePathTextBufferSize);
							ImGui::SameLine();
							if (ImGui::Button("..."))
							{
								auto path = yyOpenFileDialog("Texture file", "Select", "png dds bmp tga", "Supported files");
								if (path)
								{
									g_sceneObject->m_mdlObject->m_mdl->m_layers[g_selectedLayer]->m_texturePath[0] = path->data();
									currentLayerInfo.m_texturePathTextBuffer = path->data();
									reloadTexture(g_sceneObject->m_mdlObject, 0, (wchar_t*)path->data());
									yyDestroy(path);
								}
							}
							if (ImGui::Button("Reload"))
							{
								g_sceneObject->m_mdlObject->m_mdl->m_layers[g_selectedLayer]->m_texturePath[0] = currentLayerInfo.m_texturePathTextBuffer.data();
								reloadTexture(g_sceneObject->m_mdlObject, 0, (wchar_t*)currentLayerInfo.m_texturePathTextBuffer.to_string().data());
							}
							if (g_sceneObject->m_mdlObject->m_mdl->m_layers[g_selectedLayer]->m_textureGPU[0])
							{
								auto th = yyGetVideoDriverAPI()->GetTextureHandle(g_sceneObject->m_mdlObject->m_mdl->m_layers[g_selectedLayer]->m_textureGPU[0]);
								ImTextureID my_tex_id = th;
								float my_tex_w = 64.f;
								float my_tex_h = 64.f;
								ImVec2 pos = ImGui::GetCursorScreenPos();
								ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h));
								if (ImGui::IsItemHovered())
								{
									ImGui::BeginTooltip();
									ImGui::Image(my_tex_id, ImVec2(256.f, 256.f));
									ImGui::EndTooltip();
								}
							}
							ImGui::Separator();
						}
						if (ImGui::CollapsingHeader("Texture 1"))
						{
							ImGui::Separator();
						}
						if (ImGui::CollapsingHeader("Texture 2"))
						{
							ImGui::Separator();
						}
						if (ImGui::CollapsingHeader("Texture 3"))
						{
							ImGui::Separator();
						}
					}
				}

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Animations"))
			{
				ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(255, 0, 0, 100));
				ImGui::BeginChild("ChildAnimationList", ImVec2(ImGui::GetWindowContentRegionWidth(), 200), false, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoMove);
				yyStringA stra;
				for (int n = 0, nsz = g_sceneObject->m_mdlObject->m_mdl->m_animations.size(); n < nsz; ++n)
				{
					auto animation = g_sceneObject->m_mdlObject->m_mdl->m_animations[n];
					stra = animation->m_name.data();
					if (ImGui::Selectable(stra.data(), g_selectedAnimation == n))
					{
						g_selectedAnimation = n;
						auto newState = g_sceneObject->getState(stra.data());
						if (newState)
						{
							g_sceneObject->m_mdlObject->SetState(newState);
						}
					}
				}
				ImGui::EndChild();
				ImGui::PopStyleColor();
				ImGui::Checkbox("Bind pose", &g_bindPose);
				if (g_selectedAnimation != -1)
				{
					ImGui::DragFloat("FPS:", &g_sceneObject->m_mdlObject->m_mdl->m_animations[g_selectedAnimation]->m_fps, 1.f, 1.f, 60.f);
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Hitboxes"))
			{
				ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(255, 0, 0, 100));
				ImGui::BeginChild("ChildHitboxList", ImVec2(ImGui::GetWindowContentRegionWidth(), 200), false, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoMove);
				for (int n = 0, nsz = g_sceneObject->m_hitboxes.size(); n < nsz; ++n)
				{
					auto hitbox = g_sceneObject->m_hitboxes[n];
					if (ImGui::Selectable(hitbox->m_name.data(), g_selectedHitbox == n))
					{
						g_selectedHitbox = n;
					}
				}
				ImGui::EndChild();
				ImGui::PopStyleColor();
				if (ImGui::Button("New"))
				{
					static int hitbox_counter = 0;
					Hitbox* newHitBox = yyCreate<Hitbox>();
					newHitBox->m_name += hitbox_counter++;
					newHitBox->rebuild();
					g_sceneObject->m_hitboxes.push_back(newHitBox);
				}
				if (g_selectedHitbox != -1)
				{
					ImGui::SameLine();
					ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 0, 0, 100));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 255, 0, 255));
					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 255, 255));
					if (ImGui::Button("Delete"))
					{
						g_sceneObject->m_hitboxes.erase(g_selectedHitbox);
						if (g_sceneObject->m_hitboxes.size() == g_selectedHitbox)
							--g_selectedHitbox;
					}
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					
					if (g_selectedHitbox != -1)
					{
						Hitbox* activeHitbox = g_sceneObject->m_hitboxes[g_selectedHitbox];
						bool reb = false;
						if (ImGui::SliderFloat("MinX", &activeHitbox->m_min.x, -1000.0f, -0.01f, "%.3f", ImGuiSliderFlags_Logarithmic ))reb = true;
						if (ImGui::SliderFloat("MinY", &activeHitbox->m_min.y, -1000.0f, -0.01f, "%.3f", ImGuiSliderFlags_Logarithmic))reb = true;
						if (ImGui::SliderFloat("MinZ", &activeHitbox->m_min.z, -1000.0f, -0.01f, "%.3f", ImGuiSliderFlags_Logarithmic))reb = true;
						if (ImGui::SliderFloat("MaxX", &activeHitbox->m_max.x, 0.01f, 1000.f, "%.3f", ImGuiSliderFlags_Logarithmic))reb = true;
						if (ImGui::SliderFloat("MaxY", &activeHitbox->m_max.y, 0.01f, 1000.f, "%.3f", ImGuiSliderFlags_Logarithmic))reb = true;
						if (ImGui::SliderFloat("MaxZ", &activeHitbox->m_max.z, 0.01f, 1000.f, "%.3f", ImGuiSliderFlags_Logarithmic))reb = true;
					
						if(reb)
							activeHitbox->rebuild();
						
						static ImVec4 color = ImVec4(255.0f, 255.0f, 0.0f, 255.0f);
						color.x = activeHitbox->m_material.m_baseColor.m_data[0];
						color.y = activeHitbox->m_material.m_baseColor.m_data[1];
						color.z = activeHitbox->m_material.m_baseColor.m_data[2];
						ImGui::ColorEdit3("HB color", (float*)&color);
						activeHitbox->m_material.m_baseColor.setRed(color.x);
						activeHitbox->m_material.m_baseColor.setGreen(color.y);
						activeHitbox->m_material.m_baseColor.setBlue(color.z);

						const char * label = " ";
						if (activeHitbox->m_hitbox.m_jointID != -1)
						{
							auto & joint = g_sceneObject->m_mdlObject->m_mdl->m_joints[activeHitbox->m_hitbox.m_jointID];
							label = joint->m_name.data();
						}
						if (ImGui::BeginCombo("cmbJnt", label))
						{
							static int item_current_idx = 0;
							for (int n = 0, nsz = g_sceneObject->m_mdlObject->m_mdl->m_joints.size(); n < nsz; n++)
							{
								auto jnt = g_sceneObject->m_mdlObject->m_mdl->m_joints[n];
								const bool is_selected = (item_current_idx == n);
								if (ImGui::Selectable(jnt->m_name.data(), is_selected))
								{
									item_current_idx = n;
									activeHitbox->m_hitbox.m_jointID = n;
								}

								if (is_selected)
									ImGui::SetItemDefaultFocus();
							}
							ImGui::EndCombo();
						}
					}
				}
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

			g_videoDriver->UseDepth(true);

			g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::Projection, camera.m_camera->m_projectionMatrix);
			g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::View, camera.m_camera->m_viewMatrix);

			
			bool drawBind = false;

			if (g_selectedAnimation == -1)
				drawBind = true;

			if (g_bindPose)
				drawBind = true;
			if (!drawBind)
			{
				g_sceneObject->update(deltaTime);
			}

			for (int n = 0, nsz = g_sceneObject->m_mdlObject->m_mdl->m_layers.size(); n < nsz; ++n)
			{
				auto layer = g_sceneObject->m_mdlObject->m_mdl->m_layers[n];
				g_videoDriver->SetModel(layer->m_meshGPU);

				Mat4 WorldMatrix;

				WorldMatrix = g_sceneObject->m_mdlObject->m_mdl->m_preRotation * WorldMatrix;
				WorldMatrix[3] = g_sceneObject->m_layerInfo[n].m_offset;
				WorldMatrix[3].w = 1.f;



				g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::World,  WorldMatrix);
				g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::WorldViewProjection, camera.m_camera->m_projectionMatrix * camera.m_camera->m_viewMatrix * WorldMatrix);
				
				for (u32 t = 0; t < YY_MDL_LAYER_NUM_OF_TEXTURES; ++t)
				{
					if(layer->m_textureGPU[t])
						g_videoDriver->SetTexture(t, layer->m_textureGPU[t]);
					else
						g_videoDriver->SetTexture(t, defaultTexture);
				}

			

				/*Mat4 R;
				R.setRotation(Quat(math::degToRad(90.f), 0.f, 0.f));*/

				auto numJoints = g_sceneObject->m_mdlObject->m_joints.size();
				if (numJoints)
				{
					if (drawBind)
					{
						for (int i = 0; i < numJoints; ++i)
						{
							auto & objJoint = g_sceneObject->m_mdlObject->m_joints[i];
							auto mdlJoint = g_sceneObject->m_mdlObject->m_mdl->m_joints[i];
							objJoint.m_finalTransformation.identity();
							objJoint.m_globalTransformation = mdlJoint->m_matrixWorld;
						}
					}
					else
					{
					}
					
					g_videoDriver->UseDepth(false);
					for (int i = 0; i < numJoints; ++i)
					{
						auto & objJoint = g_sceneObject->m_mdlObject->m_joints[i];
						auto mdlJoint = g_sceneObject->m_mdlObject->m_mdl->m_joints[i];

						auto matrix = g_sceneObject->m_mdlObject->m_mdl->m_preRotation * objJoint.m_globalTransformation;
						auto point = matrix[3];

						matrix[3].set(0.f, 0.f, 0.f, 1.f);

						auto distCamera = point.distance(camera.m_camera->m_objectBase.m_globalPosition);
						f32 jointSize = 0.02f * distCamera;

						g_videoDriver->DrawLine3D(
							point + math::mul(v4f(-jointSize, 0.f, 0.f, 0.f), matrix),
							point + math::mul(v4f(jointSize, 0.f, 0.f, 0.f), matrix), ColorRed);
						g_videoDriver->DrawLine3D(
							point + math::mul(v4f(.0f, -jointSize, 0.f, 0.f), matrix),
							point + math::mul(v4f(.0f, jointSize, 0.f, 0.f), matrix), ColorGreen);
						g_videoDriver->DrawLine3D(
							point + math::mul(v4f(0.0f, 0.f, -jointSize, 0.f), matrix),
							point + math::mul(v4f(0.0f, 0.f, jointSize, 0.f), matrix), ColorBlue);

						if (mdlJoint->m_parentIndex != -1)
						{
							auto matrixP = g_sceneObject->m_mdlObject->m_mdl->m_preRotation * g_sceneObject->m_mdlObject->m_joints[mdlJoint->m_parentIndex].m_globalTransformation;
							g_videoDriver->DrawLine3D(
								point,
								matrixP[3],
								ColorLime);
						}

						g_videoDriver->SetBoneMatrix(i, objJoint.m_finalTransformation);
					}
				}
				g_videoDriver->UseDepth(true);
				g_videoDriver->Draw();
				
				
				
				g_videoDriver->SetTexture(0, defaultTexture);
				for (int i = 0, sz = g_sceneObject->m_hitboxes.size(); i < sz; ++i)
				{
					auto hb = g_sceneObject->m_hitboxes[i];
					g_videoDriver->SetMaterial(&hb->m_material);
					Mat4 W;

					if (hb->m_hitbox.m_jointID != -1)
					{
						auto & joint = g_sceneObject->m_mdlObject->m_joints[hb->m_hitbox.m_jointID];
						W = g_sceneObject->m_mdlObject->m_mdl->m_preRotation * joint.m_globalTransformation;
					}

					g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::World, W);
					g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::WorldViewProjection, camera.m_camera->m_projectionMatrix * camera.m_camera->m_viewMatrix * W);
					g_videoDriver->SetModel(hb->m_gpuModel);
					g_videoDriver->Draw();
				}
				g_videoDriver->SetMaterial(0);
				g_videoDriver->UseDepth(true);
			}

			g_videoDriver->UseDepth(false);
			yyGUIDrawAll();
		//	g_videoDriver->DrawLine2D(v3f(50.f, 50.f, 0.f), v3f(264.f, 264.f, 0.f), ColorRed);


			g_videoDriver->UseDepth(true);

			g_videoDriver->EndDraw();
			ImGui::Render();
			if (useOpenGL)
			{
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			}
			else
			{
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			}

			g_videoDriver->SwapBuffers();

		}break;
		}
	}
	if (useOpenGL)
	{
		ImGui_ImplOpenGL3_Shutdown();
	}
	else
	{
		ImGui_ImplDX11_Shutdown();
	}
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	return 0;
}

void SaveMDL(const char* fileName)
{
	yyFS::path p = fileName;
	if (!p.has_extension())
	{
		p.string_type += ".mdl";
	}

	FILE * f = fopen(p.string_type.to_stringA().data(), "wb");
	if (!f)
	{
		YY_PRINT_FAILED;
		return;
	}

	fwrite("ymdl", 4, 1, f);

	u32 version = YY_MDL_VERSION;
	fwrite(&version, sizeof(u32), 1, f);

	yyMDLHeader mdl_header;
	memset(&mdl_header, 0, sizeof(yyMDLHeader));
	mdl_header.m_numOfLayers = g_sceneObject->m_mdlObject->m_mdl->m_layers.size();
	mdl_header.m_numOfJoints = g_sceneObject->m_mdlObject->m_mdl->m_joints.size();
	mdl_header.m_numOfAnimations = g_sceneObject->m_mdlObject->m_mdl->m_animations.size();
	mdl_header.m_numOfHitboxes = g_sceneObject->m_mdlObject->m_mdl->m_hitboxes.size();
	fwrite(&mdl_header, sizeof(yyMDLHeader), 1, f);
	
	std::vector<std::string> strings;
	for (u32 i = 0; i < mdl_header.m_numOfLayers; ++i)
	{
		auto layer = g_sceneObject->m_mdlObject->m_mdl->m_layers[i];

		yyMDLLayerHeader layerHeader;
		memset(&layerHeader, 0, sizeof(yyMDLLayerHeader));
		layerHeader.m_shaderType = (u32)layer->m_model->m_material.m_type;
		for (u32 p = 0; p < YY_MDL_LAYER_NUM_OF_TEXTURES; ++p)
		{
			if (layer->m_texturePath[p].size())
			{
				layerHeader.m_textureStrID[p] = strings.size();
				yyFS::path fspath = layer->m_texturePath[p].data();
				auto relPath = yyGetRelativePath(fspath.string_type.data());
				if (relPath)
				{
					strings.push_back(relPath->to_stringA().data());
					yyDestroy(relPath);
				}
			}
		}

		layerHeader.m_vertexCount = layer->m_model->m_vCount;
		layerHeader.m_vertexType = (u32)layer->m_model->m_vertexType;
		layerHeader.m_vertexDataSize = layer->m_model->m_stride * layer->m_model->m_vCount;
		layerHeader.m_indexCount = layer->m_model->m_iCount;
		layerHeader.m_indexType = (u32)layer->m_model->m_indexType;
		switch (layer->m_model->m_indexType)
		{
		default:
		case yyMeshIndexType::u16:
			layerHeader.m_indexDataSize = layerHeader.m_indexCount * sizeof(u16);
			break;
		case yyMeshIndexType::u32:
			layerHeader.m_indexDataSize = layerHeader.m_indexCount * sizeof(u32);
			break;
		}

		fwrite(&layerHeader, sizeof(yyMDLLayerHeader), 1, f);
		fwrite(layer->m_model->m_vertices, layerHeader.m_vertexDataSize, 1, f);
		fwrite(layer->m_model->m_indices, layerHeader.m_indexDataSize, 1, f);
	}
	for (u32 i = 0; i < mdl_header.m_numOfJoints; ++i)
	{
		auto joint = g_sceneObject->m_mdlObject->m_mdl->m_joints[i];
		yyMDLJointHeader jointHeader;
		jointHeader.m_matrixBindInverse = joint->m_matrixBindInverse;
		jointHeader.m_matrixOffset = joint->m_matrixOffset;
		jointHeader.m_matrixWorld = joint->m_matrixWorld;
		jointHeader.m_parentID = joint->m_parentIndex;
		jointHeader.m_nameStrID = strings.size();
		strings.push_back(joint->m_name.data());
		fwrite(&jointHeader, sizeof(yyMDLJointHeader), 1, f);
	}
	for (u32 i = 0; i < mdl_header.m_numOfAnimations; ++i)
	{
		auto animation = g_sceneObject->m_mdlObject->m_mdl->m_animations[i];
		yyMDLAnimationHeader animHeader;
		animHeader.m_fps = animation->m_fps;
		animHeader.m_length = animation->m_len;
		animHeader.m_nameStrID = strings.size();
		strings.push_back(animation->m_name.data());
		animHeader.m_numOfAnimatedJoints = animation->m_animatedJoints.size();
		fwrite(&animHeader, sizeof(yyMDLAnimationHeader), 1, f);

		for (u32 o = 0; o < animHeader.m_numOfAnimatedJoints; ++o)
		{
			auto animJoint = animation->m_animatedJoints[o];
			yyMDLAnimatedJointHeader animJointHeader;
			animJointHeader.m_jointID = animJoint->m_jointID;
			animJointHeader.m_numOfKeyFrames = animJoint->m_animationFrames.m_keyFrames.size();
			fwrite(&animJointHeader, sizeof(yyMDLAnimatedJointHeader), 1, f);

			for (u32 k = 0; k < animJointHeader.m_numOfKeyFrames; ++k)
			{
				auto & keyFrame = animJoint->m_animationFrames.m_keyFrames[k];
				yyMDLJointKeyframeHeader keyFrameHeader;
				keyFrameHeader.m_time = keyFrame.m_time;
				keyFrameHeader.m_position = keyFrame.m_position;
				keyFrameHeader.m_rotation = keyFrame.m_rotation;
				fwrite(&keyFrameHeader, sizeof(yyMDLJointKeyframeHeader), 1, f);
			}
		}
	}
	for (u32 i = 0; i < mdl_header.m_numOfHitboxes; ++i)
	{
		auto hitbox = g_sceneObject->m_mdlObject->m_mdl->m_hitboxes[i];
		yyMDLHitboxHeader hitboxHeader;
		hitboxHeader.m_type = (u32)hitbox->m_hitboxType;
		hitboxHeader.m_jointID = hitbox->m_jointID;
		if (hitbox->m_mesh)
		{
			hitboxHeader.m_vertexCount = hitbox->m_mesh->m_vCount;
			hitboxHeader.m_indexCount = hitbox->m_mesh->m_iCount;
		}
		fwrite(&hitboxHeader, sizeof(yyMDLHitboxHeader), 1, f);
		if (hitbox->m_mesh)
		{
			fwrite(hitbox->m_mesh->m_vertices, hitbox->m_mesh->m_vCount * sizeof(yyVertexModel), 1, f);
			fwrite(hitbox->m_mesh->m_indices, hitbox->m_mesh->m_iCount * sizeof(u16), 1, f);
		}
	}

	mdl_header.m_stringsOffset = ftell(f);

	u32 numOfStrings = strings.size();
	fwrite(&numOfStrings, sizeof(u32), 1, f);
	for (u32 i = 0; i < numOfStrings; ++i)
	{
		auto & s = strings[i];
		for (size_t p = 0, sz = s.size(); p < sz; ++p)
		{
			fwrite(&(s.data())[p], 1, 1, f);
		}
		char nul = 0;
		fwrite(&nul, 1, 1, f);
	}

	fseek(f, 8, SEEK_SET);
	fwrite(&mdl_header, sizeof(yyMDLHeader), 1, f);
	
	fclose(f);
}