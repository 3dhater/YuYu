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
#include "d3d11_shader_ScreenQuad.h"
#include "d3d11_shader_simple.h"
#include "d3d11_shader_Line3D.h"

#include "d3d11_shader_standart.h"

#include "scene/common.h"
#include "scene/sprite.h"

yyVideoDriverAPI g_api;

void LoadModel(yyResource* r);
//yyResource g_defaultRes;


void SetViewport(f32 x, f32 y, f32 width, f32 height);

u32 GetAPIVersion()
{
	return yyVideoDriverAPIVersion;
}

D3D11 * g_d3d11 = nullptr;

bool Init(yyWindow* window)
{
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
void Destroy()
{
	if(g_d3d11)
	{
		yyDestroy(g_d3d11);
		g_d3d11 = nullptr;
	}
}

void SetClearColor(f32 r, f32 g, f32 b, f32 a)
{
	g_d3d11->m_clearColor.set(r, g, b, a);
}


D3D11Model* CreateD3D11Model(yyModel* model)
{
	assert(model);
	auto newModel = yyCreate<D3D11Model>();
	if(g_d3d11->initModel(model, newModel))
		return newModel;
	yyDestroy(newModel);
	return nullptr;
}
void UnloadTexture(yyResource* r)
{
	assert(r);
	assert((r->m_type == yyResourceType::Texture) || (r->m_type == yyResourceType::RenderTargetTexture));
	if(r->m_refCount == 0)
		return;

	--r->m_refCount;
	if(!r->m_refCount)
	{
		yyDestroy(g_d3d11->m_textures[r->m_index]);
		g_d3d11->m_textures[r->m_index] = nullptr;
	}
}
D3D11Texture* CreateD3D11Texture(yyImage* image, bool useLinearFilter, bool useComparisonFilter)
{
	assert(image);
	auto newTexture = yyCreate<D3D11Texture>();
	if(g_d3d11->initTexture(image, newTexture, useLinearFilter, useComparisonFilter))
		return newTexture;
	yyDestroy(newTexture);
	return nullptr;
}
void LoadTexture(yyResource* r)
{
	assert(r);
	assert((r->m_type == yyResourceType::Texture) || (r->m_type == yyResourceType::RenderTargetTexture));
	++r->m_refCount;
	if(r->m_refCount == 1)
	{
		r->m_isLoaded = true;
		if(r->m_source)
		{
			g_d3d11->m_textures[r->m_index] = CreateD3D11Texture((yyImage*)r->m_source,
				(r->m_flags & yyResource::flags::texture_useLinearFilter) == 1,
				(r->m_flags & yyResource::flags::texture_useComparisonFilter) == 1);
			return;
		}

		if(r->m_file.size())
		{
			yyPtr<yyImage> image = yyLoadImage(r->m_file.c_str());
			g_d3d11->m_textures[r->m_index] = CreateD3D11Texture(image.m_data,
				(r->m_flags & yyResource::flags::texture_useLinearFilter) == 1,
				(r->m_flags & yyResource::flags::texture_useComparisonFilter) == 1);
			return;
		} 
	}
}
yyResource* CreateTextureFromFile(const char* fileName, bool useLinearFilter, bool useComparisonFilter, bool load)
{
	assert(fileName);
	yyResource * newRes = yyCreate<yyResource>();
	newRes->m_type = yyResourceType::Texture;
	newRes->m_source = nullptr;
	newRes->m_refCount = 0;
	if(useLinearFilter)
		newRes->m_flags |= yyResource::flags::texture_useLinearFilter;
	if (useComparisonFilter)
		newRes->m_flags |= yyResource::flags::texture_useComparisonFilter;
	newRes->m_file = fileName;

	if (g_d3d11->m_freeTextureResourceIndex.head())
	{
		newRes->m_index = g_d3d11->m_freeTextureResourceIndex.head()->m_data;
		g_d3d11->m_freeTextureResourceIndex.erase_node(g_d3d11->m_freeTextureResourceIndex.head());
	}
	else
	{
		newRes->m_index = g_d3d11->m_textures.size();
		g_d3d11->m_textures.push_back(nullptr);
	}

	if(load)
		LoadTexture(newRes);
	return newRes;
}
yyResource* CreateTexture(yyImage* image, bool useLinearFilter, bool useComparisonFilter)
{
	assert(image);
	yyResource * newRes = yyCreate<yyResource>();
	newRes->m_type = yyResourceType::Texture;
	newRes->m_source = image;
	newRes->m_refCount = 1;
	if(useLinearFilter)
		newRes->m_flags |= yyResource::flags::texture_useLinearFilter;
	if (useComparisonFilter)
		newRes->m_flags |= yyResource::flags::texture_useComparisonFilter;

	bool isNewIndex = false;
	if (g_d3d11->m_freeTextureResourceIndex.head())
	{
		newRes->m_index = g_d3d11->m_freeTextureResourceIndex.head()->m_data;
		g_d3d11->m_freeTextureResourceIndex.erase_node(g_d3d11->m_freeTextureResourceIndex.head());
	}
	else
	{
		newRes->m_index = g_d3d11->m_textures.size();
		isNewIndex = true;
	}

	auto newTexture = CreateD3D11Texture(image, useLinearFilter, useComparisonFilter);
	if(newTexture)
	{
		if (isNewIndex)
			g_d3d11->m_textures.push_back(newTexture);
		else
			g_d3d11->m_textures[newRes->m_index] = newTexture;

		newRes->m_isLoaded = true;
		return newRes;
	}

	if(newRes)
		yyDestroy( newRes );
	return nullptr;
}

void UseVSync(bool v)
{
	g_d3d11->m_vsync = v;
}
void UseDepth(bool v)
{
	v ? g_d3d11->m_d3d11DevCon->OMSetDepthStencilState(g_d3d11->m_depthStencilStateEnabled, 0)
		: g_d3d11->m_d3d11DevCon->OMSetDepthStencilState(g_d3d11->m_depthStencilStateDisabled, 0);
}
void UseBlend(bool v)
{
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

yyResource* CreateModelFromFile(const char* fileName, bool load)
{
	assert(fileName);
	yyResource * newRes = yyCreate<yyResource>();
	newRes->m_type = yyResourceType::Model;
	newRes->m_source = nullptr;
	newRes->m_refCount = 0;
	newRes->m_file = fileName;

	if (g_d3d11->m_freeModelResourceIndex.head())
	{
		newRes->m_index = g_d3d11->m_freeModelResourceIndex.head()->m_data;
		g_d3d11->m_freeModelResourceIndex.erase_node(g_d3d11->m_freeModelResourceIndex.head());
	}
	else
	{
		newRes->m_index = g_d3d11->m_models.size();
		g_d3d11->m_models.push_back(nullptr);
	}

	if(load)
		LoadModel(newRes);
	return newRes;
}
yyResource* CreateModel(yyModel* model)
{
	assert(model);
	yyResource * newRes = yyCreate<yyResource>();
	newRes->m_type = yyResourceType::Model;
	newRes->m_source = model;
	newRes->m_refCount = 1;

	bool isNewIndex = false;
	if (g_d3d11->m_freeModelResourceIndex.head())
	{
		newRes->m_index = g_d3d11->m_freeModelResourceIndex.head()->m_data;
		g_d3d11->m_freeModelResourceIndex.erase_node(g_d3d11->m_freeModelResourceIndex.head());
	}
	else
	{
		newRes->m_index = g_d3d11->m_models.size();
		isNewIndex = true;
	}

	auto newModel = CreateD3D11Model(model);
	if(newModel)
	{
		if (isNewIndex)
			g_d3d11->m_models.push_back(newModel);
		else
			g_d3d11->m_models[newRes->m_index] = newModel;
		newRes->m_isLoaded = true;
		return newRes;
	}

	if(newRes)
		yyDestroy( newRes );
	return nullptr;
}
void UnloadModel(yyResource* r)
{
	assert(r);
#ifdef YY_DEBUG
	if(r->m_type != yyResourceType::Model)
	{
		YY_PRINT_FAILED;
	}
#endif
	if(r->m_refCount == 0)
		return;

	--r->m_refCount;
	if(!r->m_refCount)
	{
		r->m_isLoaded = false;
		yyDestroy(g_d3d11->m_models[r->m_index]);
		g_d3d11->m_models[r->m_index] = nullptr;
	}
}
void LoadModel(yyResource* r)
{
	assert(r);
#ifdef YY_DEBUG
	if(r->m_type != yyResourceType::Model)
	{
		YY_PRINT_FAILED;
	}
#endif
	++r->m_refCount;
	if(r->m_refCount == 1)
	{
		r->m_isLoaded = true;
		if(r->m_source)
		{
			g_d3d11->m_models[r->m_index] = CreateD3D11Model((yyModel*)r->m_source );
			return;
		}

		if(r->m_file.size())
		{
			yyPtr<yyModel> model = yyLoadModel(r->m_file.c_str());
			g_d3d11->m_models[r->m_index] = CreateD3D11Model(model.m_data);
			return;
		}
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
void BeginDrawGUI()
{
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

	g_d3d11->SetShader(g_d3d11->m_shaderGUI);

	g_d3d11->m_isGUI = true;
}
void EndDrawGUI()
{
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

void SetTexture(yyVideoDriverAPI::TextureSlot slot, yyResource* res)
{
	g_d3d11->m_currentTextures[(u32)slot] = g_d3d11->m_textures[ res->m_index ];
}
void SetModel(yyResource* res)
{
	g_d3d11->m_currentModel = g_d3d11->m_models[res->m_index];
}
void Draw()
{
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
		auto material = g_d3d11->m_currentMaterial;
		if (!material)
			material = &g_d3d11->m_currentModel->m_material;

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
				g_d3d11->SetShader(g_d3d11->m_shaderSimple);
				g_d3d11->m_shaderSimple->SetConstants(material);
			}break;
			case yyVertexType::AnimatedModel:
			{
				g_d3d11->SetShader(g_d3d11->m_shaderSimpleAnimated);
				g_d3d11->m_shaderSimpleAnimated->SetConstants(material);
			}break;
			}
			if (g_d3d11->m_currentTextures[0])
			{
				g_d3d11->m_d3d11DevCon->PSSetShaderResources(0, 1, &g_d3d11->m_currentTextures[0]->m_textureResView);
				g_d3d11->m_d3d11DevCon->PSSetSamplers(0, 1, &g_d3d11->m_currentTextures[0]->m_samplerState);
			}
			break;
		}
		g_d3d11->m_d3d11DevCon->RSSetState(g_d3d11->m_RasterizerSolidNoBackFaceCulling);
	}
	u32 offset = 0u;
//	g_d3d11->m_d3d11DevCon->RSSetState(g_d3d11->m_RasterizerWireframeNoBackFaceCulling);
	g_d3d11->m_d3d11DevCon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	g_d3d11->m_d3d11DevCon->IASetVertexBuffers(0, 1, &g_d3d11->m_currentModel->m_vBuffer, &g_d3d11->m_currentModel->m_stride, &offset);
	g_d3d11->m_d3d11DevCon->IASetIndexBuffer(g_d3d11->m_currentModel->m_iBuffer, g_d3d11->m_currentModel->m_indexType, 0);
	g_d3d11->m_d3d11DevCon->DrawIndexed(g_d3d11->m_currentModel->m_iCount, 0, 0);
}
void DrawSprite(yySprite* sprite)
{
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

	g_d3d11->SetShader(g_d3d11->m_shaderSprite);
	g_d3d11->m_d3d11DevCon->VSSetConstantBuffers(0, 1, &g_d3d11->m_shaderSprite->m_cb);


	if(sprite->m_texture)
	{
		SetTexture(yyVideoDriverAPI::TextureSlot::Texture0, sprite->m_texture);
		g_d3d11->m_d3d11DevCon->PSSetShaderResources(0, 1, &g_d3d11->m_currentTextures[0]->m_textureResView);
		g_d3d11->m_d3d11DevCon->PSSetSamplers(0, 1, &g_d3d11->m_currentTextures[0]->m_samplerState);
	}
	SetModel(sprite->m_model);
	u32 offset = 0u;
	g_d3d11->m_d3d11DevCon->IASetVertexBuffers(0, 1, &g_d3d11->m_currentModel->m_vBuffer, &g_d3d11->m_currentModel->m_stride, &offset);
	g_d3d11->m_d3d11DevCon->IASetIndexBuffer(g_d3d11->m_currentModel->m_iBuffer, g_d3d11->m_currentModel->m_indexType, 0);
	g_d3d11->m_d3d11DevCon->DrawIndexed(g_d3d11->m_currentModel->m_iCount, 0, 0);
}
void DrawLine2D(const v3f& _p1, const v3f& _p2, const yyColor& color)
{
	v4f p1 = _p1;
	v4f p2 = _p2;
	g_d3d11->m_d3d11DevCon->IASetInputLayout(NULL);
	g_d3d11->m_d3d11DevCon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	g_d3d11->SetShader(g_d3d11->m_shaderLine3D);
	g_d3d11->m_shaderLine3D->SetData(p1, p2, color, g_d3d11->m_guiProjectionMatrix);
	g_d3d11->m_shaderLine3D->SetConstants(0);

	g_d3d11->m_d3d11DevCon->Draw(2, 0);
}
void DrawLine3D(const v4f& p1, const v4f& p2, const yyColor& color)
{
	g_d3d11->m_d3d11DevCon->IASetInputLayout(NULL);
	g_d3d11->m_d3d11DevCon->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	g_d3d11->SetShader(g_d3d11->m_shaderLine3D);
	g_d3d11->m_shaderLine3D->SetData(p1, p2, color, g_d3d11->m_matrixViewProjection);
	g_d3d11->m_shaderLine3D->SetConstants(0);
	
	g_d3d11->m_d3d11DevCon->Draw(2, 0);
}
void SetMatrix(yyVideoDriverAPI::MatrixType mt, const Mat4& mat)
{
	switch(mt)
	{
	case yyVideoDriverAPI::World:
		g_d3d11->m_matrixWorld = mat;
		break;
	case yyVideoDriverAPI::View:
		g_d3d11->m_matrixView = mat;
		break;
	case yyVideoDriverAPI::Projection:
		g_d3d11->m_matrixProjection = mat;
		break;
	case yyVideoDriverAPI::ViewProjection:
		g_d3d11->m_matrixViewProjection = mat;
		break;
	case yyVideoDriverAPI::WorldViewProjection:
		g_d3d11->m_matrixWorldViewProjection = mat;
		break;
	default:
		YY_PRINT_FAILED;
		break;
	}
}
void SetBoneMatrix(u32 boneIndex, const Mat4& mat)
{
	g_d3d11->m_matrixBones[boneIndex] = mat;
}
v2f* GetSpriteCameraPosition()
{
	return &g_d3d11->m_spriteCameraPosition;
}
v2f* GetSpriteCameraScale()
{
	return &g_d3d11->m_spriteCameraScale;
}

void GetTextureSize(yyResource* r, v2i* s)
{
	assert(r);
	assert(s);
	if(r->m_type != yyResourceType::Texture)
		return;
	D3D11Texture* t = g_d3d11->m_textures[ r->m_index ];
	if(!t)
		return;
	s->x = t->m_w;
	s->y = t->m_h;
}

void SetMaterial(yyMaterial* mat)
{
	g_d3d11->m_currentMaterial = mat;
}
void MapModelForWriteVerts(yyResource* r, u8** v_ptr)
{
	assert(r);
	D3D11Model* m = g_d3d11->m_models[r->m_index];
	ID3D11Buffer* d3dbuffer = nullptr;
	d3dbuffer = m->m_vBuffer;

	static D3D11_MAPPED_SUBRESOURCE mapData;
	auto hr = g_d3d11->m_d3d11DevCon->Map(
		d3dbuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapData
	);
	if (FAILED(hr)) 
	{
		yyLogWriteError("Can not lock D3D11 render model buffer. Code : %u\n", hr);
		return;
	}
	*v_ptr = (u8*)mapData.pData;
	m->m_lockedResource = d3dbuffer;
}
void UnmapModelForWriteVerts(yyResource* r)
{
	D3D11Model* m = g_d3d11->m_models[r->m_index];
	g_d3d11->m_d3d11DevCon->Unmap(m->m_lockedResource, 0);
	m->m_lockedResource = nullptr;
}

yyVideoDriverObjectD3D11 g_yyVideoDriverObject;
void* GetVideoDriverObjects()
{
	g_yyVideoDriverObject.m_context = g_d3d11->m_d3d11DevCon;
	g_yyVideoDriverObject.m_device = g_d3d11->m_d3d11Device;
	return &g_yyVideoDriverObject;
}

const char* GetVideoDriverName()
{
	return "Direct3D 11";
}
void ClearAll()
{
	g_d3d11->m_d3d11DevCon->ClearRenderTargetView(g_d3d11->m_currentTargetView, g_d3d11->m_clearColor.data());
	g_d3d11->m_d3d11DevCon->ClearDepthStencilView(g_d3d11->m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}
void ClearColor()
{
	g_d3d11->m_d3d11DevCon->ClearRenderTargetView(g_d3d11->m_currentTargetView, g_d3d11->m_clearColor.data());
}
void ClearDepth()
{
	g_d3d11->m_d3d11DevCon->ClearDepthStencilView(g_d3d11->m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}
void BeginDraw()
{
	 // !!! ??? m_depthStencilView должен быть свой на каждый RTT ???
	g_d3d11->m_d3d11DevCon->OMSetRenderTargets(1, &g_d3d11->m_mainTarget->m_RTV, g_d3d11->m_depthStencilView);
	SetViewport(0, 0, g_d3d11->m_mainTargetSize.x, g_d3d11->m_mainTargetSize.y);
	g_d3d11->m_currentTargetView = g_d3d11->m_mainTarget->m_RTV;

	UseDepth(true);
}
void EndDraw()
{
	g_d3d11->m_d3d11DevCon->OMSetRenderTargets(1, &g_d3d11->m_MainTargetView, g_d3d11->m_depthStencilView);
	g_d3d11->m_currentTargetView = g_d3d11->m_MainTargetView;
	ClearColor();
	SetViewport(0, 0, g_d3d11->m_swapChainSize.x, g_d3d11->m_swapChainSize.y);

	UseDepth(false);

	g_d3d11->m_d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
//	const float blend_factor[4] = { 0.f, 0.f, 0.f, 1.f };
//	g_d3d11->m_d3d11DevCon->OMSetBlendState(g_d3d11->m_blendStateAlphaEnabled, blend_factor, 0xffffffff);
	g_d3d11->m_d3d11DevCon->OMSetDepthStencilState(g_d3d11->m_depthStencilStateDisabled, 0);

	g_d3d11->m_d3d11DevCon->RSSetState(g_d3d11->m_RasterizerSolidNoBackFaceCulling);
	g_d3d11->SetShader(g_d3d11->m_shaderScreenQuad);

	g_d3d11->m_d3d11DevCon->PSSetShaderResources(0, 1, &g_d3d11->m_mainTarget->m_textureResView);
	g_d3d11->m_d3d11DevCon->PSSetSamplers(0, 1, &g_d3d11->m_mainTarget->m_samplerState);
	u32 offset = 0u;
	g_d3d11->m_d3d11DevCon->IASetVertexBuffers(0, 1, &g_d3d11->m_mainTargetSurface->m_vBuffer, &g_d3d11->m_mainTargetSurface->m_stride, &offset);
	g_d3d11->m_d3d11DevCon->IASetIndexBuffer(g_d3d11->m_mainTargetSurface->m_iBuffer, g_d3d11->m_mainTargetSurface->m_indexType, 0);
	g_d3d11->m_d3d11DevCon->DrawIndexed(g_d3d11->m_mainTargetSurface->m_iCount, 0, 0);

	
}
void UpdateMainRenderTarget(const v2i& windowsSize, const v2f& bufferSize)
{
	g_d3d11->m_windowSize = windowsSize;
	g_d3d11->m_mainTargetSize = bufferSize;
	g_d3d11->updateMainTarget();
}

yyResource* CreateRenderTargetTexture(const v2f& size, bool useLinearFilter, bool useComparisonFilter)
{
	yyResource * newRes = yyCreate<yyResource>();
	newRes->m_type = yyResourceType::RenderTargetTexture;
	newRes->m_source = 0;
	newRes->m_index = g_d3d11->m_textures.size();
	newRes->m_refCount = 1;
	if (useLinearFilter)
		newRes->m_flags |= yyResource::flags::texture_useLinearFilter;
	if (useComparisonFilter)
		newRes->m_flags |= yyResource::flags::texture_useComparisonFilter;

	auto newTexture = yyCreate<D3D11Texture>();
	g_d3d11->initRTT(newTexture, size, useLinearFilter, useComparisonFilter);
	if (newTexture)
	{
		g_d3d11->m_textures.push_back(newTexture);
		newRes->m_isLoaded = true;
		return newRes;
	}

	if (newRes)
		yyDestroy(newRes);
	return nullptr;
}
void SetRenderTarget(yyResource* rtt)
{
	if (rtt)
	{
		if (rtt->m_type == yyResourceType::RenderTargetTexture)
		{
			g_d3d11->m_d3d11DevCon->OMSetRenderTargets(1, &g_d3d11->m_textures[rtt->m_index]->m_RTV, g_d3d11->m_depthStencilView);
			g_d3d11->m_currentTargetView = g_d3d11->m_textures[rtt->m_index]->m_RTV;
		}
		else
			yyLogWriteWarning("SetRenderTarget: yyResourceType is not RenderTargetTexture");
	}
	else
	{
		g_d3d11->m_d3d11DevCon->OMSetRenderTargets(1, &g_d3d11->m_mainTarget->m_RTV, g_d3d11->m_depthStencilView);
		g_d3d11->m_currentTargetView = g_d3d11->m_mainTarget->m_RTV;
	}
}
void SetViewport(f32 x, f32 y, f32 width, f32 height)
{
	D3D11_VIEWPORT viewport;
	viewport.Width = (f32)width;
	viewport.Height = (f32)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = x;
	viewport.TopLeftY = y;
	g_d3d11->m_d3d11DevCon->RSSetViewports(1, &viewport);
}
void SwapBuffers()
{
	g_d3d11->m_vsync ? g_d3d11->m_SwapChain->Present(1, 0)
		: g_d3d11->m_SwapChain->Present(0, 0);
}
void DeleteModel(yyResource* r)
{
	assert(r);
	assert(r->m_type == yyResourceType::Model);
	yyDestroy(g_d3d11->m_models[r->m_index]);
	g_d3d11->m_models[r->m_index] = nullptr;
	// чтобы не переполнять g_d3d11->m_models при создании новых ресурсов,
	// лучше вставлять их в свободные ячейки.
	// чтобы не перебирать их, наверно, лучше иметь список свободных ячеек
	// соответственно, при создании ресурса, перед указанием индекса, нужно проверить этот список
	g_d3d11->m_freeModelResourceIndex.push_back(r->m_index);
	yyDestroy(r); // удаление ресурса
}
void DeleteTexture(yyResource* r)
{
	yyDestroy(g_d3d11->m_textures[r->m_index]);
	g_d3d11->m_textures[r->m_index] = nullptr;
	g_d3d11->m_freeTextureResourceIndex.push_back(r->m_index);
	yyDestroy(r);
}
void* GetTextureHandle(yyResource* res)
{
	return g_d3d11->m_textures[res->m_index]->m_textureResView;
}

void SetGUIShaderData(yyGUIElement* guielement)
{
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

extern "C"
{
	YY_API yyVideoDriverAPI* YY_C_DECL GetAPI()
	{
		g_api.BeginDraw = BeginDraw;
		g_api.BeginDrawGUI = BeginDrawGUI;
		g_api.ClearAll = ClearAll;
		g_api.ClearColor = ClearColor;
		g_api.ClearDepth = ClearDepth;
		g_api.CreateModel = CreateModel;
		g_api.CreateModelFromFile = CreateModelFromFile;
		g_api.CreateRenderTargetTexture = CreateRenderTargetTexture;
		g_api.CreateTexture = CreateTexture;
		g_api.CreateTextureFromFile = CreateTextureFromFile;
		g_api.DeleteModel = DeleteModel;
		g_api.DeleteTexture = DeleteTexture;
		g_api.Destroy       = Destroy;
		g_api.Draw = Draw;
		g_api.DrawLine2D = DrawLine2D;
		g_api.DrawLine3D = DrawLine3D;
		g_api.DrawSprite = DrawSprite;
		g_api.EndDraw = EndDraw;
		g_api.EndDrawGUI = EndDrawGUI;
		g_api.GetAPIVersion = GetAPIVersion;
		g_api.GetSpriteCameraPosition = GetSpriteCameraPosition;
		g_api.GetSpriteCameraScale = GetSpriteCameraScale;
		g_api.GetTextureHandle = GetTextureHandle;
		g_api.GetTextureSize = GetTextureSize;
		g_api.GetVideoDriverName = GetVideoDriverName;
		g_api.GetVideoDriverObjects = GetVideoDriverObjects;
		g_api.Init          = Init;
		g_api.LoadModel = LoadModel;
		g_api.LoadTexture = LoadTexture;
		g_api.MapModelForWriteVerts = MapModelForWriteVerts;
		g_api.SetBoneMatrix = SetBoneMatrix;
		g_api.SetClearColor = SetClearColor;
		g_api.SetGUIShaderData = SetGUIShaderData;
		g_api.SetMaterial = SetMaterial;
		g_api.SetMatrix = SetMatrix;
		g_api.SetModel = SetModel;
		g_api.SetRenderTarget = SetRenderTarget;
		g_api.SetTexture = SetTexture;
		g_api.SetViewport = SetViewport;
		g_api.SwapBuffers = SwapBuffers;
		g_api.UnloadModel = UnloadModel;
		g_api.UnloadTexture = UnloadTexture;
		g_api.UnmapModelForWriteVerts = UnmapModelForWriteVerts;
		g_api.UpdateMainRenderTarget = UpdateMainRenderTarget;
		g_api.UseBlend = UseBlend;
		g_api.UseDepth = UseDepth;
		g_api.UseVSync = UseVSync;

		g_api.test_draw = 0;

		return &g_api;
	}
}
