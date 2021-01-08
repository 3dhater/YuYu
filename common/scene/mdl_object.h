#ifndef _YUYU_SCENE_MDL_H_
#define _YUYU_SCENE_MDL_H_

/*	Например, модель монстра одна, но монстров на сцене много.
	При создании монстра надо грузить модель только один раз, увеличивая ref count
	Соответственно, MDL загрузится один раз.
	Каждый монстр имеет какое-то своё состояние, и анимация может быть любая.
	Скорее всего правильнее в каждом объекте yyMDLObject (который содержит каждый монстр)
	 надо добавить массив (матрица\кватернион?) для анимации, и соответственно обновлять
	 анимацию монстра один раз за кадр
*/

void yyMDLObject_update(void * impl);
struct yyMDLObject
{
	yyMDLObject()
	{
		m_mdl = 0;
		m_objectBase.m_objectType = yySceneObjectBase::ObjectType::MDL;
		m_objectBase.m_implementationPtr = this;
		m_objectBase.m_updateImplementation = yyMDLObject_update; 
	}
	~yyMDLObject()
	{
		if (m_mdl)
		{
			// destroy or release
		}
	}
	void Update(float dt)
	{
	}
	yySceneObjectBase m_objectBase;

	yyMDL* m_mdl;

};

YY_FORCE_INLINE void yyMDLObject_update(void * impl)
{
	auto mdl_object = (yyMDLObject*)impl;

}


#endif