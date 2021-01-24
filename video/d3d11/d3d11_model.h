#ifndef YY_D3D11_MODEL_H__
#define YY_D3D11_MODEL_H__

#include "yy_material.h"
#include "containers\array.h"

class D3D11Model
{
public:
	D3D11Model();
	~D3D11Model();

	ID3D11Buffer*   m_lockedResource;
	ID3D11Buffer*   m_vBuffer;
	ID3D11Buffer*   m_iBuffer;
	u32 m_stride;
	u32 m_iCount;

	DXGI_FORMAT m_indexType;

	yyMaterial m_material;
};


#endif