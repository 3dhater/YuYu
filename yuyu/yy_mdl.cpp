#include "yy.h"

#include "yy_resource.h"
#include "yy_model.h"
#include "io/file_buffer.h"

#include <cassert>
#include <string>
#include "engine.h"

extern Engine * g_engine;

void MDL_loadVersion1(yyMDL** _mdl, yyFileBuffer* f)
{
	yyMDL* newMDL = *_mdl;

	yyMDLHeader mdlHeader;
	f->read(&mdlHeader, sizeof(yyMDLHeader));

	newMDL->m_preRotation = mdlHeader.m_preRotation;
	newMDL->m_aabb.m_min = mdlHeader.m_aabbMin;
	newMDL->m_aabb.m_max = mdlHeader.m_aabbMax;

	auto savePosition = f->tell();
	f->seek(mdlHeader.m_stringsOffset, f->SeekPos_Begin);

	std::vector<std::string> strings;
	{
		u32 numOfStrings = 0;
		f->read(&numOfStrings, sizeof(u32));

		std::string newString;
		for (u32 i = 0; i < numOfStrings; ++i)
		{
			for(u32 o = 0; o < 1000; ++o)
			{
				u8 chr = f->getNextSymbol();
				if (chr != 0)
				{
					newString += (char)chr;
				}
				else
				{
					strings.push_back(newString);
					newString.clear();
					break;
				}
			}
		}
	}

	f->seek(savePosition, f->SeekPos_Begin);
	for (u32 i = 0; i < mdlHeader.m_numOfLayers; ++i)
	{
		yyMDLLayerHeader layerHeader;
		f->read(&layerHeader, sizeof(yyMDLLayerHeader));
		
		yyMDLLayer* newLayer = yyCreate<yyMDLLayer>();
		newLayer->m_model = yyCreate<yyModel>();

		newLayer->m_model->m_material.m_type = (yyMaterialType)layerHeader.m_shaderType;
		
		newLayer->m_model->m_indices  = (u8*)yyMemAlloc(layerHeader.m_indexDataSize);
		newLayer->m_model->m_iCount = layerHeader.m_indexCount;
		newLayer->m_model->m_indexType = (yyMeshIndexType)layerHeader.m_indexType;

		newLayer->m_model->m_vertices = (u8*)yyMemAlloc(layerHeader.m_vertexDataSize);
		newLayer->m_model->m_vCount = layerHeader.m_vertexCount;
		newLayer->m_model->m_vertexType = (yyVertexType)layerHeader.m_vertexType;

		switch (newLayer->m_model->m_vertexType)
		{
		default:
		case yyVertexType::Model:
			newLayer->m_model->m_stride = sizeof(yyVertexModel);
			break;
		case yyVertexType::AnimatedModel:
			newLayer->m_model->m_stride = sizeof(yyVertexAnimatedModel);
			break;
		}
		
		for (int o = 0; o < YY_MDL_LAYER_NUM_OF_TEXTURES; ++o)
		{
			s32 id = layerHeader.m_textureStrID[o];
			if (id != -1)
			{
				newLayer->m_texturePath[o] = strings[id].data();
			}
		}

		f->read(newLayer->m_model->m_vertices, layerHeader.m_vertexDataSize);
		f->read(newLayer->m_model->m_indices, layerHeader.m_indexDataSize);

		newMDL->m_layers.push_back(newLayer);
	}

	for (u32 i = 0; i < mdlHeader.m_numOfJoints; ++i)
	{
		yyMDLJointHeader jointHeader;
		f->read(&jointHeader, sizeof(yyMDLJointHeader));

		yyJoint* newJoint = yyCreate<yyJoint>();
		newJoint->m_name = strings[jointHeader.m_nameStrID].data();
		newJoint->m_matrixBindInverse = jointHeader.m_matrixBindInverse;
		newJoint->m_matrixOffset = jointHeader.m_matrixOffset;
		newJoint->m_matrixWorld = jointHeader.m_matrixWorld;
		newJoint->m_parentIndex = jointHeader.m_parentID;

		newMDL->m_joints.push_back(newJoint);
	}

	for (u32 i = 0; i < mdlHeader.m_numOfAnimations; ++i)
	{
		yyMDLAnimationHeader animHeader;
		f->read(&animHeader, sizeof(yyMDLAnimationHeader));

		yyMDLAnimation* newAnimation = yyCreate<yyMDLAnimation>();
		newAnimation->m_fps = animHeader.m_fps;
		newAnimation->m_flags = animHeader.m_flags;
		newAnimation->m_len = animHeader.m_length;
		newAnimation->m_name = strings[animHeader.m_nameStrID].data();
		for (u32 o = 0; o < animHeader.m_numOfAnimatedJoints; ++o)
		{
			yyMDLAnimation::_joint_info* newJointInfo = yyCreate<yyMDLAnimation::_joint_info>();
			yyMDLAnimatedJointHeader animJointHeader;
			f->read(&animJointHeader, sizeof(yyMDLAnimatedJointHeader));
			newJointInfo->m_jointID = animJointHeader.m_jointID;
			for (u32 k = 0; k < animJointHeader.m_numOfKeyFrames; ++k)
			{
				yyMDLJointKeyframeHeader keyFrameHeader;
				f->read(&keyFrameHeader, sizeof(yyMDLJointKeyframeHeader));
				yyMDLAnimationKeyFrame keyFrame;
				keyFrame.m_position = keyFrameHeader.m_position;
				keyFrame.m_scale = keyFrameHeader.m_scale;
				keyFrame.m_rotation = keyFrameHeader.m_rotation;
				keyFrame.m_time = keyFrameHeader.m_time;
				newJointInfo->m_animationFrames.m_keyFrames.push_back(keyFrame);
			}
			newAnimation->m_animatedJoints.push_back(newJointInfo);
		}
		newMDL->m_animations.push_back(newAnimation);
	}
	
	for (u32 i = 0; i < mdlHeader.m_numOfHitboxes; ++i)
	{
		yyMDLHitboxHeader hitboxHeader;
		f->read(&hitboxHeader, sizeof(yyMDLHitboxHeader));
		yyMDLHitbox* newHitBox = yyCreate<yyMDLHitbox>();
		newHitBox->m_hitboxType = (yyMDLHitbox::HitboxType)hitboxHeader.m_type;
		newHitBox->m_jointID = hitboxHeader.m_jointID;
		switch (newHitBox->m_hitboxType)
		{
		case yyMDLHitbox::HitboxType::Mesh:
		{
			newHitBox->m_mesh = yyCreate<yyModel>();
			newHitBox->m_mesh->m_iCount = hitboxHeader.m_indexCount;
			newHitBox->m_mesh->m_vCount = hitboxHeader.m_vertexCount;
			newHitBox->m_mesh->m_indexType = yyMeshIndexType::u16;
			newHitBox->m_mesh->m_stride = sizeof(yyVertexModel);
			newHitBox->m_mesh->m_vertexType = yyVertexType::Model;
			newHitBox->m_mesh->m_indices = (u8*)yyMemAlloc(newHitBox->m_mesh->m_iCount * sizeof(u16));
			newHitBox->m_mesh->m_vertices = (u8*)yyMemAlloc(newHitBox->m_mesh->m_vCount * newHitBox->m_mesh->m_stride);
			
			f->read(newHitBox->m_mesh->m_vertices, newHitBox->m_mesh->m_vCount * newHitBox->m_mesh->m_stride);
			f->read(newHitBox->m_mesh->m_indices, newHitBox->m_mesh->m_iCount * sizeof(u16));
		}break;
		case yyMDLHitbox::HitboxType::End:
		default:
			break;
		}

		newMDL->m_hitboxes.push_back(newHitBox);
	}
}
