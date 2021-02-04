#ifndef YY_D3D11_SHADER_GUI_H__
#define YY_D3D11_SHADER_GUI_H__

#include "d3d11_shader_common.h"

class D3D11ShaderGUI : public D3D11ShaderCommon
{
public:
	D3D11ShaderGUI();
	virtual ~D3D11ShaderGUI();

	struct cbVertex {
		Mat4 m_ProjMtx;
		v2f m_Offset;
		v2f m_Padding;
	}m_cbVertex_impl;
	ID3D11Buffer*		m_cbVertex;

	struct cbPixel{
		yyColor m_Color;
	}m_cbPixel_impl;
	ID3D11Buffer*		m_cbPixel;

	bool init();
	virtual void SetConstants(yyMaterial* material) {}
};

#endif