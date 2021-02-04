﻿#include "yy.h"
#include "yy_window.h"
#include "yy_model.h"
#include "yy_image.h"

#include "vid_d3d11.h"
#include "d3d11_model.h"
#include "d3d11_texture.h"
#include "d3d11_shader_GUI.h"
#include "d3d11_shader_sprite.h"
#include "d3d11_shader_ScreenQuad.h"
#include "d3d11_shader_simple.h"
#include "d3d11_shader_Line3D.h"

#include "DDSTextureLoader.h"

void D3D11::UpdateGUIProjectionMatrix(const v2i& windowSize)
{
//	gglViewport(0, 0, (GLsizei)windowSize.x, (GLsizei)windowSize.y);
	float L = 0;
	float R = (float)windowSize.x;
	float T = 0;
	float B = (float)windowSize.y;

	m_guiProjectionMatrix.m_data[0] = v4f(2.0f / (R - L), 0.0f, 0.0f, 0.0f);
	m_guiProjectionMatrix.m_data[1] = v4f(0.0f, 2.0f / (T - B), 0.0f, 0.0f);
	m_guiProjectionMatrix.m_data[2] = v4f(0.0f, 0.0f, 0.5f, 0.0f);
	m_guiProjectionMatrix.m_data[3] = v4f((R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f);

	//opengl
	/*m_guiProjectionMatrix.m_data[0] = v4f(2.0f / (R - L), 0.0f, 0.0f, 0.0f);
	m_guiProjectionMatrix.m_data[1] = v4f(0.0f, 2.0f / (T - B), 0.0f, 0.0f);
	m_guiProjectionMatrix.m_data[2] = v4f(0.0f, 0.0f, -1.0f, 0.0f);
	m_guiProjectionMatrix.m_data[3] = v4f((R + L) / (L - R), (T + B) / (B - T), 0.0f, 1.0f);*/
}

D3D11::D3D11()
{
	m_mainTarget = 0;
	m_mainTargetSurface = 0;
	for (u32 i = 0; i < (u32)yyVideoDriverAPI::TextureSlot::Count; ++i)
	{
		m_currentTextures[i] = nullptr;
	}
	m_currentMaterial = nullptr;
	m_currentModel = nullptr;

	m_activeShader = 0;
	m_shaderScreenQuad = 0;
	m_shaderGUI = nullptr;
	m_shaderSprite = nullptr;
	m_shaderSimple = 0;
	m_shaderSimpleAnimated = 0;
	m_shaderLine3D = 0;

	m_isGUI = false;
	m_vsync = true;
	//m_D3DLibrary = nullptr;
	m_SwapChain = nullptr;
	//m_activeWindowGPUData = nullptr;
	m_d3d11Device = nullptr;
	m_d3d11DevCon = nullptr;
	m_MainTargetView = nullptr;
	m_depthStencilBuffer = nullptr;
	m_depthStencilStateEnabled = nullptr;
	m_depthStencilStateDisabled = nullptr;
	m_depthStencilView = nullptr;
	m_RasterizerSolid = nullptr;
	m_RasterizerSolidNoBackFaceCulling = nullptr;
	m_RasterizerWireframeNoBackFaceCulling = nullptr;
	m_RasterizerWireframe = nullptr;
	m_blendStateAlphaEnabled = nullptr;
	m_blendStateAlphaEnabledWithATC = nullptr;
	m_blendStateAlphaDisabled = nullptr;
}
D3D11::~D3D11()
{
	if (m_mainTarget) yyDestroy(m_mainTarget);
	if (m_mainTargetSurface) yyDestroy(m_mainTargetSurface);

	if (m_shaderLine3D) yyDestroy(m_shaderLine3D);
	if (m_shaderSimpleAnimated) yyDestroy(m_shaderSimpleAnimated);
	if (m_shaderSimple) yyDestroy(m_shaderSimple);
	if (m_shaderScreenQuad)yyDestroy(m_shaderScreenQuad);
	if (m_shaderSprite)yyDestroy(m_shaderSprite);
	if (m_shaderGUI) yyDestroy(m_shaderGUI);

	if (m_blendStateAlphaDisabled)              m_blendStateAlphaDisabled->Release();
	if (m_blendStateAlphaEnabledWithATC)        m_blendStateAlphaEnabledWithATC->Release();
	if (m_blendStateAlphaEnabled)               m_blendStateAlphaEnabled->Release();
	if (m_RasterizerWireframeNoBackFaceCulling) m_RasterizerWireframeNoBackFaceCulling->Release();
	if (m_RasterizerWireframe)                  m_RasterizerWireframe->Release();
	if (m_RasterizerSolidNoBackFaceCulling)     m_RasterizerSolidNoBackFaceCulling->Release();
	if (m_RasterizerSolid)                      m_RasterizerSolid->Release();
	if (m_depthStencilView)                     m_depthStencilView->Release();
	if (m_depthStencilStateDisabled)            m_depthStencilStateDisabled->Release();
	if (m_depthStencilStateEnabled)             m_depthStencilStateEnabled->Release();
	if (m_depthStencilBuffer)                   m_depthStencilBuffer->Release();
	if (m_MainTargetView)                       m_MainTargetView->Release();
	if (m_d3d11DevCon)                          m_d3d11DevCon->Release();
	if (m_SwapChain)                            m_SwapChain->Release();

	if (m_d3d11Device)                          m_d3d11Device->Release();
}
bool D3D11::Init(yyWindow* window)
{
	assert(window);
	yyLogWriteInfo("Init video driver - D3D11...\n");

	m_windowSize.x = window->m_currentSize.x;
	m_windowSize.y = window->m_currentSize.y;
	m_mainTargetSize.x = window->m_currentSize.x;
	m_mainTargetSize.y = window->m_currentSize.y;

	DXGI_MODE_DESC	bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Width = window->m_currentSize.x;
	bufferDesc.Height = window->m_currentSize.y;
	m_swapChainSize.x = window->m_currentSize.x;
	m_swapChainSize.y = window->m_currentSize.y;
	//if (m_params.m_vSync)
	bufferDesc.RefreshRate.Numerator = 60;
	//else bufferDesc.RefreshRate.Numerator = 0;
	bufferDesc.RefreshRate.Denominator = 1;

	bufferDesc.Format			= DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling			= DXGI_MODE_SCALING_UNSPECIFIED;


	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	auto hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_SINGLETHREADED,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&m_d3d11Device,
		&featureLevel,
		&m_d3d11DevCon);

	yyLogWriteInfo("featureLevel : %u\n", featureLevel);
	if (FAILED(hr)) 
	{
		yyLogWriteError("Can't create Direct3D 11 Device : code %u\n", hr);
		YY_PRINT_FAILED;
		return false;
	}

	UINT numQualityLevels = 0;
	hr = m_d3d11Device->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		4,
		&numQualityLevels);
	if (FAILED(hr)) 
	{
		yyLogWriteError("Can't Check Multisample Quality Levels : code %u\n", hr);
		YY_PRINT_FAILED;
		return false;
	}

	yyLogWriteInfo("Hardware MSAA Quality levels : %u\n", numQualityLevels);

	IDXGIDevice *  dxgiDevice = nullptr;
	IDXGIAdapter * dxgiAdapter = nullptr;
	IDXGIFactory1* dxgiFactory = nullptr;
	hr = m_d3d11Device->QueryInterface(IID_IDXGIDevice, (void**)&dxgiDevice);
	if (FAILED(hr)) 
	{
		yyLogWriteError("Can't QueryInterface : IID_IDXGIDevice, code %u\n", hr);
		YY_PRINT_FAILED;
		return false;
	}

	hr = dxgiDevice->GetParent(IID_IDXGIAdapter, (void**)&dxgiAdapter);
	if (FAILED(hr)) 
	{
		yyLogWriteError("Can't get DXGI adapter, code %u\n", hr);
		YY_PRINT_FAILED;
		return false;
	}

	hr = dxgiAdapter->GetParent(IID_IDXGIFactory, (void**)&dxgiFactory);
	if (FAILED(hr)) 
	{
		yyLogWriteError("Can't get DXGI factory, code %u\n", hr);
		YY_PRINT_FAILED;
		return false;
	}

	DXGI_SWAP_CHAIN_DESC	swapChainDesc;
	memset(&swapChainDesc, 0, sizeof(swapChainDesc));
	swapChainDesc.BufferDesc = bufferDesc;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = window->m_hWnd;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.Windowed = true/*m_params.m_fullScreen ? false : true*/;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	hr = dxgiFactory->CreateSwapChain( m_d3d11Device, &swapChainDesc, &m_SwapChain);
	if (FAILED(hr)) 
	{
		yyLogWriteError("Can't create Swap Chain : code %u\n", hr);
		YY_PRINT_FAILED;
		return false;
	}

	dxgiFactory->MakeWindowAssociation(window->m_hWnd, DXGI_MWA_NO_ALT_ENTER);
	dxgiDevice->Release();
	dxgiAdapter->Release();
	dxgiFactory->Release();

	_createBackBuffer(window->m_currentSize.x, window->m_currentSize.y);
	/*ID3D11Texture2D* BackBuffer;
	if (FAILED(m_SwapChain->GetBuffer(
		0,
		IID_ID3D11Texture2D,
		(void**)&BackBuffer))) 
	{
		yyLogWriteError("Can't create Direct3D 11 back buffer\n");
		YY_PRINT_FAILED;
		return false;
	}*/

	/*if (FAILED(this->m_d3d11Device->CreateRenderTargetView(
		BackBuffer, 0, &m_MainTargetView))) 
	{
		yyLogWriteError("Can't create Direct3D 11 render target\n");
		if (BackBuffer) BackBuffer->Release();
		YY_PRINT_FAILED;
		return false;
	}*/

