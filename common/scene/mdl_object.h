﻿#ifndef _YUYU_SCENE_MDL_H_
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
	//	m_isAnimated = false;
	}
	~yyMDLObject()
	{
		if (m_mdl)
		{
			// destroy or release like yyUnloadMDL
			yyDestroy(m_mdl);
		}
	}

	void Update(float dt)
	{
		static f32 time = 0.f;

		if (m_mdl->m_animations.size())
		{
			auto animation = m_mdl->m_animations[0];
			for (u16 i = 0, sz = animation->m_animatedJoints.size(); i < sz; ++i)
			{
				auto jointID = animation->m_animatedJoints[i]->m_jointID;
				auto mdlJoint = m_mdl->m_joints[jointID];
				auto & objJoint = m_joints[jointID];

				auto currentKey = animation->m_animatedJoints[i]->m_animationFrames.getCurrentKeyFrame((s32)time);
				auto nextKey = animation->m_animatedJoints[i]->m_animationFrames.getNextKeyFrame((s32)time);

				objJoint.m_position.x = math::lerp(objJoint.m_position.x, nextKey->m_position.x, 1.f);
				objJoint.m_position.y = math::lerp(objJoint.m_position.y, nextKey->m_position.y, 1.f);
				objJoint.m_position.z = math::lerp(objJoint.m_position.z, nextKey->m_position.z, 1.f);

				objJoint.m_rotation = math::slerp(objJoint.m_rotation, nextKey->m_rotation, 1.f);

				Mat4 TranslationM;
				TranslationM[3] = objJoint.m_position;
				TranslationM[3].w = 1.f;

				Mat4 RotationM;
				RotationM.setRotation(objJoint.m_rotation);

				Mat4 NodeTransformation = TranslationM * RotationM;

				if (mdlJoint->m_parentIndex != -1)
				{
					objJoint.m_globalTransformation =
						m_joints[mdlJoint->m_parentIndex].m_globalTransformation
						* NodeTransformation;
				}
				else
				{
					objJoint.m_globalTransformation = NodeTransformation;
				}

				objJoint.m_finalTransformation = /*m_mdl->m_preRotation * */objJoint.m_globalTransformation * mdlJoint->m_matrixOffset;
			}

			time += dt;
			if (time >= animation->m_len)
			{
				time = 0;
				for (u16 i = 0, sz = animation->m_animatedJoints.size(); i < sz; ++i)
				{
					auto jointID = animation->m_animatedJoints[i]->m_jointID;
					auto mdlJoint = m_mdl->m_joints[jointID];
					auto objJoint = m_joints[jointID];
					objJoint.m_position = mdlJoint->m_matrixBind[3];
					objJoint.m_rotation = math::matToQuat(mdlJoint->m_matrixBind);
				}
			}
		}

//		for (u16 i = 0, sz = m_joints.size(); i < sz; ++i)
//		{
//			auto mdlJoint = m_mdl->m_joints[i];
//			auto objJoint = m_joints[i];
////			objJoint.m_finalTransformation = m_GlobalInverseTransform *
////				objJoint.m_globalTransformation * mdlJoint->m_matrixOffset;
//		}

	}

	// при проигрывании анимации состояние каждого джоинта надо запомнить
	struct JointInfo
	{
		v4f m_position;
		Quat m_rotation;

		Mat4 m_globalTransformation;
		Mat4 m_finalTransformation;
	};
	yyArraySmall<JointInfo> m_joints; // должна быть в соответствии с m_mdl->m_joints
										// Устанавливать mdl надо через SetMDL

	// есть ли m_joints
	//bool m_isAnimated;

	yyMDL* m_mdl;

	// никакого удаления предидущего m_mdl нет, надо следить за этим самому
	void SetMDL(yyMDL* mdl)
	{
		m_mdl = mdl;

		m_joints.clear();
		m_joints.reserve(100);
		for (u16 i = 0, sz = m_mdl->m_joints.size(); i < sz; ++i)
		{
			auto j = m_mdl->m_joints[i];
			JointInfo ji;
			ji.m_position = j->m_matrixBind[3];
			ji.m_rotation = math::matToQuat(j->m_matrixBind);
			m_joints.push_back(ji);
		}
	}



	Mat4 m_GlobalInverseTransform;
	yySceneObjectBase m_objectBase;
};

YY_FORCE_INLINE void yyMDLObject_update(void * impl)
{
	auto mdl_object = (yyMDLObject*)impl;

}


#endif