#ifndef YY_D3D11_SHADER_SIMPLE_H__
#define YY_D3D11_SHADER_SIMPLE_H__

#include "d3d11_shader_common.h"

class D3D11ShaderSimple : public D3D11ShaderCommon
{
public:
	D3D11ShaderSimple();
	virtual ~D3D11ShaderSimple();
	
	ID3D11Buffer*		m_cbVertex;
	ID3D11Buffer*		m_cbPixel;

	struct cbVertex
	{
		Mat4 WVP;
	}m_cbVertexData;

	struct cbPixel
	{
		yyColor BaseColor;
	}m_cbPixelData;

	bool init();

	virtual void SetConstants(yyMaterial* material);
};

class D3D11ShaderSimpleAnimated : public D3D11ShaderCommon
{
public:
	D3D11ShaderSimpleAnimated();
	virtual ~D3D11ShaderSimpleAnimated();
	virtual void SetConstants(yyMaterial* material);
	bool init();

	ID3D11Buffer*		m_cbVertex;
	ID3D11Buffer*		m_cbPixel;

	struct cbVertex
	{
		Mat4 WVP;
		Mat4 Bones[255];
	}m_cbVertexData;

	struct cbPixel
	{
		yyColor BaseColor;
	}m_cbPixelData;

};

#endif