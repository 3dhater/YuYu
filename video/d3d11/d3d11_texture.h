#ifndef YY_D3D11_TEXTURE_H__
#define YY_D3D11_TEXTURE_H__

class D3D11Texture
{
public:
	D3D11Texture();
	~D3D11Texture();
	
	ID3D11Texture2D*			m_texture;
	ID3D11ShaderResourceView*	m_textureResView;
	ID3D11SamplerState*			m_samplerState;
	ID3D11RenderTargetView*     m_RTV;

	

	u32 m_h;
	u32 m_w;
};

#endif