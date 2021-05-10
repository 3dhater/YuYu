#ifndef _YY_MESH_H__
#define _YY_MESH_H__

#include "containers/array.h"
#include "math/vec.h"
#include "math/ray.h"
#include "math\triangle.h"
#include "math\aabb.h"
#include "yy_memory.h"
//#include "yy_material.h"

struct yyVertexGUI
{
	v2f m_position;
	v2f m_tcoords;
};
struct yyVertexModel
{
	v3f Position;
	v2f TCoords;
	v3f Normal;
	v3f Binormal;
	v3f Tangent;
	v4f Color;
};
struct yyVertexAnimatedModel
{
	v3f Position;
	v2f TCoords;
	v3f Normal;
	v3f Binormal;
	v3f Tangent;
	v4f Color;

	v4f Weights; // потом надо переделать на байты чтобы уменьшить вес
	v4i Bones;
};
struct yyVertexLineModel
{
	v3f Position;
	v3f Normal;
	v4f Color;
};
struct yyVertexAnimatedLineModel
{
	v3f Position;
	v3f Normal;
	v4f Color;

	v4f Weights; // потом надо переделать на байты чтобы уменьшить вес
	v4i Bones;
};

// тип
// ещё надо будет добавить для анимированных моделей
enum class yyVertexType : u32
{
	Null, // 0
	GUI,   // yyVertexGUI
	Model,  // yyVertexModel
	AnimatedModel,
	LineModel,
	AnimatedLineModel
};

// предпологается что при создании MDL индекс будет устанавливаться автоматически, в зависимости от количества треугольников 
//  пока не протестировано. условие такое if (modelInds.size() / 3 > 21845) newModel->m_indexType = yyMeshIndexType::u32; 
enum class yyMeshIndexType : u32
{
	u16,
	u32
};

struct yyJoint
{
	yyJoint()
	{
		m_parentIndex = -1;
	}

	s32 m_parentIndex; // index in yyMDL::m_joints

	Mat4					m_matrixBindInverse;
	Mat4					m_matrixOffset;
	Mat4					m_matrixWorld;

	yyStringA				m_name;

};

#define YY_MDL_VERSION 1

struct yyMDLHeader
{
	yyMDLHeader()
	{
		m_numOfLayers = 0;
		m_numOfJoints = 0;
		m_numOfAnimations = 0;
		m_numOfHitboxes = 0;
		m_stringsOffset = 0;
	}
	u32 m_numOfLayers;
	u32 m_numOfJoints;
	u32 m_numOfAnimations;
	u32 m_numOfHitboxes;

	v3f m_aabbMin;
	v3f m_aabbMax;

	u32 m_stringsOffset;

	Mat4 m_preRotation;
};
struct yyMDLLayerHeader
{
	yyMDLLayerHeader()
	{
		m_shaderType = 0;
		for (s32 i = 0; i < YY_MDL_LAYER_NUM_OF_TEXTURES; ++i)
		{
			m_textureStrID[i] = -1;
		}
		m_vertexCount = 0;
		m_vertexType = 0;
		m_vertexDataSize = 0;
		m_indexCount = 0;
		m_indexType = 0;
		m_indexDataSize = 0;
	}
	// 0 - simple
	u32 m_shaderType;
	s32 m_textureStrID[YY_MDL_LAYER_NUM_OF_TEXTURES];
	u32 m_vertexCount;
	// 0 - yyVertexModel
	// 1 - yyVertexAnimatedModel
	u32 m_vertexType;
	u32 m_vertexDataSize;
	u32 m_indexCount;
	// 0 - 16bit
	// 1 - 32bit
	u32 m_indexType;
	u32 m_indexDataSize;
};
struct yyMDLJointHeader
{
	yyMDLJointHeader()
	{
		m_nameStrID = -1;
		m_parentID = -1;
	}
	s32 m_nameStrID;
	s32 m_parentID;
	Mat4					m_matrixBindInverse;
	Mat4					m_matrixOffset;
	Mat4					m_matrixWorld;
};
struct yyMDLAnimationHeader
{
	yyMDLAnimationHeader()
	{
		m_nameStrID = -1;
		m_length = 0.f;
		m_fps = 0.f;
		m_numOfAnimatedJoints = 0;
		m_flags = 0;
	}
	s32 m_nameStrID;
	f32 m_length;
	f32 m_fps;
	u32 m_numOfAnimatedJoints;
	u32 m_flags;
};
struct yyMDLAnimatedJointHeader
{
	yyMDLAnimatedJointHeader()
	{
		m_jointID = -1;
		m_jointID = m_numOfKeyFrames;
	}
	s32 m_jointID;
	u32 m_numOfKeyFrames;
};
struct yyMDLJointKeyframeHeader
{
	yyMDLJointKeyframeHeader()
	{
		m_time = 0;
		m_scale.set(1.f);
	}
	s32 m_time;
	v3f m_position;
	v3f m_scale;
	Quat m_rotation;
};

