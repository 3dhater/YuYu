#ifndef _YY_MESH_H__
#define _YY_MESH_H__

#include "containers/array.h"
#include "math/vec.h"

struct yyVertexGUI
{
	v3f m_position;
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

struct yyMeshBuffer
{
	yyMeshBuffer(){}
	~yyMeshBuffer()
	{
		if(m_vertices) yyMemFree( m_vertices );
		if(m_indices) yyMemFree( m_indices ); 
	}

	u8* m_vertices = nullptr;
	u16* m_indices = nullptr;
	u32 m_vCount = 0;
	u32 m_iCount = 0;
	u32 m_stride = 0;

	yyVertexType m_vertexType = yyVertexType::GUI;

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
	void GenerateTangents()
	{
		if(m_vertexType != yyVertexType::Model)
			return;

		u16* idx = m_indices;
		yyVertexModel* v = (yyVertexModel*)m_vertices;

		v3f localNormal; 
		for(u32 i = 0; i < m_iCount; i += 3)
		{
			calculateTangents(
				localNormal,
				v[idx[i+0]].Tangent,
				v[idx[i+0]].Binormal,
				v[idx[i+0]].Position,
				v[idx[i+1]].Position,
				v[idx[i+2]].Position,
				v[idx[i+0]].TCoords,
				v[idx[i+1]].TCoords,
				v[idx[i+2]].TCoords);
			//if (recalculateNormals)
				//v[idx[i+0]].Normal=localNormal;

			calculateTangents(
				localNormal,
				v[idx[i+1]].Tangent,
				v[idx[i+1]].Binormal,
				v[idx[i+1]].Position,
				v[idx[i+2]].Position,
				v[idx[i+0]].Position,
				v[idx[i+1]].TCoords,
				v[idx[i+2]].TCoords,
				v[idx[i+0]].TCoords);
			//if (recalculateNormals)
				//v[idx[i+1]].Normal=localNormal;

			calculateTangents(
				localNormal,
				v[idx[i+2]].Tangent,
				v[idx[i+2]].Binormal,
				v[idx[i+2]].Position,
				v[idx[i+0]].Position,
				v[idx[i+1]].Position,
				v[idx[i+2]].TCoords,
				v[idx[i+0]].TCoords,
				v[idx[i+1]].TCoords);
			//if (recalculateNormals)
				//v[idx[i+2]].Normal=localNormal;
		}
	}
};

struct yyModel
{
	yyModel(){}
	~yyModel()
	{
		for(u16 i = 0, sz = m_meshBuffers.size(); i < sz; ++i)
		{
			yyDestroy( m_meshBuffers[i] );
		}
	}

	yyArraySmall<yyMeshBuffer*> m_meshBuffers;
};

#endif