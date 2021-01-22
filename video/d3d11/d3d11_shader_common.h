#ifndef YY_D3D11_SHADER_COMMON_H__
#define YY_D3D11_SHADER_COMMON_H__

class D3D11ShaderCommon
{
public:
	D3D11ShaderCommon() :
		m_vShader(0), 
		m_pShader(0), 
		m_vLayout(0)
	{}

	virtual ~D3D11ShaderCommon() 
	{
		if (m_vLayout) m_vLayout->Release();
		if (m_vShader) m_vShader->Release();
		if (m_pShader) m_pShader->Release();
	}

	ID3D11VertexShader*	m_vShader;
	ID3D11PixelShader*	m_pShader;
	ID3D11InputLayout*	m_vLayout;
};

#endif