#define YY_EXPORTS
#include "yy.h"
#include "yy_color.h"
#include "yy_window.h"
#include "yy_image.h"
#include "yy_material.h"
#include "yy_ptr.h"
#include "yy_model.h"
#include "yy_gui.h"

#include "vid_d3d11.h"
#include "d3d11_texture.h"
#include "d3d11_model.h"
#include "d3d11_shader_GUI.h"
#include "d3d11_shader_sprite.h"
#include "d3d11_shader_sprite2.h"
#include "d3d11_shader_ScreenQuad.h"
#include "d3d11_shader_simple.h"
#include "d3d11_shader_Line3D.h"
#include "d3d11_shader_LineModel.h"
#include "d3d11_shader_Rectangle.h"
#include "d3d11_shader_points.h"

#include "d3d11_shader_standart.h"

#include "scene/common.h"
#include "scene/sprite.h"
#include "scene/sprite2.h"

yyVideoDriverAPI g_api;
D3D11 * g_d3d11 = nullptr;

void SetViewport(f32 x, f32 y, f32 width, f32 height, yyWindow* window) {
	D3D11_VIEWPORT viewport;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = x;
	viewport.TopLeftY = y;
	g_d3d11->m_d3d11DevCon->RSSetViewports(1, &viewport);
	g_d3d11->m_viewportSize.x = width;
	g_d3d11->m_viewportSize.y = height;
}

u32 GetAPIVersion(){
	return yyVideoDriverAPIVersion;
}

void GetDepthRange(v2f* d) {
	assert(d);
	d->x = 0.f;
	d->y = 1.f;
}

bool Init(yyWindow* window){
	assert(window);
	if(g_d3d11)
		return true;

	g_d3d11 = yyCreate<D3D11>();
	if(!g_d3d11->Init(window))
	{
		yyDestroy(g_d3d11);
		g_d3d11 = nullptr;
		return false;
	}
	return true;
}

void Destroy(){
	if(g_d3d11)
	{
		yyDestroy(g_d3d11);
		g_d3d11 = nullptr;
	}
}

void SetClearColor(f32 r, f32 g, f32 b, f32 a){
	g_d3d11->m_clearColor.set(r, g, b, a);
}

void UseVSync(bool v){
	g_d3d11->m_vsync = v;
}

void UseDepth(bool v){
	v ? g_d3d11->m_d3d11DevCon->OMSetDepthStencilState(g_d3d11->m_depthStencilStateEnabled, 0)
		: g_d3d11->m_d3d11DevCon->OMSetDepthStencilState(g_d3d11->m_depthStencilStateDisabled, 0);
}

void UseBlend(bool v){
	if (v)
	{
		const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
		g_d3d11->m_d3d11DevCon->OMSetBlendState(g_d3d11->m_blendStateAlphaEnabled, blend_factor, 0xffffffff);
	}
	else
	{
		g_d3d11->m_d3d11DevCon->OMSetBlendState(g_d3d11->m_blendStateAlphaDisabled, 0, 0xffffffff);
	}
}

