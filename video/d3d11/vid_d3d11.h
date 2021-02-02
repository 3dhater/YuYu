#ifndef __YY_D3D11_H_
#define __YY_D3D11_H_

#include <d3d11.h>

#include "yy_color.h"
#include "math\mat.h"

//#define YY_D3D11_CALL
//using yyD3D11CreateDevice_t = HRESULT(YY_D3D11_CALL*)(
//	_In_opt_ IDXGIAdapter* pAdapter,
//	D3D_DRIVER_TYPE DriverType,
//	HMODULE Software,
//	UINT Flags,
//	_In_reads_opt_(FeatureLevels) CONST D3D_FEATURE_LEVEL* pFeatureLevels,
//	UINT FeatureLevels,
//	UINT SDKVersion,
//	_Out_opt_ ID3D11Device** ppDevice,
//	_Out_opt_ D3D_FEATURE_LEVEL* pFeatureLevel,
//	_Out_opt_ ID3D11DeviceContext** ppImmediateContext);
//
//using yyCreateDXGIFactory_t = HRESULT(YY_D3D11_CALL*)(
//	REFIID riid, _COM_Outptr_ void **ppFactory);
//
//using yyD3D11CreateDeviceAndSwapChain_t = HRESULT(YY_D3D11_CALL*)(
//	__in_opt IDXGIAdapter* pAdapter,
//	D3D_DRIVER_TYPE DriverType,
//	HMODULE Software,
//	UINT Flags,
//	__in_ecount_opt(FeatureLevels) CONST D3D_FEATURE_LEVEL* pFeatureLevels,
//	UINT FeatureLevels,
//	UINT SDKVersion,
//	__in_opt CONST DXGI_SWAP_CHAIN_DESC* pSwapChainDesc,
//	__out_opt IDXGISwapChain** ppSwapChain,
//	__out_opt ID3D11Device** ppDevice,
//	__out_opt D3D_FEATURE_LEVEL* pFeatureLevel,
//	__out_opt ID3D11DeviceContext** ppImmediateContext);
//
////	D3dcompiler_47.dll
//using yyD3DCompile_t = HRESULT(__stdcall*)(
//	LPCVOID pSrcData,
//	SIZE_T SrcDataSize,
//	LPCSTR pSourceName,
//	CONST D3D_SHADER_MACRO* pDefines,
//	ID3DInclude* pInclude,
//	LPCSTR pEntrypoint,
//	LPCSTR pTarget,
//	UINT Flags1,
//	UINT Flags2,
//	ID3DBlob** ppCode,
//	ID3DBlob** ppErrorMsgs);

//struct WindowD3D11Data
//{
//	WindowD3D11Data()
//	{
//		m_SwapChain = 0;
//		m_MainTargetView = 0;
//		m_depthStencilBuffer = nullptr;
//	}
//	~WindowD3D11Data()
//	{
//		if (m_depthStencilBuffer)                   m_depthStencilBuffer->Release();
//		if (m_MainTargetView)                       m_MainTargetView->Release();
//		if (m_SwapChain)                            m_SwapChain->Release();
//	}
//	IDXGISwapChain*			m_SwapChain;
//	ID3D11RenderTargetView* m_MainTargetView;
//	ID3D11Texture2D*		m_depthStencilBuffer;
//};

class D3D11Texture;
class D3D11Model;

class D3D11ShaderCommon;
class D3D11ShaderGUI;
class D3D11ShaderSprite;
class D3D11ShaderScreenQuad;
class D3D11ShaderSimple;
class D3D11ShaderSimpleAnimated;
class D3D11ShaderLine3D;

class D3D11
{
	bool _createBackBuffer(int x, int y);
public:
	D3D11();
	~D3D11();
	
	//HMODULE					m_D3DLibrary;
	IDXGISwapChain*			m_SwapChain;
	ID3D11Device*			m_d3d11Device;
	ID3D11DeviceContext*	m_d3d11DevCon;
	ID3D11RenderTargetView* m_MainTargetView;
	ID3D11RenderTargetView* m_currentTargetView;
	ID3D11Texture2D*		m_depthStencilBuffer;
	ID3D11DepthStencilState*m_depthStencilStateEnabled;
	ID3D11DepthStencilState*m_depthStencilStateDisabled;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState*	m_RasterizerSolid;
	ID3D11RasterizerState*	m_RasterizerSolidNoBackFaceCulling;
	ID3D11RasterizerState*	m_RasterizerWireframeNoBackFaceCulling;
	ID3D11RasterizerState*	m_RasterizerWireframe;
	ID3D11BlendState*		m_blendStateAlphaEnabled;
	ID3D11BlendState*		m_blendStateAlphaEnabledWithATC;
	ID3D11BlendState*		m_blendStateAlphaDisabled;

	D3D11_DEPTH_STENCIL_VIEW_DESC	m_depthStencilViewDesc;

	std::vector<D3D11Texture*> m_textures;
	std::vector<D3D11Model*> m_models;

	void UpdateGUIProjectionMatrix(const v2i& windowSize);
	Mat4 m_guiProjectionMatrix;

	D3D11Texture* m_currentTextures[(u32)yyVideoDriverAPI::TextureSlot::Count];
	D3D11Model*   m_currentModel;
	yyMaterial * m_currentMaterial;
	
	Mat4 m_matrixWorld;
	Mat4 m_matrixView;
	Mat4 m_matrixProjection;
	Mat4 m_matrixViewProjection;
	Mat4 m_matrixWorldViewProjection;
	Mat4 m_matrixBones[255];

	v2f m_spriteCameraPosition;
	v2f m_spriteCameraScale;

	D3D11ShaderGUI* m_shaderGUI;
	D3D11ShaderSprite* m_shaderSprite;
	D3D11ShaderScreenQuad* m_shaderScreenQuad;
	D3D11ShaderSimple* m_shaderSimple;
	D3D11ShaderSimpleAnimated* m_shaderSimpleAnimated;
	D3D11ShaderLine3D* m_shaderLine3D;
	D3D11ShaderCommon* m_activeShader;
	void SetShader(D3D11ShaderCommon*);


	v2f m_mainTargetSize;
	v2i m_windowSize;
	v2i m_swapChainSize;
	D3D11Texture* m_mainTarget;
	D3D11Model*   m_mainTargetSurface;
	bool updateMainTarget();

	bool m_isGUI;

	yyColor m_clearColor;
	bool m_vsync;

	bool Init(yyWindow* window);
	bool initModel(yyModel* model, D3D11Model* openglModel);
	bool initTexture(yyImage* image, D3D11Texture* newTexture, bool useLinearFilter, bool useComparedFilter);
	bool initRTT(D3D11Texture*, const v2f& size, bool useLinearFilter, bool useComparisonFilter);
	HRESULT	createSamplerState(D3D11_FILTER filter,
		D3D11_TEXTURE_ADDRESS_MODE addressMode,
		u32 anisotropic_level,
		ID3D11SamplerState** samplerState);

	yyListFast<size_t> m_freeModelResourceIndex;
	yyListFast<size_t> m_freeTextureResourceIndex;
};

#endif