struct yyMDLHitboxHeader
{
	yyMDLHitboxHeader()
	{
		m_type = 0;
		m_jointID = -1;
		m_vertexCount = 0;
		m_indexCount = 0;
	}
	u32 m_type;
	s32 m_jointID;
	u32 m_vertexCount;
	u32 m_indexCount;
};

// описание одного буфера для рисования
struct yyModel
{
	yyModel()
	:
		m_vertices(nullptr),
		m_indices(nullptr),
		m_indexType(yyMeshIndexType::u16),
		m_vCount(0),
		m_iCount(0),
		m_stride(0),
		m_vertexType(yyVertexType::GUI)
	{
		//m_material = yyMegaAllocator::CreateMaterial();
	}
	~yyModel()
	{
		if(m_vertices) yyMemFree( m_vertices );
		if(m_indices) yyMemFree( m_indices ); 
		//if (m_material) yyMegaAllocator::Destroy(m_material);
	}

	Aabb m_aabb;

	u8* m_vertices;
	u8* m_indices;
	yyMeshIndexType m_indexType;

	u32 m_vCount;
	u32 m_iCount;
	u32 m_stride;

	yyVertexType m_vertexType;

	//yyMaterial* m_material;
	yyStringA m_name;

	

	void calculateTangents( v3f& normal, v3f& tangent, v3f& binormal,
		const v3f& vt1, const v3f& vt2, const v3f& vt3, // vertices
		const v2f& tc1, const v2f& tc2, const v2f& tc3) // texture coords
	{
		v3f e1 = vt2 - vt1;
		v3f e2 = vt3 - vt1;
		normal = e1.cross(e2);
		normal.normalize2();
		f32 deltaX1 = tc1.x - tc2.x;
		f32 deltaX2 = tc3.x - tc1.x;
		binormal = (e1 * deltaX2) - (e2 * deltaX1);
		binormal.normalize2();
		f32 deltaY1 = tc1.y - tc2.y;
		f32 deltaY2 = tc3.y - tc1.y;
		tangent = (e1 * deltaY2) - (e2 * deltaY1);
		tangent.normalize2();
		v3f txb = tangent.cross(binormal);
		if(txb.dot(normal) < 0.0f)
		{
			tangent *= -1.0f;
			binormal *= -1.0f;
		}
	}