// `dear imgui`
struct BACKUP_DX11_STATE
{
	UINT                        ScissorRectsCount, ViewportsCount;
	D3D11_RECT                  ScissorRects[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	D3D11_VIEWPORT              Viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	ID3D11RasterizerState*      RS;
	ID3D11BlendState*           BlendState;
	FLOAT                       BlendFactor[4];
	UINT                        SampleMask;
	UINT                        StencilRef;
	ID3D11DepthStencilState*    DepthStencilState;
	ID3D11ShaderResourceView*   PSShaderResource;
	ID3D11SamplerState*         PSSampler;
	ID3D11PixelShader*          PS;
	ID3D11VertexShader*         VS;
	ID3D11GeometryShader*       GS;
	UINT                        PSInstancesCount, VSInstancesCount, GSInstancesCount;
	ID3D11ClassInstance         *PSInstances[256], *VSInstances[256], *GSInstances[256];   // 256 is max according to PSSetShader documentation
	D3D11_PRIMITIVE_TOPOLOGY    PrimitiveTopology;
	ID3D11Buffer*               IndexBuffer, *VertexBuffer, *VSConstantBuffer;
	UINT                        IndexBufferOffset, VertexBufferStride, VertexBufferOffset;
	DXGI_FORMAT                 IndexBufferFormat;
	ID3D11InputLayout*          InputLayout;
};
BACKUP_DX11_STATE old;
void BeginDrawGUI(){
	old.ScissorRectsCount = old.ViewportsCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	g_d3d11->m_d3d11DevCon->RSGetScissorRects(&old.ScissorRectsCount, old.ScissorRects);
	g_d3d11->m_d3d11DevCon->RSGetViewports(&old.ViewportsCount, old.Viewports);
	g_d3d11->m_d3d11DevCon->RSGetState(&old.RS);
	g_d3d11->m_d3d11DevCon->OMGetBlendState(&old.BlendState, old.BlendFactor, &old.SampleMask);
	g_d3d11->m_d3d11DevCon->OMGetDepthStencilState(&old.DepthStencilState, &old.StencilRef);
	g_d3d11->m_d3d11DevCon->PSGetShaderResources(0, 1, &old.PSShaderResource);
	g_d3d11->m_d3d11DevCon->PSGetSamplers(0, 1, &old.PSSampler);
	old.PSInstancesCount = old.VSInstancesCount = old.GSInstancesCount = 256;
	g_d3d11->m_d3d11DevCon->PSGetShader(&old.PS, old.PSInstances, &old.PSInstancesCount);
	g_d3d11->m_d3d11DevCon->VSGetShader(&old.VS, old.VSInstances, &old.VSInstancesCount);
	g_d3d11->m_d3d11DevCon->VSGetConstantBuffers(0, 1, &old.VSConstantBuffer);
	g_d3d11->m_d3d11DevCon->GSGetShader(&old.GS, old.GSInstances, &old.GSInstancesCount);

	g_d3d11->m_d3d11DevCon->IAGetPrimitiveTopology(&old.PrimitiveTopology);
	g_d3d11->m_d3d11DevCon->IAGetIndexBuffer(&old.IndexBuffer, &old.IndexBufferFormat, &old.IndexBufferOffset);
	g_d3d11->m_d3d11DevCon->IAGetVertexBuffers(0, 1, &old.VertexBuffer, &old.VertexBufferStride, &old.VertexBufferOffset);
	g_d3d11->m_d3d11DevCon->IAGetInputLayout(&old.InputLayout);

	g_d3d11->m_d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
	g_d3d11->m_d3d11DevCon->OMSetBlendState(g_d3d11->m_blendStateAlphaEnabled, blend_factor, 0xffffffff);

	g_d3d11->m_d3d11DevCon->OMSetDepthStencilState(g_d3d11->m_depthStencilStateDisabled, 0);
	g_d3d11->m_d3d11DevCon->RSSetState(g_d3d11->m_RasterizerSolidNoBackFaceCulling);

	g_d3d11->SetActiveShader(g_d3d11->m_shaderGUI);

	g_d3d11->m_isGUI = true;
}

void EndDrawGUI(){
	g_d3d11->m_isGUI = false;

	// Restore modified GL state
	g_d3d11->m_d3d11DevCon->RSSetScissorRects(old.ScissorRectsCount, old.ScissorRects);
	g_d3d11->m_d3d11DevCon->RSSetViewports(old.ViewportsCount, old.Viewports);
	g_d3d11->m_d3d11DevCon->RSSetState(old.RS); if (old.RS) old.RS->Release();
	g_d3d11->m_d3d11DevCon->OMSetBlendState(old.BlendState, old.BlendFactor, old.SampleMask); if (old.BlendState) old.BlendState->Release();
	g_d3d11->m_d3d11DevCon->OMSetDepthStencilState(old.DepthStencilState, old.StencilRef); if (old.DepthStencilState) old.DepthStencilState->Release();
	g_d3d11->m_d3d11DevCon->PSSetShaderResources(0, 1, &old.PSShaderResource); if (old.PSShaderResource) old.PSShaderResource->Release();
	g_d3d11->m_d3d11DevCon->PSSetSamplers(0, 1, &old.PSSampler); if (old.PSSampler) old.PSSampler->Release();
	g_d3d11->m_d3d11DevCon->PSSetShader(old.PS, old.PSInstances, old.PSInstancesCount); if (old.PS) old.PS->Release();
	for (UINT i = 0; i < old.PSInstancesCount; i++) if (old.PSInstances[i]) old.PSInstances[i]->Release();
	g_d3d11->m_d3d11DevCon->VSSetShader(old.VS, old.VSInstances, old.VSInstancesCount); if (old.VS) old.VS->Release();
	g_d3d11->m_d3d11DevCon->VSSetConstantBuffers(0, 1, &old.VSConstantBuffer); if (old.VSConstantBuffer) old.VSConstantBuffer->Release();
	g_d3d11->m_d3d11DevCon->GSSetShader(old.GS, old.GSInstances, old.GSInstancesCount); if (old.GS) old.GS->Release();
	for (UINT i = 0; i < old.VSInstancesCount; i++) if (old.VSInstances[i]) old.VSInstances[i]->Release();
	g_d3d11->m_d3d11DevCon->IASetPrimitiveTopology(old.PrimitiveTopology);
	g_d3d11->m_d3d11DevCon->IASetIndexBuffer(old.IndexBuffer, old.IndexBufferFormat, old.IndexBufferOffset); if (old.IndexBuffer) old.IndexBuffer->Release();
	g_d3d11->m_d3d11DevCon->IASetVertexBuffers(0, 1, &old.VertexBuffer, &old.VertexBufferStride, &old.VertexBufferOffset); if (old.VertexBuffer) old.VertexBuffer->Release();
	g_d3d11->m_d3d11DevCon->IASetInputLayout(old.InputLayout); if (old.InputLayout) old.InputLayout->Release();
}

void SetScissorRect(const v4f& rect, yyWindow* window) {
	D3D11_RECT r;
	r.left = (LONG)rect.x;
	r.top = (LONG)rect.y;
	r.right = (LONG)rect.z;
	r.bottom = (LONG)rect.w;
	g_d3d11->m_d3d11DevCon->RSSetScissorRects(1, &r);
}

void SetTexture(u32 slot, yyResource* res){
	if (res)
		g_d3d11->m_currentTextures[(u32)slot] = (D3D11Texture*)res->GetImplementation();
	else
		g_d3d11->m_currentTextures[slot] = nullptr;
}

void SetModel(yyResource* res){
	if (res)
		g_d3d11->m_currentModel = (D3D11Model*)res->GetImplementation();
	else
		g_d3d11->m_currentModel = nullptr;
}

void Draw(){
	if( !g_d3d11->m_currentModel )
		return;

	if(g_d3d11->m_isGUI)
	{
		//g_d3d11->m_d3d11DevCon->RSSetState(g_d3d11->m_RasterizerSolidNoBackFaceCulling);
		if(g_d3d11->m_currentTextures[0])
		{
			g_d3d11->m_d3d11DevCon->PSSetShaderResources(0, 1, &g_d3d11->m_currentTextures[0]->m_textureResView);
			g_d3d11->m_d3d11DevCon->PSSetSamplers(0, 1, &g_d3d11->m_currentTextures[0]->m_samplerState);
		}
	}
	else
	{
		auto material = yyGetMaterial();

		if (material->m_wireframe)
		{
			if (material->m_cullBackFace)
				g_d3d11->m_d3d11DevCon->RSSetState(g_d3d11->m_RasterizerWireframe);
			else
				g_d3d11->m_d3d11DevCon->RSSetState(g_d3d11->m_RasterizerWireframeNoBackFaceCulling);
		}
		else
		{
			if (material->m_cullBackFace)
				g_d3d11->m_d3d11DevCon->RSSetState(g_d3d11->m_RasterizerSolid);
			else
				g_d3d11->m_d3d11DevCon->RSSetState(g_d3d11->m_RasterizerSolidNoBackFaceCulling);
		}

		switch (material->m_type)
		{
		default:
		case yyMaterialType::Simple:
			switch (g_d3d11->m_currentModel->m_vertexType)
			{
			case yyVertexType::Model:
			{
				g_d3d11->SetActiveShader(g_d3d11->m_shaderSimple);
				g_d3d11->m_shaderSimple->SetConstants(material);
			}break;
			case yyVertexType::AnimatedModel:
			{
				g_d3d11->SetActiveShader(g_d3d11->m_shaderSimpleAnimated);
				g_d3d11->m_shaderSimpleAnimated->SetConstants(material);
			}break;
			case yyVertexType::LineModel:
			{
				g_d3d11->SetActiveShader(g_d3d11->m_shaderLineModel);
				g_d3d11->m_shaderLineModel->SetConstants(material);
			}break;
			case yyVertexType::AnimatedLineModel:
			{
				g_d3d11->SetActiveShader(g_d3d11->m_shaderLineModelAnimated);
				g_d3d11->m_shaderLineModelAnimated->SetConstants(material);
			}break;
			case yyVertexType::Point:
			{
				g_d3d11->SetActiveShader(g_d3d11->m_shaderPoints);
				g_d3d11->m_shaderPoints->SetConstants(material);
			}break;
			case yyVertexType::AnimatedPoint:
			{
				g_d3d11->SetActiveShader(g_d3d11->m_shaderPointsAnimated);
				g_d3d11->m_shaderPointsAnimated->SetConstants(material);
			}break;
			}
			if (g_d3d11->m_currentTextures[0])
			{
				g_d3d11->m_d3d11DevCon->PSSetShaderResources(0, 1, &g_d3d11->m_currentTextures[0]->m_textureResView);
				g_d3d11->m_d3d11DevCon->PSSetSamplers(0, 1, &g_d3d11->m_currentTextures[0]->m_samplerState);
			}
			break;
		}
	}
	u32 offset = 0u;
//	g_d3d11->m_d3d11DevCon->RSSetState(g_d3d11->m_RasterizerWireframeNoBackFaceCulling);
	switch (g_d3d11->m_currentModel->m_vertexType)
	{
	default:
		g_d3d11->m_d3d11DevCon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	break;
	case yyVertexType::AnimatedPoint:
	case yyVertexType::Point:
		g_d3d11->m_d3d11DevCon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		break;
	case yyVertexType::LineModel:
	case yyVertexType::AnimatedLineModel:
		g_d3d11->m_d3d11DevCon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	break;
	}

	g_d3d11->m_d3d11DevCon->IASetVertexBuffers(0, 1, &g_d3d11->m_currentModel->m_vBuffer, &g_d3d11->m_currentModel->m_stride, &offset);
	switch (g_d3d11->m_currentModel->m_vertexType)
	{
	default:
		g_d3d11->m_d3d11DevCon->IASetIndexBuffer(g_d3d11->m_currentModel->m_iBuffer, g_d3d11->m_currentModel->m_indexType, 0);
		g_d3d11->m_d3d11DevCon->DrawIndexed(g_d3d11->m_currentModel->m_iCount, 0, 0);
	break;
	case yyVertexType::AnimatedPoint:
	case yyVertexType::Point:
		g_d3d11->m_d3d11DevCon->Draw(g_d3d11->m_currentModel->m_iCount, 0);
		break;
	}
}

void DrawSprite(yySprite* sprite){
	assert(sprite);
	g_d3d11->m_shaderSprite->m_structCB.ProjMtx = g_d3d11->m_guiProjectionMatrix;
	g_d3d11->m_shaderSprite->m_structCB.World = sprite->m_objectBase.m_globalMatrix;
	g_d3d11->m_shaderSprite->m_structCB.flags = 0;
	if(sprite->m_currentState)
	{
		if(sprite->m_currentState->m_invertX)
			g_d3d11->m_shaderSprite->m_structCB.flags |= 1;
		if(sprite->m_currentState->m_invertY)
			g_d3d11->m_shaderSprite->m_structCB.flags |= 2;
	}
	g_d3d11->m_shaderSprite->m_structCB.uv1 = sprite->m_tcoords_1;
	g_d3d11->m_shaderSprite->m_structCB.uv2 = sprite->m_tcoords_2;
	g_d3d11->m_shaderSprite->m_structCB.CameraPositionScale.x = g_d3d11->m_spriteCameraPosition.x;
	g_d3d11->m_shaderSprite->m_structCB.CameraPositionScale.y = g_d3d11->m_spriteCameraPosition.y;
	g_d3d11->m_shaderSprite->m_structCB.CameraPositionScale.z = g_d3d11->m_spriteCameraScale.x;
	g_d3d11->m_shaderSprite->m_structCB.CameraPositionScale.w = g_d3d11->m_spriteCameraScale.y;
	g_d3d11->m_shaderSprite->SetConstants(0);

	g_d3d11->m_d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

//	const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
//	g_d3d11->m_d3d11DevCon->OMSetBlendState(g_d3d11->m_blendStateAlphaEnabled, blend_factor, 0xffffffff);
	g_d3d11->m_d3d11DevCon->OMSetDepthStencilState(g_d3d11->m_depthStencilStateDisabled, 0);
	g_d3d11->m_d3d11DevCon->RSSetState(g_d3d11->m_RasterizerSolidNoBackFaceCulling);

	g_d3d11->SetActiveShader(g_d3d11->m_shaderSprite);
	g_d3d11->m_d3d11DevCon->VSSetConstantBuffers(0, 1, &g_d3d11->m_shaderSprite->m_cb);


	if(sprite->m_texture)
	{
		SetTexture(0, sprite->m_texture);
		g_d3d11->m_d3d11DevCon->PSSetShaderResources(0, 1, &g_d3d11->m_currentTextures[0]->m_textureResView);
		g_d3d11->m_d3d11DevCon->PSSetSamplers(0, 1, &g_d3d11->m_currentTextures[0]->m_samplerState);
	}
	SetModel(sprite->m_model);
	u32 offset = 0u;
	g_d3d11->m_d3d11DevCon->IASetVertexBuffers(0, 1, &g_d3d11->m_currentModel->m_vBuffer, &g_d3d11->m_currentModel->m_stride, &offset);
	g_d3d11->m_d3d11DevCon->IASetIndexBuffer(g_d3d11->m_currentModel->m_iBuffer, g_d3d11->m_currentModel->m_indexType, 0);
	g_d3d11->m_d3d11DevCon->DrawIndexed(g_d3d11->m_currentModel->m_iCount, 0, 0);
}

void DrawSprite2(yySprite2* sprite){
	assert(sprite);
	g_d3d11->m_shaderSprite2->m_structCB.ProjMtx = g_d3d11->m_guiProjectionMatrix;
	g_d3d11->m_shaderSprite2->m_structCB.World = sprite->m_objectBase.m_globalMatrix;
	g_d3d11->m_shaderSprite2->m_structCB.CameraPositionScale.x = g_d3d11->m_spriteCameraPosition.x;
	g_d3d11->m_shaderSprite2->m_structCB.CameraPositionScale.y = g_d3d11->m_spriteCameraPosition.y;
	g_d3d11->m_shaderSprite2->m_structCB.CameraPositionScale.z = g_d3d11->m_spriteCameraScale.x;
	g_d3d11->m_shaderSprite2->m_structCB.CameraPositionScale.w = g_d3d11->m_spriteCameraScale.y;
	g_d3d11->m_shaderSprite2->SetConstants(0);

	g_d3d11->m_d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//	const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
	//	g_d3d11->m_d3d11DevCon->OMSetBlendState(g_d3d11->m_blendStateAlphaEnabled, blend_factor, 0xffffffff);
	g_d3d11->m_d3d11DevCon->OMSetDepthStencilState(g_d3d11->m_depthStencilStateDisabled, 0);
	g_d3d11->m_d3d11DevCon->RSSetState(g_d3d11->m_RasterizerSolidNoBackFaceCulling);

	g_d3d11->SetActiveShader(g_d3d11->m_shaderSprite2);
	g_d3d11->m_d3d11DevCon->VSSetConstantBuffers(0, 1, &g_d3d11->m_shaderSprite2->m_cb);


	if (sprite->m_texture)
	{
		SetTexture(0, sprite->m_texture);
		g_d3d11->m_d3d11DevCon->PSSetShaderResources(0, 1, &g_d3d11->m_currentTextures[0]->m_textureResView);
		g_d3d11->m_d3d11DevCon->PSSetSamplers(0, 1, &g_d3d11->m_currentTextures[0]->m_samplerState);
	}
	SetModel(sprite->m_currentState->m_activeFrameGPU);
	u32 offset = 0u;
	g_d3d11->m_d3d11DevCon->IASetVertexBuffers(0, 1, &g_d3d11->m_currentModel->m_vBuffer, &g_d3d11->m_currentModel->m_stride, &offset);
	g_d3d11->m_d3d11DevCon->IASetIndexBuffer(g_d3d11->m_currentModel->m_iBuffer, g_d3d11->m_currentModel->m_indexType, 0);
	g_d3d11->m_d3d11DevCon->DrawIndexed(g_d3d11->m_currentModel->m_iCount, 0, 0);
}

void DrawLine2D(const v3f& _p1, const v3f& _p2, const yyColor& color){
	v4f p1 = _p1;
	v4f p2 = _p2;
	g_d3d11->m_d3d11DevCon->IASetInputLayout(NULL);
	g_d3d11->m_d3d11DevCon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	g_d3d11->SetActiveShader(g_d3d11->m_shaderLine3D);
	g_d3d11->m_shaderLine3D->SetData(p1, p2, color, g_d3d11->m_guiProjectionMatrix);
	g_d3d11->m_shaderLine3D->SetConstants(0);

	g_d3d11->m_d3d11DevCon->Draw(2, 0);
}

void DrawLine3D(const v4f& p1, const v4f& p2, const yyColor& color){
	g_d3d11->m_d3d11DevCon->IASetInputLayout(NULL);
	g_d3d11->m_d3d11DevCon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	g_d3d11->SetActiveShader(g_d3d11->m_shaderLine3D);
	g_d3d11->m_shaderLine3D->SetData(p1, p2, color, *yyGetMatrix(yyMatrixType::ViewProjection));
	g_d3d11->m_shaderLine3D->SetConstants(0);
	
	g_d3d11->m_d3d11DevCon->Draw(2, 0);
}

v2f* GetSpriteCameraPosition(){
	return &g_d3d11->m_spriteCameraPosition;
}

v2f* GetSpriteCameraScale(){
	return &g_d3d11->m_spriteCameraScale;
}


yyVideoDriverObjectD3D11 g_yyVideoDriverObject;
void* GetVideoDriverObjects(){
	g_yyVideoDriverObject.m_context = g_d3d11->m_d3d11DevCon;
	g_yyVideoDriverObject.m_device = g_d3d11->m_d3d11Device;
	return &g_yyVideoDriverObject;
}

const char* GetVideoDriverName(){
	return "Direct3D 11";
}

void ClearAll(){
	g_d3d11->m_d3d11DevCon->ClearRenderTargetView(g_d3d11->m_currentTargetView, g_d3d11->m_clearColor.data());
	g_d3d11->m_d3d11DevCon->ClearDepthStencilView(g_d3d11->m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void ClearColor(){
	g_d3d11->m_d3d11DevCon->ClearRenderTargetView(g_d3d11->m_currentTargetView, g_d3d11->m_clearColor.data());
}

void ClearDepth(){
	g_d3d11->m_d3d11DevCon->ClearDepthStencilView(g_d3d11->m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void BeginDraw(){
	 // !!! ??? m_depthStencilView должен быть свой на каждый RTT ???
	g_d3d11->m_d3d11DevCon->OMSetRenderTargets(1, &g_d3d11->m_mainTarget->m_RTV, g_d3d11->m_depthStencilView);
	SetViewport(0, 0, g_d3d11->m_mainTargetSize.x, g_d3d11->m_mainTargetSize.y, 0);
	g_d3d11->m_currentTargetView = g_d3d11->m_mainTarget->m_RTV;

	UseDepth(true);
}

void EndDraw(){
	g_d3d11->m_d3d11DevCon->OMSetRenderTargets(1, &g_d3d11->m_MainTargetView, g_d3d11->m_depthStencilView);
	g_d3d11->m_currentTargetView = g_d3d11->m_MainTargetView;
	SetViewport(0, 0, (f32)g_d3d11->m_windowSize.x, (f32)g_d3d11->m_windowSize.y, 0);
	SetScissorRect(v4f(0.f, 0.f, (f32)g_d3d11->m_windowSize.x, (f32)g_d3d11->m_windowSize.y), 0);
	ClearColor();

	UseDepth(false);

	g_d3d11->m_d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	//	const float blend_factor[4] = { 0.f, 0.f, 0.f, 1.f };
//	g_d3d11->m_d3d11DevCon->OMSetBlendState(g_d3d11->m_blendStateAlphaEnabled, blend_factor, 0xffffffff);
	g_d3d11->m_d3d11DevCon->OMSetDepthStencilState(g_d3d11->m_depthStencilStateDisabled, 0);

	g_d3d11->m_d3d11DevCon->RSSetState(g_d3d11->m_RasterizerSolidNoBackFaceCulling);
	g_d3d11->SetActiveShader(g_d3d11->m_shaderScreenQuad);

	g_d3d11->m_d3d11DevCon->PSSetShaderResources(0, 1, &g_d3d11->m_mainTarget->m_textureResView);
	g_d3d11->m_d3d11DevCon->PSSetSamplers(0, 1, &g_d3d11->m_mainTarget->m_samplerState);
	u32 offset = 0u;
	g_d3d11->m_d3d11DevCon->IASetVertexBuffers(0, 1, &g_d3d11->m_mainTargetSurface->m_vBuffer, &g_d3d11->m_mainTargetSurface->m_stride, &offset);
	g_d3d11->m_d3d11DevCon->IASetIndexBuffer(g_d3d11->m_mainTargetSurface->m_iBuffer, g_d3d11->m_mainTargetSurface->m_indexType, 0);
	g_d3d11->m_d3d11DevCon->DrawIndexed(g_d3d11->m_mainTargetSurface->m_iCount, 0, 0);
}

void UpdateMainRenderTarget(const v2i& windowsSize, const v2f& bufferSize){
	g_d3d11->m_windowSize = windowsSize;
	g_d3d11->m_swapChainSize = windowsSize;
	g_d3d11->m_mainTargetSize = bufferSize;
	g_d3d11->updateMainTarget();
	
	g_d3d11->UpdateGUIProjectionMatrix(windowsSize);
}

void SetRenderTarget(yyResource* rtt){
	if (rtt)
	{
		D3D11Texture* t = (D3D11Texture*)rtt->GetImplementation();
		g_d3d11->m_d3d11DevCon->OMSetRenderTargets(1, &t->m_RTV, g_d3d11->m_depthStencilView);
		g_d3d11->m_currentTargetView = t->m_RTV;
	}
	else
	{
		g_d3d11->m_d3d11DevCon->OMSetRenderTargets(1, &g_d3d11->m_mainTarget->m_RTV, g_d3d11->m_depthStencilView);
		g_d3d11->m_currentTargetView = g_d3d11->m_mainTarget->m_RTV;
	}
}

void SwapBuffers(){
	g_d3d11->m_vsync ? g_d3d11->m_SwapChain->Present(1, 0)
		: g_d3d11->m_SwapChain->Present(0, 0);
}

yyResourceImplementation* CreateTextureImplementation() {
	return yyCreate<D3D11Texture>();
}

yyResourceImplementation* CreateModelImplementation() {
	return yyCreate<D3D11Model>();
}

void SetGUIShaderData(yyGUIElement* guielement){
	assert(guielement);
	
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	D3D11_BUFFER_DESC d;

	g_d3d11->m_d3d11DevCon->Map(g_d3d11->m_shaderGUI->m_cbVertex, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		g_d3d11->m_shaderGUI->m_cbVertex->GetDesc(&d);
		g_d3d11->m_shaderGUI->m_cbVertex_impl.m_ProjMtx = g_d3d11->m_guiProjectionMatrix;
		g_d3d11->m_shaderGUI->m_cbVertex_impl.m_Offset = guielement->m_offset;
		memcpy(mappedResource.pData, &g_d3d11->m_shaderGUI->m_cbVertex_impl, d.ByteWidth);
	g_d3d11->m_d3d11DevCon->Unmap(g_d3d11->m_shaderGUI->m_cbVertex, 0);
	g_d3d11->m_d3d11DevCon->VSSetConstantBuffers(0, 1, &g_d3d11->m_shaderGUI->m_cbVertex);

	g_d3d11->m_d3d11DevCon->Map(g_d3d11->m_shaderGUI->m_cbPixel, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		g_d3d11->m_shaderGUI->m_cbPixel->GetDesc(&d);
		g_d3d11->m_shaderGUI->m_cbPixel_impl.m_Color = guielement->m_color;
		memcpy(mappedResource.pData, &g_d3d11->m_shaderGUI->m_cbPixel_impl, d.ByteWidth);
	g_d3d11->m_d3d11DevCon->Unmap(g_d3d11->m_shaderGUI->m_cbPixel, 0);
	g_d3d11->m_d3d11DevCon->PSSetConstantBuffers(0, 1, &g_d3d11->m_shaderGUI->m_cbPixel);
}

void DrawRectangle(const v4f& corners, const yyColor& color1, const yyColor& color2) {
	auto oldShader = g_d3d11->GetActiveShader();

	g_d3d11->m_shaderRectangle->m_cbVertex_impl.m_ProjMtx = g_d3d11->m_guiProjectionMatrix;
	g_d3d11->m_shaderRectangle->m_cbVertex_impl.m_Corners = corners;
	g_d3d11->m_shaderRectangle->m_cbVertex_impl.m_Color1 = color1;
	g_d3d11->m_shaderRectangle->m_cbVertex_impl.m_Color2 = color2;
	g_d3d11->SetActiveShader(g_d3d11->m_shaderRectangle);
	g_d3d11->m_shaderRectangle->SetConstants(0);
	g_d3d11->m_d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	g_d3d11->m_d3d11DevCon->OMSetDepthStencilState(g_d3d11->m_depthStencilStateDisabled, 0);
	g_d3d11->m_d3d11DevCon->RSSetState(g_d3d11->m_RasterizerSolidNoBackFaceCulling);
	g_d3d11->m_d3d11DevCon->VSSetConstantBuffers(0, 1, &g_d3d11->m_shaderRectangle->m_cbVertex);
	g_d3d11->m_d3d11DevCon->Draw(6, 0);

	if(oldShader)
		g_d3d11->SetActiveShader(oldShader);
}

extern "C"
{
	YY_API yyVideoDriverAPI* YY_C_DECL GetAPI(){
		YY_DEBUG_PRINT_FUNC;
		g_api.BeginDraw = BeginDraw;
		g_api.BeginDrawGUI = BeginDrawGUI;
		g_api.ClearAll = ClearAll;
		g_api.ClearColor = ClearColor;
		g_api.ClearDepth = ClearDepth;
		g_api.Destroy       = Destroy;
		g_api.Draw = Draw;
		g_api.DrawLine2D = DrawLine2D;
		g_api.DrawLine3D = DrawLine3D;
		g_api.DrawRectangle = DrawRectangle;
		g_api.DrawSprite = DrawSprite;
		g_api.DrawSprite2 = DrawSprite2;
		g_api.EndDraw = EndDraw;
		g_api.EndDrawGUI = EndDrawGUI;
		g_api.GetAPIVersion = GetAPIVersion;
		g_api.GetDepthRange = GetDepthRange;
		g_api.GetSpriteCameraPosition = GetSpriteCameraPosition;
		g_api.GetSpriteCameraScale = GetSpriteCameraScale;
		g_api.GetVideoDriverName = GetVideoDriverName;
		g_api.GetVideoDriverObjects = GetVideoDriverObjects;
		g_api.Init          = Init;
		g_api.SetClearColor = SetClearColor;
		g_api.SetGUIShaderData = SetGUIShaderData;
		g_api.SetModel = SetModel;
		g_api.SetRenderTarget = SetRenderTarget;
		g_api.SetScissorRect = SetScissorRect;
		g_api.SetTexture = SetTexture;
		g_api.SetViewport = SetViewport;
		g_api.SwapBuffers = SwapBuffers;
		g_api.CreateTextureImplementation = CreateTextureImplementation;
		g_api.CreateModelImplementation = CreateModelImplementation;
		g_api.UpdateMainRenderTarget = UpdateMainRenderTarget;
		g_api.UseBlend = UseBlend;
		g_api.UseDepth = UseDepth;
		g_api.UseVSync = UseVSync;

		g_api.test_draw = 0;

		return &g_api;
	}
}
