#ifndef YY_D3D11_SHADER_SPRITE2_H__
#define YY_D3D11_SHADER_SPRITE2_H__

#include "d3d11_shader_common.h"

class D3D11ShaderSprite2 : public D3D11ShaderCommon
{
public:
	D3D11ShaderSprite2();
	virtual ~D3D11ShaderSprite2();
	
	struct cbVertex
	{
		Mat4 ProjMtx;
		Mat4 World;
		v4f CameraPositionScale;
	}m_structCB;

	ID3D11Buffer*		m_cb;

	bool init();
	virtual void SetConstants(yyMaterial* material);
};

#endif