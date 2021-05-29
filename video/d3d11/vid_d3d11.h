#ifndef __YY_D3D11_H_
#define __YY_D3D11_H_

#include <d3d11.h>

#include "yy_color.h"
#include "yy_material.h"
#include "math\mat.h"

class D3D11Texture;
class D3D11Model;

class D3D11ShaderCommon;
class D3D11ShaderGUI;
class D3D11ShaderSprite;
class D3D11ShaderSprite2;
class D3D11ShaderScreenQuad;
class D3D11ShaderSimple;
class D3D11ShaderSimpleAnimated;
class D3D11ShaderLine3D;
class D3D11ShaderLineModel;
class D3D11ShaderLineModelAnimated;
class D3D11ShaderRectangle;
class D3D11ShaderPoints;
class D3D11ShaderPointsAnimated;
class D3D11ShaderStd;

class D3D11
{
	bool _createBackBuffer(f32 x, f32 y);
public:
	D3D11();
	~D3D11();
	
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

	/*std::vector<D3D11Texture*> m_textures;
	std::vector<D3D11Model*> m_models;*/

	void UpdateGUIProjectionMatrix(const v2f& windowSize);
	Mat4 m_guiProjectionMatrix;

	D3D11Texture* m_currentTextures[yyVideoDriverMaxTextures];
	D3D11Model*   m_currentModel;
	
	v2f m_spriteCameraPosition;
	v2f m_spriteCameraScale;

	D3D11ShaderStd* m_shaderStd;
	D3D11ShaderPoints* m_shaderPoints;
	D3D11ShaderPointsAnimated* m_shaderPointsAnimated;
	D3D11ShaderRectangle* m_shaderRectangle;
	D3D11ShaderGUI* m_shaderGUI;
	D3D11ShaderSprite* m_shaderSprite;
	D3D11ShaderSprite2* m_shaderSprite2;
	D3D11ShaderScreenQuad* m_shaderScreenQuad;
	D3D11ShaderSimple* m_shaderSimple;
	D3D11ShaderSimpleAnimated* m_shaderSimpleAnimated;
	D3D11ShaderLine3D* m_shaderLine3D;
	D3D11ShaderLineModel* m_shaderLineModel;
	D3D11ShaderLineModelAnimated* m_shaderLineModelAnimated;
	D3D11ShaderCommon* m_activeShader;
	void SetActiveShader(D3D11ShaderCommon*);
	D3D11ShaderCommon* GetActiveShader();


	v2f m_mainTargetSize;
	v2f m_windowSize;
	v2f m_viewportSize;
	v2f m_swapChainSize;
	D3D11Texture* m_mainTarget;
	D3D11Model*   m_mainTargetSurface;
	bool updateMainTarget();

	bool m_isGUI;

	yyColor m_clearColor;
	bool m_vsync;
	
	bool Init(yyWindow* window);
};

#endif