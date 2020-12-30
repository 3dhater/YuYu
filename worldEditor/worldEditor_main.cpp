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
#include "yy_gameInfo.h"

#include "scene/common.h"
#include "scene/camera.h"
#include "scene/sprite.h"
#include "scene/terrain.h"

#include "Viewport.h"

#include "KrGui.h"
using namespace Kr;

#include "WindowParameters.h"

int g_terGen_sectorsX = 1;
int g_terGen_sectorsY = 1;

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

bool textInputFilter_all(char16_t ch)
{
	return true;
}
bool textInputFilter_float(char16_t ch)
{
	switch (ch)
	{
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '.':
	case '-':
		return true;
	default:
		break;
	}
	return false;
}

void window_onCLose(yyWindow* window)
{
	window->Hide();
	yyQuit(); // change yySystemState - set yySystemState::Quit
}
void terrainGen_onCLose(yyWindow* window)
{
	window->Hide();
}
// you can implement what you want (write to file, write to game console)
void log_onError(const char* message)
{
	fprintf(stderr, message);
}
void log_onInfo(const char* message)
{
	fprintf(stdout, message);
}

yyVideoDriverAPI* g_videoDriver = nullptr;
void asyncLoadEventHandler(u32 userIndex, void* data)
{
	switch (userIndex)
	{
	default:
		break;
	case 1:
	{
	}break;
	case 2:
	{
	}break;
	}
}

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

void window_callbackKeyboard(yyWindow*, bool isPress, u32 key, char16_t character)
{
	if (isPress)
	{
		Kr::Gui::GuiSystem::m_character = character;
		switch ((yyKey)key)
		{
		case yyKey::K_A:
			Kr::Gui::GuiSystem::m_IsA = true;
			break;
		case yyKey::K_X:
			Kr::Gui::GuiSystem::m_IsX = true;
			break;
		case yyKey::K_C:
			Kr::Gui::GuiSystem::m_IsC = true;
			break;
		case yyKey::K_V:
			Kr::Gui::GuiSystem::m_IsV = true;
			break;
		case yyKey::K_LEFT:
			Kr::Gui::GuiSystem::m_IsLeft = true;
			break;
		case yyKey::K_RIGHT:
			Kr::Gui::GuiSystem::m_IsRight = true;
			break;
		case yyKey::K_DELETE:
			Kr::Gui::GuiSystem::m_IsDelete = true;
			break;
		case yyKey::K_BACKSPACE:
			Kr::Gui::GuiSystem::m_IsBackspace = true;
			break;
		case yyKey::K_HOME:
			Kr::Gui::GuiSystem::m_IsHome = true;
			break;
		case yyKey::K_END:
			Kr::Gui::GuiSystem::m_IsEnd = true;
			break;
		default:
			break;
		}

		if (key < 256)
			g_inputContex->m_key_hold[key] = 1;
	}
	else
	{
		if (key < 256)
			g_inputContex->m_key_hold[key] = 0;
	}
}

