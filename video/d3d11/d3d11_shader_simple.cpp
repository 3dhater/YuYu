﻿#include "yy.h"

#include "yy_video.h"

#include "vid_d3d11.h"
#include "d3d11_shader.h"
#include "d3d11_shader_simple.h"

#include "math/mat.h"

extern D3D11 * g_d3d11;

D3D11ShaderSimple::D3D11ShaderSimple()
{
}

D3D11ShaderSimple::~D3D11ShaderSimple()
{
}

bool D3D11ShaderSimple::init()
{
	const char * text =
		"Texture2D tex2d_1;\n"
		"SamplerState tex2D_sampler_1;\n"
		"struct VSIn{\n"
		"   float3 position : POSITION;\n"
		"	float2 uv : TEXCOORD;\n"
		"   float3 normal : NORMAL;\n"
		"   float3 binormal : BINORMAL;\n"
		"   float3 tangent : TANGENT;\n"
		"};\n"
		"cbuffer cbVertex  : register(b0) {\n"
		"	float4x4 WVP;\n"
		"};\n"
		"struct VSOut{\n"
		"   float4 pos : SV_POSITION;\n"
		"	float2 uv : TEXCOORD0;\n"
		"};\n"
		"struct PSOut{\n"
		"    float4 color : SV_Target;\n"
		"};\n"
		"VSOut VSMain(VSIn input){\n"
		"   VSOut output;\n"
		"	output.pos   = mul(WVP, float4(input.position.x, input.position.y, input.position.z, 1.f));\n"
		"	output.uv.x    = input.uv.x;\n"
		"	output.uv.y    = input.uv.y;\n"
		"	return output;\n"
		"}\n"
		"PSOut PSMain(VSOut input){\n"
		"    PSOut output;\n"
		"    output.color = tex2d_1.Sample(tex2D_sampler_1, input.uv);\n"
		"    return output;\n"
		"}\n";
	if (!D3D11_createShaders(
		"vs_4_0",
		"ps_4_0",
		text,
		text,
		"VSMain",
		"PSMain",
		yyVertexType::Model,
		&this->m_vShader,
		&this->m_pShader,
		&this->m_vLayout))
	{
		YY_PRINT_FAILED;
		return false;
	}

	if (!D3D11_createConstantBuffer(sizeof(cb), &m_cb))
	{
		YY_PRINT_FAILED;
		return false;
	}

	return true;
}

void D3D11ShaderSimple::SetConstants(yyMaterial* material)
{
	m_cbData.WVP = g_d3d11->m_matrixWorldViewProjection;

	g_d3d11->m_d3d11DevCon->VSSetConstantBuffers(0, 1, &m_cb);
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	g_d3d11->m_d3d11DevCon->Map(m_cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	D3D11_BUFFER_DESC d;
	m_cb->GetDesc(&d);
	memcpy(mappedResource.pData, &m_cbData, d.ByteWidth);
	g_d3d11->m_d3d11DevCon->Unmap(m_cb, 0);
}



D3D11ShaderSimpleAnimated::D3D11ShaderSimpleAnimated() {

}
D3D11ShaderSimpleAnimated::~D3D11ShaderSimpleAnimated() {

}
void D3D11ShaderSimpleAnimated::SetConstants(yyMaterial* material) {
	m_cbData.WVP = g_d3d11->m_matrixWorldViewProjection;
	memcpy(m_cbData.Bones, g_d3d11->m_matrixBones[0].getPtr(), 255 * sizeof(Mat4));

	g_d3d11->m_d3d11DevCon->VSSetConstantBuffers(0, 1, &m_cb);
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	g_d3d11->m_d3d11DevCon->Map(m_cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	D3D11_BUFFER_DESC d;
	m_cb->GetDesc(&d);
	memcpy(mappedResource.pData, &m_cbData, d.ByteWidth);
	g_d3d11->m_d3d11DevCon->Unmap(m_cb, 0);
}
bool D3D11ShaderSimpleAnimated::init() {
	const char * text =
		"Texture2D tex2d_1;\n"
		"SamplerState tex2D_sampler_1;\n"
		"struct VSIn{\n"
		"   float3 position : POSITION;\n"
		"	float2 uv : TEXCOORD;\n"
		"   float3 normal : NORMAL;\n"
		"   float3 binormal : BINORMAL;\n"
		"   float3 tangent : TANGENT;\n"
		"	float4 weights : WEIGHTS;\n"
		"	uint4 bones : BONES;\n"
		"};\n"
		"cbuffer cbVertex  : register(b0) {\n"
		"	float4x4 WVP;\n"
		"	float4x4 Bones[255];\n"
		"};\n"
		"struct VSOut{\n"
		"   float4 pos : SV_POSITION;\n"
		"	float2 uv : TEXCOORD0;\n"
		"};\n"
		"struct PSOut{\n"
		"    float4 color : SV_Target;\n"
		"};\n"
		"VSOut VSMain(VSIn input){\n"
		"   VSOut output;\n"
		
		"	float4 inPos = float4(input.position.xyz,1.0f);\n"
		"	float4x4 BoneTransform = mul(Bones[input.bones.x], input.weights.x);\n"
		"	BoneTransform     += mul(Bones[input.bones.y], input.weights.y);\n"
		"	BoneTransform     += mul(Bones[input.bones.z], input.weights.z);\n"
		"	BoneTransform     += mul(Bones[input.bones.w], input.weights.w);\n"
		"	float4 outPos = mul(BoneTransform, inPos);\n"

		"	output.pos   = mul(WVP, outPos);\n"
		"	output.uv.x    = input.uv.x;\n"
		"	output.uv.y    = input.uv.y;\n"
		"	return output;\n"
		"}\n"
		"PSOut PSMain(VSOut input){\n"
		"    PSOut output;\n"
		"    output.color = tex2d_1.Sample(tex2D_sampler_1, input.uv);\n"
		"    return output;\n"
		"}\n";
	if (!D3D11_createShaders(
		"vs_4_0",
		"ps_4_0",
		text,
		text,
		"VSMain",
		"PSMain",
		yyVertexType::AnimatedModel,
		&this->m_vShader,
		&this->m_pShader,
		&this->m_vLayout))
	{
		YY_PRINT_FAILED;
		return false;
	}

	if (!D3D11_createConstantBuffer(sizeof(cb), &m_cb))
	{
		YY_PRINT_FAILED;
		return false;
	}

	return true;
}