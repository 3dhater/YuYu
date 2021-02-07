﻿#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_opengl3.h"

#include <GL/gl3w.h>

#include "yy.h"
#include "yy_window.h"

#include "yy_ptr.h"
#include "math/math.h"

#include "yy_async.h"
#include "yy_resource.h"

#include "strings/string.h"

#include <cstdio>
#include <vector>
#include <string>
#include <iostream>

#include "yy_fs.h"
#include "yy_image.h"
#include "yy_model.h"
#include "yy_gui.h"
#include "yy_input.h"

#include "camera.h"
#include "scene/sprite.h"
#include "scene/mdl_object.h"

//#include <assimp/Importer.hpp>      // C++ importer interface
//#include <assimp/scene.h>       // Output data structure
//#include <assimp/postprocess.h> // Post processing flags

yyVideoDriverAPI* g_videoDriver = nullptr;
yyInputContext* g_inputContex = nullptr;

yyStringA g_renameTextBuffer;
const u32 g_renameTextBufferSize = 128;
int g_shaderCombo_item_current = 0;

int g_selectedLayer = -1;
int g_selectedAnimation = -1;
int g_selectedHitbox = -1;
bool g_bindPose = false;

bool g_SMDIsZeroBased = true;

void SaveMDL(const wchar_t* fileName);

// для того чтобы настраивать модель в редакторе
// например, можно изменить позицию
// При сохранении нужно будет учитывать эти данные
struct LayerInfo
{
	LayerInfo()
	{
		m_texturePathTextBufferSize = 512;
		m_texturePathTextBuffer.reserve(m_texturePathTextBufferSize);
	}
	v3f m_offset;
	yyStringA m_texturePathTextBuffer;
	u32 m_texturePathTextBufferSize;
};

struct Hitbox
{
	Hitbox()
	{
		m_min.set(-0.1f, -0.1f, -0.1f);
		m_max.set(0.1f, 0.1f, 0.1f);

		m_type = type::Box;
		m_gpuModel = 0;
	}
	~Hitbox()
	{
		if (m_gpuModel) g_videoDriver->DeleteModel(m_gpuModel);
	}

	enum type
	{
		Box
	}m_type;

	void rebuild()
	{
		if (m_gpuModel) g_videoDriver->DeleteModel(m_gpuModel);
		if (m_hitbox.m_mesh) yyDestroy(m_hitbox.m_mesh);

		if (m_type == type::Box)
		{
			m_hitbox.m_mesh = yyCreate<yyModel>();
			m_hitbox.m_mesh->m_vertexType = yyVertexType::Model;
			m_hitbox.m_mesh->m_indexType = yyMeshIndexType::u16;
			m_hitbox.m_mesh->m_stride = sizeof(yyVertexModel);
			m_hitbox.m_mesh->m_vCount = 8;
			m_hitbox.m_mesh->m_iCount = 36;
			m_hitbox.m_mesh->m_vertices = (u8*)yyMemAlloc(m_hitbox.m_mesh->m_vCount * m_hitbox.m_mesh->m_stride);
			m_hitbox.m_mesh->m_indices = (u8*)yyMemAlloc(m_hitbox.m_mesh->m_iCount * sizeof(u16));

			yyVertexModel* vertex = (yyVertexModel*)m_hitbox.m_mesh->m_vertices;
			u16* index = (u16*)m_hitbox.m_mesh->m_indices;

			vertex[0].Position.set(m_min.x, m_min.y, m_min.z);
			vertex[1].Position.set(m_min.x, m_min.y, m_max.z);
			vertex[2].Position.set(m_max.x, m_min.y, m_min.z);
			vertex[3].Position.set(m_max.x, m_min.y, m_max.z);

			vertex[4].Position.set(m_min.x, m_max.y, m_min.z);
			vertex[5].Position.set(m_min.x, m_max.y, m_max.z);
			vertex[6].Position.set(m_max.x, m_max.y, m_min.z);
			vertex[7].Position.set(m_max.x, m_max.y, m_max.z);
			
			// Bottom
			index[0] = 0;
			index[1] = 1;
			index[2] = 2;
			index[3] = 2;
			index[4] = 1;
			index[5] = 3;

			// Top
			index[6]  = 4;
			index[7]  = 6;
			index[8]  = 5;
			index[9]  = 6;
			index[10] = 7;
			index[11] = 5;

			// Left
			index[12] = 3;
			index[13] = 6;
			index[14] = 2;
			index[15] = 3;
			index[16] = 7;
			index[17] = 6;

			// Right
			index[18] = 0;
			index[19] = 5;
			index[20] = 1;
			index[21] = 0;
			index[22] = 4;
			index[23] = 5;

			// front
			index[24] = 2;
			index[25] = 4;
			index[26] = 0;
			index[27] = 2;
			index[28] = 6;
			index[29] = 4;

			// back
			index[30] = 3;
			index[31] = 5;
			index[32] = 1;
			index[33] = 3;
			index[34] = 7;
			index[35] = 5;

			m_gpuModel = g_videoDriver->CreateModel(m_hitbox.m_mesh);
		}
	}


