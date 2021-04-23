#ifndef YY_D3D11_TEXTURE_H__
#define YY_D3D11_TEXTURE_H__

class D3D11Texture : public yyResourceImplementation
{
public:
	D3D11Texture();
	virtual ~D3D11Texture();

	virtual void Load(yyResourceData*);
	virtual void Unload();
	
	ID3D11Texture2D*			m_texture;
	ID3D11ShaderResourceView*	m_textureResView;
	ID3D11SamplerState*			m_samplerState;
	ID3D11RenderTargetView*     m_RTV;

	

	u32 m_h;
	u32 m_w;
};

#endif