//	if (BackBuffer) BackBuffer->Release();

	/*D3D11_TEXTURE2D_DESC	DSD;
	ZeroMemory(&DSD, sizeof(DSD));
	DSD.Width = window->m_currentSize.x;
	DSD.Height = window->m_currentSize.y;
	DSD.MipLevels = 1;
	DSD.ArraySize = 1;
	DSD.Format = DXGI_FORMAT_D32_FLOAT;
	DSD.SampleDesc.Count = 1;
	DSD.SampleDesc.Quality = 0;
	DSD.Usage = D3D11_USAGE_DEFAULT;
	DSD.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DSD.CPUAccessFlags = 0;
	DSD.MiscFlags = 0;
	if (FAILED(m_d3d11Device->CreateTexture2D(&DSD, 0, &m_depthStencilBuffer))) 
	{
		yyLogWriteError("Can't create Direct3D 11 depth stencil buffer\n");
		YY_PRINT_FAILED;
		return false;
	}*/

	D3D11_DEPTH_STENCIL_DESC	depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	if (FAILED(m_d3d11Device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilStateEnabled))) 
	{
		yyLogWriteError("Can't create Direct3D 11 depth stencil state\n");
		YY_PRINT_FAILED;
		return false;
	}

	m_d3d11DevCon->OMSetDepthStencilState(this->m_depthStencilStateEnabled, 0);

	depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;
	depthStencilDesc.StencilEnable = false;
	depthStencilDesc.DepthEnable = false;
	if (FAILED(m_d3d11Device->CreateDepthStencilState(&depthStencilDesc, &this->m_depthStencilStateDisabled))) 
	{
		yyLogWriteError("Can't create Direct3D 11 depth stencil state\n");
		YY_PRINT_FAILED;
		return false;
	}

	/*ZeroMemory(&m_depthStencilViewDesc, sizeof(m_depthStencilViewDesc));
	m_depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	m_depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	m_depthStencilViewDesc.Texture2D.MipSlice = 0;
	if (FAILED(m_d3d11Device->CreateDepthStencilView(m_depthStencilBuffer, &m_depthStencilViewDesc, &m_depthStencilView))) 
	{
		yyLogWriteError("Can't create Direct3D 11 depth stencil view\n");
		YY_PRINT_FAILED;
		return false;
	}
	m_d3d11DevCon->OMSetRenderTargets(1, &m_MainTargetView, m_depthStencilView);*/

	D3D11_RASTERIZER_DESC	rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterDesc.AntialiasedLineEnable = true;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = true;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	if (FAILED(m_d3d11Device->CreateRasterizerState(&rasterDesc, &m_RasterizerSolid))) 
	{
		yyLogWriteError("Can not create rasterizer state\n");
		YY_PRINT_FAILED;
		return false;
	}

	rasterDesc.CullMode = D3D11_CULL_NONE;
	m_d3d11Device->CreateRasterizerState(&rasterDesc, &m_RasterizerSolidNoBackFaceCulling);
	rasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	m_d3d11Device->CreateRasterizerState(&rasterDesc, &m_RasterizerWireframeNoBackFaceCulling);
	rasterDesc.CullMode = D3D11_CULL_BACK;
	m_d3d11Device->CreateRasterizerState(&rasterDesc, &m_RasterizerWireframe);

	m_d3d11DevCon->RSSetState(m_RasterizerSolid);

	D3D11_BLEND_DESC  bd;
	memset(&bd, 0, sizeof(bd));
	bd.AlphaToCoverageEnable = 0;
	bd.RenderTarget[0].BlendEnable = TRUE;
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	if (FAILED(m_d3d11Device->CreateBlendState(&bd, &m_blendStateAlphaEnabled))) 
	{
		yyLogWriteError("Can't create Direct3D 11 blend state\n");
		YY_PRINT_FAILED;
		return false;
	}

	bd.AlphaToCoverageEnable = 1;
	if (FAILED(m_d3d11Device->CreateBlendState(&bd, &m_blendStateAlphaEnabledWithATC))) 
	{
		yyLogWriteError("Can't create Direct3D 11 blend state\n");
		YY_PRINT_FAILED;
		return false;
	}


	bd.RenderTarget[0].BlendEnable = FALSE;
	if (FAILED(m_d3d11Device->CreateBlendState(&bd, &m_blendStateAlphaDisabled)))
	{
		yyLogWriteError("Can't create Direct3D 11 blend state\n");
		YY_PRINT_FAILED;
		return false;
	}

	float blendFactor[4];
	blendFactor[0] = 0.0f;
	blendFactor[1] = 0.0f;
	blendFactor[2] = 0.0f;
	blendFactor[3] = 0.0f;
	//	if (atc)
		//	m_d3d11DevCon->OMSetBlendState(m_blendStateAlphaEnabledWithATC, blendFactor, 0xffffffff);
	m_d3d11DevCon->OMSetBlendState(m_blendStateAlphaEnabled, blendFactor, 0xffffffff);

	D3D11_RECT sr;
	sr.left = 0;
	sr.top = 0;
	sr.right = window->m_currentSize.x;
	sr.bottom = window->m_currentSize.y;
	m_d3d11DevCon->RSSetScissorRects(1, &sr);

	D3D11_VIEWPORT viewport;
	viewport.Width = (f32)window->m_currentSize.x;
	viewport.Height = (f32)window->m_currentSize.y;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	m_d3d11DevCon->RSSetViewports(1, &viewport);
	
	UpdateGUIProjectionMatrix(window->m_currentSize);

	m_shaderGUI = yyCreate<D3D11ShaderGUI>();
	if (!m_shaderGUI->init())
	{
		yyLogWriteError("Can't create GUI shader...");
		YY_PRINT_FAILED;
		return false;
	}

	m_shaderSprite = yyCreate<D3D11ShaderSprite>();
	if (!m_shaderSprite->init())
	{
		yyLogWriteError("Can't create sprite shader...");
		YY_PRINT_FAILED;
		return false;
	}

	m_shaderScreenQuad = yyCreate<D3D11ShaderScreenQuad>();
	if (!m_shaderScreenQuad->init())
	{
		yyLogWriteError("Can't create screen quad shader...");
		YY_PRINT_FAILED;
		return false;
	}

	m_shaderSimple = yyCreate<D3D11ShaderSimple>();
	if (!m_shaderSimple->init())
	{
		yyLogWriteError("Can't create simple shader...");
		YY_PRINT_FAILED;
		return false;
	}

	m_shaderSimpleAnimated = yyCreate<D3D11ShaderSimpleAnimated>();
	if (!m_shaderSimpleAnimated->init())
	{
		yyLogWriteError("Can't create simple animated shader...");
		YY_PRINT_FAILED;
		return false;
	}
	 
	m_shaderLine3D = yyCreate<D3D11ShaderLine3D>();
	if (!m_shaderLine3D->init())
	{
		yyLogWriteError("Can't create Line 3D shader...");
		YY_PRINT_FAILED;
		return false;
	}

	if (!updateMainTarget())
	{
		YY_PRINT_FAILED;
		return false;
	}

	return true;
}

