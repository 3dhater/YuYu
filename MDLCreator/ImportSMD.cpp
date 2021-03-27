#include "yy.h"
#include "yy_model.h"
#include "math\math.h"
#include "scene\common.h"
#include "scene\mdl_object.h"

#include "MDLCreator_common.h"
#include "SceneObject.h"

bool g_SMDIsZeroBased = true;

extern SceneObject* g_sceneObject;

struct SMDNode
{
	SMDNode()
	{
		m_ID = 0;
		m_parentID = -1;
	}
	s32 m_ID;
	std::string m_name;
	s32 m_parentID;
};
struct SMDNodeTransformation
{
	SMDNodeTransformation()
	{
		m_boneID = 0;
	}
	s32 m_boneID;
	v3f m_position;
	v3f m_rotation;
};
struct SMDVertex
{
	SMDVertex()
	{
		m_parentBone = 0;
		m_links = 1;
		memset(m_boneID, 0, sizeof(int) * 4);
		memset(m_weight, 0, sizeof(f32) * 4);
	}
	s32 m_parentBone;
	v3f m_position;
	v3f m_normal;
	v2f m_UV;
	s32 m_links;
	s32 m_boneID[4];
	f32 m_weight[4];
};
struct SMDTriangle
{
	std::string m_materialName;
	SMDVertex m_triangle[3];
};
struct SMDKeyFrame
{
	SMDKeyFrame()
	{
		m_time = 0;
	}
	std::vector<SMDNodeTransformation> m_nodeTransforms;
	s32 m_time;
};
char * SMDNextLine(char * ptr){
	while (*ptr){
		if (*ptr == '\n'){
			ptr++;
			return ptr;
		}
		ptr++;
	}
	return ptr;
}
char * SMDSkipSpaces(char * ptr){
	while (*ptr){
		if (*ptr != '\t' && *ptr != ' ' && *ptr != '\f' && *ptr != '\r' && *ptr != '\n' && *ptr != '\v')
			break;
		ptr++;
	}
	return ptr;
}
char * SMDReadString(char * ptr, std::string& str)
{
	ptr = SMDSkipSpaces(ptr);
	str.clear();
	bool isBegin = false;
	while (*ptr) {
		if (isspace(*ptr) && !isBegin)
			break;
		if (*ptr == '\"')
		{
			if (!isBegin)
			{
				isBegin = true;
			}
			else
			{
				ptr++;
				break;
			}
		}
		else
		{
			if(isBegin)
				str += (char)*ptr;
		}
		ptr++;
	}
	return ptr;
}
char * SMDReadWord(char * ptr, std::string& str)
{
	ptr = SMDSkipSpaces(ptr);
	str.clear();
	while (*ptr){
		if (isspace(*ptr))
			break;
		str += (char)*ptr;
		ptr++;
	}
	return ptr;
}
char * SMDReadFloat(char * ptr, f32& value)
{
	ptr = SMDSkipSpaces(ptr);
	char str[32u];
	memset(str, 0, 32);
	char * p = &str[0u];
	while (*ptr) {
		if (!isdigit(*ptr) && (*ptr != '-') && (*ptr != '+')
			&& (*ptr != 'e') && (*ptr != 'E') && (*ptr != '.')) break;
		*p = *ptr;
		++p;
		++ptr;
	}
	value = (f32)atof(str);
	return ptr;
}
char * SMDReadInt(char * ptr, s32& i)
{
	ptr = SMDSkipSpaces(ptr);
	char str[8u];
	memset(str, 0, 8);
	char * p = &str[0u];
	while (*ptr){
		if (!isdigit(*ptr) && *ptr != '-') break;
		*p = *ptr;
		++p;
		++ptr;
	}
	i = atoi(str);
	return ptr;
}

