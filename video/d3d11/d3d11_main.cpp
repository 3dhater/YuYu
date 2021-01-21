#define YY_EXPORTS
#include "yy.h"
#include "yy_color.h"
#include "yy_window.h"
#include "yy_image.h"
#include "yy_material.h"
#include "yy_ptr.h"
#include "yy_model.h"

#include "vid_d3d11.h"
#include "d3d11_texture.h"
#include "d3d11_model.h"
#include "d3d11_shader_GUI.h"
#include "d3d11_shader_sprite.h"
#include "d3d11_shader_Line3D.h"
#include "d3d11_shader_standart.h"

#include "scene/common.h"
#include "scene/sprite.h"

yyVideoDriverAPI g_api;

void LoadModel(yyResource* r);

//yyResource g_defaultRes;

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

// `if` is really expensive instruction...
void BeginDrawClearAll()
{
	g_d3d11->m_d3d11DevCon->ClearRenderTargetView(g_d3d11->m_MainTargetView, g_d3d11->m_clearColor.data());
	g_d3d11->m_d3d11DevCon->ClearDepthStencilView(g_d3d11->m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}
void BeginDrawClearColor()
{
	g_d3d11->m_d3d11DevCon->ClearRenderTargetView(g_d3d11->m_MainTargetView, g_d3d11->m_clearColor.data());
}
void BeginDrawClearDepth()
{
	g_d3d11->m_d3d11DevCon->ClearDepthStencilView(g_d3d11->m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}
void BeginDrawNotClear()
{
}
void EndDraw()
{
	g_d3d11->m_vsync ? g_d3d11->m_SwapChain->Present(1, 0) 
		: g_d3d11->m_SwapChain->Present(0, 0);
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
#ifdef YY_DEBUG
	if(r->m_type != yyResourceType::Texture)
	{
		YY_PRINT_FAILED;
	}
#endif
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
#ifdef YY_DEBUG
	if(r->m_type != yyResourceType::Texture)
	{
		YY_PRINT_FAILED;
	}
#endif
	++r->m_refCount;
	if(r->m_refCount == 1)
	{
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
	newRes->m_index = g_d3d11->m_textures.size();
	newRes->m_refCount = 0;
	if(useLinearFilter)
		newRes->m_flags |= yyResource::flags::texture_useLinearFilter;
	if (useComparisonFilter)
		newRes->m_flags |= yyResource::flags::texture_useComparisonFilter;
	newRes->m_file = fileName;

	g_d3d11->m_textures.push_back(nullptr);

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
	newRes->m_index = g_d3d11->m_textures.size();
	newRes->m_refCount = 1;
	if(useLinearFilter)
		newRes->m_flags |= yyResource::flags::texture_useLinearFilter;

	auto newTexture = CreateD3D11Texture(image, useLinearFilter, useComparisonFilter);
	if(newTexture)
	{
		g_d3d11->m_textures.push_back(newTexture);
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


yyResource* CreateModelFromFile(const char* fileName, bool load)
{
	assert(fileName);
	yyResource * newRes = yyCreate<yyResource>();
	newRes->m_type = yyResourceType::Model;
	newRes->m_source = nullptr;
	newRes->m_index = g_d3d11->m_models.size();
	newRes->m_refCount = 0;
	newRes->m_file = fileName;

	g_d3d11->m_models.push_back(nullptr);

	if(load)
		LoadModel(newRes);
	return newRes;
}
yyResource* CreateModel(yyModel* model)
{
	assert(model);
	yyResource * newRes = yyCreate<yyResource>();
	newRes->m_type = yyResourceType::Model;
	newRes->m_index = g_d3d11->m_models.size();
	newRes->m_source = model;
	newRes->m_refCount = 1;

	auto newModel = CreateD3D11Model(model);
	if(newModel)
	{
		g_d3d11->m_models.push_back(newModel);
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


	D3D11_MAPPED_SUBRESOURCE mappedResource;
	g_d3d11->m_d3d11DevCon->Map(g_d3d11->m_shaderGUI->m_cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	D3D11_BUFFER_DESC d;
	g_d3d11->m_shaderGUI->m_cb->GetDesc(&d);
	memcpy(mappedResource.pData, g_d3d11->m_guiProjectionMatrix.getPtr(), d.ByteWidth);
	g_d3d11->m_d3d11DevCon->Unmap(g_d3d11->m_shaderGUI->m_cb, 0);

	g_d3d11->m_d3d11DevCon->IASetInputLayout(g_d3d11->m_shaderGUI->m_vLayout);
	g_d3d11->m_d3d11DevCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	const float blend_factor[4] = { 1.f, 1.f, 1.f, 1.f };
	g_d3d11->m_d3d11DevCon->OMSetBlendState(g_d3d11->m_blendStateAlphaEnabled, blend_factor, 0xffffffff);
	g_d3d11->m_d3d11DevCon->OMSetDepthStencilState(g_d3d11->m_depthStencilStateDisabled, 0);
	g_d3d11->m_d3d11DevCon->RSSetState(g_d3d11->m_RasterizerSolidNoBackFaceCulling);
	g_d3d11->m_d3d11DevCon->VSSetShader(g_d3d11->m_shaderGUI->m_vShader, 0, 0);
	g_d3d11->m_d3d11DevCon->PSSetShader(g_d3d11->m_shaderGUI->m_pShader, 0, 0);
	g_d3d11->m_d3d11DevCon->VSSetConstantBuffers(0, 1, &g_d3d11->m_shaderGUI->m_cb);

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
		if (g_d3d11->m_currentMaterial)
		{
			if (g_d3d11->m_currentMaterial->m_wireframe)
			{
				if (g_d3d11->m_currentMaterial->m_cullBackFace)
					g_d3d11->m_d3d11DevCon->RSSetState(g_d3d11->m_RasterizerWireframe);
				else
					g_d3d11->m_d3d11DevCon->RSSetState(g_d3d11->m_RasterizerWireframeNoBackFaceCulling);
			}
			else
			{
				if (g_d3d11->m_currentMaterial->m_cullBackFace)
					g_d3d11->m_d3d11DevCon->RSSetState(g_d3d11->m_RasterizerSolid);
				else
					g_d3d11->m_d3d11DevCon->RSSetState(g_d3d11->m_RasterizerSolidNoBackFaceCulling);
			}
		}
		else
		{
			g_d3d11->m_d3d11DevCon->RSSetState(g_d3d11->m_RasterizerSolidNoBackFaceCulling);
		}
		//glUseProgram( g_openGL->m_shader_std->m_program );
		//glUniformMatrix4fv(g_openGL->m_shader_std->m_uniform_WVP, 1, GL_FALSE, g_openGL->m_matrixWorldViewProjection.getPtr() );
		/*if(g_openGL->m_currentTextures[0])
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D,g_openGL->m_currentTextures[0]->m_texture);
		}*/
	}
	u32 offset = 0u;
	g_d3d11->m_d3d11DevCon->IASetVertexBuffers(0, 1, &g_d3d11->m_currentModel->m_vBuffer, &g_d3d11->m_currentModel->m_stride, &offset);
	g_d3d11->m_d3d11DevCon->IASetIndexBuffer(g_d3d11->m_currentModel->m_iBuffer, g_d3d11->m_currentModel->m_indexType, 0);
	g_d3d11->m_d3d11DevCon->DrawIndexed(g_d3d11->m_currentModel->m_iCount, 0, 0);
}
void DrawSprite(yySprite* sprite)
{
	/*assert(sprite);
	glUseProgram( g_openGL->m_shader_sprite->m_program );
	glUniformMatrix4fv(g_openGL->m_shader_sprite->m_uniform_ProjMtx, 1, GL_FALSE, g_openGL->m_guiProjectionMatrix.getPtr() );
	glUniformMatrix4fv(g_openGL->m_shader_sprite->m_uniform_WorldMtx, 1, GL_FALSE, sprite->m_objectBase.m_globalMatrix.getPtr() );
	glUniform2fv(g_openGL->m_shader_sprite->m_uniform_CameraPosition, 1, &g_openGL->m_spriteCameraPosition.x);
	glUniform2fv(g_openGL->m_shader_sprite->m_uniform_CameraScale, 1, &g_openGL->m_spriteCameraScale.x);
	glUniform2fv(g_openGL->m_shader_sprite->m_uniform_uv1, 1, &sprite->m_tcoords_1.x);
	glUniform2fv(g_openGL->m_shader_sprite->m_uniform_uv2, 1, &sprite->m_tcoords_2.x);

	int flags = 0;
	if(sprite->m_currentState)
	{
		if(sprite->m_currentState->m_invertX)
			flags |= 1;
		if(sprite->m_currentState->m_invertY)
			flags |= 2;
	}
	
	glUniform1i(g_openGL->m_shader_sprite->m_uniform_flags, flags);

	if(sprite->m_texture)
	{
		SetTexture(yyVideoDriverAPI::TextureSlot::Texture0, sprite->m_texture);
		gglActiveTexture(GL_TEXTURE0);
		gglBindTexture(GL_TEXTURE_2D,g_openGL->m_currentTextures[0]->m_texture);
	}
	SetModel(sprite->m_model);
	auto meshBuffer = g_openGL->m_currentModel->m_meshBuffers[0];
	gglBindVertexArray(meshBuffer->m_VAO);
	gglDrawElements(GL_TRIANGLES, meshBuffer->m_iCount, GL_UNSIGNED_SHORT, 0);*/
}
void DrawLine3D(const v4f& _p1, const v4f& _p2, const yyColor& color)
{
	/*glUseProgram( g_openGL->m_shader_line3d->m_program );
	glUniformMatrix4fv(g_openGL->m_shader_line3d->m_uniform_ProjMtx, 1, GL_FALSE, g_openGL->m_matrixViewProjection.getPtr() );
	glUniform4fv(g_openGL->m_shader_line3d->m_uniform_P1, 1, _p1.cdata());
	glUniform4fv(g_openGL->m_shader_line3d->m_uniform_P2, 1, _p2.cdata());
	glUniform4fv(g_openGL->m_shader_line3d->m_uniform_Color, 1, color.data());

	glBindVertexArray(g_openGL->m_shader_line3d->m_VAO);
	glDrawArrays(GL_LINES, 0, 2);*/
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

extern "C"
{
	YY_API yyVideoDriverAPI* YY_C_DECL GetAPI()
	{
		g_api.GetAPIVersion = GetAPIVersion;
		g_api.Init          = Init;
		g_api.Destroy       = Destroy;
	
		g_api.SetClearColor = SetClearColor;
		//g_api.BeginDraw		= BeginDraw;
		g_api.BeginDrawClearAll	= BeginDrawClearAll;
		g_api.BeginDrawClearColor	= BeginDrawClearColor;
		g_api.BeginDrawClearDepth	= BeginDrawClearDepth;
		g_api.BeginDrawNotClear	= BeginDrawNotClear;
		g_api.EndDraw		= EndDraw;

		g_api.CreateTexture = CreateTexture;
		g_api.CreateTextureFromFile = CreateTextureFromFile;
		g_api.UnloadTexture = UnloadTexture;
		g_api.LoadTexture = LoadTexture;

		g_api.UseVSync = UseVSync;
		g_api.UseDepth = UseDepth;

		g_api.CreateModel = CreateModel;
		g_api.CreateModelFromFile = CreateModelFromFile;
		g_api.LoadModel = LoadModel;
		g_api.UnloadModel = UnloadModel;

		g_api.BeginDrawGUI = BeginDrawGUI;
		g_api.EndDrawGUI = EndDrawGUI;
		g_api.SetTexture = SetTexture;
		g_api.SetModel = SetModel;
		
		g_api.Draw = Draw;
		g_api.DrawSprite = DrawSprite;
		g_api.DrawLine3D = DrawLine3D;
		g_api.SetMatrix = SetMatrix;
		g_api.GetSpriteCameraPosition = GetSpriteCameraPosition;
		g_api.GetSpriteCameraScale = GetSpriteCameraScale;

		g_api.GetTextureSize = GetTextureSize;

		g_api.SetMaterial = SetMaterial;
		g_api.MapModelForWriteVerts = MapModelForWriteVerts;
		g_api.UnmapModelForWriteVerts = UnmapModelForWriteVerts;
		g_api.GetVideoDriverObjects = GetVideoDriverObjects;
		
		g_api.test_draw = 0;

		g_api.GetVideoDriverName = GetVideoDriverName;

		return &g_api;
	}
}
