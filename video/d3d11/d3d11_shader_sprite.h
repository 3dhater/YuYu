#ifndef YY_D3D11_SHADER_SPRITE_H__
#define YY_D3D11_SHADER_SPRITE_H__

#include "d3d11_shader_common.h"

class D3D11ShaderSprite : public D3D11ShaderCommon
{
public:
	D3D11ShaderSprite();
	virtual ~D3D11ShaderSprite();
	
	struct cbVertex
	{
		Mat4 ProjMtx;
		Mat4 World;
		v4f CameraPositionScale;
		v2f uv1;
		v2f uv2;
		u32 flags;
		u32 padding[3];
	}m_structCB;

	ID3D11Buffer*		m_cb;

	bool init();
	void updateConstantBuffer();
};

#endif