bool D3D11::updateMainTarget()
{
	if (m_mainTarget) yyDestroy(m_mainTarget);
	if (m_mainTargetSurface) yyDestroy(m_mainTargetSurface);

	m_mainTarget = yyCreate<D3D11Texture>();
	if (!this->initRTT(m_mainTarget, m_mainTargetSize, false, false))
	{
		yyLogWriteError("Can't create main render target...");
		YY_PRINT_FAILED;
		return false;
	}

	auto model = yyCreate<yyModel>();
	model->m_iCount = 6;
	model->m_vCount = 4;
	model->m_stride = sizeof(yyVertexGUI);
	model->m_vertexType = yyVertexType::GUI;
	model->m_vertices = (u8*)yyMemAlloc(model->m_vCount * model->m_stride);
	model->m_indices = (u8*)yyMemAlloc(model->m_iCount * sizeof(u16));
	u16* inds = (u16*)model->m_indices;

	yyVertexGUI * vertex = (yyVertexGUI*)model->m_vertices;
	vertex->m_position.set(-1.f, 1.f);
	vertex->m_tcoords.set(0.f, 0.f);
	vertex++;
	vertex->m_position.set(-1.f, -1.f);
	vertex->m_tcoords.set(0.f, 1.f);
	vertex++;
	vertex->m_position.set(1.f, -1.f);
	vertex->m_tcoords.set(1.f, 1.f);
	vertex++;
	vertex->m_position.set(1.f, 1.f);
	vertex->m_tcoords.set(1.f, 0.f);
	vertex++;

	inds[0] = 0;
	inds[1] = 1;
	inds[2] = 2;
	inds[3] = 0;
	inds[4] = 2;
	inds[5] = 3;

	m_mainTargetSurface = yyCreate<D3D11Model>();
	if (!initModel(model, m_mainTargetSurface))
	{
		yyDestroy(model);
		yyLogWriteError("Can't create main render target surface...");
		YY_PRINT_FAILED;
		return false;
	}
	yyDestroy(model);

	/*if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}
	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}
	if (m_MainTargetView)
	{
		m_MainTargetView->Release();
		m_MainTargetView = 0;
	}*/
	
	//m_SwapChain->ResizeBuffers(0, m_windowSize.x, m_windowSize.y, DXGI_FORMAT_UNKNOWN, 0);
	//_createBackBuffer(m_windowSize.x, m_windowSize.y);

	return true;
}

