#include "yy.h"
#include "yy_model.h"
#include "math\math.h"
#include "scene\common.h"
#include "scene\mdl_object.h"

#include "MDLCreator_common.h"
#include "SceneObject.h"

extern SceneObject* g_sceneObject;

void ImportAssimp(yyMDLObject* object, const char* fileName)
{
	
	//Assimp::Importer Importer;
	//const aiScene* pScene = Importer.ReadFile(
	//	stra.c_str(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
	//if (!pScene)
	//{
	//	YY_PRINT_FAILED;
	//	return;
	//}

	//object->m_GlobalInverseTransform = aiMatrixToGameMatrix(pScene->mRootNode->mTransformation);
	//object->m_GlobalInverseTransform.invert();

	//auto assRoot = pScene->mRootNode;
	//aiPrintNames(assRoot, 1);

	//for (int i = 0; i < pScene->mNumMeshes; ++i)
	//{
	//	yyArray<yyVertexAnimatedModel> verts;
	//	yyArray<u32> inds;

	//	auto assMesh = pScene->mMeshes[i];

	//	for (int o = 0; o < assMesh->mNumVertices; o++)
	//	{
	//		yyVertexAnimatedModel newVertex;

	//		const aiVector3D* pPos = &(assMesh->mVertices[o]);

	//		newVertex.Position.x = pPos->x;
	//		newVertex.Position.y = pPos->y;
	//		newVertex.Position.z = pPos->z;

	//		newVertex.Bones.x = 255;
	//		newVertex.Bones.y = 255;
	//		newVertex.Bones.z = 255;
	//		newVertex.Bones.w = 255;

	//		if (assMesh->HasNormals())
	//		{
	//			const aiVector3D* pNormal = &(assMesh->mNormals[o]);
	//			newVertex.Normal.x = pNormal->x;
	//			newVertex.Normal.y = pNormal->y;
	//			newVertex.Normal.z = pNormal->z;
	//		}
	//		if (assMesh->HasTextureCoords(0))
	//		{
	//			const aiVector3D* pTexCoord = &(assMesh->mTextureCoords[0][o]);
	//			newVertex.TCoords.x = pTexCoord->x;
	//			newVertex.TCoords.y = 1.f - pTexCoord->y;
	//		}
	//		verts.push_back(newVertex);
	//	}

	//	for (int o = 0; o < assMesh->mNumFaces; o++)
	//	{
	//		const aiFace& Face = assMesh->mFaces[o];
	//		assert(Face.mNumIndices == 3);
	//		inds.push_back(Face.mIndices[0]);
	//		inds.push_back(Face.mIndices[1]);
	//		inds.push_back(Face.mIndices[2]);
	//	}



	//	//newMDLLayer->m_model->m_aabb
	//	// теперь можно собрать модель
	//	
	//	if(assMesh->mNumBones)
	//		is_animated = true;
	//	
	//	// Здесь заполняю вершины весом и индексами
	//	// Так-же получаю те кости, которые воздействуют на модельки
	//	// После цикла, нужно найти всех парентов
	//	for (u32 b = 0; b < assMesh->mNumBones; b++) 
	//	{
	//		auto assBone = assMesh->mBones[b];
	//		
	//		u32 BoneIndex = 0;
	//		auto joint = object->m_mdl->GetJointByName(assBone->mName.data, &BoneIndex);
	//		if (!joint)
	//		{
	//			yyJoint * newJoint = yyCreate<yyJoint>();
	//			newJoint->m_name = assBone->mName.data;
	//		//	printf("Joint: %s\n", assBone->mName.data);
	//			
	//			newJoint->m_matrixOffset = aiMatrixToGameMatrix(assBone->mOffsetMatrix);
	//			
	//			//auto assNode = aiFindNode(assBone->mName.data, assRoot);
	//			//if(assNode)
	//			//	newJoint->m_nodeTransformation = aiMatrixToGameMatrix(assNode->mTransformation);

	//			BoneIndex = object->m_mdl->m_joints.size();
	//			object->m_mdl->m_joints.push_back(newJoint);
	//		}

	//		for (u32 j = 0; j < assMesh->mBones[b]->mNumWeights; j++)
	//		{
	//			s32* boneIndsData = verts[assBone->mWeights[j].mVertexId].Bones.data();
	//			f32* boneWeightsData = verts[assBone->mWeights[j].mVertexId].Weights.data();
	//			for (u32 m = 0; m < 4; ++m)
	//			{
	//				if (boneIndsData[m] == 255)
	//				{
	//					boneIndsData[m] = BoneIndex;
	//					boneWeightsData[m] = assBone->mWeights[j].mWeight;
	//					break;
	//				}
	//			}
	//			/*if (verts[assBone->mWeights[j].mVertexId].Bones.x == 255)
	//			{
	//				verts[assBone->mWeights[j].mVertexId].Bones.x = BoneIndex;
	//				verts[assBone->mWeights[j].mVertexId].Weights.x = assBone->mWeights[j].mWeight;
	//			}
	//			else if (verts[assBone->mWeights[j].mVertexId].Bones.y == 255)
	//			{
	//				verts[assBone->mWeights[j].mVertexId].Bones.y = BoneIndex;
	//				verts[assBone->mWeights[j].mVertexId].Weights.y = assBone->mWeights[j].mWeight;
	//			}*/
	//		}
	//	}
	//	
	//	// Поиск родителей
	//	for (u32 j = 0; j < object->m_mdl->m_joints.size(); j++)
	//	{
	//		auto currentJoint = object->m_mdl->m_joints[j];
	//
	//		// Ищу aiNode c таким же именем как и джоинт.
	//		auto assNode = aiFindNode(currentJoint->m_name.data(), assRoot);

	//		// Если у этой ноды есть дети, то этот джоинт является родителем
	//		for (u32 c = 0; c < assNode->mNumChildren; c++)
	//		{
	//			auto child = assNode->mChildren[c];
	//			// нахожу джоинт с таким-же именем
	//			auto childJoint = object->m_mdl->GetJointByName(child->mName.data, 0);
	//			if (childJoint)
	//			{
	//				childJoint->m_parentIndex = j; // j это индекс родителя в yyMDL::m_joints
	//			}
	//		}
	//	}

	//	// read animations
	//	for (int a = 0; a < pScene->mNumAnimations; ++a)
	//	{
	//		const aiAnimation* pAnimation = pScene->mAnimations[a];

	//		yyMDLAnimation* newAnimation = yyCreate<yyMDLAnimation>();
	//		newAnimation->m_name = pAnimation->mName.data;
	//		if (!newAnimation->m_name.size())
	//		{
	//			newAnimation->m_name = "Animation";
	//			newAnimation->m_name += a;
	//		}
	//		newAnimation->m_len = pAnimation->mDuration;

	//		for (int c = 0; c < pAnimation->mNumChannels; ++c)
	//		{
	//			auto channel = pAnimation->mChannels[c];
	//			
	//			u32 mdlJointIndex = 0;
	//			auto mdlJoint = object->m_mdl->GetJointByName(channel->mNodeName.data, &mdlJointIndex);
	//			if (mdlJoint)
	//			{
	//				yyMDLAnimation::_joint_info ji;
	//				ji.m_jointID = mdlJointIndex;

	//				for (int k = 0; k < channel->mNumPositionKeys; ++k)
	//				{
	//					auto key = channel->mPositionKeys[k];
	//					ji.m_animationFrames.insertPosition(v3f(key.mValue.x, key.mValue.y, key.mValue.z), (s32)key.mTime);
	//				}
	//			}
	//		}

	//		object->m_mdl->m_animations.push_back(newAnimation);
	//	}


	//		if (is_animated)
	//		{
	//			newMDLLayer->m_model->m_vertexType = yyVertexType::AnimatedModel;
	//			newMDLLayer->m_model->m_stride = sizeof(yyVertexAnimatedModel);
	//			newMDLLayer->m_model->m_vertices = (u8*)yyMemAlloc(verts.size() * sizeof(yyVertexAnimatedModel));
	//			memcpy(newMDLLayer->m_model->m_vertices, verts.data(), verts.size() * sizeof(yyVertexAnimatedModel));
	//		}
	//		else
	//		{
	//			newMDLLayer->m_model->m_vertexType = yyVertexType::Model;
	//			newMDLLayer->m_model->m_stride = sizeof(yyVertexModel);
	//			newMDLLayer->m_model->m_vertices = (u8*)yyMemAlloc(verts.size() * sizeof(yyVertexModel));
	//			auto v_ptr = (yyVertexModel*)newMDLLayer->m_model->m_vertices;
	//			for (u32 o = 0, sz = verts.size(); o < sz; ++o)
	//			{
	//				v_ptr[o].Position = verts[o].Position;
	//				v_ptr[o].Normal = verts[o].Normal;
	//				v_ptr[o].TCoords = verts[o].TCoords;
	//				v_ptr[o].Binormal = verts[o].Binormal;
	//				v_ptr[o].Tangent = verts[o].Tangent;
	//			}
	//		}
	//
	//		if (inds.size() / 3 > 21845)
	//		{
	//			newMDLLayer->m_model->m_indexType = yyMeshIndexType::u32;
	//			newMDLLayer->m_model->m_indices = (u8*)yyMemAlloc(inds.size() * sizeof(u32));
	//			memcpy(newMDLLayer->m_model->m_indices, inds.data(), inds.size() * sizeof(u32));
	//		}
	//		else
	//		{
	//			newMDLLayer->m_model->m_indexType = yyMeshIndexType::u16;
	//			newMDLLayer->m_model->m_indices = (u8*)yyMemAlloc(inds.size() * sizeof(u16));
	//			u16 * i_ptr = (u16*)newMDLLayer->m_model->m_indices;
	//			for (u32 o = 0, sz = inds.size(); o < sz; ++o)
	//			{
	//				*i_ptr = (u16)inds[o];
	//				++i_ptr;
	//			}
	//		}
	//
	//		//newMDLLayer->m_model->m_name = checkName(object, assMesh->mName.C_Str());
	//		newMDLLayer->m_model->m_vCount = verts.size();
	//		newMDLLayer->m_model->m_iCount = inds.size();
	//
	//		newMDLLayer->m_meshGPU = yyGetVideoDriverAPI()->CreateModel(newMDLLayer->m_model);
	//
	//		object->m_mdl->m_layers.push_back(newMDLLayer);
	//		g_layerInfo.push_back(LayerInfo());
	////	}
}