void SMDReadNodes(std::vector<SMDNode>& nodes, char* fileBuffer)
{
	g_SMDIsZeroBased = false;

	bool firstID = true;

	std::string word_name;
	while (*fileBuffer)
	{
		auto savePtr = fileBuffer;
		fileBuffer = SMDReadWord(fileBuffer, word_name);
		if (word_name == "end") break;
		else fileBuffer = savePtr;

		s32 id = 0;
		s32 parentID = 0;
		fileBuffer = SMDReadInt(fileBuffer, id);
		fileBuffer = SMDReadString(fileBuffer, word_name);
		fileBuffer = SMDReadInt(fileBuffer, parentID);

		if (firstID)
		{
			firstID = false;
			if (id == 0)
			{
				g_SMDIsZeroBased = true;
			}
		}

		if (!g_SMDIsZeroBased)
		{
			--id;

			if (parentID > 0)
				--parentID;
		}

		SMDNode newNode;
		newNode.m_ID = id;
		newNode.m_name = word_name;
		newNode.m_parentID = parentID;
		nodes.push_back(newNode);
	}
}
s32 SMDReadSkeleton(std::vector<SMDKeyFrame>& skeleton, char* fileBuffer)
{
	s32 num_of_frames = 0;
	std::string word_time;
	std::string word_frameNum;
	std::string word_boneID;
	v3f position;
	v3f rotation;

	SMDKeyFrame* keyFramePtr = 0;

	while (*fileBuffer)
	{
		auto savePtr = fileBuffer;

		fileBuffer = SMDReadWord(fileBuffer, word_time);
		if (word_time == "end")
			break;

		fileBuffer = SMDReadWord(fileBuffer, word_frameNum);
		if (word_frameNum == "end")
			break;

		s32 time = atoi(word_frameNum.data());

		if (word_time == "time")
		{
			++num_of_frames;
			savePtr = fileBuffer;
			std::string checkNext;
			fileBuffer = SMDReadWord(fileBuffer, checkNext);
			fileBuffer = savePtr;
			if (checkNext == "end")
				break;
			if (checkNext != "time")
			{
				SMDKeyFrame newKeyFrame;
				newKeyFrame.m_time = time;
				skeleton.push_back(newKeyFrame);
				keyFramePtr = &skeleton[skeleton.size() - 1];
			}
			else
				continue;
		}
		else
		{
			fileBuffer = savePtr;
		}


		fileBuffer = SMDReadWord(fileBuffer, word_boneID);
		if (word_boneID == "end")
			break;

		auto boneID = atoi(word_boneID.data());
		if (!g_SMDIsZeroBased)
		{
			--boneID;
		}

		fileBuffer = SMDReadFloat(fileBuffer, position.x);
		fileBuffer = SMDReadFloat(fileBuffer, position.y);
		fileBuffer = SMDReadFloat(fileBuffer, position.z);
		fileBuffer = SMDReadFloat(fileBuffer, rotation.x);
		fileBuffer = SMDReadFloat(fileBuffer, rotation.y);
		fileBuffer = SMDReadFloat(fileBuffer, rotation.z);

		SMDNodeTransformation nt;
		nt.m_boneID = boneID;
		nt.m_position = position;
		nt.m_rotation = rotation;
		keyFramePtr->m_nodeTransforms.push_back(nt);
	}

	if (skeleton.size())
	{
		auto t = std::abs(0 - skeleton[0].m_time);
		for (s32 i = 0, sz = skeleton.size(); i < sz; ++i)
		{
			skeleton[i].m_time -= t;
		}
	}


	return num_of_frames;
}
bool SMDIsEndLine(char* fileBuffer)
{
	while (*fileBuffer) 
	{
		if (isspace(*fileBuffer))
		{
			if (*fileBuffer == '\n')
				return true;
		}
		else break;
		++fileBuffer;
	}
	return false;
}
void SMDReadTriangles(std::vector<SMDTriangle>& triangles, char* fileBuffer)
{
	std::string word_material;
	while (*fileBuffer)
	{
		fileBuffer = SMDReadWord(fileBuffer, word_material);
		if (word_material == "end")
			break;

		SMDTriangle newTriangle;
		newTriangle.m_materialName = word_material;

		for (int i = 0; i < 3; ++i)
		{
			fileBuffer = SMDReadInt(fileBuffer, newTriangle.m_triangle[i].m_parentBone);
			fileBuffer = SMDReadFloat(fileBuffer, newTriangle.m_triangle[i].m_position.x);
			fileBuffer = SMDReadFloat(fileBuffer, newTriangle.m_triangle[i].m_position.y);
			fileBuffer = SMDReadFloat(fileBuffer, newTriangle.m_triangle[i].m_position.z);
			fileBuffer = SMDReadFloat(fileBuffer, newTriangle.m_triangle[i].m_normal.x);
			fileBuffer = SMDReadFloat(fileBuffer, newTriangle.m_triangle[i].m_normal.y);
			fileBuffer = SMDReadFloat(fileBuffer, newTriangle.m_triangle[i].m_normal.z);
			fileBuffer = SMDReadFloat(fileBuffer, newTriangle.m_triangle[i].m_UV.x);
			fileBuffer = SMDReadFloat(fileBuffer, newTriangle.m_triangle[i].m_UV.y);
			newTriangle.m_triangle[i].m_UV.y = 1.f - newTriangle.m_triangle[i].m_UV.y;

			newTriangle.m_triangle[i].m_links = 1;
			newTriangle.m_triangle[i].m_boneID[0] = newTriangle.m_triangle[i].m_parentBone;
			newTriangle.m_triangle[i].m_weight[0] = 1.f;

			if (!SMDIsEndLine(fileBuffer))
			{
				fileBuffer = SMDReadInt(fileBuffer, newTriangle.m_triangle[i].m_links);
				if (newTriangle.m_triangle[i].m_links > 4)
				{
					newTriangle.m_triangle[i].m_links = 4;
					yyLogWriteWarning("SMD: links > 4\n");
				}
				for (int q = 0; q < newTriangle.m_triangle[i].m_links; ++q)
				{
					fileBuffer = SMDReadInt(fileBuffer, newTriangle.m_triangle[i].m_boneID[q]);

					if (!g_SMDIsZeroBased)
					{
						--newTriangle.m_triangle[i].m_boneID[q];
					}

					fileBuffer = SMDReadFloat(fileBuffer, newTriangle.m_triangle[i].m_weight[q]);
				}
			}

		}
		triangles.push_back(newTriangle);
	}
}