bool D3D11::_createBackBuffer(int x, int y)
{
	if (m_depthStencilBuffer)
	{
		m_depthStencilBuffer->Release();
		m_depthStencilBuffer = 0;
	}
	if (m_depthStencilView)
	{
		m_depthStencilView->Release();
		m_depthStencilView = 0;
	}
	if (m_MainTargetView)
	{
		m_MainTargetView->Release();
		m_MainTargetView = 0;
	}

	ID3D11Texture2D* BackBuffer;
	if (FAILED(m_SwapChain->GetBuffer(
		0,
		IID_ID3D11Texture2D,
		(void**)&BackBuffer)))
	{
		yyLogWriteError("Can't create Direct3D 11 back buffer\n");
		YY_PRINT_FAILED;
		return false;
	}
	if (FAILED(this->m_d3d11Device->CreateRenderTargetView(
		BackBuffer, 0, &m_MainTargetView)))
	{
		yyLogWriteError("Can't create Direct3D 11 render target\n");
		if (BackBuffer) BackBuffer->Release();
		YY_PRINT_FAILED;
		return false;
	}
	if (BackBuffer) BackBuffer->Release();
	D3D11_TEXTURE2D_DESC	DSD;
	ZeroMemory(&DSD, sizeof(DSD));
	DSD.Width = x;
	DSD.Height = y;
	DSD.MipLevels = 1;
	DSD.ArraySize = 1;
	DSD.Format = DXGI_FORMAT_D32_FLOAT;
	DSD.SampleDesc.Count = 1;
	DSD.SampleDesc.Quality = 0;
	DSD.Usage = D3D11_USAGE_DEFAULT;
	DSD.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	DSD.CPUAccessFlags = 0;
	DSD.MiscFlags = 0;
	if (FAILED(m_d3d11Device->CreateTexture2D(&DSD, 0, &m_depthStencilBuffer)))
	{
		yyLogWriteError("Can't create Direct3D 11 depth stencil buffer\n");
		YY_PRINT_FAILED;
		return false;
	}
	ZeroMemory(&m_depthStencilViewDesc, sizeof(m_depthStencilViewDesc));
	m_depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	m_depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	m_depthStencilViewDesc.Texture2D.MipSlice = 0;
	if (FAILED(m_d3d11Device->CreateDepthStencilView(m_depthStencilBuffer, &m_depthStencilViewDesc, &m_depthStencilView)))
	{
		yyLogWriteError("Can't create Direct3D 11 depth stencil view\n");
		YY_PRINT_FAILED;
		return false;
	}
	m_d3d11DevCon->OMSetRenderTargets(1, &m_MainTargetView, m_depthStencilView);
	return true;
}

