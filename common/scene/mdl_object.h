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

/* Аналогично с yySpriteState, у yyMDLObject будет свой вариант.
	Для того чтобы включить анимацию нужно создать стейт. 
	Должна быть функция которая автоматически создаёт стейт для обычной анимации.
*/
struct yyMDLObjectStateNode {
	yyMDLObjectStateNode() {
		m_time = 0.f;
		m_animation = 0;
	}
	f32 m_time;
	yyMDLAnimation* m_animation;


	// анимацию играют только те джоинты, которые указаны в m_animatedJointss
	yyArraySmall<s32> m_animatedJoints;
};
struct yyMDLObjectState
{
	yyMDLObjectState() {}
	~yyMDLObjectState() 
	{
		for (u16 i = 0, sz = m_animations.size(); i < sz; ++i)
		{
			yyDestroy(m_animations[i]);
		}
	}

	yyStringA m_name;

	yyArraySmall<yyMDLObjectStateNode*> m_animations;
};

struct yyMDLObject
{
	yyMDLObject()
	{
		m_mdl = 0;
		m_currentState = 0;
		m_objectBase.m_objectType = yySceneObjectBase::ObjectType::MDL;
		m_objectBase.m_implementationPtr = this;
		m_objectBase.m_updateImplementation = yyMDLObject_update; 
	//	m_isAnimated = false;
	}
	~yyMDLObject()
	{
		if (m_mdl)
		{
			// destroy or release like yyUnloadMDL ???
			yyDestroy(m_mdl);
		}
	}


