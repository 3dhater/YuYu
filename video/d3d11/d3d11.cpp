#include "yy.h"
#include "yy_window.h"

#include "d3d11.h"

D3D11::D3D11()
{
	m_vsync = false;
	m_D3DLibrary = nullptr;
	m_SwapChain = nullptr;
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
	if (m_D3DLibrary)							FreeLibrary(m_D3DLibrary);
}
bool D3D11::Init(yyWindow* window)
{
	assert(window);
	yyLogWriteInfo("Init video driver - D3D11...\n");

	DXGI_MODE_DESC	bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Width = window->m_clientSize.x;
	bufferDesc.Height = window->m_clientSize.y;
	//if (m_params.m_vSync)
	bufferDesc.RefreshRate.Numerator = 60;
	//else bufferDesc.RefreshRate.Numerator = 0;
	bufferDesc.RefreshRate.Denominator = 1;

	bufferDesc.Format			= DXGI_FORMAT_R8G8B8A8_UNORM;
	bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	bufferDesc.Scaling			= DXGI_MODE_SCALING_UNSPECIFIED;

	wchar_t systemDir[256];

	GetSystemDirectoryW(systemDir, 256);
	yyString d3dlib_str = systemDir;
	d3dlib_str += u"\\d3d11.dll";

	m_D3DLibrary = LoadLibrary((wchar_t*)d3dlib_str.data());
	if (!m_D3DLibrary) 
	{
		yyLogWriteError("Could not load d3d11.dll\n");
		YY_PRINT_FAILED;
		return false;
	}

	yyD3D11CreateDevice_t D3D11CreateDevice = (yyD3D11CreateDevice_t)GetProcAddress(m_D3DLibrary, "D3D11CreateDevice");
	if (!D3D11CreateDevice) 
	{
		yyLogWriteError("Could not get proc adress of D3D11CreateDevice\n");
		YY_PRINT_FAILED;
		return false;
	}

	yyD3D11CreateDeviceAndSwapChain_t D3D11CreateDeviceAndSwapChain =
		(yyD3D11CreateDeviceAndSwapChain_t)GetProcAddress(m_D3DLibrary, "D3D11CreateDeviceAndSwapChain");
	if (!D3D11CreateDeviceAndSwapChain) 
	{
		yyLogWriteError("Could not get proc adress of D3D11CreateDeviceAndSwapChain");
		YY_PRINT_FAILED;
		return false;
	}

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
	DSD.Width = bufferDesc.Width;
	DSD.Height = bufferDesc.Height;
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

	D3D11_DEPTH_STENCIL_DESC	depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
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

	depthStencilDesc.DepthEnable = false;
	if (FAILED(m_d3d11Device->CreateDepthStencilState(&depthStencilDesc, &this->m_depthStencilStateDisabled))) 
	{
		yyLogWriteError("Can't create Direct3D 11 depth stencil state\n");
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
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
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
	blendFactor[0] = 1.0f;
	blendFactor[1] = 1.0f;
	blendFactor[2] = 1.0f;
	blendFactor[3] = 1.0f;
	//	if (atc)
		//	m_d3d11DevCon->OMSetBlendState(m_blendStateAlphaEnabledWithATC, blendFactor, 0xffffffff);
	m_d3d11DevCon->OMSetBlendState(m_blendStateAlphaEnabled, blendFactor, 0xffffffff);

	D3D11_VIEWPORT viewport;
	viewport.Width = bufferDesc.Width;
	viewport.Height = bufferDesc.Height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	m_d3d11DevCon->RSSetViewports(1, &viewport);

	return true;
}