	void GenerateTangents_u16()
	{
		u16* idx = (u16*)m_indices;
		yyVertexModel* v = (yyVertexModel*)m_vertices;

		v3f localNormal;
		for (u32 i = 0; i < m_iCount; i += 3)
		{
			calculateTangents(
				localNormal,
				v[idx[i + 0]].Tangent,
				v[idx[i + 0]].Binormal,
				v[idx[i + 0]].Position,
				v[idx[i + 1]].Position,
				v[idx[i + 2]].Position,
				v[idx[i + 0]].TCoords,
				v[idx[i + 1]].TCoords,
				v[idx[i + 2]].TCoords);
			//if (recalculateNormals)
			//v[idx[i+0]].Normal=localNormal;

			calculateTangents(
				localNormal,
				v[idx[i + 1]].Tangent,
				v[idx[i + 1]].Binormal,
				v[idx[i + 1]].Position,
				v[idx[i + 2]].Position,
				v[idx[i + 0]].Position,
				v[idx[i + 1]].TCoords,
				v[idx[i + 2]].TCoords,
				v[idx[i + 0]].TCoords);
			//if (recalculateNormals)
			//v[idx[i+1]].Normal=localNormal;

			calculateTangents(
				localNormal,
				v[idx[i + 2]].Tangent,
				v[idx[i + 2]].Binormal,
				v[idx[i + 2]].Position,
				v[idx[i + 0]].Position,
				v[idx[i + 1]].Position,
				v[idx[i + 2]].TCoords,
				v[idx[i + 0]].TCoords,
				v[idx[i + 1]].TCoords);
			//if (recalculateNormals)
			//v[idx[i+2]].Normal=localNormal;
		}
	}
	void GenerateTangents_u32()
	{
		u32* idx = (u32*)m_indices;
		yyVertexModel* v = (yyVertexModel*)m_vertices;

		v3f localNormal;
		for (u32 i = 0; i < m_iCount; i += 3)
		{
			calculateTangents(
				localNormal,
				v[idx[i + 0]].Tangent,
				v[idx[i + 0]].Binormal,
				v[idx[i + 0]].Position,
				v[idx[i + 1]].Position,
				v[idx[i + 2]].Position,
				v[idx[i + 0]].TCoords,
				v[idx[i + 1]].TCoords,
				v[idx[i + 2]].TCoords);
			//if (recalculateNormals)
			//v[idx[i+0]].Normal=localNormal;

			calculateTangents(
				localNormal,
				v[idx[i + 1]].Tangent,
				v[idx[i + 1]].Binormal,
				v[idx[i + 1]].Position,
				v[idx[i + 2]].Position,
				v[idx[i + 0]].Position,
				v[idx[i + 1]].TCoords,
				v[idx[i + 2]].TCoords,
				v[idx[i + 0]].TCoords);
			//if (recalculateNormals)
			//v[idx[i+1]].Normal=localNormal;

			calculateTangents(
				localNormal,
				v[idx[i + 2]].Tangent,
				v[idx[i + 2]].Binormal,
				v[idx[i + 2]].Position,
				v[idx[i + 0]].Position,
				v[idx[i + 1]].Position,
				v[idx[i + 2]].TCoords,
				v[idx[i + 0]].TCoords,
				v[idx[i + 1]].TCoords);
			//if (recalculateNormals)
			//v[idx[i+2]].Normal=localNormal;
		}
	}
	void GenerateTangents()
	{
		if(m_vertexType != yyVertexType::Model)
			return;

		switch (m_indexType)
		{
		case yyMeshIndexType::u16:
		default:
			GenerateTangents_u16();
			break;
		case yyMeshIndexType::u32:
			GenerateTangents_u32();
			break;
		}

		
	}
};

// Идея такая - MDL это оболочка к yyModel
// MDL может хранить множество слоёв (мешбуферов)
struct yyMDLLayer
{
	yyMDLLayer() {
		m_model = 0;
		for (u32 i = 0; i < YY_MDL_LAYER_NUM_OF_TEXTURES; ++i)
		{
			m_textureGPU[i] = 0;
		}
		m_meshGPU = 0;
		m_gpu = yyGetVideoDriverAPI();
	}
	~yyMDLLayer() {
		if (m_model) yyMegaAllocator::Destroy(m_model);
		Unload();

		for (u32 i = 0; i < YY_MDL_LAYER_NUM_OF_TEXTURES; ++i)
		{
			if(m_textureGPU[i])
				yyDeleteTexture(m_textureGPU[i], true);
		}
		if (m_meshGPU)
			yyMegaAllocator::Destroy(m_meshGPU);
	}
	
	yyMDLLayerHeader m_layerHeader;

	yyVideoDriverAPI * m_gpu;

	yyModel * m_model;
	yyStringA m_texturePath[YY_MDL_LAYER_NUM_OF_TEXTURES];

	yyResource* m_textureGPU[YY_MDL_LAYER_NUM_OF_TEXTURES];
	yyResource* m_meshGPU;

	void Load(bool async)
	{
		for (u32 i = 0; i < YY_MDL_LAYER_NUM_OF_TEXTURES; ++i)
		{
			//if (m_textureGPU[i])m_gpu->LoadTexture(m_textureGPU[i]);
			if (m_textureGPU[i])
				m_textureGPU[i]->Load(async);
		}

		//if (m_meshGPU) m_gpu->LoadModel(m_meshGPU);
		if (m_meshGPU)
			m_meshGPU->Load();
	}
	void Unload()
	{
		for (u32 i = 0; i < YY_MDL_LAYER_NUM_OF_TEXTURES; ++i)
		{
			if (m_textureGPU[i])
			{
				if(m_textureGPU[i]->IsLoaded())
					m_textureGPU[i]->Unload();
			}

			/*if (m_textureGPU[i]->IsLoaded() == false)
			{
				yyRemoveTextureFromCache(m_textureGPU[i]);
			}*/
		}

		if (m_meshGPU)
		{
			if(m_meshGPU->IsLoaded())
				m_meshGPU->Unload();
		}
	}
};

