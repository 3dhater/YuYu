#include "stdafx.h"
#include "yy.h"
#include "math\math.h"
#include "modelCreator.h"

#include "MainFrm.h"
#include "TabLayers.h"
#include "TabAnimations.h"

#include <cmath>
#include "yy_window.h"
#include "yy_model.h"

#include <fbxsdk.h>

#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(pManager->GetIOSettings()))
#endif

struct FBXAnimationBoneInfo
{
	yyStringA m_boneName;

	yyArray<v3f> m_position_x;
	yyArray<v3f> m_position_y;
	yyArray<v3f> m_position_z;
};
struct FBXAnimation
{
	FBXAnimation()
	{
	}
	~FBXAnimation()
	{
		for (u32 i = 0, sz = m_boneInfo.size(); i < sz; ++i)
		{
			delete m_boneInfo[i]; 
		}
	}
	yyStringA m_name;
	// анимация содержит массив анимированных костей
	yyArray<FBXAnimationBoneInfo*> m_boneInfo;
};
void FBXGetAnimation(FbxAnimLayer* pAnimLayer, FbxNode* pNode, FBXAnimation* as)
{
	FBXAnimationBoneInfo * boneInfo = 0;

	FbxAnimCurve* lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
	if (lAnimCurve)
	{
		boneInfo = new FBXAnimationBoneInfo;
		boneInfo->m_boneName = pNode->GetName();
		as->m_boneInfo.push_back(boneInfo);

		int lKeyCount = lAnimCurve->KeyGetCount();
		for (int lCount = 0; lCount < lKeyCount; lCount++)
		{
			auto lKeyValue = static_cast<float>(lAnimCurve->KeyGetValue(lCount));
			auto lKeyTime = lAnimCurve->KeyGetTime(lCount);
			boneInfo->m_position_x.push_back(v3f((f32)lKeyTime.GetFrameCount(), lKeyValue, 0.f));
		}

		lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		if (lAnimCurve)
		{
			lKeyCount = lAnimCurve->KeyGetCount();
			for (int lCount = 0; lCount < lKeyCount; lCount++)
			{
				auto lKeyValue = static_cast<float>(lAnimCurve->KeyGetValue(lCount));
				auto lKeyTime = lAnimCurve->KeyGetTime(lCount);
				boneInfo->m_position_y.push_back(v3f((f32)lKeyTime.GetFrameCount(), lKeyValue, 0.f));
			}
		}

		lAnimCurve = pNode->LclTranslation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_Z);
		if (lAnimCurve)
		{
			lKeyCount = lAnimCurve->KeyGetCount();
			for (int lCount = 0; lCount < lKeyCount; lCount++)
			{
				auto lKeyValue = static_cast<float>(lAnimCurve->KeyGetValue(lCount));
				auto lKeyTime = lAnimCurve->KeyGetTime(lCount);
				boneInfo->m_position_z.push_back(v3f((f32)lKeyTime.GetFrameCount(), lKeyValue, 0.f));
			}
		}
		/*lAnimCurve = pNode->LclRotation.GetCurve(pAnimLayer, FBXSDK_CURVENODE_COMPONENT_X);
		if (lAnimCurve)
		{
			lKeyCount = lAnimCurve->KeyGetCount();
			for (int lCount = 0; lCount < lKeyCount; lCount++)
			{
				auto lKeyValue = static_cast<float>(lAnimCurve->KeyGetValue(lCount));
				auto lKeyTime = lAnimCurve->KeyGetTime(lCount);
				boneInfo->m_position_z.push_back(v3f((f32)lKeyTime.GetFrameCount(), lKeyValue, 0.f));
			}
		}*/
	}

	int lModelCount;
	for (lModelCount = 0; lModelCount < pNode->GetChildCount(); lModelCount++)
	{
		FBXGetAnimation(pAnimLayer, pNode->GetChild(lModelCount), as);
	}
}
void FBXGetAnimation(FbxAnimStack* pAnimStack, FbxNode* pNode, yyArraySmall<FBXAnimation*>& animations)
{
	int nbAnimLayers = pAnimStack->GetMemberCount<FbxAnimLayer>();
	for (int l = 0; l < nbAnimLayers; l++)
	{
		FbxAnimLayer* lAnimLayer = pAnimStack->GetMember<FbxAnimLayer>(l);
		yyStringA name = pAnimStack->GetName();
		name += l;
		
		FBXAnimation* new_animation = new FBXAnimation;
		animations.push_back(new_animation);

		new_animation->m_name = name;
		FBXGetAnimation(lAnimLayer, pNode, new_animation);
	}
}

