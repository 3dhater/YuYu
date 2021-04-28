#ifndef YY_D3D11_MODEL_H__
#define YY_D3D11_MODEL_H__

#include "yy_material.h"
#include "yy_model.h"
#include "containers\array.h"

class D3D11Model : public yyResourceImplementation
{
public:
	D3D11Model();
	virtual ~D3D11Model();

	virtual void Load(yyResourceData*);
	virtual void Unload();

	virtual void GetTextureSize(v2f*);
	virtual void GetTextureHandle(void**);
	virtual void MapModelForWriteVerts(u8** v_ptr);
	virtual void UnmapModelForWriteVerts();

	ID3D11Buffer*   m_lockedResource;
	ID3D11Buffer*   m_vBuffer;
	ID3D11Buffer*   m_iBuffer;
	u32 m_stride;
	u32 m_iCount;

	DXGI_FORMAT m_indexType;

	yyVertexType m_vertexType;
	//yyMaterial m_material;
};


#endif