struct yyMDLAnimationKeyFrame
{
	yyMDLAnimationKeyFrame() :m_time(0) {}
	yyMDLAnimationKeyFrame(s32 time) :m_time(time) {}
	s32 m_time;
	v3f m_position;
	v3f m_scale;
	Quat m_rotation;
};
struct yyMDLAnimationFrames
{
	yyArraySmall<yyMDLAnimationKeyFrame> m_keyFrames;

	yyMDLAnimationKeyFrame* getKeyFrame(s32 time)
	{
		for (u16 i = 0, sz = m_keyFrames.size(); i < sz; ++i)
		{
			if (m_keyFrames[i].m_time == time)
				return &m_keyFrames[i];
		}
		return nullptr;
	}
	yyMDLAnimationKeyFrame* insertKeyFrame(s32 time)
	{
		for (u16 i = 0, sz = m_keyFrames.size(); i < sz; ++i)
		{
			if (time < m_keyFrames[i].m_time)
			{
				m_keyFrames.insert(i, yyMDLAnimationKeyFrame());
				return &m_keyFrames[i];
			}
		}

		m_keyFrames.push_back(yyMDLAnimationKeyFrame(time));
		return &m_keyFrames[m_keyFrames.size()-1];
	}

	void insertPosition(const v3f& position, s32 time)
	{
		auto keyFrame = getKeyFrame(time);
		if (!keyFrame)
			keyFrame = insertKeyFrame(time);
		keyFrame->m_position = position;
	}
	void insertRotation(const Quat& rotation, s32 time)
	{
		auto keyFrame = getKeyFrame(time);
		if (!keyFrame)
			keyFrame = insertKeyFrame(time);
		keyFrame->m_rotation = rotation;
	}
	void insertScale(const v3f& scale, s32 time)
	{
		auto keyFrame = getKeyFrame(time);
		if (!keyFrame)
			keyFrame = insertKeyFrame(time);
		keyFrame->m_scale = scale;
	}

	yyMDLAnimationKeyFrame* getCurrentKeyFrame(s32 time)
	{
		for (s32 i = m_keyFrames.size() - 1; i >= 0; --i)
		{
			auto & k = m_keyFrames[i];
			if (k.m_time < time)
				return &m_keyFrames[i];
		}
		return &m_keyFrames[0];
	}
	yyMDLAnimationKeyFrame* getNextKeyFrame(s32 time)
	{
		for (u16 i = 0, sz = m_keyFrames.size(); i < sz; ++i)
		{
			auto & k = m_keyFrames[i];
			if ( k.m_time > time )
				return &m_keyFrames[i];
		}
		return &m_keyFrames[0];
	}
};
struct yyMDLAnimation
{
	yyMDLAnimation() :m_len(0.f), m_fps(30.f), m_flags(0){}
	~yyMDLAnimation() {
		for (u16 i = 0, sz = m_animatedJoints.size(); i < sz; ++i)
		{
			yyDestroy(m_animatedJoints[i]);
		}
	}
	yyStringA m_name;


	// анимация это список джоинтов которые задействованны в анимации
	// вычисление анимации должно происходить раз за кадр, результат
	//  должен быть сохранён где-то, например надо создать 
	//   дополнительную структуру
	struct _joint_info{
		s32  m_jointID;     // индекс yyMDL::m_joints
		
		// фреймы анимации для конкретного джоинта
		yyMDLAnimationFrames m_animationFrames;
	};
	yyArraySmall<_joint_info*> m_animatedJoints;

	// длинна анимации
	f32 m_len;
	f32 m_fps;
	u32 m_flags;
	enum {
		flag_disableSmoothLoop = BIT(0)
	};


	void AddKeyFrame(s32 jointID, s32 time, const v3f& position, const Quat& rotation, const v3f& scale){
		_joint_info* ji = _get_joint_info(jointID);
		if (!ji)
		{
			yyLogWriteWarning("yyMDLAnimation::AddKeyFrame - joint not found\n");
			return;
		}
		ji->m_animationFrames.insertPosition(position, time);
		ji->m_animationFrames.insertRotation(rotation, time);
		ji->m_animationFrames.insertScale(scale, time);
	}

