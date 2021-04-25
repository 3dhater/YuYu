#ifndef _YUYU_SCENE_MDL_H_
#define _YUYU_SCENE_MDL_H_

#include "containers\array.h"
#include "yy_model.h"
#include "math\math.h"

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
		for (u16 i = 0, sz = m_stateNodesWithAnimations.size(); i < sz; ++i)
		{
			yyDestroy(m_stateNodesWithAnimations[i]);
		}
	}

	yyStringA m_name;

	yyArraySmall<yyMDLObjectStateNode*> m_stateNodesWithAnimations;
};

// Установленную модель нужно будет удалять вручную
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
		m_onUpdate = 0;
		m_onAnimationEnd = 0;
		m_userData = 0;

	}
	~yyMDLObject()
	{
		if (m_mdl)
			m_mdl->Unload();
	}

	Mat4(*m_onUpdate)(yyJoint* mdlJoint, s32 jointIndex, const Mat4& translationMatrix, const Mat4& rotationMatrix, const Mat4& scaleMatrix);

	void Update(float dt)
	{
		if (m_currentState)
		{
			// there's many animations...
			bool isLoopEnd = false;

			for (u16 ai = 0, aisz = m_currentState->m_stateNodesWithAnimations.size(); ai < aisz; ++ai)
			{
				auto & currentNodeAnimation = m_currentState->m_stateNodesWithAnimations.at(ai);
				f32 fps_factor = currentNodeAnimation->m_animation->m_fps * dt;
				//f32 fps_factor = 5.f * dt;

				for (u16 i = 0, sz = currentNodeAnimation->m_animatedJoints.size(); i < sz; ++i)
				{
					auto jointID = currentNodeAnimation->m_animatedJoints[i];
					auto animatedJoint = currentNodeAnimation->m_animation->m_animatedJoints[jointID];
					auto mdlJoint = m_mdl->m_joints[animatedJoint->m_jointID];
					auto & objJoint = m_joints[animatedJoint->m_jointID];

					auto currentKey = animatedJoint->m_animationFrames.getCurrentKeyFrame((s32)currentNodeAnimation->m_time);
					auto nextKey = animatedJoint->m_animationFrames.getNextKeyFrame((s32)currentNodeAnimation->m_time);

					auto timeSize = (f32)(nextKey->m_time - currentKey->m_time);
					auto timeLeft = (f32)nextKey->m_time - currentNodeAnimation->m_time;
					auto timeCoef = 1.f - math::get_0_1(timeSize, timeLeft);
					timeCoef *= fps_factor;

					if (math::isinf(timeCoef))
						continue;

					//if(i==0)
					//printf("T: [%f][%f] timeSize: [%f] timeLeft: [%f] timeCoef: [%f]\n", currentAnimation->m_time, currentAnimation->m_animation->m_len, timeSize, timeLeft, timeCoef);

					f32 interpolation_factor = timeCoef;

					objJoint.m_position.x = math::lerp(objJoint.m_position.x, nextKey->m_position.x, interpolation_factor);
					objJoint.m_position.y = math::lerp(objJoint.m_position.y, nextKey->m_position.y, interpolation_factor);
					objJoint.m_position.z = math::lerp(objJoint.m_position.z, nextKey->m_position.z, interpolation_factor);
					objJoint.m_scale.x = math::lerp(objJoint.m_scale.x, nextKey->m_scale.x, interpolation_factor);
					objJoint.m_scale.y = math::lerp(objJoint.m_scale.y, nextKey->m_scale.y, interpolation_factor);
					objJoint.m_scale.z = math::lerp(objJoint.m_scale.z, nextKey->m_scale.z, interpolation_factor);
					objJoint.m_rotation   = math::slerp(objJoint.m_rotation, nextKey->m_rotation, interpolation_factor);

					Mat4 TranslationM;
					TranslationM[3] = objJoint.m_position;
					TranslationM[3].w = 1.f;

					Mat4 ScaleM;
					ScaleM[0].x = objJoint.m_scale.x;
					ScaleM[1].y = objJoint.m_scale.y;
					ScaleM[2].z = objJoint.m_scale.z;

					Mat4 RotationM;
					RotationM.setRotation(objJoint.m_rotation);
					Mat4 NodeTransformation = TranslationM * RotationM * ScaleM;

					if (m_onUpdate)
					{
						NodeTransformation = m_onUpdate(mdlJoint, animatedJoint->m_jointID, 
							TranslationM, RotationM, ScaleM);
					}
					else
					{
						NodeTransformation = TranslationM * RotationM * ScaleM;
					}

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
				currentNodeAnimation->m_time += fps_factor;

				if (currentNodeAnimation->m_time >= currentNodeAnimation->m_animation->m_len)
				{
					currentNodeAnimation->m_time = 0;
					isLoopEnd = true;

					if (currentNodeAnimation->m_animation->m_flags & yyMDLAnimation::flag_disableSmoothLoop
						&& !m_onAnimationEnd)
					{
					//	currentAnimation->m_animation->
						for (u16 i = 0, sz = currentNodeAnimation->m_animatedJoints.size(); i < sz; ++i)
						{
							auto jointID = currentNodeAnimation->m_animatedJoints[i];
							auto animatedJoint = currentNodeAnimation->m_animation->m_animatedJoints[jointID];

							//auto mdlJoint = m_mdl->m_joints[jointID];
							auto & objJoint = m_joints[jointID];

							
							objJoint.m_position = animatedJoint->m_animationFrames.m_keyFrames.m_data[0].m_position;
							objJoint.m_rotation = animatedJoint->m_animationFrames.m_keyFrames.m_data[0].m_rotation;
							objJoint.m_scale = animatedJoint->m_animationFrames.m_keyFrames.m_data[0].m_scale;
						}
					}
				}
			}
			if (m_onAnimationEnd && isLoopEnd)
			{
		//		printf("End \n");
				m_onAnimationEnd(m_userData);
			}
		}
	}

	// при проигрывании анимации состояние каждого джоинта надо запомнить
	struct JointInfo
	{
		JointInfo()
		{
			m_scale.set(1.f);
		}
		v3f m_position;
		v3f m_scale;
		Quat m_rotation;

		Mat4 m_globalTransformation;
		Mat4 m_finalTransformation;
	};
	yyArraySmall<JointInfo> m_joints; // должна быть в соответствии с m_mdl->m_joints
										// Устанавливать mdl надо через SetMDL

	yyMDL* m_mdl;

	// никакого удаления предидущего m_mdl нет, надо следить за этим самому
	void SetMDL(yyMDL* mdl){
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

	void * m_userData;
	void(*m_onAnimationEnd)(void*);
	void SetOnAnimationEnd(void(*onAnimationEnd)(void*), void * userData) {
		m_onAnimationEnd = onAnimationEnd;
		m_userData = userData;
	}

	yyMDLObjectState* AddState(const char* name){
		assert(name);
		auto s = yyCreate<yyMDLObjectState>();
		s->m_name = name;
		return s;
	}

	yyMDLObjectState* AddStateWithAnimation(const char* name, yyMDLAnimation* ani){
		assert(name);
		assert(ani);
		auto s = yyCreate<yyMDLObjectState>();
		s->m_name = name;
		yyMDLObjectStateNode* newStateNode = yyCreate<yyMDLObjectStateNode>();
		newStateNode->m_animation = ani;
		for (u16 i = 0, sz = ani->m_animatedJoints.size(); i < sz; ++i)
		{
			newStateNode->m_animatedJoints.push_back((s32)i);
		}
		s->m_stateNodesWithAnimations.push_back(newStateNode);
		return s;
	}
	void SetState(yyMDLObjectState* newState, bool resetTransforms)
	{
		m_currentState = newState;
		for (u16 i = 0, sz = m_currentState->m_stateNodesWithAnimations.size(); i < sz; ++i)
		{
			auto & currentNodeAnimation = m_currentState->m_stateNodesWithAnimations.at(i);

			currentNodeAnimation->m_time = 0.f;

			if (resetTransforms)
			{
				for (u16 i2 = 0, sz2 = currentNodeAnimation->m_animatedJoints.size(); i2 < sz2; ++i2)
				{
					auto jointID = currentNodeAnimation->m_animatedJoints[i2];
					auto animatedJoint = currentNodeAnimation->m_animation->m_animatedJoints[jointID];
					auto & objJoint = m_joints[jointID];
					objJoint.m_position = animatedJoint->m_animationFrames.m_keyFrames.m_data[0].m_position;
					objJoint.m_rotation = animatedJoint->m_animationFrames.m_keyFrames.m_data[0].m_rotation;
					objJoint.m_scale = animatedJoint->m_animationFrames.m_keyFrames.m_data[0].m_scale;
				}
			}
		}
	}
	
};

YY_FORCE_INLINE void yyMDLObject_update(void * impl)
{
	auto mdl_object = (yyMDLObject*)impl;

}


#endif