	yyMDLHitbox m_hitbox;
	yyResource* m_gpuModel;

	v3f m_min;
	v3f m_max;

	yyStringA m_name;
};

// У движка есть только примитивы для создания более сложных объектов.
// Например, тот объект который рисуется в редакторе не совсем простой. 
// У него есть слои - подмодели со своими текстурами и шейдерами.
// У объекта может быть анимация, много анимаций. Для каждой анимации
//    нужен свой yyMDLObjectState.
struct  SceneObject
{
	SceneObject()
	{
		m_mdlObject = 0;
	}
	~SceneObject()
	{
		for (u16 i = 0, sz = m_hitboxes.size(); i < sz; ++i)
		{
			yyDestroy(m_hitboxes[i]);
		}
		for (u16 i = 0, sz = m_MDLObjectStates.size(); i < sz; ++i)
		{
			yyDestroy(m_MDLObjectStates[i]);
		}
		if (m_mdlObject)
		{
			yyDestroy(m_mdlObject);
		}
	}
	bool init()
	{
		m_mdlObject = yyCreate<yyMDLObject>();
		m_mdlObject->m_mdl = yyCreate<yyMDL>();
		return true;
	}
	void update(f32 dt)
	{
		m_mdlObject->Update(dt);
	}
	yyMDLObjectState* getState(const char* name)
	{
		for (u16 i = 0; i < m_MDLObjectStates.size(); ++i)
		{
			if (strcmp(m_MDLObjectStates[i]->m_name.data(), name) == 0)
				return m_MDLObjectStates[i];
		}
		return nullptr;
	}
	yyMDLObject* m_mdlObject;
	yyArraySmall<LayerInfo> m_layerInfo;
	yyArraySmall<yyMDLObjectState*> m_MDLObjectStates;
	yyArraySmall<Hitbox*> m_hitboxes;
};
SceneObject* g_sceneObject = 0;
//Mat4 aiMatrixToGameMatrix(const aiMatrix4x4& a)
//{
//	Mat4 m;
//	auto dst = m.getPtr();
//	f32 * src = (f32*)&a.a1;
//	for (int i = 0; i < 16; ++i)
//	{
//		dst[i] = src[i];
//	}
//	return m;
//}
//aiNode* aiFindNode(const char* name, aiNode* node)
//{
//	if (strcmp(node->mName.data, name) == 0)
//	{
//		return node;
//	}
//	for (int i = 0; i < node->mNumChildren; ++i)
//	{
//		auto n = aiFindNode(name, node->mChildren[i]);
//		if (n)
//			return n;
//	}
//	return 0;
//}

struct yyEngineContext
{
	yyEngineContext()
	{
		m_state = nullptr;
	}
	~yyEngineContext()
	{
		yyStop(); // destroy main class, free memory
	}

	void init(yyInputContext* input)
	{
		m_state = yyStart(input); // allocate memory for main class inside yuyu.dll
	}

	yySystemState * m_state;
};
void window_onCLose(yyWindow* window)
{
	window->Hide();
	yyQuit(); // change yySystemState - set yySystemState::Quit
}
void log_onError(const char* message)
{
	fprintf(stderr, message);
}
void log_onInfo(const char* message)
{
	fprintf(stdout, message);
}


void window_callbackMouse(yyWindow* w, s32 wheel, s32 x, s32 y, u32 click)
{
	g_inputContex->m_cursorCoords.x = (f32)x;
	g_inputContex->m_cursorCoords.y = (f32)y;

	g_inputContex->m_mouseDelta.x = (f32)x - g_inputContex->m_cursorCoordsOld.x;
	g_inputContex->m_mouseDelta.y = (f32)y - g_inputContex->m_cursorCoordsOld.y;

	g_inputContex->m_cursorCoordsOld = g_inputContex->m_cursorCoords;

	if (click & yyWindow_mouseClickMask_LMB_DOWN)
	{
		g_inputContex->m_isLMBDown = true;
		g_inputContex->m_isLMBHold = true;
	}
	if (click & yyWindow_mouseClickMask_LMB_UP)
	{
		g_inputContex->m_isLMBHold = false;
		g_inputContex->m_isLMBUp = true;
	}
	if (click & yyWindow_mouseClickMask_LMB_DOUBLE)
	{
		g_inputContex->m_isLMBDbl = true;
	}

	if (click & yyWindow_mouseClickMask_RMB_DOWN)
	{
		g_inputContex->m_isRMBDown = true;
		g_inputContex->m_isRMBHold = true;
	}
	if (click & yyWindow_mouseClickMask_RMB_UP)
	{
		g_inputContex->m_isRMBHold = false;
	}
}
void updateInputContext() // call before all callbacks
{
	g_inputContex->m_isLMBDbl = false;
	g_inputContex->m_isLMBDown = false;
	g_inputContex->m_isRMBDown = false;
	g_inputContex->m_isLMBUp = false;
	g_inputContex->m_mouseDelta.x = 0.f;
	g_inputContex->m_mouseDelta.y = 0.f;
}

