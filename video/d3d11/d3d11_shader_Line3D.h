#ifndef YY_D3D11_SHADER_LINE3D_H__
#define YY_D3D11_SHADER_LINE3D_H__

#include "d3d11_shader_common.h"
class D3D11ShaderLine3D : public D3D11ShaderCommon
{
public:
	D3D11ShaderLine3D();
	virtual ~D3D11ShaderLine3D();

	ID3D11Buffer*		m_cb;

	struct cb
	{
		Mat4 VP;
		v4f  P1;
		v4f  P2;
		yyColor  Color;
	}m_cbData;
	virtual void SetConstants(yyMaterial* material);

	void SetData(const v4f& p1, const v4f& p2, const yyColor& color);

	bool init();
};

#endif