int main()
{

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
	if (!window.m_data)
	{
		YY_PRINT_FAILED;
		return 1;
	}
	auto p_window = window.m_data;

	

	RECT rc;
	GetWindowRect(GetDesktopWindow(), &rc);
	//rc.left = 0; rc.top = 0; rc.right = 800; rc.bottom = 600;
	if (!p_window->init(rc.right - rc.left, rc.bottom - rc.top, 0))
	{
		YY_PRINT_FAILED;
		return 1;
	}

	p_window->m_onClose = window_onCLose;
	p_window->m_onMouseButton = window_callbackMouse;
	p_window->m_onKeyboard = window_callbackKeyboard;

	yyListFast<int> lf;
	auto node_1 = lf.push_front(1);
	auto node_2 = lf.push_front(2);
	auto node_3 = lf.push_back(3);

	lf.erase_node(node_2);

	// init video driver
	const char * videoDriverType = "opengl.yyvd";
	if (!yyInitVideoDriver(videoDriverType, p_window))
	{
		YY_PRINT_FAILED;
		return 0;
	}
	g_videoDriver = yyGetVideoDriverAPI();
	g_videoDriver->SetClearColor(0.3f, 0.3f, 0.4f, 1.f);

	f32 te_sphere_radius = 1.f;
	Mat4 te_sphere_matrix;
	yyMaterial te_sphere_material;
	te_sphere_material.m_cullBackFace = true;
	te_sphere_material.m_wireframe = true;
	auto te_sphere_modelGPU = g_videoDriver->CreateModelFromFile("../res/models/editor/te_sphere.tr3d", true );
	auto white_texture_GPU = g_videoDriver->CreateTextureFromFile("../res/textures/editor/white.dds", true, false, true);
	auto red_texture_GPU  = g_videoDriver->CreateTextureFromFile("../res/textures/editor/red.dds", true, false, true);
	auto grassGPU = g_videoDriver->CreateTextureFromFile("../res/tr/grass.dds", true, false, true);

	yyPtr<Viewport> viewport = yyCreate<Viewport>();
	if (!viewport.m_data)
	{
		YY_PRINT_FAILED;
		return 1;
	}
	viewport.m_data->init(window.m_data);

	window.m_data->SetFocus();

	yyTerrain* newTerrain = 0;

	if (yyFS::exists("../res/world/terrain.xml"))
	{
		newTerrain = yyCreate<yyTerrain>();
		newTerrain->Load("../res/world/terrain.xml");
	}

	Gui::GuiSystem* gui = Gui::CreateSystem(Gui::GraphicsSystemType::OpenGL3);
	Gui::Window guiWindow_main;
	guiWindow_main.OSWindow = window.m_data->GetHandle();


	yyRay cursorRay;
	v4f intersectionPoint;
	v4f intersectionPoint2;
	bool onEdit = false;
	bool draw_te_sphere = false;


	enum EDITOR_MODE
	{
		EditorMode_DEFAULT,
		EditorMode_TERRAIN_EDITOR,
		EditorMode_TERRAIN_GENERATOR,
		EditorMode_NEW_ENTITY_SELECTOR,
		EditorMode_CREATE_ENTITY,
		EditorMode_ENTITY_PARAMETERS,
	};
	EDITOR_MODE editorMode = EditorMode_DEFAULT;

	yyPtr<yyGameInfo> game_info = yyCreate<yyGameInfo>();
	if (!game_info.m_data->Init())
	{
		YY_PRINT_FAILED;
		return 1;
	}
	if (yyFS::exists("../res/world/map_info.xml"))
	{
		game_info.m_data->clear();
		game_info.m_data->load();
	}

	s32 newEntityID = -1;
	yyListNode<yyGameEntity*>* pickedEntity = nullptr;

	bool cloneEntity = false;

	f32 mainMenuHeight = 0.f;
	bool cursorInGUI = false;
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
			cursorRay.createFrom2DCoords(g_inputContex->m_cursorCoords, 
				v4f(0, 0, window.m_data->m_clientSize.x, window.m_data->m_clientSize.y ),
				v2f((f32)window.m_data->m_clientSize.x, (f32)window.m_data->m_clientSize.y),
				viewport.m_data->m_activeCamera->m_camera->m_viewProjectionInvertMatrix
				);

			if (g_inputContex->m_isRMBHold)
			{

				viewport.m_data->m_activeCamera->rotate(g_inputContex->m_mouseDelta, deltaTime);
				if (g_inputContex->isKeyHold(yyKey::K_LSHIFT) || g_inputContex->isKeyHold(yyKey::K_RSHIFT))
					viewport.m_data->m_activeCamera->m_moveSpeed = viewport.m_data->m_activeCamera->m_moveSpeedDefault * 15.f;
				else
					viewport.m_data->m_activeCamera->m_moveSpeed = viewport.m_data->m_activeCamera->m_moveSpeedDefault;

				if (g_inputContex->isKeyHold(yyKey::K_W))
				{
					viewport.m_data->m_activeCamera->moveForward(deltaTime);
				}
				if (g_inputContex->isKeyHold(yyKey::K_S))
				{
					viewport.m_data->m_activeCamera->moveBackward(deltaTime);
				}
				if (g_inputContex->isKeyHold(yyKey::K_A))
				{
					viewport.m_data->m_activeCamera->moveLeft(deltaTime);
				}
				if (g_inputContex->isKeyHold(yyKey::K_D))
				{
					viewport.m_data->m_activeCamera->moveRight(deltaTime);
				}
				if (g_inputContex->isKeyHold(yyKey::K_E))
				{
					viewport.m_data->m_activeCamera->moveUp(deltaTime);
				}
				if (g_inputContex->isKeyHold(yyKey::K_Q))
				{
					viewport.m_data->m_activeCamera->moveDown(deltaTime);
				}

				auto cursorX = std::floor((f32)window.m_data->m_clientSize.x / 2.f);
				auto cursorY = std::floor((f32)window.m_data->m_clientSize.y / 2.f);
				g_inputContex->m_cursorCoordsOld.set(cursorX, cursorY);

				yySetCursorPosition(cursorX, cursorY, window.m_data);
			}


			cursorInGUI = false;
			g_videoDriver->BeginDrawClearAll();

			if (editorMode == EditorMode_DEFAULT && pickedEntity)
			{
				if (g_inputContex->isKeyHold(yyKey::K_LSHIFT) || g_inputContex->isKeyHold(yyKey::K_RSHIFT))
				{
					if (g_inputContex->isKeyHold(yyKey::K_LEFT)
						|| g_inputContex->isKeyHold(yyKey::K_RIGHT)
						|| g_inputContex->isKeyHold(yyKey::K_UP)
						|| g_inputContex->isKeyHold(yyKey::K_DOWN)
						|| g_inputContex->isKeyHold(yyKey::K_PGDOWN)
						|| g_inputContex->isKeyHold(yyKey::K_PGUP))
					{
						if (!cloneEntity)
						{
							cloneEntity = true;
							pickedEntity = game_info.m_data->cloneEntity(pickedEntity);
						}
					}
				}
				else
				{
					cloneEntity = false;
				}

				if (window.m_data->IsInFocus())
				{
					// перемещение энтити
					f32 sp = 10.f * deltaTime;
					if (g_inputContex->isKeyHold(yyKey::K_LALT))
						sp *= 0.1f;

					if (g_inputContex->isKeyHold(yyKey::K_LEFT))
						pickedEntity->m_data->m_entityPosition.x -= sp;
					if (g_inputContex->isKeyHold(yyKey::K_RIGHT))
						pickedEntity->m_data->m_entityPosition.x += sp;
					if (g_inputContex->isKeyHold(yyKey::K_DOWN))
						pickedEntity->m_data->m_entityPosition.z -= sp;
					if (g_inputContex->isKeyHold(yyKey::K_UP))
						pickedEntity->m_data->m_entityPosition.z += sp;
					if (g_inputContex->isKeyHold(yyKey::K_PGDOWN))
						pickedEntity->m_data->m_entityPosition.y -= sp;
					if (g_inputContex->isKeyHold(yyKey::K_PGUP))
						pickedEntity->m_data->m_entityPosition.y += sp;

					sp = 30.f * deltaTime;
					if (g_inputContex->isKeyHold(yyKey::K_LALT))
						sp *= 0.01f;
					if (g_inputContex->isKeyHold(yyKey::K_NUM_7))
					{
						pickedEntity->m_data->m_base.SetRotation(pickedEntity->m_data->m_base.m_rotation + v4f(sp * 0.01, 0.f, 0.f, 0.f));
						pickedEntity->m_data->m_base.UpdateBase();
					}
					if (g_inputContex->isKeyHold(yyKey::K_NUM_4))
					{
						pickedEntity->m_data->m_base.SetRotation(pickedEntity->m_data->m_base.m_rotation - v4f(sp * 0.01, 0.f, 0.f, 0.f));
						pickedEntity->m_data->m_base.UpdateBase();
					}
					if (g_inputContex->isKeyHold(yyKey::K_NUM_8))
					{
						pickedEntity->m_data->m_base.SetRotation(pickedEntity->m_data->m_base.m_rotation + v4f(0.f, sp * 0.01, 0.f, 0.f));
						pickedEntity->m_data->m_base.UpdateBase();
					}
					if (g_inputContex->isKeyHold(yyKey::K_NUM_5))
					{
						pickedEntity->m_data->m_base.SetRotation(pickedEntity->m_data->m_base.m_rotation - v4f(0.f, sp * 0.01, 0.f, 0.f));
						pickedEntity->m_data->m_base.UpdateBase();
					}
					if (g_inputContex->isKeyHold(yyKey::K_NUM_9))
					{
						pickedEntity->m_data->m_base.SetRotation(pickedEntity->m_data->m_base.m_rotation + v4f(0.f, 0.f, sp * 0.01, 0.f));
						pickedEntity->m_data->m_base.UpdateBase();
					}
					if (g_inputContex->isKeyHold(yyKey::K_NUM_6))
					{
						pickedEntity->m_data->m_base.SetRotation(pickedEntity->m_data->m_base.m_rotation - v4f(0.f, 0.f, sp * 0.01, 0.f));
						pickedEntity->m_data->m_base.UpdateBase();
					}

					if (g_inputContex->isKeyHold(yyKey::K_DELETE))
					{
						game_info.m_data->deleteEntity(pickedEntity);
						pickedEntity = nullptr;
					}
					else if (g_inputContex->isKeyHold(yyKey::K_ESCAPE))
						pickedEntity = nullptr;
				}
				
			}
			if (editorMode == EditorMode_CREATE_ENTITY)
			{
				if (g_inputContex->isKeyHold(yyKey::K_ESCAPE))
				{
					editorMode = EditorMode_DEFAULT;
				}
				else
				{
					if (g_inputContex->m_isLMBDown)
					{
						// add entity
						editorMode = EditorMode_DEFAULT;
						pickedEntity = game_info.m_data->addEntity(newEntityID, intersectionPoint);
					}
				}
			}

			g_videoDriver->UseDepth(true);

			// UPDATE CAMERA
		//	viewport.m_data->m_activeCamera->m_camera->m_objectBase.SetRotation(
	//			viewport.m_data->m_activeCamera->m_camera->m_objectBase.m_rotation +
//				v4f(0.2f * deltaTime, 0.f, 0.f, 0.f)
			//);
			//printf("%f\n", math::radToDeg( viewport.m_data->m_activeCamera->m_camera->m_objectBase.m_rotation.x) );
			viewport.m_data->m_activeCamera->update();
			g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::Projection, viewport.m_data->m_activeCamera->m_camera->m_projectionMatrix);
			g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::View, viewport.m_data->m_activeCamera->m_camera->m_viewMatrix);
			g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::ViewProjection, viewport.m_data->m_activeCamera->m_camera->m_viewProjectionMatrix);

			g_videoDriver->DrawLine3D(v4f(-2.f, 0.f, 0.f, 0.f), v4f(2.f, 0.f, 0.f, 0.f), ColorRed);
			g_videoDriver->DrawLine3D(v4f(0.f, 0.f, -2.f, 0.f), v4f(0.f, 0.f, 2.f, 0.f), ColorRed);

			if (newTerrain)
			{
				newTerrain->OptimizeView(viewport.m_data->m_activeCamera->m_camera);

				auto inersectionSector = newTerrain->GetClosestIntersection(cursorRay, intersectionPoint, viewport.m_data->m_activeCamera->m_camera);

				for (u16 i = 0, sz = newTerrain->m_sectors.size(); i < sz; ++i)
				{
					auto sector = newTerrain->m_sectors[i];
					if (sector->m_visible && sector->m_modelGPU_forRender)
					{
						g_videoDriver->SetModel(sector->m_modelGPU_forRender);
						g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::WorldViewProjection, 
							viewport.m_data->m_activeCamera->m_camera->m_projectionMatrix * 
							viewport.m_data->m_activeCamera->m_camera->m_viewMatrix * 
							Mat4());
						
						g_videoDriver->SetMaterial(&sector->m_material);

						if (sector->m_material.m_wireframe && editorMode == EditorMode_TERRAIN_EDITOR)
						{
							g_videoDriver->SetTexture(yyVideoDriverAPI::TextureSlot::Texture0, white_texture_GPU);
							g_videoDriver->Draw();
						}

						sector->m_material.m_wireframe = false;
						sector->m_material.m_cullBackFace = true;
						g_videoDriver->SetMaterial(&sector->m_material);

						g_videoDriver->SetTexture(yyVideoDriverAPI::TextureSlot::Texture0, grassGPU);
						g_videoDriver->Draw();


						if (editorMode == EditorMode_TERRAIN_EDITOR)
						{
							if (g_inputContex->m_isLMBUp)
							{
								onEdit = false;
								newTerrain->SaveAfterEdit();
							}
							if (!onEdit)
							{
								//if (sector->isRayIntersect(cursorRay, intersectionPoint))
								if (inersectionSector == sector)
								{
									intersectionPoint2 = intersectionPoint;
									if (g_inputContex->m_isLMBDown)
									{
										newTerrain->FindEditableVerts(intersectionPoint, te_sphere_radius);
										onEdit = true;
									}
									draw_te_sphere = true;
								}
								else
									draw_te_sphere = false;
							}

							if(onEdit)
							{
								if (g_inputContex->m_mouseDelta.y != 0.f)
								{
									newTerrain->EditMove(g_inputContex->m_mouseDelta.y, deltaTime,
										g_inputContex->isKeyHold(yyKey::K_LALT) || g_inputContex->isKeyHold(yyKey::K_RALT));
								}
								intersectionPoint2.y -= g_inputContex->m_mouseDelta.y * deltaTime;
								g_videoDriver->DrawLine3D(intersectionPoint, intersectionPoint2,ColorRed);
							}

							if (draw_te_sphere || onEdit)
							{
								intersectionPoint.w = 1.f;
								te_sphere_matrix[3] = intersectionPoint;
								te_sphere_matrix[0].x = te_sphere_radius;
								te_sphere_matrix[1].y = te_sphere_radius;
								te_sphere_matrix[2].z = te_sphere_radius;
								g_videoDriver->SetModel(te_sphere_modelGPU);
								g_videoDriver->SetTexture(yyVideoDriverAPI::TextureSlot::Texture0, white_texture_GPU);
								g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::WorldViewProjection, viewport.m_data->m_activeCamera->m_camera->m_projectionMatrix * viewport.m_data->m_activeCamera->m_camera->m_viewMatrix * te_sphere_matrix);
								g_videoDriver->SetMaterial(&te_sphere_material);
								g_videoDriver->Draw();
							}
						}
					}
				}
			}

			yyListNode<yyGameEntity*>* hoverEntity = nullptr;
			// draw ents
			{
				auto ent_head = game_info.m_data->m_ents_in_scene.head();
				if (ent_head)
				{
					f32 len = FLT_MAX;
					auto ent_last = ent_head->m_left;
					while (true)
					{
						if (ent_head->m_data->m_modelForGPU)
						{
							if (viewport.m_data->
								m_activeCamera->
								m_camera->m_frustum.sphereInFrustum(
									ent_head->m_data->m_bv.m_radius,
									ent_head->m_data->m_entityPosition))
							{
								
								f32 out_len = 0.f;
								if (ent_head->m_data->m_modelForRayIntersection->isRayIntersect(cursorRay, intersectionPoint, out_len, ent_head->m_data->m_entityPosition, &ent_head->m_data->m_base.m_globalMatrix))
								{
									if (out_len < len)
									{
										len = out_len;
										hoverEntity = ent_head;
									}
								}

								Mat4 WorldMatrix = ent_head->m_data->m_base.m_globalMatrix;
								WorldMatrix[3].x = ent_head->m_data->m_entityPosition.x;
								WorldMatrix[3].y = ent_head->m_data->m_entityPosition.y;
								WorldMatrix[3].z = ent_head->m_data->m_entityPosition.z;
								g_videoDriver->SetModel(ent_head->m_data->m_modelForGPU);
								if(ent_head->m_data->m_textureGPU)
									g_videoDriver->SetTexture(yyVideoDriverAPI::TextureSlot::Texture0, ent_head->m_data->m_textureGPU);
								else
									g_videoDriver->SetTexture(yyVideoDriverAPI::TextureSlot::Texture0, red_texture_GPU);
								g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::WorldViewProjection, 
									viewport.m_data->m_activeCamera->m_camera->m_projectionMatrix * 
									viewport.m_data->m_activeCamera->m_camera->m_viewMatrix * 
									WorldMatrix);
								yyMaterial material;
								g_videoDriver->SetMaterial(&material);
								g_videoDriver->Draw();

								if (pickedEntity == ent_head)
								{
									material.m_wireframe = true;
									g_videoDriver->SetTexture(yyVideoDriverAPI::TextureSlot::Texture0, white_texture_GPU);
									g_videoDriver->SetMaterial(&material);
									g_videoDriver->Draw();
								}
							}
						}
						if (ent_head == ent_last)
						{
							break;
						}
						ent_head = ent_head->m_right;
					}
				}
			}

			if (hoverEntity)
			{
				if (editorMode == EditorMode_DEFAULT)
				{
					if (g_inputContex->m_isLMBDown)
					{
						pickedEntity = hoverEntity;
					}

					if (g_inputContex->m_isLMBDbl)
						editorMode = EditorMode_ENTITY_PARAMETERS;
				}
			}

			if (editorMode == EditorMode_CREATE_ENTITY)
			{
				intersectionPoint.w = 1.f;
				te_sphere_matrix[3] = intersectionPoint;
				te_sphere_matrix[0].x = te_sphere_radius;
				te_sphere_matrix[1].y = te_sphere_radius;
				te_sphere_matrix[2].z = te_sphere_radius;
				g_videoDriver->SetModel(te_sphere_modelGPU);
				g_videoDriver->SetTexture(yyVideoDriverAPI::TextureSlot::Texture0, white_texture_GPU);
				g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::WorldViewProjection, viewport.m_data->m_activeCamera->m_camera->m_projectionMatrix * viewport.m_data->m_activeCamera->m_camera->m_viewMatrix * te_sphere_matrix);
				g_videoDriver->SetMaterial(&te_sphere_material);
				g_videoDriver->Draw();
			}

			g_videoDriver->UseDepth(false);
			
			yyGUIDrawAll();

			gui->newFrame(&guiWindow_main, deltaTime);
			if (gui->menuBarBegin(true))
			{
				if (gui->menuBarMenu((const char16_t*)L"File"))
				{
					bool showMenu = true;
					if (gui->popupMenuBegin(&showMenu))
					{
						if (gui->addMenuItem((const char16_t*)L"Save", 0, 0))
						{
							game_info.m_data->save();
						}
						gui->addSeparator();
						if (gui->addMenuItem((const char16_t*)L"Exit", (const char16_t*)L"Alt+F4", 0))
						{
							yyQuit();
						}
						if (gui->isLastItemCursorHover() || gui->isLastItemCursorMove())
							cursorInGUI = true;

						if (gui->popupMenuEnd())
							cursorInGUI = true;
					}
				}
				if (gui->menuBarMenu((const char16_t*)L"Add"))
				{
					bool showMenu = true;
					if (gui->popupMenuBegin(&showMenu))
					{
						if (gui->addMenuItem((const char16_t*)L"Terrain", 0, 0))
							editorMode = EditorMode_TERRAIN_GENERATOR;
						if (gui->addMenuItem((const char16_t*)L"Entity", 0, 0))
							editorMode = EditorMode_NEW_ENTITY_SELECTOR;
						if (gui->isLastItemCursorHover() || gui->isLastItemCursorMove())
							cursorInGUI = true;
						if (gui->popupMenuEnd())
							cursorInGUI = true;
					}
				}
				if (gui->menuBarMenu((const char16_t*)L"Edit"))
				{
					bool showMenu = true;
					if (gui->popupMenuBegin(&showMenu))
					{
						if (gui->addMenuItem((const char16_t*)L"Edit terrain", 0, 0))
						{
							editorMode = EditorMode_TERRAIN_EDITOR;
						}
						if (gui->isLastItemCursorHover() || gui->isLastItemCursorMove())
							cursorInGUI = true;

						if (gui->popupMenuEnd())
							cursorInGUI = true;
					}
				}
				if (gui->menuBarEnd(&mainMenuHeight))
					cursorInGUI = true;
			}

			if (newTerrain)
			{
				if (editorMode == EditorMode_TERRAIN_EDITOR)
				{
					if (gui->addButton((const char16_t*)L"End edit terrain", 0, Gui::Vec2f(140.f, 20.f)))
					{
						editorMode = EditorMode_DEFAULT;
					}
					gui->newLine(5.f);
					gui->addRangeSlider(1.f, 100.f, &te_sphere_radius, Gui::Vec2f(100.f, 10.f));
					gui->addText(0, (const char16_t*)L"%f", te_sphere_radius);
				}
			}
			else
			{
				if (editorMode == EditorMode_TERRAIN_GENERATOR)
				{
					gui->addText("Add terrain");
					gui->newLine(10);
					gui->addText(0, (const char16_t*)L"1 sector = %ix%i",
						yyTerrainSectorSize* (s32)yyTerrainQuadSize,
						yyTerrainSectorSize* (s32)yyTerrainQuadSize);
					gui->newLine(10);
					gui->addText("X sectors: ");
					gui->addRangeSliderInt(1, 100, &g_terGen_sectorsX, Gui::Vec2f(100.f, 15.f));
					gui->addText(0, (const char16_t*)L"  %i", g_terGen_sectorsX);
					gui->newLine(10);
					gui->addText("Y sectors: ");
					gui->addRangeSliderInt(1, 100, &g_terGen_sectorsY, Gui::Vec2f(100.f, 15.f));
					gui->addText(0, (const char16_t*)L"  %i", g_terGen_sectorsY);
					gui->newLine(10);
					gui->addText(0, (const char16_t*)L"Size:  %i meters x %i meters",
						g_terGen_sectorsX * yyTerrainSectorSize * (s32)yyTerrainQuadSize,
						g_terGen_sectorsY * yyTerrainSectorSize * (s32)yyTerrainQuadSize);
					gui->newLine(50);
					if (gui->addButton((const char16_t*)L"Create", 0, Gui::Vec2f(50.f, 20.f)))
					{
						editorMode = EditorMode_DEFAULT;
						if (!newTerrain)
						{
							newTerrain = yyCreate<yyTerrain>();
							newTerrain->Generate(g_terGen_sectorsX, g_terGen_sectorsY);
						}
					}
					gui->moveLeftRight(50.f);
					if (gui->addButton((const char16_t*)L"Cancel", 0, Gui::Vec2f(50.f, 20.f)))
						editorMode = EditorMode_DEFAULT;
				}
			}

			if (editorMode == EditorMode_NEW_ENTITY_SELECTOR)
			{
				if (gui->beginGroup(Gui::Vec2f(400.f, 550.f)))
				{
					for (u16 i = 0, sz = game_info.m_data->m_ent_list.size(); i < sz; ++i)
					{
						auto ent = game_info.m_data->m_ent_list[i];
						if (gui->addButton(ent->m_name.c_str(), 0, Gui::Vec2f(400.f, 15.f)))
						{
							newEntityID = i;
							editorMode = EditorMode_CREATE_ENTITY;
						}
						gui->newLine();
					}
					gui->endGroup();
					gui->newLine(5.f);
				}
				gui->setDrawPosition(5.f, 580.f);
				if( gui->addButton((const char16_t*)L"Cancel", 0, Gui::Vec2f(50.f, 15.f)) )
					editorMode = EditorMode_DEFAULT;
			}

			gui->render();
			g_videoDriver->EndDraw();

			//if (showEntityListWindow && entityListWindow.m_data->m_visible)
			//{
			//	g_videoDriver->SetActiveWindow(entityListWindow.m_data);
			//	g_videoDriver->BeginDrawClearAll();
			//	gui->switchWindow(&guiWindow_entityList);
			//	{
			//		
			//	}
			//	gui->render();
			//	g_videoDriver->EndDraw();
			//}
			//if (windowParameters.m_data->m_window->m_visible)
			//{
			//	g_videoDriver->SetActiveWindow(windowParameters.m_data->m_window);
			//	g_videoDriver->BeginDrawClearAll();
			//	gui->switchWindow(&guiWindow_windowParameters);
			//	{
			//		if (gui->beginGroup(Gui::Vec2f(800.f, 550.f)))
			//		{
			//			if (pickedEntity)
			//			{
			//				gui->addText("Entity:   ");
			//				gui->addText(pickedEntity->m_data->m_name.data());
			//				gui->newLine(3.f);
			//				gui->addText("Position:  ");
			//				static yyString t_positionx; t_positionx.clear();
			//				static yyString t_positiony; t_positiony.clear();
			//				static yyString t_positionz; t_positionz.clear();
			//				t_positionx += pickedEntity->m_data->m_entityPosition.x;
			//				t_positiony += pickedEntity->m_data->m_entityPosition.y;
			//				t_positionz += pickedEntity->m_data->m_entityPosition.z;
			//				if (gui->addTextInput(t_positionx.data(), Gui::Vec2f(150.f, 15.f), textInputFilter_float))
			//				{
			//					t_positionx = gui->getTextInputResult();
			//					pickedEntity->m_data->m_entityPosition.x = util::to_float(t_positionx.data());
			//				}
			//				gui->moveLeftRight(153.f);
			//				if (gui->addTextInput(t_positiony.data(), Gui::Vec2f(150.f, 15.f), textInputFilter_float))
			//				{
			//					t_positiony = gui->getTextInputResult();
			//					pickedEntity->m_data->m_entityPosition.y = util::to_float(t_positiony.data());
			//				}
			//				gui->moveLeftRight(153.f);
			//				if (gui->addTextInput(t_positionz.data(), Gui::Vec2f(150.f, 15.f), textInputFilter_float))
			//				{
			//					t_positionz = gui->getTextInputResult();
			//					pickedEntity->m_data->m_entityPosition.z = util::to_float(t_positionz.data());
			//				}
			//				gui->newLine(3.f);
			//				gui->addText("Parameters:  ");
			//				gui->newLine(3.f);
			//				auto drawPosition = gui->getDrawPosition();
			//				for (u16 i = 0, sz = pickedEntity->m_data->m_parameters.size(); i < sz; ++i)
			//				{
			//					auto parameter = pickedEntity->m_data->m_parameters[i];
			//					gui->setDrawPosition(drawPosition.x, drawPosition.y);
			//					gui->addText(parameter->m_name.data());
			//					
			//					gui->setDrawPosition(drawPosition.x + 200.f, drawPosition.y);

			//					if (strcmp(parameter->m_type.data(), "string") == 0)
			//					{
			//						static yyString textStr;
			//						textStr = parameter->get_string();
			//						if (gui->addTextInput(textStr.data(), Gui::Vec2f(300.f, 15.f), textInputFilter_all))
			//						{
			//							static yyString textResult;
			//							textResult = gui->getTextInputResult();
			//							parameter->set_string(textResult.to_stringA().data());
			//						}
			//					}
			//					drawPosition.y += 18.f;
			//					drawPosition.x = 0.f;
			//				}
			//			}
			//			gui->endGroup();
			//		}
			//	}
			//	gui->render();
			//	g_videoDriver->EndDraw();
			//}

			gui->endFrame();
		}break;
		}
	}

	//delete gui;

	return 0;
}