void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene)
{
	//The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
	pManager = FbxManager::Create();
	if (!pManager)
	{
		FBXSDK_printf("Error: Unable to create FBX Manager!\n");
		exit(1);
	}
	else FBXSDK_printf("Autodesk FBX SDK version %s\n", pManager->GetVersion());

	//Create an IOSettings object. This object holds all import/export settings.
	FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
	pManager->SetIOSettings(ios);

	//Load plugins from the executable directory (optional)
	FbxString lPath = FbxGetApplicationDirectory();
	pManager->LoadPluginsDirectory(lPath.Buffer());

	//Create an FBX scene. This object holds most objects imported/exported from/to files.
	pScene = FbxScene::Create(pManager, "My Scene");
	if (!pScene)
	{
		FBXSDK_printf("Error: Unable to create FBX scene!\n");
		exit(1);
	}
}
void DestroySdkObjects(FbxManager* pManager, bool pExitStatus)
{
	//Delete the FBX Manager. All the objects that have been allocated using the FBX Manager and that haven't been explicitly destroyed are also automatically destroyed.
	if (pManager) pManager->Destroy();
	if (pExitStatus) FBXSDK_printf("Program Success!\n");
}
bool LoadScene(FbxManager* pManager, FbxDocument* pScene, const char* pFilename)
{
	int lFileMajor, lFileMinor, lFileRevision;
	int lSDKMajor, lSDKMinor, lSDKRevision;
	//int lFileFormat = -1;
	int lAnimStackCount;
	bool lStatus;
	char lPassword[1024];

	// Get the file version number generate by the FBX SDK.
	FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

	// Create an importer.
	FbxImporter* lImporter = FbxImporter::Create(pManager, "");

	// Initialize the importer by providing a filename.
	const bool lImportStatus = lImporter->Initialize(pFilename, -1, pManager->GetIOSettings());
	lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

	if (!lImportStatus)
	{
		FbxString error = lImporter->GetStatus().GetErrorString();
		FBXSDK_printf("Call to FbxImporter::Initialize() failed.\n");
		FBXSDK_printf("Error returned: %s\n\n", error.Buffer());

		if (lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
		{
			FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
			FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
		}

		return false;
	}

	FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

	if (lImporter->IsFBX())
	{
		FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

		// From this point, it is possible to access animation stack information without
		// the expense of loading the entire file.

		FBXSDK_printf("Animation Stack Information\n");

		lAnimStackCount = lImporter->GetAnimStackCount();

		FBXSDK_printf("    Number of Animation Stacks: %d\n", lAnimStackCount);
		FBXSDK_printf("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
		FBXSDK_printf("\n");

		for (int i = 0; i < lAnimStackCount; i++)
		{
			FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

			FBXSDK_printf("    Animation Stack %d\n", i);
			FBXSDK_printf("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
			FBXSDK_printf("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

			// Change the value of the import name if the animation stack should be imported 
			// under a different name.
			FBXSDK_printf("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

			// Set the value of the import state to false if the animation stack should be not
			// be imported. 
			FBXSDK_printf("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
			FBXSDK_printf("\n");
		}

		// Set the import states. By default, the import states are always set to 
		// true. The code below shows how to change these states.
		IOS_REF.SetBoolProp(IMP_FBX_MATERIAL, true);
		IOS_REF.SetBoolProp(IMP_FBX_TEXTURE, true);
		IOS_REF.SetBoolProp(IMP_FBX_LINK, true);
		IOS_REF.SetBoolProp(IMP_FBX_SHAPE, true);
		IOS_REF.SetBoolProp(IMP_FBX_GOBO, true);
		IOS_REF.SetBoolProp(IMP_FBX_ANIMATION, true);
		IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
	}

	// Import the scene.
	lStatus = lImporter->Import(pScene);
	if (lStatus == true)
	{
		// Check the scene integrity!
		FbxStatus status;
		FbxArray< FbxString*> details;
		FbxSceneCheckUtility sceneCheck(FbxCast<FbxScene>(pScene), &status, &details);
		lStatus = sceneCheck.Validate(FbxSceneCheckUtility::eCkeckData);
		bool lNotify = (!lStatus && details.GetCount() > 0) || (lImporter->GetStatus().GetCode() != FbxStatus::eSuccess);
		if (lNotify)
		{
			FBXSDK_printf("\n");
			FBXSDK_printf("********************************************************************************\n");
			if (details.GetCount())
			{
				FBXSDK_printf("Scene integrity verification failed with the following errors:\n");
				for (int i = 0; i < details.GetCount(); i++)
					FBXSDK_printf("   %s\n", details[i]->Buffer());

				FbxArrayDelete<FbxString*>(details);
			}

			if (lImporter->GetStatus().GetCode() != FbxStatus::eSuccess)
			{
				FBXSDK_printf("\n");
				FBXSDK_printf("WARNING:\n");
				FBXSDK_printf("   The importer was able to read the file but with errors.\n");
				FBXSDK_printf("   Loaded scene may be incomplete.\n\n");
				FBXSDK_printf("   Last error message:'%s'\n", lImporter->GetStatus().GetErrorString());
			}
			FBXSDK_printf("********************************************************************************\n");
			FBXSDK_printf("\n");
		}
	}

	if (lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
	{
		FBXSDK_printf("Please enter password: ");

		lPassword[0] = '\0';

		FBXSDK_CRT_SECURE_NO_WARNING_BEGIN
			scanf("%s", lPassword);
		FBXSDK_CRT_SECURE_NO_WARNING_END

			FbxString lString(lPassword);

		IOS_REF.SetStringProp(IMP_FBX_PASSWORD, lString);
		IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

		lStatus = lImporter->Import(pScene);

		if (lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
		{
			FBXSDK_printf("\nPassword is wrong, import aborted.\n");
		}
	}

	// Destroy the importer.
	lImporter->Destroy();

	return lStatus;
}

void FBXGetSkeleton(FbxNode* pNode, yyMDL* mdl, yyJoint* parentJoint)
{
	yyJoint* newJoint = 0;

	if (pNode->GetNodeAttribute())
	{

		FbxNodeAttribute::EType lAttributeType = (pNode->GetNodeAttribute()->GetAttributeType());
		switch (lAttributeType)
		{
		case FbxNodeAttribute::eSkeleton:
		{
			newJoint = yyCreate<yyJoint>();
			newJoint->m_name = pNode->GetName();

			if (!mdl->m_skeleton)
				mdl->m_skeleton = newJoint; // set root
			mdl->m_joints.push_back(newJoint);
			if (parentJoint)
			{
				parentJoint->m_children.push_back(newJoint);
				newJoint->m_parent = parentJoint;
			}
		}break;
		default:
			break;
		}
	}

	for (int i = 0; i < pNode->GetChildCount(); i++)
	{
		FBXGetSkeleton(pNode->GetChild(i), mdl, newJoint);
	}
}
void FBXGetMeshNodes(FbxNode* pNode, 
	yyArraySmall<FbxNode*>& meshNodes)
{
	if (pNode->GetNodeAttribute())
	{

		FbxNodeAttribute::EType lAttributeType = (pNode->GetNodeAttribute()->GetAttributeType());
		switch (lAttributeType)
		{
		case FbxNodeAttribute::eMesh:
		{
			meshNodes.push_back(pNode);
		}break;
		default:
			break;
		}
	}

	for (int i = 0; i < pNode->GetChildCount(); i++)
	{
		FBXGetMeshNodes(pNode->GetChild(i), meshNodes);
	}
}

void CMainFrame::_importFBX(const wchar_t* filePath, CString& outName)
{
	FbxManager* lSdkManager = NULL;
	FbxScene* lScene = NULL;

	InitializeSdkObjects(lSdkManager, lScene);

	yyString utf16 = filePath;
	yyStringA utf8;

	util::utf16_to_utf8(&utf16, &utf8);

	auto lResult = LoadScene(lSdkManager, lScene, utf8.data());

	if (lResult == false)
	{
		MessageBox(L"An error occurred while loading the FBX scene...");
	}
	else
	{
		yyArraySmall<FbxNode*> meshNodes;

		FbxNode* rootNode = lScene->GetRootNode();
		if (rootNode)
		{
			for (int i = 0; i < rootNode->GetChildCount(); i++)
			{
				auto child = rootNode->GetChild(i);
				FBXGetMeshNodes(child, meshNodes);
			}
		}
		
		// послать m_mdlFile->m_skeleton так как первый joint это root
		//  на случай если будет загружена ещё одна модель, с дополнительными костями
		//   надо будет просто дополнить скелет
		FBXGetSkeleton(rootNode, m_mdlFile, m_mdlFile->m_skeleton);

		// далее читаю анимацию
		// лучше сначала вытащить всю информацию, потом сунуть в MDL
		yyArraySmall<FBXAnimation*> animations;
		for (int i = 0; i < lScene->GetSrcObjectCount<FbxAnimStack>(); i++)
		{
			FbxAnimStack* lAnimStack = lScene->GetSrcObject<FbxAnimStack>(i);
			FBXGetAnimation(lAnimStack, lScene->GetRootNode(), animations);
		}
		for (u16 i = 0; i < animations.size(); ++i)
		{
			auto a = animations[i];

			yyMDLAnimation* newMdlAnimation = yyCreate<yyMDLAnimation>();
			newMdlAnimation->m_name = a->m_name;
			
			for (u32 j = 0; j < a->m_boneInfo.size(); ++j)
			{
				auto bi = a->m_boneInfo[j];
				yyMDLAnimation::_joint_info ji;

				// сначала ищу индекс joint
				for (u16 k = 0; k < m_mdlFile->m_joints.size(); ++k)
				{
					if (m_mdlFile->m_joints[k]->m_name == bi->m_boneName)
					{
						ji.m_jointID = k;
						break;
					}
				}
				
				// у FBX может такое быть что значения XYZ могут не совпадать или отсутствовать
				//  например это можно сделать в 3ds max в Curve Editor
				//   это даёт возможность настроить анимацию более тщательно,
				//    но в игре будет достаточно обычных 3д координат
				//  в общем надо как-то расставить имеющиеся значения в нужном порядке
				// буду всовывать значения и одновременно сортировать
				for (u32 k = 0; k < bi->m_position_x.size(); ++k)
				{
					// 3 значения
					// x - номер фрейма, time
					// y - значение компонента
					// z - 0-linear !=0-cubic
					auto & value = bi->m_position_x[k];
					ji.m_animationLayer.insertPosition((s32)value.x, value.y, yyVectorComponent::x);

					if ((s32)value.x > newMdlAnimation->m_len)
						newMdlAnimation->m_len = (s32)value.x;
				}
				for (u32 k = 0; k < bi->m_position_y.size(); ++k)
				{
					auto & value = bi->m_position_y[k];
					ji.m_animationLayer.insertPosition((s32)value.x, value.y, yyVectorComponent::y);
					if ((s32)value.x > newMdlAnimation->m_len)
						newMdlAnimation->m_len = (s32)value.x;
				}
				for (u32 k = 0; k < bi->m_position_z.size(); ++k)
				{
					auto & value = bi->m_position_z[k];
					ji.m_animationLayer.insertPosition((s32)value.x, value.y, yyVectorComponent::z);
					if ((s32)value.x > newMdlAnimation->m_len)
						newMdlAnimation->m_len = (s32)value.x;
				}

				newMdlAnimation->m_animatedJoints.push_back(ji);
			}

			m_mdlFile->m_animations.push_back(newMdlAnimation);
			this->m_infoPanel->m_animationsTab->m_animationsList.InsertString(
				this->m_infoPanel->m_animationsTab->m_animationsList.GetCount(), (wchar_t*)newMdlAnimation->m_name.to_string().data());
			this->m_infoPanel->m_animationsTab->m_animationsList.SetCurSel(
				this->m_infoPanel->m_animationsTab->m_animationsList.GetCount() - 1);

			delete a;
		}

		auto lPoseCount = lScene->GetPoseCount();
		for (int i = 0; i < lPoseCount; i++)
		{
			FbxPose* lPose = lScene->GetPose(i);
			if (lPose->IsBindPose())
			{
				for (int j = 0; j<lPose->GetCount(); j++)
				{
					auto lName = lPose->GetNodeName(j).GetCurrentName();

					//if (!lPose->IsBindPose())
					//{
					//	// Rest pose can have local matrix
					////	DisplayBool("    Is local space matrix: ", lPose->IsLocalMatrix(j));
					//}

					for (u16 o = 0; o < m_mdlFile->m_joints.size(); ++o)
					{
						auto joint = m_mdlFile->m_joints[o];
						if (strcmp(lName, joint->m_name.data()) == 0)
						{
							FbxMatrix  lMatrix = lPose->GetMatrix(j);

							joint->m_matrixBind[0].x = lMatrix.mData[0].mData[0];
							joint->m_matrixBind[0].y = lMatrix.mData[0].mData[2];
							joint->m_matrixBind[0].z = lMatrix.mData[0].mData[1];
							joint->m_matrixBind[1].x = lMatrix.mData[1].mData[0];
							joint->m_matrixBind[1].y = lMatrix.mData[1].mData[2];
							joint->m_matrixBind[1].z = lMatrix.mData[1].mData[1];
							joint->m_matrixBind[2].x = lMatrix.mData[2].mData[0];
							joint->m_matrixBind[2].y = lMatrix.mData[2].mData[2];
							joint->m_matrixBind[2].z = lMatrix.mData[2].mData[1];
							joint->m_matrixBind[3].x = lMatrix.mData[3].mData[0];
							joint->m_matrixBind[3].y = lMatrix.mData[3].mData[1];
							joint->m_matrixBind[3].z = -lMatrix.mData[3].mData[2];
							joint->toBind();
							joint->updateMatrix();

							break;
						}
					}
				}
			}
		}

		if (meshNodes.size())
		{
			for (u16 o = 0; o < meshNodes.size(); ++o)
			{
				auto node = meshNodes[o];
				outName = node->GetName();

				yyArray<yyVertexModel> modelVerts;
				modelVerts.reserve(0xffff);
				modelVerts.setAddMemoryValue(0xffff);
				yyArray<u32> modelInds;
				modelInds.reserve(0xffff);
				modelInds.setAddMemoryValue(0xffff);

				auto pivotPreRotation = node->GetPreRotation(FbxNode::eSourcePivot);
				Quat Q( 
					math::degToRad((f32)pivotPreRotation.mData[0]), 
					0,
					0);
				Mat4 M;
				math::makeRotationMatrix(M, Q);

				FbxMesh* pMesh = (FbxMesh*)node->GetNodeAttribute();
				FbxVector4* lControlPoints = pMesh->GetControlPoints();
				int lPolygonCount = pMesh->GetPolygonCount();
				for (int i = 0; i < lPolygonCount; ++i)
				{
					int lTextureUVIndex1 = 0;
					int lControlPointIndex1 = 0;
					int lPolygonSize = pMesh->GetPolygonSize(i);
					for (int j = 0; j < lPolygonSize-2; ++j)
					{
						if(j == 0)
							lControlPointIndex1 = pMesh->GetPolygonVertex(i, j);
						int lControlPointIndex2 = pMesh->GetPolygonVertex(i, j+1);
						int lControlPointIndex3 = pMesh->GetPolygonVertex(i, j+2);
						
						if (lControlPointIndex1 < 0) continue;
						if (lControlPointIndex2 < 0) continue;
						if (lControlPointIndex3 < 0) continue;

						auto V1 = lControlPoints[lControlPointIndex1];
						auto V2 = lControlPoints[lControlPointIndex2];
						auto V3 = lControlPoints[lControlPointIndex3];

						yyVertexModel newVertex1;
						yyVertexModel newVertex2;
						yyVertexModel newVertex3;
						newVertex1.Position.x = V1.mData[0] * -1.f;
						newVertex1.Position.y = V1.mData[1];
						newVertex1.Position.z = V1.mData[2] * -1.f;
						newVertex2.Position.x = V2.mData[0] * -1.f;
						newVertex2.Position.y = V2.mData[1];
						newVertex2.Position.z = V2.mData[2] * -1.f;
						newVertex3.Position.x = V3.mData[0] * -1.f;
						newVertex3.Position.y = V3.mData[1];
						newVertex3.Position.z = V3.mData[2] * -1.f;

						newVertex1.Position = math::mul(newVertex1.Position, M);
						newVertex2.Position = math::mul(newVertex2.Position, M);
						newVertex3.Position = math::mul(newVertex3.Position, M);

						if(pMesh->GetElementUVCount())
						{
							FbxGeometryElementUV* leUV = pMesh->GetElementUV(0);
							//FBXSDK_sprintf(header, 100, "            Texture UV: ");
							switch (leUV->GetMappingMode())
							{
							default:
								break;
							case FbxGeometryElement::eByControlPoint:
								switch (leUV->GetReferenceMode())
								{
								case FbxGeometryElement::eDirect:
									//Display2DVector(header, leUV->GetDirectArray().GetAt(lControlPointIndex));
									newVertex1.TCoords.x = leUV->GetDirectArray().GetAt(lControlPointIndex1).mData[0];
									newVertex1.TCoords.y = leUV->GetDirectArray().GetAt(lControlPointIndex1).mData[1];
									newVertex2.TCoords.x = leUV->GetDirectArray().GetAt(lControlPointIndex2).mData[0];
									newVertex2.TCoords.y = leUV->GetDirectArray().GetAt(lControlPointIndex2).mData[1];
									newVertex3.TCoords.x = leUV->GetDirectArray().GetAt(lControlPointIndex3).mData[0];
									newVertex3.TCoords.y = leUV->GetDirectArray().GetAt(lControlPointIndex3).mData[1];
									break;
								case FbxGeometryElement::eIndexToDirect:
								{
									newVertex1.TCoords.x = leUV->GetDirectArray().GetAt(leUV->GetIndexArray().GetAt(lControlPointIndex1)).mData[0];
									newVertex1.TCoords.y = leUV->GetDirectArray().GetAt(leUV->GetIndexArray().GetAt(lControlPointIndex1)).mData[1];
									newVertex2.TCoords.x = leUV->GetDirectArray().GetAt(leUV->GetIndexArray().GetAt(lControlPointIndex2)).mData[0];
									newVertex2.TCoords.y =  leUV->GetDirectArray().GetAt(leUV->GetIndexArray().GetAt(lControlPointIndex2)).mData[1];
									newVertex3.TCoords.x = leUV->GetDirectArray().GetAt(leUV->GetIndexArray().GetAt(lControlPointIndex3)).mData[0];
									newVertex3.TCoords.y =  leUV->GetDirectArray().GetAt(leUV->GetIndexArray().GetAt(lControlPointIndex3)).mData[1];
								}
								break;
								default:
									break; // other reference modes not shown here!
								}
								break;

							case FbxGeometryElement::eByPolygonVertex:
							{
								switch (leUV->GetReferenceMode())
								{
								case FbxGeometryElement::eDirect:
								case FbxGeometryElement::eIndexToDirect:
								{
									if (j == 0)
										lTextureUVIndex1 = pMesh->GetTextureUVIndex(i, j);
									int lTextureUVIndex2 = pMesh->GetTextureUVIndex(i, j+1);
									int lTextureUVIndex3 = pMesh->GetTextureUVIndex(i, j+2);

									newVertex1.TCoords.x =  leUV->GetDirectArray().GetAt(lTextureUVIndex1).mData[0];
									newVertex1.TCoords.y = leUV->GetDirectArray().GetAt(lTextureUVIndex1).mData[1];
									newVertex2.TCoords.x =  leUV->GetDirectArray().GetAt(lTextureUVIndex2).mData[0];
									newVertex2.TCoords.y = leUV->GetDirectArray().GetAt(lTextureUVIndex2).mData[1];
									newVertex3.TCoords.x =  leUV->GetDirectArray().GetAt(lTextureUVIndex3).mData[0];
									newVertex3.TCoords.y = leUV->GetDirectArray().GetAt(lTextureUVIndex3).mData[1];
								}
								break;
								default:
									break; // other reference modes not shown here!
								}
							}
							break;

							case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
							case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
							case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
								break;
							}
						}

						if (pMesh->GetElementNormalCount())
						{
							//FbxGeometryElementNormal* leNormal = pMesh->GetElementNormal(0);
							//if (leNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
							//{
							//	switch (leNormal->GetReferenceMode())
							//	{
							//	case FbxGeometryElement::eDirect:
							//		Display3DVector(header, leNormal->GetDirectArray().GetAt(vertexId));
							//		break;
							//	case FbxGeometryElement::eIndexToDirect:
							//	{
							//		int id = leNormal->GetIndexArray().GetAt(vertexId);
							//		Display3DVector(header, leNormal->GetDirectArray().GetAt(id));
							//	}
							//	break;
							//	default:
							//		break; // other reference modes not shown here!
							//	}
							//}
						}

						u32 vIndex1 = 0xffffffff;
						u32 vIndex2 = 0xffffffff;
						u32 vIndex3 = 0xffffffff;
						yyVertexModel* v1_ptr = &newVertex1;
						yyVertexModel* v2_ptr = &newVertex2;
						yyVertexModel* v3_ptr = &newVertex3;
						for (u32 k = 0, ksz = modelVerts.size(); k < ksz; ++k)
						{
							auto _v = &modelVerts[k];
							if (_v->Position == v1_ptr->Position
								&& _v->Normal == v1_ptr->Normal
								&& _v->TCoords == v1_ptr->TCoords) {
								v1_ptr = _v; vIndex1 = k;
							}
							if (_v->Position == v2_ptr->Position
								&& _v->Normal == v2_ptr->Normal
								&& _v->TCoords == v2_ptr->TCoords) {
								v2_ptr = _v; vIndex2 = k;
							}
							if (_v->Position == v3_ptr->Position
								&& _v->Normal == v3_ptr->Normal
								&& _v->TCoords == v3_ptr->TCoords) {
								v3_ptr = _v; vIndex3 = k;
							}
						}

						if (vIndex1 == 0xffffffff) { vIndex1 = modelVerts.size(); modelVerts.push_back(*v1_ptr); }
						if (vIndex2 == 0xffffffff) { vIndex2 = modelVerts.size(); modelVerts.push_back(*v2_ptr); }
						if (vIndex3 == 0xffffffff) { vIndex3 = modelVerts.size(); modelVerts.push_back(*v3_ptr); }

						modelInds.push_back(vIndex1);
						modelInds.push_back(vIndex2);
						modelInds.push_back(vIndex3);
					}
				}

				yyModel* newModel = yyCreate<yyModel>();
				newModel->m_vertices = (u8*)yyMemAlloc(modelVerts.size() * sizeof(yyVertexModel));
				memcpy(newModel->m_vertices, modelVerts.data(), modelVerts.size() * sizeof(yyVertexModel));

				if (modelInds.size() / 3 > 21845)
				{
					newModel->m_indexType = yyMeshIndexType::u32;
					newModel->m_indices = (u8*)yyMemAlloc(modelInds.size() * sizeof(u32));
					memcpy(newModel->m_indices, modelInds.data(), modelInds.size() * sizeof(u32));
				}
				else
				{
					newModel->m_indexType = yyMeshIndexType::u16;
					newModel->m_indices = (u8*)yyMemAlloc(modelInds.size() * sizeof(u16));
					u16 * i_ptr = (u16*)newModel->m_indices;
					for (u32 i = 0, sz = modelInds.size(); i < sz; ++i)
					{
						*i_ptr = (u16)modelInds[i];
						++i_ptr;
					}
				}

				newModel->m_name = outName.GetBuffer();
				newModel->m_vCount = modelVerts.size();
				newModel->m_iCount = modelInds.size();
				newModel->m_stride = sizeof(yyVertexModel);
				newModel->m_vertexType = yyVertexType::Model;

				yyMDLLayer * newLayer = yyCreate<yyMDLLayer>();
				newLayer->m_model = newModel;
				m_mdlFile->m_layers.push_back(newLayer);
				m_infoPanel->m_layersTab->addLayer(outName.GetBuffer());
				//MDLRenameLayer(m_mdlFile->m_layers.size() - 1, outName.GetBuffer());
				MDLUpdateAABB();
				newLayer->m_meshGPU = yyGetVideoDriverAPI()->CreateModel(newLayer->m_model);
				m_layerInfo.push_back(LayerInfo());
			//	return newModel;
			}
		}


	}

	DestroySdkObjects(lSdkManager, lResult);
//	return 0;
}