#ifndef YY_D3D11_MODEL_H__
#define YY_D3D11_MODEL_H__

#include "containers\array.h"

class D3D11MeshBuffer
{
public:
	D3D11MeshBuffer();
	~D3D11MeshBuffer();

	ID3D11Buffer*   m_lockedResource;
	ID3D11Buffer*   m_vBuffer;
	ID3D11Buffer*   m_iBuffer;
	u32 m_stride;
	u32 m_iCount;

	DXGI_FORMAT m_indexType;
};

class D3D11Model
{
public:
	D3D11Model();
	~D3D11Model();
	yyArraySmall<D3D11MeshBuffer*> m_meshBuffers;
};

#endif