// up / down
#define	PITCH	0
// left / right
#define	YAW		1
// fall over
#define	ROLL	2 
void SMDAngleMatrix(v3f& angles, Mat4& matrix)
{
	float		angle;
	float		sr, sp, sy, cr, cp, cy;

	angle = angles[YAW];// *(math::PI * 2 / 360);
	sy = sin(angle);
	cy = cos(angle);
	angle = angles[PITCH];// * (math::PI * 2 / 360);
	sp = sin(angle);
	cp = cos(angle);
	angle = angles[ROLL];// * (math::PI * 2 / 360);
	sr = sin(angle);
	cr = cos(angle);

	// matrix = (YAW * PITCH) * ROLL
	matrix[0][0] = cp*cy;
	matrix[0][1] = cp*sy;
	matrix[0][2] = -sp;
	matrix[1][0] = sr*sp*cy + cr*-sy;
	matrix[1][1] = sr*sp*sy + cr*cy;
	matrix[1][2] = sr*cp;
	matrix[2][0] = (cr*sp*cy + -sr*-sy);
	matrix[2][1] = (cr*sp*sy + -sr*cy);
	matrix[2][2] = cr*cp;
	matrix[0][3] = 0.0;
	matrix[1][3] = 0.0;
	matrix[2][3] = 0.0;
}

