#ifndef YY_D3D11_SHADER_RECT_H__
#define YY_D3D11_SHADER_RECT_H__

#include "d3d11_shader_common.h"

class D3D11ShaderRectangle : public D3D11ShaderCommon
{
public:
	D3D11ShaderRectangle();
	virtual ~D3D11ShaderRectangle();

	struct cbVertex {
		Mat4 m_ProjMtx;
		v4f m_Corners;
		yyColor m_Color1;
		yyColor m_Color2;
	}m_cbVertex_impl;
	ID3D11Buffer*		m_cbVertex;

	bool init();
	virtual void SetConstants(yyMaterial* material);
};

#endif