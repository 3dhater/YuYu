#ifndef YY_D3D11_SHADER_SIMPLE_H__
#define YY_D3D11_SHADER_SIMPLE_H__

#include "d3d11_shader_common.h"

class D3D11ShaderSimple : public D3D11ShaderCommon
{
public:
	D3D11ShaderSimple();
	virtual ~D3D11ShaderSimple();
	
	ID3D11Buffer*		m_cb;

	struct cb
	{
		Mat4 WVP;
	}m_cbData;

	bool init();

	virtual void SetConstants(yyMaterial* material);
};

#endif