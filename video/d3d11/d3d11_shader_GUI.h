#ifndef YY_D3D11_SHADER_GUI_H__
#define YY_D3D11_SHADER_GUI_H__

#include "d3d11_shader_common.h"

class D3D11ShaderGUI : public D3D11ShaderCommon
{
public:
	D3D11ShaderGUI();
	virtual ~D3D11ShaderGUI();

	ID3D11Buffer*		m_cb;

	bool init();
};

#endif