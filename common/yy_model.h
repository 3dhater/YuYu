#ifndef _YY_MESH_H__
#define _YY_MESH_H__

#include "containers/array.h"
#include "math/vec.h"
#include "math/ray.h"
#include "math\triangle.h"
#include "math\aabb.h"
#include "yy_material.h"

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
};
struct yyVertexAnimatedModel
{
	v3f Position;
	v2f TCoords;
	v3f Normal;
	v3f Binormal;
	v3f Tangent;

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
	AnimatedModel
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
	//	m_parent = nullptr;
		m_parentIndex = -1;
	}

	//yyJoint*				m_parent;
	s32 m_parentIndex; // index in yyMDL::m_joints
	//yyArraySmall<yyJoint*>	m_children;

	Mat4					m_matrixBind;
	//Mat4					m_matrixOffset;
	//Mat4					m_nodeTransformation;

	//v4f					m_position;
	//Quat					m_rotation;

	yyStringA				m_name;

	void toBind()
	{
	//	m_position = m_matrixBind[3];
	//	m_rotation = math::matToQuat(m_matrixBind);
	}
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
		m_vertexType(yyVertexType::GUI),
		m_refCount(0)
	{
	}
	~yyModel()
	{
		if(m_vertices) yyMemFree( m_vertices );
		if(m_indices) yyMemFree( m_indices ); 
		
	}

	Aabb m_aabb;

	u8* m_vertices;
	u8* m_indices;
	yyMeshIndexType m_indexType;

	u32 m_vCount;
	u32 m_iCount;
	u32 m_stride;

	yyVertexType m_vertexType;

	yyMaterial m_material;
	yyStringW m_name;

	

		// используется в yyGetModel и yyDeleteModel
		// если вызван yyGetModel то ++m_refCount
		// если вызван yyDeleteModel то --m_refCount
	u32 m_refCount;

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

//struct yyModel
//{
//	yyModel():
//		m_refCount(0)
//	{}
//	~yyModel()
//	{
//		for(u16 i = 0, sz = m_meshBuffers.size(); i < sz; ++i)
//		{
//			yyDestroy( m_meshBuffers[i] );
//		}
//	}
//
//	yyArraySmall<yyMeshBuffer*> m_meshBuffers;
//
//	bool isRayIntersect(const yyRay& ray, v4f& ip, f32& len, const v4f& position, Mat4* matrix = nullptr)
//	{
//		yyTriangle triangle;
//		for (u16 imb = 0, szmb = m_meshBuffers.size(); imb < szmb; ++imb)
//		{
//			auto mb = m_meshBuffers[imb];
//			yyVertexModel* verts = (yyVertexModel*)mb->m_vertices;
//			u16* inds = (u16*)mb->m_indices;
//			for (u32 i = 0; i < mb->m_iCount; i += 3)
//			{
//				triangle.v1 = verts[inds[i]].Position;
//				if (matrix) 
//					triangle.v1 = math::mul(triangle.v1, *matrix);
//				triangle.v1 += position;
//
//				triangle.v2 = verts[inds[i + 1]].Position;
//				if (matrix)
//					triangle.v2 = math::mul(triangle.v2, *matrix);
//				triangle.v2 += position;
//
//				triangle.v3 = verts[inds[i + 2]].Position;
//				if (matrix)
//					triangle.v3 = math::mul(triangle.v3, *matrix);
//				triangle.v3 += position;
//
//				triangle.update();
//
//				f32 U, V, W = 0.f;
//				if (triangle.rayTest_MT(ray, false, len, U, V, W))
//				{
//					ip = ray.m_origin + (ray.m_direction * len);
//					return true;
//				}
//			}
//		}
//
//		return false;
//	}
//
//	// используется в yyGetModel и yyDeleteModel
//	// если вызван yyGetModel то ++m_refCount
//	// если вызван yyDeleteModel то --m_refCount
//	u32 m_refCount;
//
//	Aabb m_aabb;
//};

// Идея такая - MDL это оболочка к yyModel
// MDL может хранить множество слоёв (мешбуферов)
struct yyMDLLayer
{
	yyMDLLayer() {
		m_model = 0;
		m_textureGPU1 = 0;
		m_textureGPU2 = 0;
		m_textureGPU3 = 0;
		m_textureGPU4 = 0;
		m_meshGPU = 0;
		m_gpu = yyGetVideoDriverAPI();
	}
	~yyMDLLayer() {
		if (m_model) yyDestroy(m_model);
		Unload();
	}

