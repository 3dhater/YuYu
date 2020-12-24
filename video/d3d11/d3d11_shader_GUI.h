#ifndef YY_D3D11_SHADER_GUI_H__
#define YY_D3D11_SHADER_GUI_H__

class D3D11ShaderGUI
{
public:
	D3D11ShaderGUI();
	~D3D11ShaderGUI();

	ID3D11VertexShader*	m_vShader;
	ID3D11PixelShader*	m_pShader;
	ID3D11InputLayout*	m_vLayout;
	ID3D11Buffer*		m_cb;

	bool init();
};

#endif