	void Update(float dt)
	{
		if (m_currentState)
		{
			for (u16 ai = 0, aisz = m_currentState->m_animations.size(); ai < aisz; ++ai)
			{
				auto & currentAnimation = m_currentState->m_animations.at(ai);
				f32 fps_factor = currentAnimation->m_animation->m_fps * dt;

				for (u16 i = 0, sz = currentAnimation->m_animatedJoints.size(); i < sz; ++i)
				{
					auto jointID = currentAnimation->m_animatedJoints[i];
					auto animatedJoint = currentAnimation->m_animation->m_animatedJoints[jointID];
					auto mdlJoint = m_mdl->m_joints[animatedJoint->m_jointID];
					auto & objJoint = m_joints[animatedJoint->m_jointID];

					auto currentKey = animatedJoint->m_animationFrames.getCurrentKeyFrame((s32)currentAnimation->m_time);
					auto nextKey = animatedJoint->m_animationFrames.getNextKeyFrame((s32)currentAnimation->m_time);

					auto timeSize = (f32)(nextKey->m_time - currentKey->m_time);
					auto timeLeft = (f32)nextKey->m_time - currentAnimation->m_time;
					auto timeCoef = 1.f - math::get_0_1(timeSize, timeLeft);
					timeCoef *= fps_factor;

					f32 interpolation_factor = timeCoef;

					objJoint.m_position.x = math::lerp(objJoint.m_position.x, nextKey->m_position.x, interpolation_factor);
					objJoint.m_position.y = math::lerp(objJoint.m_position.y, nextKey->m_position.y, interpolation_factor);
					objJoint.m_position.z = math::lerp(objJoint.m_position.z, nextKey->m_position.z, interpolation_factor);
					objJoint.m_rotation   = math::slerp(objJoint.m_rotation, nextKey->m_rotation, interpolation_factor);

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
					objJoint.m_finalTransformation =
						objJoint.m_globalTransformation *  mdlJoint->m_matrixOffset;// *mdlJoint->m_matrixBindInverse;
				}
				currentAnimation->m_time += fps_factor;

				if (currentAnimation->m_time >= currentAnimation->m_animation->m_len)
				{
					currentAnimation->m_time = 0;
					for (u16 i = 0, sz = currentAnimation->m_animation->m_animatedJoints.size(); i < sz; ++i)
					{
						auto jointID = currentAnimation->m_animation->m_animatedJoints[i]->m_jointID;
						auto mdlJoint = m_mdl->m_joints[jointID];
						auto objJoint = m_joints[jointID];
						objJoint.m_position = mdlJoint->m_matrixWorld[3];
						objJoint.m_rotation = math::matToQuat(mdlJoint->m_matrixWorld);
					}
				}
			}

			//for (u16 ai = 0, aisz = m_currentState->m_animations.size(); ai < aisz; ++ai)
			//{
			//	auto & animation = m_currentState->m_animations.at(ai);
			////	printf("Time: [%f] Len: [%f] DT: [%f]\n", m_time, animation->m_len, dt);

			//	f32 fps_factor = animation.m_animation->m_fps * dt;
			//	for (u16 i = 0, sz = animation.m_animation->m_animatedJoints.size(); i < sz; ++i)
			//	{
			//		auto jointID = animation.m_animation->m_animatedJoints[i]->m_jointID;
			//		auto mdlJoint = m_mdl->m_joints[jointID];
			//		auto & objJoint = m_joints[jointID];

			//		auto currentKey = animation.m_animation->m_animatedJoints[i]->m_animationFrames.getCurrentKeyFrame((s32)animation.m_time);
			//		auto nextKey = animation.m_animation->m_animatedJoints[i]->m_animationFrames.getNextKeyFrame((s32)animation.m_time);

			//		auto timeSize = (f32)(nextKey->m_time - currentKey->m_time);
			//		auto timeLeft = (f32)nextKey->m_time - animation.m_time;
			//		auto timeCoef = 1.f - math::get_0_1(timeSize, timeLeft );
			//		timeCoef *= fps_factor;


			//		f32 interpolation_factor = timeCoef;

			//		objJoint.m_position.x = math::lerp(objJoint.m_position.x, nextKey->m_position.x, interpolation_factor);
			//		objJoint.m_position.y = math::lerp(objJoint.m_position.y, nextKey->m_position.y, interpolation_factor);
			//		objJoint.m_position.z = math::lerp(objJoint.m_position.z, nextKey->m_position.z, interpolation_factor);

			//		objJoint.m_rotation = math::slerp(objJoint.m_rotation, nextKey->m_rotation, interpolation_factor);

			//		Mat4 TranslationM;
			//		TranslationM[3] = objJoint.m_position;
			//		TranslationM[3].w = 1.f;

			//		Mat4 RotationM;
			//		RotationM.setRotation(objJoint.m_rotation);

			//		Mat4 NodeTransformation = TranslationM * RotationM;

			//		if (mdlJoint->m_parentIndex != -1)
			//		{
			//			objJoint.m_globalTransformation =
			//				m_joints[mdlJoint->m_parentIndex].m_globalTransformation
			//				* NodeTransformation;
			//		}
			//		else
			//		{
			//			objJoint.m_globalTransformation = NodeTransformation;
			//		}

			//		objJoint.m_finalTransformation =

			//		objJoint.m_globalTransformation *  mdlJoint->m_matrixOffset;// *mdlJoint->m_matrixBindInverse;
			//	}
			//	animation.m_time += fps_factor;
			//	if (animation.m_time >= animation.m_animation->m_len)
			//	{
			//		animation.m_time = 0;
			//		for (u16 i = 0, sz = animation.m_animation->m_animatedJoints.size(); i < sz; ++i)
			//		{
			//			auto jointID = animation.m_animation->m_animatedJoints[i]->m_jointID;
			//			auto mdlJoint = m_mdl->m_joints[jointID];
			//			auto objJoint = m_joints[jointID];
			//			objJoint.m_position = mdlJoint->m_matrixWorld[3];
			//			objJoint.m_rotation = math::matToQuat(mdlJoint->m_matrixWorld);
			//		}
			//	}
			//}
		}
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

	yyMDL* m_mdl;

	// никакого удаления предидущего m_mdl нет, надо следить за этим самому
	void SetMDL(yyMDL* mdl)
	{
		m_mdl = mdl;

		m_joints.clear();
		m_joints.reserve(255);
		for (u16 i = 0, sz = m_mdl->m_joints.size(); i < sz; ++i)
		{
			auto j = m_mdl->m_joints[i];
			JointInfo ji;
			ji.m_position = j->m_matrixWorld[3];
			ji.m_rotation = math::matToQuat(j->m_matrixWorld);
			m_joints.push_back(ji);
		}
	}


	yySceneObjectBase m_objectBase;
	yyMDLObjectState* m_currentState;

	yyMDLObjectState* AddState(const char* name)
	{
		auto s = yyCreate<yyMDLObjectState>();
		s->m_name = name;
		return s;
	}
	void SetState(yyMDLObjectState* newState)
	{
		m_currentState = newState;
		for (u16 i = 0, sz = m_currentState->m_animations.size(); i < sz; ++i)
		{
			m_currentState->m_animations[i]->m_time = 0.f;
		}
	}
	
};

YY_FORCE_INLINE void yyMDLObject_update(void * impl)
{
	auto mdl_object = (yyMDLObject*)impl;

}


#endif