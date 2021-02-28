#include "demo.h"
#include "demo_example.h"
#include "demo_example_MDL.h"

#include "yy_model.h"

#include "yy_input.h"

extern Demo* g_demo;

s32 g_controlledBoneIndex = -1;

Mat4 DemoExample_MDL_onUpdate(
	yyJoint* mdlJoint, 
	s32 jointIndex, 
	const Mat4& translationMatrix, const Mat4& rotationMatrix, const Mat4& scaleMatrix)
{
	Mat4 transformation;
	if (g_controlledBoneIndex == jointIndex)
	{
		static f32 a = 0.f;
		a += 0.01f;
		if (a > math::PIPI)
			a = 0.f;
		Mat4 R;
		R.setRotation(Quat(a, 0.f, 0.f));
		transformation = translationMatrix * (rotationMatrix * R) * scaleMatrix;
	}
	else
	{
		transformation = translationMatrix * rotationMatrix * scaleMatrix;
	}

	return transformation;
}

DemoExample_MDL::DemoExample_MDL()
{
	m_mdl_struct = 0;
	m_flyCamera = 0;

	m_mdl_object = 0;
}

DemoExample_MDL::~DemoExample_MDL()
{
	Shutdown();
}

bool DemoExample_MDL::Init()
{
	m_flyCamera = yyCreate<yyFlyCamera>();

	m_mdl_struct = yyGetModel("../res/test/mdl/struct.mdl");

	m_mdl_object = yyCreate<yyMDLObject>();
	m_mdl_object->SetMDL(yyGetModel("../res/test/mdl/test.mdl"));
	m_mdl_object->m_onUpdate = DemoExample_MDL_onUpdate;

	m_mdl_object->m_mdl->GetJointByName("Bone008", &g_controlledBoneIndex);

	auto newState = m_mdl_object->AddState("2animations");
	yyMDLObjectStateNode* newStateNodeWalk = yyCreate<yyMDLObjectStateNode>();
	newStateNodeWalk->m_animation = m_mdl_object->m_mdl->GetAnimationByName("walk");
	for (u16 i = 0, sz = newStateNodeWalk->m_animation->m_animatedJoints.size(); i < sz; ++i)
	{
		auto mdl_joint = m_mdl_object->m_mdl->m_joints[i];
		if (
			(strcmp(mdl_joint->m_name.data(), "bone_pelvis") == 0)
			|| (strcmp(mdl_joint->m_name.data(), "Bone001") == 0)
			|| (strcmp(mdl_joint->m_name.data(), "Bone002") == 0)
			|| (strcmp(mdl_joint->m_name.data(), "Bone003") == 0)
			|| (strcmp(mdl_joint->m_name.data(), "Bone028") == 0)
			|| (strcmp(mdl_joint->m_name.data(), "Bone029") == 0)
			|| (strcmp(mdl_joint->m_name.data(), "Bone004") == 0)
			|| (strcmp(mdl_joint->m_name.data(), "Bone005") == 0)
			|| (strcmp(mdl_joint->m_name.data(), "Bone006") == 0)
			|| (strcmp(mdl_joint->m_name.data(), "Bone026") == 0)
			|| (strcmp(mdl_joint->m_name.data(), "Bone027") == 0)
			)
		{
			newStateNodeWalk->m_animatedJoints.push_back((s32)i);
		}
	}
	newState->m_animations.push_back(newStateNodeWalk);
	yyMDLObjectStateNode* newStateNodeHands = yyCreate<yyMDLObjectStateNode>();
	newStateNodeHands->m_animation = m_mdl_object->m_mdl->GetAnimationByName("handsmove");
	for (u16 i = 0, sz = newStateNodeHands->m_animation->m_animatedJoints.size(); i < sz; ++i)
	{
		auto mdl_joint = m_mdl_object->m_mdl->m_joints[i];
		if (
			(strcmp(mdl_joint->m_name.data(), "bone_pelvis") == 0)
			|| (strcmp(mdl_joint->m_name.data(), "Bone007") == 0)
			|| (strcmp(mdl_joint->m_name.data(), "Bone008") == 0)
			|| (strcmp(mdl_joint->m_name.data(), "Bone009") == 0)
			|| (strcmp(mdl_joint->m_name.data(), "Bone010") == 0)
			|| (strcmp(mdl_joint->m_name.data(), "Bone012") == 0)
			|| (strcmp(mdl_joint->m_name.data(), "Bone013") == 0)
			|| (strcmp(mdl_joint->m_name.data(), "Bone014") == 0)
			|| (strcmp(mdl_joint->m_name.data(), "Bone015") == 0)
			|| (strcmp(mdl_joint->m_name.data(), "Bone020") == 0)
			|| (strcmp(mdl_joint->m_name.data(), "Bone021") == 0)
			|| (strcmp(mdl_joint->m_name.data(), "Bone016") == 0)
			|| (strcmp(mdl_joint->m_name.data(), "Bone017") == 0)
			|| (strcmp(mdl_joint->m_name.data(), "Bone018") == 0)
			|| (strcmp(mdl_joint->m_name.data(), "Bone019") == 0)
			|| (strcmp(mdl_joint->m_name.data(), "Bone024") == 0)
			|| (strcmp(mdl_joint->m_name.data(), "Bone025") == 0)
			|| (strcmp(mdl_joint->m_name.data(), "Bone011") == 0)
			|| (strcmp(mdl_joint->m_name.data(), "Bone022") == 0)
			|| (strcmp(mdl_joint->m_name.data(), "Bone023") == 0)
			)
		{
			newStateNodeHands->m_animatedJoints.push_back((s32)i);
		}
	}
	newState->m_animations.push_back(newStateNodeHands);

	m_mdl_object->SetState(newState);

	return true;
}