	_joint_info* _get_joint_info(s32 jointID)
	{
		for (u16 i = 0, sz = m_animatedJoints.size(); i < sz; ++i)
		{
			if (m_animatedJoints[i]->m_jointID == jointID)
				return m_animatedJoints[i];
		}
		_joint_info * new_ji = yyCreate<_joint_info>();
		new_ji->m_jointID = jointID;
		m_animatedJoints.push_back(new_ji);
		return new_ji;
	}
};


struct yyMDLHitbox
{
	yyMDLHitbox() :m_mesh(0), m_jointID(-1), m_uniqueID(-1), m_userData(0) {}
	~yyMDLHitbox() { if (m_mesh) yyMegaAllocator::Destroy(m_mesh); }
	enum HitboxType
	{
		Mesh,
		End = 0xffffffff
	}m_hitboxType;

	yyModel* m_mesh; // if HitboxType::Mesh
	s32 m_jointID;

	s32 m_uniqueID; 
	void* m_userData;
};

struct yyMDL
{
	yyMDL(){
		m_refCount = 0;
	}

	~yyMDL(){
		for (u16 i = 0, sz = m_hitboxes.size(); i < sz; ++i)
		{
			yyDestroy(m_hitboxes[i]);
		}
		for (u16 i = 0, sz = m_layers.size(); i < sz; ++i)
		{
			yyDestroy(m_layers[i]);
		}

		for (u16 i = 0, sz = m_joints.size(); i < sz; ++i)
		{
			yyDestroy(m_joints[i]);
		}
		for (u16 i = 0, sz = m_animations.size(); i < sz; ++i)
		{
			yyDestroy(m_animations[i]);
		}
	}

	

	yyArraySmall<yyMDLLayer*> m_layers;
	yyArraySmall<yyMDLHitbox*> m_hitboxes;

	// так как объектов с одним и тем же yyModel может быть множество, и все они могут использовать
	// один набор костей, то нужно хранить эти кости в одном экземпляре
	// и не создавать лишние ссылки.
	// пока получается так, что для анимированных моделей обязательно 
	// нужно использовать yyMDL (другого-то способа и нет)
	// скорее всего при рисовании нужно будет передавать что-то
	// ещё, чтобы работал нужный шейдер и т.д.
	yyArraySmall<yyJoint*> m_joints;
	//yyJoint* m_skeleton; // иерархия
	yyJoint* GetJointByName(const char* name, s32* index)
	{
		for (u16 i = 0, sz = m_joints.size(); i < sz; ++i)
		{
			if(index)
				*index = i;
			if (strcmp(name, m_joints[i]->m_name.data()) == 0)
				return m_joints[i];
		}
		return 0;
	}
	
	yyArraySmall<yyMDLAnimation*> m_animations;
	yyMDLAnimation* GetAnimationByName(const char* name)
	{
		for (u16 i = 0, sz = m_animations.size(); i < sz; ++i)
		{
			if (strcmp(name, m_animations[i]->m_name.data()) == 0)
				return m_animations[i];
		}
		return 0;
	}

	// Я планирую использовать yyMDL везде и всегда
	// Так-же изначально была идея о выгрузке ресурсов как это было ранее
	// По этому нужно сделать так-же, методы Unload и Load
	// Где-то отдельной функцией при загурзке файла надо дать возможность выбора, 
	//  загружать ли ресурс сразу или отложить загрузку на потом.
	void Load(bool async = false)
	{
		++m_refCount;

		if (m_refCount == 1)
		{
			for (u16 i = 0, sz = m_layers.size(); i < sz; ++i)
			{
				m_layers[i]->Load(async);
			}
		}
	}
	void Unload()
	{
		--m_refCount;

		if (m_refCount == 0)
		{
			for (u16 i = 0, sz = m_layers.size(); i < sz; ++i)
			{
				m_layers[i]->Unload();
			}
		}
	}

	Mat4	m_preRotation;
	//Mat4	m_rootTransformInvert;

	Aabb m_aabb;


	// используется в yyGetMDLFromCache и yyRemoveMDLFromCache
	// если вызван yyGetMDLFromCache то ++m_refCount
	// если вызван yyRemoveMDLFromCache то --m_refCount
	u32 m_refCount;
};

#endif