	yyVideoDriverAPI * m_gpu;

	yyModel * m_model;

	yyStringA m_texture1Path;
	yyStringA m_texture2Path;
	yyStringA m_texture3Path;
	yyStringA m_texture4Path;

	yyResource* m_textureGPU1;
	yyResource* m_textureGPU2;
	yyResource* m_textureGPU3;
	yyResource* m_textureGPU4;

	yyResource* m_meshGPU;

	void Load()
	{
		if (m_textureGPU1) m_gpu->LoadTexture(m_textureGPU1);
		if (m_textureGPU2) m_gpu->LoadTexture(m_textureGPU2);
		if (m_textureGPU3) m_gpu->LoadTexture(m_textureGPU3);
		if (m_textureGPU4) m_gpu->LoadTexture(m_textureGPU4);

		if (m_meshGPU) m_gpu->LoadModel(m_meshGPU);
	}
	void Unload()
	{
		if (m_textureGPU1) m_gpu->UnloadTexture(m_textureGPU1);
		if (m_textureGPU2) m_gpu->UnloadTexture(m_textureGPU2);
		if (m_textureGPU3) m_gpu->UnloadTexture(m_textureGPU3);
		if (m_textureGPU4) m_gpu->UnloadTexture(m_textureGPU4);

		if (m_meshGPU) m_gpu->UnloadModel(m_meshGPU);
	}
};

struct yyMDLAnimationKeyFrame
{
	yyMDLAnimationKeyFrame() :m_time(0) {}
	yyMDLAnimationKeyFrame(s32 time) :m_time(time) {}
	s32 m_time;
	v4f m_position;
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

	yyMDLAnimationKeyFrame* getCurrentKeyFrame(s32 time)
	{
		return nullptr;
	}
	yyMDLAnimationKeyFrame* getNextKeyFrame(s32 time)
	{
		for (u16 i = 0, sz = m_keyFrames.size(); i < sz; ++i)
		{
			auto & k = m_keyFrames[i];
			if ( k.m_time > time )
				return &m_keyFrames[i];
		}
		return nullptr;
	}
};
struct yyMDLAnimation
{
	yyMDLAnimation() :m_len(0.f) {}
	yyStringA m_name;


	// анимация это список джоинтов которые задействованны в анимации
	// вычисление анимации должно происходить раз за кадр, результат
	//  должен быть сохранён где-то, например надо создать 
	//   дополнительную структуру
	struct _joint_info
	{
		s32  m_jointID;     // индекс yyMDL::m_joints
		//Mat4 m_matrixFinal; // финальная матрица, которая пойдёт в шейдер

		//v4f						m_position;
		//Quat					m_rotation;
		
		// фреймы анимации для конкретного джоинта
		yyMDLAnimationFrames m_animationFrames;
	};
	yyArraySmall<_joint_info> m_animatedJoints;

	// длинна анимации
	f32 m_len;
};

struct yyMDL
{
	yyMDL()
	//	:
	//	m_skeleton(0)
	{
	}

	~yyMDL()
	{
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

	struct header
	{

	};

	yyArraySmall<yyMDLLayer*> m_layers;

	// так как объектов с одним и тем же yyModel может быть множество, и все они могут использовать
	// один набор костей, то нужно хранить эти кости в одном экземпляре
	// и не создавать лишние ссылки.
	// пока получается так, что для анимированных моделей обязательно 
	// нужно использовать yyMDL (другого-то способа и нет)
	// скорее всего при рисовании нужно будет передавать что-то
	// ещё, чтобы работал нужный шейдер и т.д.
	std::vector<yyJoint*> m_joints;
	//yyJoint* m_skeleton; // иерархия
	yyJoint* GetJointByName(const char* name, u32* index)
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

	// Я планирую использовать yyMDL везде и всегда
	// Так-же изначально была идея о выгрузке ресурсов как это было ранее
	// По этому нужно сделать так-же, методы Unload и Load
	// Где-то отдельной функцией при загурзке файла надо дать возможность выбора, 
	//  загружать ли ресурс сразу или отложить загрузку на потом.
	void Load()
	{
		for (u16 i = 0, sz = m_layers.size(); i < sz; ++i)
		{
			m_layers[i]->Load();
		}
	}
	void Unload()
	{
		for (u16 i = 0, sz = m_layers.size(); i < sz; ++i)
		{
			m_layers[i]->Unload();
		}
	}

	Aabb m_aabb;
};

#endif