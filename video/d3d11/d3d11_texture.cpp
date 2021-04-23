#include "yy.h"
#include "yy_image.h"

#include "vid_d3d11.h"

#include "d3d11_texture.h"
#include "DDSTextureLoader.h"

extern D3D11 * g_d3d11;

HRESULT	D3D11Texture_createSamplerState(D3D11_FILTER filter,
	D3D11_TEXTURE_ADDRESS_MODE addressMode,
	u32 anisotropic_level,
	ID3D11SamplerState** samplerState) 
{
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));
	samplerDesc.Filter = filter;
	samplerDesc.MipLODBias = 0.0f;

	samplerDesc.AddressU = addressMode;
	samplerDesc.AddressV = addressMode;
	samplerDesc.AddressW = addressMode;

	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	samplerDesc.MaxAnisotropy = anisotropic_level;

	return g_d3d11->m_d3d11Device->CreateSamplerState(&samplerDesc, samplerState);
}

D3D11Texture::D3D11Texture()
	:
	m_texture(0),
	m_textureResView(0),
	m_samplerState(0),
	m_RTV(0),
	m_h(0),
	m_w(0)
{}

D3D11Texture::~D3D11Texture(){
	Unload();
}

void D3D11Texture::Load(yyResourceData* imageData) {
	yyImage * img = (yyImage *)imageData->m_source;
	m_h = img->m_height;
	m_w = img->m_width;

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = m_w;
	desc.Height = m_h;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;

	bool isGenMips = yyIsUseMipMaps();

	switch (img->m_format)
	{
	case yyImageFormat::BC1:
	case yyImageFormat::BC2:
	case yyImageFormat::BC3:
	{
		auto hr = DirectX::CreateDDSTextureFromMemory(
			g_d3d11->m_d3d11Device,
			g_d3d11->m_d3d11DevCon,
			(const uint8_t*)img->m_data,
			(size_t)img->m_fileSize,
			(ID3D11Resource**)&m_texture,
			&m_textureResView);

		if (FAILED(hr))
		{
			YY_PRINT_FAILED;
			return;
		}
	}break;
	case yyImageFormat::R8G8B8A8:
	{
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		desc.MiscFlags = 0;
		desc.MipLevels = 1;

		desc.ArraySize = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;

		if (isGenMips)
		{
			desc.MipLevels = 0;
			desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
			auto hr = g_d3d11->m_d3d11Device->CreateTexture2D(&desc, 0, &m_texture);
			if (FAILED(hr))
			{
				yyLogWriteError("Can't create 2D texture\n");
				YY_PRINT_FAILED;
				return;
			}
			g_d3d11->m_d3d11DevCon->UpdateSubresource(m_texture, 0, NULL, img->m_data,
				img->m_pitch, 0);
		}
		else
		{
			D3D11_SUBRESOURCE_DATA initData;
			ZeroMemory(&initData, sizeof(initData));
			initData.pSysMem = img->m_data;
			initData.SysMemPitch = img->m_pitch;
			initData.SysMemSlicePitch = img->m_dataSize;
			auto hr = g_d3d11->m_d3d11Device->CreateTexture2D(&desc, &initData, &m_texture);
			if (FAILED(hr))
			{
				yyLogWriteError("Can't create 2D texture\n");
				YY_PRINT_FAILED;
				return;
			}
		}


		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory(&SRVDesc, sizeof(SRVDesc));
		SRVDesc.Format = desc.Format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MostDetailedMip = 0;
		SRVDesc.Texture2D.MipLevels = 1;
		if (isGenMips)
			SRVDesc.Texture2D.MipLevels = -1;

		auto hr = g_d3d11->m_d3d11Device->CreateShaderResourceView(m_texture,
			&SRVDesc, &m_textureResView);
		if (FAILED(hr))
		{
			yyLogWriteError("Can't create shader resource view\n");
			YY_PRINT_FAILED;
			return;
		}
	}break;
	default:
		yyLogWriteWarning("Unsupported texture format\n");
		YY_PRINT_FAILED;
		return;
	}


	D3D11_FILTER filter;
	yyTextureFilter tf = yyGetTextureFilter();
	switch (tf)
	{
	case yyTextureFilter::PPP:
		filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT;
		break;
	case yyTextureFilter::PPL:
		filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
		break;
	case yyTextureFilter::PLP:
		filter = D3D11_FILTER::D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		break;
	case yyTextureFilter::PLL:
		filter = D3D11_FILTER::D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
		break;
	case yyTextureFilter::LPP:
		filter = D3D11_FILTER::D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		break;
	case yyTextureFilter::LPL:
		filter = D3D11_FILTER::D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		break;
	case yyTextureFilter::LLP:
		filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		break;
	case yyTextureFilter::LLL:
		filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		break;
	case yyTextureFilter::ANISOTROPIC:
		filter = D3D11_FILTER::D3D11_FILTER_ANISOTROPIC;
		break;
	case yyTextureFilter::CMP_PPP:
		filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
		break;
	case yyTextureFilter::CMP_PPL:
		filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
		break;
	case yyTextureFilter::CMP_PLP:
		filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
		break;
	case yyTextureFilter::CMP_PLL:
		filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
		break;
	case yyTextureFilter::CMP_LPP:
		filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
		break;
	case yyTextureFilter::CMP_LPL:
		filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		break;
	case yyTextureFilter::CMP_LLP:
		filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		break;
	case yyTextureFilter::CMP_LLL:
		filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		break;
	case yyTextureFilter::CMP_ANISOTROPIC:
		filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_ANISOTROPIC;
		break;
	default:
		break;
	}

	if (isGenMips)
		g_d3d11->m_d3d11DevCon->GenerateMips(m_textureResView);

	auto hr = D3D11Texture_createSamplerState(filter, D3D11_TEXTURE_ADDRESS_WRAP, 1, &m_samplerState);
	if (FAILED(hr))
	{
		yyLogWriteError("Can't create sampler state\n");
		YY_PRINT_FAILED;
		return;
	}
}

void D3D11Texture::Unload() {
	if (m_RTV) {
		m_RTV->Release();
		m_RTV = 0;
	}
	if (m_samplerState) {
		m_samplerState->Release();
		m_samplerState = 0;
	}
	if (m_textureResView) {
		m_textureResView->Release();
		m_textureResView = 0;
	}
	if (m_texture) {
		m_texture->Release();
		m_texture = 0;
	}
}