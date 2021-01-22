#ifndef YY_D3D11_SHADER_SCREENQUAD_H__
#define YY_D3D11_SHADER_SCREENQUAD_H__

#include "d3d11_shader_common.h"

class D3D11ShaderScreenQuad : public D3D11ShaderCommon
{
public:
	D3D11ShaderScreenQuad();
	virtual ~D3D11ShaderScreenQuad();


	bool init();
};

#endif