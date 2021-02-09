#ifndef __MDLCREATOR_SCENEOBJECT_H_
#define __MDLCREATOR_SCENEOBJECT_H_

#include "yy_color.h"
#include "yy_model.h"

struct Hitbox
{
	Hitbox();
	~Hitbox();

	enum type
	{
		Box
	}m_type;

	void rebuild();


	yyMDLHitbox m_hitbox;
	yyResource* m_gpuModel;

	v3f m_min;
	v3f m_max;

	yyStringA m_name;
	yyMaterial m_material;
};

// У движка есть только примитивы для создания более сложных объектов.
// Например, тот объект который рисуется в редакторе не совсем простой. 
// У него есть слои - подмодели со своими текстурами и шейдерами.
// У объекта может быть анимация, много анимаций. Для каждой анимации
//    нужен свой yyMDLObjectState.
struct  SceneObject
{
	SceneObject();
	~SceneObject();
	bool init();
	void update(f32 dt);
	yyMDLObjectState* getState(const char* name);

	yyMDLObject* m_mdlObject;
	yyArraySmall<LayerInfo> m_layerInfo;
	yyArraySmall<yyMDLObjectState*> m_MDLObjectStates;
	yyArraySmall<Hitbox*> m_hitboxes;
};

#endif