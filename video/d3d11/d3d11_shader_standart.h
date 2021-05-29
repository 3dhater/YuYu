#ifndef YY_D3D11_SHADER_STD_H__
#define YY_D3D11_SHADER_STD_H__
#include "d3d11_shader_common.h"

class D3D11ShaderStd : public D3D11ShaderCommon
{
public:
	D3D11ShaderStd();
	virtual ~D3D11ShaderStd();

	ID3D11Buffer*		m_cbVertex;
	ID3D11Buffer*		m_cbPixel;

	struct cbVertex
	{
		Mat4 WVP;
		Mat4 W;
	}m_cbVertexData;

	struct cbPixel
	{
		yyColor BaseColor;
		v4f SunPosition;
	}m_cbPixelData;

	bool init();

	virtual void SetConstants(yyMaterial* material);
};

#endif