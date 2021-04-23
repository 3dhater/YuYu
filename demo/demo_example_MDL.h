#ifndef __DEMO_EXAMPLE_MDL_H__
#define __DEMO_EXAMPLE_MDL_H__

#include "scene\fly_camera.h"
#include "scene\mdl_object.h"

class DemoExample_MDL : public DemoExample
{
public:
	DemoExample_MDL();
	virtual ~DemoExample_MDL();

	virtual bool Init();
	virtual void Shutdown();
	virtual const wchar_t* GetTitle();
	virtual const wchar_t* GetDescription();

	virtual bool DemoStep(f32 deltaTime);

	// Загрузка MDL происходит через yuyu.dll (функция yyGetMDLFromCache).
	// yyMDL содержит слои\субмеши.
	// каждый слой имеет указатель на GPU ресурс для модели и для текстур
	//  эти указатели будут такими на протяжении всей работы программы
	//   по этому надо использовать m_gpu->Load... m_gpu->Unload...
	// слои имеют строки\пути на файлы текстуры
	// функция yyGetModel загружает модель и по желанию пользователя загружает текстуры
	// повторный вызов yyGetModel просто вернёт указатель и сделает ++m_refCount;
	// Функция yyLoadModel просто загрузит модель, без добавления в кеш.
	// Удалить модель нужно функцией yyDeleteModel. Сначала удалит из кеша, потом
	// если m_refCount == 0 то удалит из памяти. Если модель загружена через yyLoadModel
	//  то просто удалит (так-как m_refCount уже == 0)
	// При удалении, деструктор yyMDL вызовет m_gpu->UnloadTexture и m_gpu->UnloadModel
	yyMDL* m_mdl_struct;

	// Класс-обёртка который играет анимацию.
	yyMDLObject* m_mdl_object;
	yyMDL* m_mdl_hero;
	
	yyMDL* m_mdl_luger;

	yyMDLObject* m_mdl_playerGun;
	yyMDLObjectState* m_playerGunState_idle;
	yyMDLObjectState* m_playerGunState_idleempty;
	yyMDLObjectState* m_playerGunState_shot;
	yyMDLObjectState* m_playerGunState_shotlast;
	yyMDLObjectState* m_playerGunState_reload;
	s32 m_gunBulletMax;
	s32 m_gunBulletCurr;
	bool m_gunReady;
	yyMDLObjectState* m_playerGunState_curr;
	yyGUIText* m_guiTextBullets;

	yyFlyCamera* m_flyCamera;
	
	yyFlyCamera* m_handsCamera;
};

#endif