void ImportSMD(yyMDLObject* object, const char* file)
{
	std::vector<SMDNode> nodes;
	std::vector<SMDKeyFrame> skeleton;
	std::vector<SMDTriangle> triangles;

	auto fileSz = yy_fs::file_size(file);
	char * fileBuffer = new char[fileSz + 1];
	fileBuffer[fileSz] = 0;

	auto deletePtr = fileBuffer;

	FILE * f = fopen(file, "rb");
	fread(fileBuffer, fileSz, 1, f);
	fclose(f);

	s32 num_of_frames = 0;
	std::string word;
	while (*fileBuffer)
	{
		fileBuffer = SMDReadWord(fileBuffer, word);

		// skip comments
		//  "Stand-alone line comments start with // (double foreslash)"
		if (word == "//")
		{
			fileBuffer = SMDNextLine(fileBuffer);
			continue;
		}
		else if(word.size() > 2)
		{
			if (word[0] == '/' && word[1] == '/')
			{
				fileBuffer = SMDNextLine(fileBuffer);
				continue;
			}
		}

		if (word == "nodes")
		{
			SMDReadNodes(nodes, fileBuffer);
		}
		else if (word == "skeleton")
		{
			num_of_frames = SMDReadSkeleton(skeleton, fileBuffer);
		}
		else if (word == "triangles")
		{
			SMDReadTriangles(triangles, fileBuffer);
		}
	}

	// create joints
	for (size_t i = 0, sz = nodes.size(); i < sz; ++i)
	{
		auto & node = nodes[i];

		auto joint = object->m_mdl->GetJointByName(node.m_name.data(), 0);
		if (joint)
			continue;

		yyJoint* newJoint = yyCreate<yyJoint>();
		newJoint->m_name = node.m_name.data();
		newJoint->m_parentIndex = node.m_parentID; // better to find parents later
		//newJoint->m_children
		//newJoint->m_matrixBind.setRotation(Quat(node.));
		object->m_mdl->m_joints.push_back(newJoint);
	}

	yyMDLAnimation* newMDLAnimation = 0;
	bool isAnimation = skeleton.size() > 1;
	if (isAnimation)
	{
		newMDLAnimation = yyCreate<yyMDLAnimation>();
		newMDLAnimation->m_len = num_of_frames - 1; 
		yy_fs::path p = file;
		auto fn = p.filename();
		newMDLAnimation->m_name = fn.generic_string().data();
		util::stringPopBackBefore(newMDLAnimation->m_name, '.');
		newMDLAnimation->m_name.pop_back(); // .
	
		newMDLAnimation->m_name = AnimationCheckName(object, newMDLAnimation->m_name.data());
		object->m_mdl->m_animations.push_back(newMDLAnimation);
	}

	// skeleton
	for (size_t i = 0, sz = skeleton.size(); i < sz; ++i)
	{
		auto & f = skeleton[i];
		for (size_t i2 = 0, sz2 = f.m_nodeTransforms.size(); i2 < sz2; ++i2)
		{
			auto & nt = f.m_nodeTransforms[i2];
			auto & node = nodes[nt.m_boneID];
			s32 jointID = 0;
			auto joint = object->m_mdl->GetJointByName(node.m_name.data(), &jointID);
			if (joint)
			{
				if (isAnimation)
				{
					Mat4 R;
					SMDAngleMatrix(v3f(nt.m_rotation.y, nt.m_rotation.z, nt.m_rotation.x), R);
					Quat q = math::matToQuat(R);
					newMDLAnimation->AddKeyFrame(jointID, f.m_time, nt.m_position, q, v3f(1.f,1.f,1.f));
				}
				else
				{
					Mat4 R;					
					SMDAngleMatrix(v3f(nt.m_rotation.y, nt.m_rotation.z, nt.m_rotation.x), R);

					/*Quat qX(v3f(1.f, 0.f, 0.f), -nt.m_rotation.x);
					Quat qY(v3f(0.f, 1.f, 0.f), -nt.m_rotation.y);
					Quat qZ(v3f(0.f, 0.f, 1.f), -nt.m_rotation.z);
					Quat qR = qX * qY * qZ;
					qR.normalize();
					R.setRotation(qR);*/

					Mat4 T;
					T[3] = nt.m_position;
					T[3].w = 1.f;
					
					if (node.m_parentID == -1)
						object->m_mdl->m_preRotation.setRotation(Quat(math::degToRad(90.f), 0.f, 0.f));

					Mat4 localMatrix = T * R;

					if (node.m_parentID == -1)
					{
						joint->m_matrixWorld = localMatrix;
					}
					else
					{
						joint->m_matrixWorld =
							object->m_mdl->m_joints[node.m_parentID]->m_matrixWorld *
							localMatrix;
					}

					joint->m_matrixBindInverse = joint->m_matrixWorld;
					joint->m_matrixBindInverse.invert();
					joint->m_matrixOffset = joint->m_matrixBindInverse * joint->m_matrixWorld;
					joint->m_matrixOffset.invert();

					joint->m_matrixOffset = joint->m_matrixOffset * joint->m_matrixBindInverse;
					//printf("i2 %u\n", i2);
				}
			}
		}
	}

	// need to find unique material names.
	// every unique material name is new MDLLayer
	std::vector<std::string> materialNames;
	if (!isAnimation)
	{
		for (size_t i = 0, sz = triangles.size(); i < sz; ++i)
		{
			auto & currentMaterialName = triangles[i].m_materialName;

			bool needToAdd = true;

			for (size_t o = 0, osz = materialNames.size(); o < osz; ++o)
			{
				if (materialNames[o] == currentMaterialName)
				{
					needToAdd = false;
					break;
				}
			}

			if (needToAdd)
			{
				materialNames.push_back(currentMaterialName);
			}
		}

		for (size_t i = 0, sz = materialNames.size(); i < sz; ++i)
		{
			yyMDLLayer* newMDLLayer = yyCreate<yyMDLLayer>();
			newMDLLayer->m_model = yyCreate<yyModel>();

			newMDLLayer->m_model->m_name = "Model";

			yyArray<yyVertexAnimatedModel> verts;
			verts.setAddMemoryValue(0xffff);
			yyArray<u32> inds;
			inds.setAddMemoryValue(0xffff);

			for (size_t k = 0, ksz = triangles.size(); k < ksz; ++k)
			{
				auto & tri = triangles[k];
				if (tri.m_materialName != materialNames[i])
					continue;

				if (tri.m_materialName.size())
					newMDLLayer->m_model->m_name.clear();

				for (u32 s = 0, ssz = tri.m_materialName.size(); s < ssz; ++s)
				{
					auto ch = tri.m_materialName[s];
					if (ch == '.')
						break;
					newMDLLayer->m_model->m_name += ch;
				}

				yyVertexAnimatedModel v1, v2, v3;

				v1.Position = tri.m_triangle[0].m_position;
				v2.Position = tri.m_triangle[1].m_position;
				v3.Position = tri.m_triangle[2].m_position;

				v1.Normal = tri.m_triangle[0].m_normal;
				v2.Normal = tri.m_triangle[1].m_normal;
				v3.Normal = tri.m_triangle[2].m_normal;

				v1.TCoords = tri.m_triangle[0].m_UV;
				v2.TCoords = tri.m_triangle[1].m_UV;
				v3.TCoords = tri.m_triangle[2].m_UV;

				v1.Bones.x = tri.m_triangle[0].m_boneID[0];
				v1.Bones.y = tri.m_triangle[0].m_boneID[1];
				v1.Bones.z = tri.m_triangle[0].m_boneID[2];
				v1.Bones.w = tri.m_triangle[0].m_boneID[3];
				v2.Bones.x = tri.m_triangle[1].m_boneID[0];
				v2.Bones.y = tri.m_triangle[1].m_boneID[1];
				v2.Bones.z = tri.m_triangle[1].m_boneID[2];
				v2.Bones.w = tri.m_triangle[1].m_boneID[3];
				v3.Bones.x = tri.m_triangle[2].m_boneID[0];
				v3.Bones.y = tri.m_triangle[2].m_boneID[1];
				v3.Bones.z = tri.m_triangle[2].m_boneID[2];
				v3.Bones.w = tri.m_triangle[2].m_boneID[3];

				v1.Weights.x = tri.m_triangle[0].m_weight[0];
				v1.Weights.y = tri.m_triangle[0].m_weight[1];
				v1.Weights.z = tri.m_triangle[0].m_weight[2];
				v1.Weights.w = tri.m_triangle[0].m_weight[3];
				v2.Weights.x = tri.m_triangle[1].m_weight[0];
				v2.Weights.y = tri.m_triangle[1].m_weight[1];
				v2.Weights.z = tri.m_triangle[1].m_weight[2];
				v2.Weights.w = tri.m_triangle[1].m_weight[3];
				v3.Weights.x = tri.m_triangle[2].m_weight[0];
				v3.Weights.y = tri.m_triangle[2].m_weight[1];
				v3.Weights.z = tri.m_triangle[2].m_weight[2];
				v3.Weights.w = tri.m_triangle[2].m_weight[3];

				verts.push_back(v1);
				verts.push_back(v2);
				verts.push_back(v3);

				auto ind = inds.size();
				inds.push_back(ind + 0);
				inds.push_back(ind + 1);
				inds.push_back(ind + 2);
			}

			newMDLLayer->m_model->m_name = ModelCheckName(object, newMDLLayer->m_model->m_name.data());
			newMDLLayer->m_model->m_vertexType = yyVertexType::AnimatedModel;
			newMDLLayer->m_model->m_stride = sizeof(yyVertexAnimatedModel);
			newMDLLayer->m_model->m_vertices = (u8*)yyMemAlloc(verts.size() * sizeof(yyVertexAnimatedModel));
			memcpy(newMDLLayer->m_model->m_vertices, verts.data(), verts.size() * sizeof(yyVertexAnimatedModel));
			if (inds.size() / 3 > 21845)
			{
				newMDLLayer->m_model->m_indexType = yyMeshIndexType::u32;
				newMDLLayer->m_model->m_indices = (u8*)yyMemAlloc(inds.size() * sizeof(u32));
				memcpy(newMDLLayer->m_model->m_indices, inds.data(), inds.size() * sizeof(u32));
			}
			else
			{
				newMDLLayer->m_model->m_indexType = yyMeshIndexType::u16;
				newMDLLayer->m_model->m_indices = (u8*)yyMemAlloc(inds.size() * sizeof(u16));
				u16 * i_ptr = (u16*)newMDLLayer->m_model->m_indices;
				for (u32 o = 0, sz = inds.size(); o < sz; ++o)
				{
					*i_ptr = (u16)inds[o];
					++i_ptr;
				}
			}

			newMDLLayer->m_model->m_vCount = verts.size();
			newMDLLayer->m_model->m_iCount = inds.size();

			newMDLLayer->m_meshGPU = yyGetVideoDriverAPI()->CreateModel(newMDLLayer->m_model);

			object->m_mdl->m_layers.push_back(newMDLLayer);
			g_sceneObject->m_layerInfo.push_back(LayerInfo());
		}
	} // !isAnimation

	if (isAnimation)
	{
		auto newState = g_sceneObject->m_mdlObject->AddState(newMDLAnimation->m_name.to_stringA().c_str());
		
		yyMDLObjectStateNode* sn = yyCreate<yyMDLObjectStateNode>();
		sn->m_animation = newMDLAnimation;
		for (u16 i = 0, sz = newMDLAnimation->m_animatedJoints.size(); i < sz; ++i)
		{
			sn->m_animatedJoints.push_back((s32)i);
		}

		newState->m_stateNodesWithAnimations.push_back(sn);

		g_sceneObject->m_MDLObjectStates.push_back(newState);
	}

	delete deletePtr;
}


