#include "yy.h"

#include "vid_d3d11.h"

#include "d3d11_texture.h"

D3D11Texture::D3D11Texture()
	:
	m_texture(0),
	m_textureResView(0),
	m_samplerState(0),
	m_RTV(0),
	m_h(0),
	m_w(0)
{

}

D3D11Texture::~D3D11Texture()
{
	if (m_RTV)            m_RTV->Release();
	if (m_samplerState)   m_samplerState->Release();
	if (m_textureResView) m_textureResView->Release();
	if (m_texture)        m_texture->Release();
}