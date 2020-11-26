#ifndef _YY_MESH_H__
#define _YY_MESH_H__

#include "containers/array.h"

struct yyVertexGUI
{
	v3f m_position;
	v2f m_tcoords;
};

enum class yyVertexType : u32
{
	GUI
};

struct yyMeshBuffer
{
	yyMeshBuffer(){}
	~yyMeshBuffer()
	{
		if(m_vertices) delete[] m_vertices;
		if(m_indices) delete[] m_indices;
	}

	u8* m_vertices = nullptr;
	u16* m_indices = nullptr;
	u32 m_vCount = 0;
	u32 m_iCount = 0;
	u32 m_stride = 0;

	yyVertexType m_vertexType = yyVertexType::GUI;
};

struct yyModel
{
	yyModel(){}
	~yyModel()
	{
		for(u16 i = 0, sz = m_meshBuffers.size(); i < sz; ++i)
		{
			delete m_meshBuffers[i];
		}
	}

	yyArraySmall<yyMeshBuffer*> m_meshBuffers;
};

#endif