yyStringA AnimationCheckName(yyMDLObject* object, yyStringA name)
{
	static int number = 0;
	yyStringA newName = name;

check_again:;

	int count = 0;
	for (u16 i = 0; i < object->m_mdl->m_animations.size(); ++i)
	{
		auto a = object->m_mdl->m_animations[i];
		if (a->m_name == newName)
			++count;
	}
	if (count > 0)
	{
		newName += number++;
		goto check_again;
	}
	return newName;
}
yyStringA ModelCheckName(yyMDLObject* object, yyStringA name)
{
	static int number = 0;
	yyStringA newName = name;

check_again:;

	int count = 0;
	for (u16 i = 0; i < object->m_mdl->m_layers.size(); ++i)
	{
		auto layer = object->m_mdl->m_layers[i];
		if (layer->m_model->m_name == newName)
		{
			++count;
		}
	}
	if (count > 0)
	{
		newName += number++;
		goto check_again;
	}
	return newName;
}

//void aiPrintNames(aiNode* node, int depth)
//{
//	for (int i = 0; i < depth; ++i)
//	{
//		printf("  ");
//	}
//	if (node->mName.data)
//		printf("%s", node->mName.data);
//	else
//		printf("[noname]");
//	printf("\n");
//	for (int i = 0; i < node->mNumChildren; ++i)
//	{
//		aiPrintNames(node->mChildren[i], depth + 1);
//	}
//}

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
char * skipSpaces(char * ptr){
	while (*ptr){
		if (*ptr != '\t' && *ptr != ' ' && *ptr != '\f' && *ptr != '\r' && *ptr != '\n' && *ptr != '\v')
			break;
		ptr++;
	}
	return ptr;
}
char * SMDReadString(char * ptr, std::string& str)
{
	ptr = skipSpaces(ptr);
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
	ptr = skipSpaces(ptr);
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
	ptr = skipSpaces(ptr);
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
	ptr = skipSpaces(ptr);
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
	s32 time = 0;

	while (*fileBuffer)
	{
		auto savePtr = fileBuffer;

		fileBuffer = SMDReadWord(fileBuffer, word_time);
		if (word_time == "end")
			break;

		fileBuffer = SMDReadWord(fileBuffer, word_frameNum);
		if (word_frameNum == "end")
			break;

		time = atoi(word_frameNum.data());

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
void readSMD(yyMDLObject* object, const char* file)
{
	std::vector<SMDNode> nodes;
	std::vector<SMDKeyFrame> skeleton;
	std::vector<SMDTriangle> triangles;

	auto fileSz = yyFS::file_size(file);
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
		yyFS::path p = file;
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
					newMDLAnimation->AddKeyFrame(jointID, f.m_time, nt.m_position, q);
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

				inds.push_back(inds.size());
				inds.push_back(inds.size());
				inds.push_back(inds.size());
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

		newState->m_animations.push_back(sn);

		g_sceneObject->m_MDLObjectStates.push_back(newState);
	}

	delete deletePtr;
}



// load model, create new layer
void newLayer(yyMDLObject* object, const char16_t* file)
{
	yyStringA stra;
	stra = file;

	bool is_animated = false;
	yyArray<yyVertexAnimatedModel> verts;
	yyArray<u32> inds;

	yyFS::path path = file;
	auto ext = path.extension();
	
	if (ext.string_type == L".smd")
	{
		is_animated = true;
		readSMD(object, stra.data());
	}
	else
	{

	}


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

	g_selectedLayer = object->m_mdl->m_layers.size()-1;
	object->SetMDL(object->m_mdl);
	// update MDL aabb
}
void deleteLayer(yyMDLObject* object)
{
	auto layer = object->m_mdl->m_layers[g_selectedLayer];
	object->m_mdl->m_layers.erase(g_selectedLayer);
	g_sceneObject->m_layerInfo.erase(g_selectedLayer);
	if (layer->m_meshGPU)
	{
		yyGetVideoDriverAPI()->DeleteModel(layer->m_meshGPU);
		layer->m_meshGPU = nullptr;
	}
	yyDestroy(layer);
	// update MDL aabb

	if (object->m_mdl->m_layers.size() == g_selectedLayer)
		--g_selectedLayer;
}
void reloadTexture(yyMDLObject* object, int textureSlot, const wchar_t* path)
{
	auto layer = object->m_mdl->m_layers[g_selectedLayer];

	if (!yyFS::exists(yyFS::path(path)))
	{
		printf("Not exist\n");
		switch (textureSlot)
		{
		case 0: 
		if (layer->m_textureGPU1) 
		{
			yyGetVideoDriverAPI()->DeleteTexture(layer->m_textureGPU1); 
			layer->m_textureGPU1 = 0;
		}
		break;
		case 1:
			if (layer->m_textureGPU2)
			{
				yyGetVideoDriverAPI()->DeleteTexture(layer->m_textureGPU2);
				layer->m_textureGPU2 = 0;
			}
			break;
		case 2:
			if (layer->m_textureGPU3)
			{
				yyGetVideoDriverAPI()->DeleteTexture(layer->m_textureGPU3);
				layer->m_textureGPU3 = 0;
			}
			break;
		case 3:
			if (layer->m_textureGPU4)
			{
				yyGetVideoDriverAPI()->DeleteTexture(layer->m_textureGPU4);
				layer->m_textureGPU4 = 0;
			}
			break;
		default: YY_PRINT_FAILED; break;
		}
		return;
	}


	yyResource* texture = 0;
	switch (textureSlot)
	{
	case 0: texture = layer->m_textureGPU1; break;
	case 1: texture = layer->m_textureGPU2; break;
	case 2: texture = layer->m_textureGPU3; break;
	case 3: texture = layer->m_textureGPU4; break;
	default: YY_PRINT_FAILED; break;
	}
	
	if (texture)
	{
		yyGetVideoDriverAPI()->DeleteTexture(texture);
	}

	yyStringA str;
	str = path;
	texture = yyGetVideoDriverAPI()->CreateTextureFromFile(str.data(), true, false, true);

	switch (textureSlot)
	{
	case 0: layer->m_textureGPU1 = texture; break;
	case 1: layer->m_textureGPU2 = texture; break;
	case 2: layer->m_textureGPU3 = texture; break;
	case 3: layer->m_textureGPU4 = texture; break;
	default: YY_PRINT_FAILED; break;
	}
}
int main(int argc, char* argv[])
{
	bool useOpenGL = false;
	for (int i = 0; i < argc; ++i)
	{
		if (strcmp(argv[i], "opengl") == 0)
		{
			useOpenGL = true;
		}
	}

	yyPtr<yyInputContext> inputContext = yyCreate<yyInputContext>();
	g_inputContex = inputContext.m_data;

	yyPtr<yyEngineContext> engineContext = yyCreate<yyEngineContext>();
	engineContext.m_data->init(inputContext.m_data);
	auto p_engineContext = engineContext.m_data;

	g_renameTextBuffer.reserve(g_renameTextBufferSize);

	yyLogSetErrorOutput(log_onError);
	yyLogSetInfoOutput(log_onInfo);
	yyLogSetWarningOutput(log_onError);

	yyPtr<yyWindow> window = yyCreate<yyWindow>();
	if (!window.m_data)
	{
		YY_PRINT_FAILED;
		return 1;
	}
	auto p_window = window.m_data;

	Camera camera;
	camera.init();

	RECT rc;
	GetWindowRect(GetDesktopWindow(), &rc);
	rc.left = 0; rc.top = 0; rc.right = 1200; rc.bottom = 800;
	if (!p_window->init(rc.right - rc.left, rc.bottom - rc.top, 0))
	{
		YY_PRINT_FAILED;
		return 1;
	}

	p_window->m_onClose = window_onCLose;
	p_window->m_onMouseButton = window_callbackMouse;
	yySetMainWindow(p_window);
	//p_window->ToFullscreenMode();

	// init video driver
	const char * videoDriverType = "d3d11.yyvd";
	if(useOpenGL)
		videoDriverType = "opengl.yyvd";
	if (!yyInitVideoDriver(videoDriverType, p_window))
	{
		YY_PRINT_FAILED;
		return 0;
	}
	g_videoDriver = yyGetVideoDriverAPI();
	g_videoDriver->SetClearColor(0.3f, 0.3f, 0.4f, 1.f);
	g_videoDriver->UseVSync(true);

	{
		yyStringA t;
		t = "MDLCreator";
		t += " - ";
		t += g_videoDriver->GetVideoDriverName();
		p_window->SetTitle(t.data());
	}

	yyPtr<SceneObject> new_model = yyCreate<SceneObject>();
	g_sceneObject = new_model.m_data;
	g_sceneObject->init();

	yyVideoDriverObjectD3D11* vdo = (yyVideoDriverObjectD3D11*)g_videoDriver->GetVideoDriverObjects();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(p_window->m_hWnd);
	if (useOpenGL)
	{
		gl3wInit();
		const char* glsl_version = "#version 330";
		ImGui_ImplOpenGL3_Init(glsl_version);
	}
	else
		ImGui_ImplDX11_Init((ID3D11Device*)vdo->m_device, (ID3D11DeviceContext*)vdo->m_context);
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_FrameBg] = ImVec4(0.24f, 0.25f, 0.25f, 1.00f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.85f, 0.85f, 0.85f, 0.67f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
	colors[ImGuiCol_CheckMark] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.88f, 0.88f, 0.88f, 1.00f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.40f, 0.65f, 1.00f, 1.00f);
	colors[ImGuiCol_Button] = ImVec4(0.80f, 0.80f, 0.80f, 0.40f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.73f, 0.73f, 0.73f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.76f, 0.76f, 0.76f, 0.31f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.76f, 0.77f, 0.77f, 0.80f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.87f, 0.87f, 0.87f, 0.20f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 1.00f, 1.00f, 0.67f);
	colors[ImGuiCol_Tab] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.75f, 0.75f, 0.75f, 0.80f);
	colors[ImGuiCol_TabActive] = ImVec4(0.56f, 0.57f, 0.59f, 1.00f);

	
	window.m_data->SetFocus();
	//window.m_data->ToFullscreenMode();
	
	//yyPtr<yyMDLObject> mdlObject = yyCreate<yyMDLObject>();
	//mdlObject.m_data->m_mdl = yyCreate<yyMDL>();

	auto defaultTexture = yyGetTextureResource("../res/textures/editor/white.dds", false, false, true);

	f32 deltaTime = 0.f;
	bool run = true;
	while (run)
	{		
		static u64 t1 = 0u;
		u64 t2 = yyGetTime();
		f32 m_tick = f32(t2 - t1);
		t1 = t2;
		deltaTime = m_tick / 1000.f;
		updateInputContext();
#ifdef YY_PLATFORM_WINDOWS
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			GetMessage(&msg, NULL, 0, 0);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
#else
#error For windows
#endif
		
		

		// Start the Dear ImGui frame
		if (useOpenGL)
		{
			ImGui_ImplOpenGL3_NewFrame();
		}
		else
		{
			ImGui_ImplDX11_NewFrame();
		}
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (g_inputContex->m_isRMBHold)
		{
			camera.rotate(v2f(-g_inputContex->m_mouseDelta.x, -g_inputContex->m_mouseDelta.y), deltaTime);
	//		printf("%f %f\n", io.MouseDelta.x, io.MouseDelta.y);

			if (io.KeysDownDuration[(int)yyKey::K_W] >= 0.0f)
				camera.moveForward(deltaTime, io.KeyShift);

			if (io.KeysDownDuration[(int)yyKey::K_S] >= 0.0f)
				camera.moveBackward(deltaTime, io.KeyShift);

			if (io.KeysDownDuration[(int)yyKey::K_A] >= 0.0f)
				camera.moveLeft(deltaTime, io.KeyShift);

			if (io.KeysDownDuration[(int)yyKey::K_D] >= 0.0f)
				camera.moveRight(deltaTime, io.KeyShift);

			if (io.KeysDownDuration[(int)yyKey::K_E] >= 0.0f)
				camera.moveUp(deltaTime, io.KeyShift);

			if (io.KeysDownDuration[(int)yyKey::K_Q] >= 0.0f)
				camera.moveDown(deltaTime, io.KeyShift);

			auto cursorX = std::floor((f32)window.m_data->m_currentSize.x / 2.f);
			auto cursorY = std::floor((f32)window.m_data->m_currentSize.y / 2.f);
			g_inputContex->m_cursorCoordsOld.set(cursorX, cursorY);

			yySetCursorPosition(cursorX, cursorY, window.m_data);
		}
	//	printf("%f\n", deltaTime);
		camera.update();
		g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::ViewProjection, camera.m_camera->m_viewProjectionMatrix);
		

		
		ImGui::Begin("Main menu", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoTitleBar);
		ImGuiTabBarFlags mainemnuTab_bar_flags = ImGuiTabBarFlags_None;
		if (ImGui::BeginTabBar("menutab", mainemnuTab_bar_flags))
		{
			if (ImGui::BeginTabItem("File"))
			{
				if (ImGui::Button("Save"))
				{
					auto filePath = yySaveFileDialog("Save file", "Save", "MDL");
					if (filePath)
					{
						SaveMDL((const wchar_t*)filePath->data());
						yyDestroy(filePath);
					}
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("View"))
			{
				if (ImGui::Button("Reset camera"))
				{
					camera.reset();
				}
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::End();

		ImGui::SetNextWindowSizeConstraints(ImVec2(300.f, 600.f), ImVec2(300.f, 600.f));
		ImGui::Begin("Parameters", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoTitleBar);
		if (ImGui::BeginTabBar("parameterstab"))
		{
			if (ImGui::BeginTabItem("Layers"))
			{
				ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(255, 0, 0, 100));
				ImGui::BeginChild("ChildLayerList", ImVec2(ImGui::GetWindowContentRegionWidth(), 200), false, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoMove);
				yyStringA stra;
				for (int n = 0, nsz = g_sceneObject->m_mdlObject->m_mdl->m_layers.size(); n < nsz; ++n)
				{
					auto layer = g_sceneObject->m_mdlObject->m_mdl->m_layers[n];
					stra = layer->m_model->m_name.data();

					if (ImGui::Selectable(stra.data(), g_selectedLayer == n))
					{
						g_selectedLayer = n;
				//		printf("Select %i\n", g_selectedLayer);
					}
				}
				ImGui::EndChild();
				ImGui::PopStyleColor();
				if (ImGui::Button("New"))
				{
					auto path = yyOpenFileDialog("Import model", "Import", "smd", "SMD");
					if (path)
					{
						//yyLogWriteInfo("OpenFileDialog: %s\n", path->to_stringA().data());
						newLayer(g_sceneObject->m_mdlObject, path->data());

						yyDestroy(path);
					}
				}
				
				if (g_selectedLayer != -1)
				{
					auto & currentLayerInfo = g_sceneObject->m_layerInfo[g_selectedLayer];
					
					ImGui::SameLine();
					if (ImGui::Button("Rename"))
					{
						g_renameTextBuffer = g_sceneObject->m_mdlObject->m_mdl->m_layers[g_selectedLayer]->m_model->m_name.data();
						ImGui::OpenPopup("Rename");
					}
					ImVec2 center(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f);
					ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
					if (ImGui::BeginPopupModal("Rename", NULL, ImGuiWindowFlags_AlwaysAutoResize))
					{
						ImGui::InputText("new name", g_renameTextBuffer.data(), g_renameTextBufferSize);
						ImGui::Separator();

						if (ImGui::Button("OK", ImVec2(120, 0))) 
						{
							g_sceneObject->m_mdlObject->m_mdl->m_layers[g_selectedLayer]->m_model->m_name = ModelCheckName(g_sceneObject->m_mdlObject, g_renameTextBuffer.data());
							ImGui::CloseCurrentPopup(); 
						}
						ImGui::SetItemDefaultFocus();
						ImGui::SameLine();
						if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
						ImGui::EndPopup();
					}
					ImGui::SameLine();
					ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 0, 0, 100));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 255, 0, 255));
					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 255, 255));
					if (ImGui::Button("Delete"))
					{
						deleteLayer(g_sceneObject->m_mdlObject);
					}
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					if (ImGui::CollapsingHeader("Position"))
					{
						ImGui::DragFloat("X", &currentLayerInfo.m_offset.x, 0.005f);
						ImGui::DragFloat("Y", &currentLayerInfo.m_offset.y, 0.005f);
						ImGui::DragFloat("Z", &currentLayerInfo.m_offset.z, 0.005f);
						ImGui::Separator();
					}
					if (ImGui::CollapsingHeader("Shader") && g_selectedLayer != -1)
					{
						const char* shaderCombo_items[] = { "Simple" };
						ImGui::Combo("Type", &g_shaderCombo_item_current, shaderCombo_items, IM_ARRAYSIZE(shaderCombo_items));

						if (ImGui::CollapsingHeader("Texture 0"))
						{
							ImGui::InputText("path", currentLayerInfo.m_texturePathTextBuffer.data(), currentLayerInfo.m_texturePathTextBufferSize);
							ImGui::SameLine();
							if (ImGui::Button("..."))
							{
								auto path = yyOpenFileDialog("Texture file", "Select", "png dds bmp tga", "Supported files");
								if (path)
								{
									g_sceneObject->m_mdlObject->m_mdl->m_layers[g_selectedLayer]->m_texture1Path = path->data();
									currentLayerInfo.m_texturePathTextBuffer = path->data();
									reloadTexture(g_sceneObject->m_mdlObject, 0, (wchar_t*)path->data());
									yyDestroy(path);
								}
							}
							if (ImGui::Button("Reload"))
							{
								g_sceneObject->m_mdlObject->m_mdl->m_layers[g_selectedLayer]->m_texture1Path = currentLayerInfo.m_texturePathTextBuffer.data();
								reloadTexture(g_sceneObject->m_mdlObject, 0, (wchar_t*)currentLayerInfo.m_texturePathTextBuffer.to_string().data());
							}
							if (g_sceneObject->m_mdlObject->m_mdl->m_layers[g_selectedLayer]->m_textureGPU1)
							{
								auto th = yyGetVideoDriverAPI()->GetTextureHandle(g_sceneObject->m_mdlObject->m_mdl->m_layers[g_selectedLayer]->m_textureGPU1);
								ImTextureID my_tex_id = th;
								float my_tex_w = 64.f;
								float my_tex_h = 64.f;
								ImVec2 pos = ImGui::GetCursorScreenPos();
								ImGui::Image(my_tex_id, ImVec2(my_tex_w, my_tex_h));
								if (ImGui::IsItemHovered())
								{
									ImGui::BeginTooltip();
									ImGui::Image(my_tex_id, ImVec2(256.f, 256.f));
									ImGui::EndTooltip();
								}
							}
							ImGui::Separator();
						}
						if (ImGui::CollapsingHeader("Texture 1"))
						{
							ImGui::Separator();
						}
						if (ImGui::CollapsingHeader("Texture 2"))
						{
							ImGui::Separator();
						}
						if (ImGui::CollapsingHeader("Texture 3"))
						{
							ImGui::Separator();
						}
					}
				}

				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Animations"))
			{
				ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(255, 0, 0, 100));
				ImGui::BeginChild("ChildAnimationList", ImVec2(ImGui::GetWindowContentRegionWidth(), 200), false, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoMove);
				yyStringA stra;
				for (int n = 0, nsz = g_sceneObject->m_mdlObject->m_mdl->m_animations.size(); n < nsz; ++n)
				{
					auto animation = g_sceneObject->m_mdlObject->m_mdl->m_animations[n];
					stra = animation->m_name.data();
					if (ImGui::Selectable(stra.data(), g_selectedAnimation == n))
					{
						g_selectedAnimation = n;
						auto newState = g_sceneObject->getState(stra.data());
						if (newState)
						{
							g_sceneObject->m_mdlObject->SetState(newState);
						}
					}
				}
				ImGui::EndChild();
				ImGui::PopStyleColor();
				ImGui::Checkbox("Bind pose", &g_bindPose);
				if (g_selectedAnimation != -1)
				{
					ImGui::DragFloat("FPS:", &g_sceneObject->m_mdlObject->m_mdl->m_animations[g_selectedAnimation]->m_fps, 1.f, 1.f, 60.f);
				}
				ImGui::EndTabItem();
			}
			if (ImGui::BeginTabItem("Hitboxes"))
			{
				ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(255, 0, 0, 100));
				ImGui::BeginChild("ChildHitboxList", ImVec2(ImGui::GetWindowContentRegionWidth(), 200), false, ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_NoMove);
				for (int n = 0, nsz = g_sceneObject->m_hitboxes.size(); n < nsz; ++n)
				{
					auto hitbox = g_sceneObject->m_hitboxes[n];
					if (ImGui::Selectable(hitbox->m_name.data(), g_selectedHitbox == n))
					{
						g_selectedHitbox = n;
					}
				}
				ImGui::EndChild();
				ImGui::PopStyleColor();
				if (ImGui::Button("New"))
				{
					static int hitbox_counter = 0;
					Hitbox* newHitBox = yyCreate<Hitbox>();
					newHitBox->m_name += hitbox_counter++;
					newHitBox->rebuild();
					g_sceneObject->m_hitboxes.push_back(newHitBox);
				}
				if (g_selectedHitbox != -1)
				{
					ImGui::SameLine();
					ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(255, 0, 0, 100));
					ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 255, 0, 255));
					ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 255, 255));
					if (ImGui::Button("Delete"))
					{
						g_sceneObject->m_hitboxes.erase(g_selectedHitbox);
						if (g_sceneObject->m_hitboxes.size() == g_selectedHitbox)
							--g_selectedHitbox;
					}
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					ImGui::PopStyleColor();
					
					if (g_selectedHitbox != -1)
					{
						Hitbox* activeHitbox = g_sceneObject->m_hitboxes[g_selectedHitbox];
						bool reb = false;
						if (ImGui::SliderFloat("MinX", &activeHitbox->m_min.x, -1000.0f, -0.01f, "%.3f", ImGuiSliderFlags_Logarithmic ))reb = true;
						if (ImGui::SliderFloat("MinY", &activeHitbox->m_min.y, -1000.0f, -0.01f, "%.3f", ImGuiSliderFlags_Logarithmic))reb = true;
						if (ImGui::SliderFloat("MinZ", &activeHitbox->m_min.z, -1000.0f, -0.01f, "%.3f", ImGuiSliderFlags_Logarithmic))reb = true;
						if (ImGui::SliderFloat("MaxX", &activeHitbox->m_max.x, 0.01f, 1000.f, "%.3f", ImGuiSliderFlags_Logarithmic))reb = true;
						if (ImGui::SliderFloat("MaxY", &activeHitbox->m_max.y, 0.01f, 1000.f, "%.3f", ImGuiSliderFlags_Logarithmic))reb = true;
						if (ImGui::SliderFloat("MaxZ", &activeHitbox->m_max.z, 0.01f, 1000.f, "%.3f", ImGuiSliderFlags_Logarithmic))reb = true;
					
						if(reb)
							activeHitbox->rebuild();
					}
				}
				ImGui::EndTabItem();
			}
			ImGui::EndTabBar();
		}
		ImGui::End();

		yyGUIUpdate(1.f);
		yyUpdateAsyncLoader();

		//WaitForSingleObject(GetCurrentThread(), 5);


		switch(*p_engineContext->m_state)
		{
		default:
			run = false;
			break;
		case yySystemState::Run:
		{
			g_videoDriver->BeginDraw();
			g_videoDriver->ClearAll();

			g_videoDriver->DrawLine3D(v4f(10.f, 0.f, 0.f, 0.f), v4f(-10.f, 0.f, 0.f, 0.f), ColorRed);
			g_videoDriver->DrawLine3D(v4f(0.f, 0.f, 10.f, 0.f), v4f(0.f, 0.f, -10.f, 0.f), ColorBlue);

			g_videoDriver->UseDepth(true);

			g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::Projection, camera.m_camera->m_projectionMatrix);
			g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::View, camera.m_camera->m_viewMatrix);

			
			bool drawBind = false;

			if (g_selectedAnimation == -1)
				drawBind = true;

			if (g_bindPose)
				drawBind = true;
			if (!drawBind)
			{
				g_sceneObject->update(deltaTime);
			}

			for (int n = 0, nsz = g_sceneObject->m_mdlObject->m_mdl->m_layers.size(); n < nsz; ++n)
			{
				auto layer = g_sceneObject->m_mdlObject->m_mdl->m_layers[n];
				g_videoDriver->SetModel(layer->m_meshGPU);

				Mat4 WorldMatrix;

				WorldMatrix = g_sceneObject->m_mdlObject->m_mdl->m_preRotation * WorldMatrix;
				WorldMatrix[3] = g_sceneObject->m_layerInfo[n].m_offset;
				WorldMatrix[3].w = 1.f;



				g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::World,  WorldMatrix);
				g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::WorldViewProjection, camera.m_camera->m_projectionMatrix * camera.m_camera->m_viewMatrix * WorldMatrix);
				if(layer->m_textureGPU1)
					g_videoDriver->SetTexture(yyVideoDriverAPI::TextureSlot::Texture0, layer->m_textureGPU1);
				else
					g_videoDriver->SetTexture(yyVideoDriverAPI::TextureSlot::Texture0, defaultTexture);

			

				/*Mat4 R;
				R.setRotation(Quat(math::degToRad(90.f), 0.f, 0.f));*/

				auto numJoints = g_sceneObject->m_mdlObject->m_joints.size();
				if (numJoints)
				{
					if (drawBind)
					{
						for (int i = 0; i < numJoints; ++i)
						{
							auto & objJoint = g_sceneObject->m_mdlObject->m_joints[i];
							auto mdlJoint = g_sceneObject->m_mdlObject->m_mdl->m_joints[i];
							objJoint.m_finalTransformation.identity();
							objJoint.m_globalTransformation = mdlJoint->m_matrixWorld;
						}
					}
					else
					{
					}
					
					g_videoDriver->UseDepth(false);
					for (int i = 0; i < numJoints; ++i)
					{
						auto & objJoint = g_sceneObject->m_mdlObject->m_joints[i];
						auto mdlJoint = g_sceneObject->m_mdlObject->m_mdl->m_joints[i];

						auto matrix = g_sceneObject->m_mdlObject->m_mdl->m_preRotation * objJoint.m_globalTransformation;
						auto point = matrix[3];

						matrix[3].set(0.f, 0.f, 0.f, 1.f);

						auto distCamera = point.distance(camera.m_camera->m_objectBase.m_globalPosition);
						f32 jointSize = 0.02f * distCamera;

						g_videoDriver->DrawLine3D(
							point + math::mul(v4f(-jointSize, 0.f, 0.f, 0.f), matrix),
							point + math::mul(v4f(jointSize, 0.f, 0.f, 0.f), matrix), ColorRed);
						g_videoDriver->DrawLine3D(
							point + math::mul(v4f(.0f, -jointSize, 0.f, 0.f), matrix),
							point + math::mul(v4f(.0f, jointSize, 0.f, 0.f), matrix), ColorGreen);
						g_videoDriver->DrawLine3D(
							point + math::mul(v4f(0.0f, 0.f, -jointSize, 0.f), matrix),
							point + math::mul(v4f(0.0f, 0.f, jointSize, 0.f), matrix), ColorBlue);

						if (mdlJoint->m_parentIndex != -1)
						{
							auto matrixP = g_sceneObject->m_mdlObject->m_mdl->m_preRotation * g_sceneObject->m_mdlObject->m_joints[mdlJoint->m_parentIndex].m_globalTransformation;
							g_videoDriver->DrawLine3D(
								point,
								matrixP[3],
								ColorLime);
						}

						g_videoDriver->SetBoneMatrix(i, objJoint.m_finalTransformation);
					}
				}
				g_videoDriver->Draw();
				
				yyMaterial matWireframe;
				matWireframe.m_wireframe = true;
				matWireframe.m_baseColor = ColorLime;
				g_videoDriver->SetMaterial(&matWireframe);
				for (int i = 0, sz = g_sceneObject->m_hitboxes.size(); i < sz; ++i)
				{
					auto hb = g_sceneObject->m_hitboxes[i];
					Mat4 W;
					g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::World, W);
					g_videoDriver->SetMatrix(yyVideoDriverAPI::MatrixType::WorldViewProjection, camera.m_camera->m_projectionMatrix * camera.m_camera->m_viewMatrix * W);
					g_videoDriver->SetModel(hb->m_gpuModel);
					g_videoDriver->Draw();
				}
				g_videoDriver->SetMaterial(0);
				g_videoDriver->UseDepth(true);
			}

			g_videoDriver->UseDepth(false);
			yyGUIDrawAll();
		//	g_videoDriver->DrawLine2D(v3f(50.f, 50.f, 0.f), v3f(264.f, 264.f, 0.f), ColorRed);


			g_videoDriver->UseDepth(true);

			g_videoDriver->EndDraw();
			ImGui::Render();
			if (useOpenGL)
			{
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			}
			else
			{
				ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
			}

			g_videoDriver->SwapBuffers();

		}break;
		}
	}
	if (useOpenGL)
	{
		ImGui_ImplOpenGL3_Shutdown();
	}
	else
	{
		ImGui_ImplDX11_Shutdown();
	}
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	return 0;
}

void SaveMDL(const wchar_t* fileName)
{

}