void D3D11::SetShader(D3D11ShaderCommon* shader)
{
//	if (shader != m_activeShader)
	{
		m_activeShader = shader;
		m_d3d11DevCon->IASetInputLayout(shader->m_vLayout);
		m_d3d11DevCon->VSSetShader(shader->m_vShader, 0, 0);
		m_d3d11DevCon->PSSetShader(shader->m_pShader, 0, 0);
	}
}

HRESULT	D3D11::createSamplerState(
	D3D11_FILTER filter,
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

	return m_d3d11Device->CreateSamplerState(&samplerDesc, samplerState);
}
bool D3D11::initRTT(D3D11Texture* newTexture, const v2f& size, bool useLinearFilter, bool useComparisonFilter)
{
	newTexture->m_h = size.x;
	newTexture->m_w = size.y;

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = newTexture->m_h;
	desc.Height = newTexture->m_w;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.MiscFlags = 0;
	desc.ArraySize = 1;
	desc.MipLevels = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;

	auto hr = m_d3d11Device->CreateTexture2D(&desc, NULL, &newTexture->m_texture);
	if (FAILED(hr)) 
	{
		yyLogWriteError("Can't create render target texture\n");
		YY_PRINT_FAILED;
		return false;
	}
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = desc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	hr = m_d3d11Device->CreateRenderTargetView(newTexture->m_texture, &renderTargetViewDesc, &newTexture->m_RTV);
	if (FAILED(hr)) 
	{
		yyLogWriteError("Can't create render target view\n");
		YY_PRINT_FAILED;
		return false;
	}



	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = desc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	SRVDesc.Texture2D.MostDetailedMip = 0;
	SRVDesc.Texture2D.MipLevels = 1;

	hr = m_d3d11Device->CreateShaderResourceView(newTexture->m_texture,
		&SRVDesc, &newTexture->m_textureResView);
	if (FAILED(hr))
	{
		yyLogWriteError("Can't create shader resource view\n");
		YY_PRINT_FAILED;
		return false;
	}

	//if (!is_RTT) 
	{
		D3D11_FILTER filter;

		if (useLinearFilter)
		{
			filter = D3D11_FILTER::D3D11_FILTER_ANISOTROPIC;
			if (useComparisonFilter)
				filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_ANISOTROPIC;
		}
		else
		{
			filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT;
			if (useComparisonFilter)
				filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
		}

		auto hr = this->createSamplerState(filter, D3D11_TEXTURE_ADDRESS_WRAP, 1, &newTexture->m_samplerState);
		if (FAILED(hr))
		{
			yyLogWriteError("Can't create sampler state\n");
			YY_PRINT_FAILED;
			return false;
		}
	}

	return true;
}
bool D3D11::initTexture(yyImage* image, D3D11Texture* newTexture, bool useLinearFilter, bool useComparedFilter)
{
	newTexture->m_h = image->m_height;
	newTexture->m_w = image->m_width;

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = image->m_width;
	desc.Height = image->m_height;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;


	switch (image->m_format)
	{
	case yyImageFormat::BC1:
	case yyImageFormat::BC2:
	case yyImageFormat::BC3:
	{
		auto hr = DirectX::CreateDDSTextureFromMemory(
			m_d3d11Device,
			m_d3d11DevCon,
			(const uint8_t*)image->m_data,
			(size_t)image->m_fileSize,
			(ID3D11Resource**)&newTexture->m_texture,
			&newTexture->m_textureResView);

		if (FAILED(hr))
		{
			YY_PRINT_FAILED;
			return false;
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

		if (useLinearFilter)
		{
			desc.MipLevels = 0;
			desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
			auto hr = m_d3d11Device->CreateTexture2D(&desc, 0, &newTexture->m_texture);
			if (FAILED(hr))
			{
				yyLogWriteError("Can't create 2D texture\n");
				YY_PRINT_FAILED;
				return false;
			}
			m_d3d11DevCon->UpdateSubresource(newTexture->m_texture, 0, NULL, image->m_data, image->m_pitch, 0);
		}
		else
		{
			D3D11_SUBRESOURCE_DATA initData;
			ZeroMemory(&initData, sizeof(initData));
			initData.pSysMem = image->m_data;
			initData.SysMemPitch = image->m_pitch;
			initData.SysMemSlicePitch = image->m_dataSize;
			auto hr = m_d3d11Device->CreateTexture2D(&desc, &initData, &newTexture->m_texture);
			if (FAILED(hr))
			{
				yyLogWriteError("Can't create 2D texture\n");
				YY_PRINT_FAILED;
				return false;
			}
		}


		D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
		ZeroMemory(&SRVDesc, sizeof(SRVDesc));
		SRVDesc.Format = desc.Format;
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MostDetailedMip = 0;
		SRVDesc.Texture2D.MipLevels = 1;
		if (useLinearFilter)
			SRVDesc.Texture2D.MipLevels = -1;

		auto hr = m_d3d11Device->CreateShaderResourceView(newTexture->m_texture,
			&SRVDesc, &newTexture->m_textureResView);
		if (FAILED(hr))
		{
			yyLogWriteError("Can't create shader resource view\n");
			YY_PRINT_FAILED;
			return false;
		}
	}break;
	default:
		yyLogWriteWarning("Unsupported texture format\n");
		YY_PRINT_FAILED;
		return false;
	}

	
	D3D11_FILTER filter;

	if (useLinearFilter)
	{
		filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		if(useComparedFilter)
			filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;

		m_d3d11DevCon->GenerateMips(newTexture->m_textureResView);
	}
	else
	{
		filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT;
		if (useComparedFilter)
			filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
	}

	auto hr = this->createSamplerState( filter, D3D11_TEXTURE_ADDRESS_WRAP, 1, &newTexture->m_samplerState);
	if (FAILED(hr)) 
	{
		yyLogWriteError("Can't create sampler state\n");
		YY_PRINT_FAILED;
		return false;
	}

	return true;
}

bool D3D11::initModel(yyModel* model, D3D11Model* d3d11Model)
{
	d3d11Model->m_material = model->m_material;
	d3d11Model->m_vertexType = model->m_vertexType;

	D3D11_BUFFER_DESC	vbd, ibd;

	ZeroMemory(&vbd, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&ibd, sizeof(D3D11_BUFFER_DESC));

	vbd.Usage = D3D11_USAGE_DEFAULT;
	//vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	ibd.Usage = D3D11_USAGE_DEFAULT;
	//ibd.Usage = D3D11_USAGE_DYNAMIC;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	//vbd.CPUAccessFlags = 0;
	//ibd.CPUAccessFlags = 0; //D3D11_CPU_ACCESS_WRITE

	D3D11_SUBRESOURCE_DATA	vData, iData;
	ZeroMemory(&vData, sizeof(D3D11_SUBRESOURCE_DATA));
	ZeroMemory(&iData, sizeof(D3D11_SUBRESOURCE_DATA));
	HRESULT	hr = 0;


	vbd.ByteWidth = model->m_stride * model->m_vCount;
	vData.pSysMem = &model->m_vertices[0];

	hr = m_d3d11Device->CreateBuffer(&vbd, &vData, &d3d11Model->m_vBuffer);
	if (FAILED(hr)) 
	{
		yyLogWriteError("Can't create Direct3D 11 vertex buffer [%u]\n", hr);
		YY_PRINT_FAILED;
		return false;
	}


	u32 index_sizeof = sizeof(u16);
	d3d11Model->m_indexType = DXGI_FORMAT_R16_UINT;
	if (model->m_indexType == yyMeshIndexType::u32)
	{
		d3d11Model->m_indexType = DXGI_FORMAT_R32_UINT;
		index_sizeof = sizeof(u32);
	}
	ibd.ByteWidth = index_sizeof * model->m_iCount;
	iData.pSysMem = &model->m_indices[0];

	d3d11Model->m_iCount = model->m_iCount;
	d3d11Model->m_stride = model->m_stride;

	hr = m_d3d11Device->CreateBuffer(&ibd, &iData, &d3d11Model->m_iBuffer);
	if (FAILED(hr)) 
	{
		yyLogWriteError("Can't create Direct3D 11 index buffer [%u]\n", hr);
		YY_PRINT_FAILED;
		return false;
	}

	return true;
}