void DemoExample_MDL::Shutdown()
{
	if (m_flyCamera)
	{
		yyDestroy(m_flyCamera);
		m_flyCamera = 0;
	}
	if (m_mdl_struct)
	{
		yyDeleteModel(m_mdl_struct);
		m_mdl_struct = 0;
	}
	if (m_mdl_object)
	{
		yyDestroy(m_mdl_object);
		m_mdl_object = 0;
	}
}

const wchar_t* DemoExample_MDL::GetTitle()
{
	return L"MDL";
}

const wchar_t* DemoExample_MDL::GetDescription()
{
	return L"Everything about MDL";
}

bool DemoExample_MDL::DemoStep(f32 deltaTime)
{
	m_flyCamera->Update();
	
	m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::ViewProjection, m_flyCamera->m_camera->m_viewProjectionMatrix);
	
	if (g_demo->m_inputContext->m_isRMBHold)
	{
		m_flyCamera->Rotate(v2f(-g_demo->m_inputContext->m_mouseDelta.x, -g_demo->m_inputContext->m_mouseDelta.y), deltaTime);

		if (g_demo->m_inputContext->isKeyHold(yyKey::K_W))
			m_flyCamera->MoveForward(deltaTime);

		if (g_demo->m_inputContext->isKeyHold(yyKey::K_S))
			m_flyCamera->MoveBackward(deltaTime);

		if (g_demo->m_inputContext->isKeyHold(yyKey::K_A))
			m_flyCamera->MoveLeft(deltaTime);

		if (g_demo->m_inputContext->isKeyHold(yyKey::K_D))
			m_flyCamera->MoveRight(deltaTime);

		if (g_demo->m_inputContext->isKeyHold(yyKey::K_E))
			m_flyCamera->MoveUp(deltaTime);

		if (g_demo->m_inputContext->isKeyHold(yyKey::K_Q))
			m_flyCamera->MoveDown(deltaTime);

		auto cursorX = std::floor((f32)g_demo->m_window->m_currentSize.x / 2.f);
		auto cursorY = std::floor((f32)g_demo->m_window->m_currentSize.y / 2.f);
		g_demo->m_inputContext->m_cursorCoordsOld.set(cursorX, cursorY);

		yySetCursorPosition(cursorX, cursorY, g_demo->m_window);
	}

	auto size = m_mdl_struct->m_layers.size();
	for (u16 i = 0; i < size; ++i)
	{
		auto layer = m_mdl_struct->m_layers[i];
		m_gpu->SetModel(layer->m_meshGPU);

		Mat4 WorldMatrix;
		m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::World, WorldMatrix);
		m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::WorldViewProjection,
			m_flyCamera->m_camera->m_projectionMatrix * m_flyCamera->m_camera->m_viewMatrix * WorldMatrix);

		for (u16 m = 0; m < YY_MDL_LAYER_NUM_OF_TEXTURES; ++m)
		{
			m_gpu->SetTexture(m, layer->m_textureGPU[m]);
		}
		m_gpu->Draw();
	}

	m_mdl_object->Update(deltaTime);
	for (int n = 0, nsz = m_mdl_object->m_mdl->m_layers.size(); n < nsz; ++n)
	{
		auto layer = m_mdl_object->m_mdl->m_layers[n];
		m_gpu->SetModel(layer->m_meshGPU);

		Mat4 WorldMatrix;

		WorldMatrix = m_mdl_object->m_mdl->m_preRotation * WorldMatrix;
		//WorldMatrix[3] = g_sceneObject->m_layerInfo[n].m_offset;
		WorldMatrix[3].w = 1.f;



		m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::World, WorldMatrix);
		m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::WorldViewProjection, 
			m_flyCamera->m_camera->m_projectionMatrix * m_flyCamera->m_camera->m_viewMatrix * WorldMatrix);

		for (u32 t = 0; t < YY_MDL_LAYER_NUM_OF_TEXTURES; ++t)
		{
			if (layer->m_textureGPU[t])
				m_gpu->SetTexture(t, layer->m_textureGPU[t]);
			else
				m_gpu->SetTexture(t, 0);
		}



		/*Mat4 R;
		R.setRotation(Quat(math::degToRad(90.f), 0.f, 0.f));*/
		m_gpu->UseDepth(true);
		m_gpu->Draw();

		auto numJoints = m_mdl_object->m_joints.size();
		if (numJoints)
		{
			m_gpu->UseDepth(false);
			for (int i = 0; i < numJoints; ++i)
			{
				auto & objJoint = m_mdl_object->m_joints[i];
				auto mdlJoint = m_mdl_object->m_mdl->m_joints[i];

				auto matrix = m_mdl_object->m_mdl->m_preRotation * objJoint.m_globalTransformation;
				auto point = matrix[3];

				matrix[3].set(0.f, 0.f, 0.f, 1.f);

				auto distCamera = point.distance(m_flyCamera->m_camera->m_objectBase.m_globalPosition);
				f32 jointSize = 0.02f * distCamera;

				m_gpu->DrawLine3D(
					point + math::mul(v4f(-jointSize, 0.f, 0.f, 0.f), matrix),
					point + math::mul(v4f(jointSize, 0.f, 0.f, 0.f), matrix), ColorRed);
				m_gpu->DrawLine3D(
					point + math::mul(v4f(.0f, -jointSize, 0.f, 0.f), matrix),
					point + math::mul(v4f(.0f, jointSize, 0.f, 0.f), matrix), ColorGreen);
				m_gpu->DrawLine3D(
					point + math::mul(v4f(0.0f, 0.f, -jointSize, 0.f), matrix),
					point + math::mul(v4f(0.0f, 0.f, jointSize, 0.f), matrix), ColorBlue);

				if (mdlJoint->m_parentIndex != -1)
				{
					auto matrixP = m_mdl_object->m_mdl->m_preRotation * m_mdl_object->m_joints[mdlJoint->m_parentIndex].m_globalTransformation;
					m_gpu->DrawLine3D(
						point,
						matrixP[3],
						ColorLime);
				}

				m_gpu->SetBoneMatrix(i, objJoint.m_finalTransformation);
			}
		}
		m_gpu->UseDepth(true);



		/*m_gpu->SetTexture(0, defaultTexture);
		for (int i = 0, sz = m_mdl_object->m_mdl->m_hitboxes.size(); i < sz; ++i)
		{
			auto hb = m_mdl_object->m_mdl->m_hitboxes[i];
			m_gpu->SetMaterial(&hb->m_material);
			Mat4 W;

			if (hb->m_jointID != -1)
			{
				auto & joint = m_mdl_object->m_joints[hb->m_jointID];
				W = m_mdl_object->m_mdl->m_preRotation * joint.m_globalTransformation;
			}

			m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::World, W);
			m_gpu->SetMatrix(yyVideoDriverAPI::MatrixType::WorldViewProjection, 
				m_flyCamera->m_camera->m_projectionMatrix * m_flyCamera->m_camera->m_viewMatrix * W);
			m_gpu->SetModel(hb->m_gpuModel);
			m_gpu->Draw();
		}*/
	//	m_gpu->SetMaterial(0);
	//	m_gpu->UseDepth(true);
	}

	return g_demo->m_inputContext->isKeyHit(yyKey::K_ESCAPE) == false;
}