﻿#ifndef _YY_MESH_H__
#define _YY_MESH_H__

#include "containers/array.h"
#include "math/vec.h"
#include "math/ray.h"
#include "math\triangle.h"
#include "math\aabb.h"

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

enum class yyVertexType : u32
{
	GUI,
	Model
};

enum class yyMeshIndexType : u32
{
	u16,
	u32
};

struct yyMeshBuffer
{
	yyMeshBuffer()
	:
		m_vertices(nullptr),
		m_indices(nullptr),
		m_indexType(yyMeshIndexType::u16),
		m_vCount(0),
		m_iCount(0),
		m_stride(0),
		m_vertexType(yyVertexType::GUI)
	{
	
	}
	~yyMeshBuffer()
	{
		if(m_vertices) yyMemFree( m_vertices );
		if(m_indices) yyMemFree( m_indices ); 
	}

	Aabb m_aabb;

	u8* m_vertices;
	
	//u32* m_indices;
	u8* m_indices;

	yyMeshIndexType m_indexType;

	u32 m_vCount;
	u32 m_iCount;
	u32 m_stride;

	yyVertexType m_vertexType;

	void calculateTangents( v3f& normal, v3f& tangent, v3f& binormal,
		const v3f& vt1, const v3f& vt2, const v3f& vt3, // vertices
		const v2f& tc1, const v2f& tc2, const v2f& tc3) // texture coords
	{
		v3f v1 = vt1 - vt2;
		v3f v2 = vt3 - vt1;
		normal = v2.cross(v1);
		normal.normalize2();
		f32 deltaX1 = tc1.x - tc2.x;
		f32 deltaX2 = tc3.x - tc1.x;
		binormal = (v1 * deltaX2) - (v2 * deltaX1);
		binormal.normalize2();
		f32 deltaY1 = tc1.y - tc2.y;
		f32 deltaY2 = tc3.y - tc1.y;
		tangent = (v1 * deltaY2) - (v2 * deltaY1);
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

struct yyModel
{
	yyModel():
		m_refCount(0)
	{}
	~yyModel()
	{
		for(u16 i = 0, sz = m_meshBuffers.size(); i < sz; ++i)
		{
			yyDestroy( m_meshBuffers[i] );
		}
	}

	yyArraySmall<yyMeshBuffer*> m_meshBuffers;

	bool isRayIntersect(const yyRay& ray, v4f& ip, f32& len, const v4f& position, Mat4* matrix = nullptr)
	{
		yyTriangle triangle;
		for (u16 imb = 0, szmb = m_meshBuffers.size(); imb < szmb; ++imb)
		{
			auto mb = m_meshBuffers[imb];
			yyVertexModel* verts = (yyVertexModel*)mb->m_vertices;
			u16* inds = (u16*)mb->m_indices;
			for (u32 i = 0; i < mb->m_iCount; i += 3)
			{
				triangle.v1 = verts[inds[i]].Position;
				if (matrix) 
					triangle.v1 = math::mul(triangle.v1, *matrix);
				triangle.v1 += position;

				triangle.v2 = verts[inds[i + 1]].Position;
				if (matrix)
					triangle.v2 = math::mul(triangle.v2, *matrix);
				triangle.v2 += position;

				triangle.v3 = verts[inds[i + 2]].Position;
				if (matrix)
					triangle.v3 = math::mul(triangle.v3, *matrix);
				triangle.v3 += position;

				triangle.update();

				f32 U, V, W = 0.f;
				if (triangle.rayTest_MT(ray, false, len, U, V, W))
				{
					ip = ray.m_origin + (ray.m_direction * len);
					return true;
				}
			}
		}

		return false;
	}

	// используется в yyGetModel и yyDeleteModel
	// если вызван yyGetModel то ++m_refCount
	// если вызван yyDeleteModel то --m_refCount
	u32 m_refCount;

	Aabb m_aabb;
};

#endif