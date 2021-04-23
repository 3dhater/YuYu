#ifndef YY_D3D11_TEXTURE_H__
#define YY_D3D11_TEXTURE_H__

class D3D11Texture : public yyResourceImplementation
{
public:
	D3D11Texture();
	virtual ~D3D11Texture();

	virtual void Load(yyResourceData*);
	virtual void Unload();

	virtual void GetTextureSize(v2f*);
	virtual void GetTextureHandle(void**);
	virtual void MapModelForWriteVerts(u8** v_ptr);
	virtual void UnmapModelForWriteVerts();
	
	ID3D11Texture2D*			m_texture;
	ID3D11ShaderResourceView*	m_textureResView;
	ID3D11SamplerState*			m_samplerState;
	ID3D11RenderTargetView*     m_RTV;

	

	f32 m_h;
	f32 m_w;
};

#endif