#ifndef YY_D3D11_SHADER_POINTS_H__
#define YY_D3D11_SHADER_POINTS_H__

#include "d3d11_shader_common.h"

class D3D11ShaderPoints : public D3D11ShaderCommon
{
public:
	D3D11ShaderPoints();
	virtual ~D3D11ShaderPoints();
	
	ID3D11Buffer*		m_cbVertex;

	struct cbVertex
	{
		Mat4 W;
		Mat4 V;
		Mat4 Vi;
		Mat4 P;
	}m_cbVertexData;

	bool init();

	virtual void SetConstants(yyMaterial* material);
};

class D3D11ShaderPointsAnimated : public D3D11ShaderCommon
{
public:
	D3D11ShaderPointsAnimated();
	virtual ~D3D11ShaderPointsAnimated();
	virtual void SetConstants(yyMaterial* material);
	bool init();

	ID3D11Buffer*		m_cbVertex;
	ID3D11Buffer*		m_cbPixel;

	struct cbVertex
	{
		Mat4 W;
		Mat4 V;
		Mat4 Vi;
		Mat4 P;
		Mat4 Bones[YY_MAX_BONES];
	}m_cbVertexData;

};

#endif