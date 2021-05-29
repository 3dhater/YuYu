#include "yy.h"

#include "yy_video.h"

#include "vid_d3d11.h"
#include "d3d11_shader.h"
#include "d3d11_shader_standart.h"

#include "math/mat.h"

extern D3D11 * g_d3d11;

D3D11ShaderStd::D3D11ShaderStd(){
	m_cbVertex = 0;
	m_cbPixel = 0;
}

D3D11ShaderStd::~D3D11ShaderStd(){
	if (m_cbVertex)m_cbVertex->Release();
	if (m_cbPixel)m_cbPixel->Release();
}

bool D3D11ShaderStd::init() {
	const char * text =
		"Texture2D tex2d_1;\n"
		"SamplerState tex2D_sampler_1;\n"
		"struct VSIn{\n"
		"   float3 position : POSITION;\n"
		"	float2 uv : TEXCOORD;\n"
		"   float3 normal : NORMAL;\n"
		"   float3 binormal : BINORMAL;\n"
		"   float3 tangent : TANGENT;\n"
		"   float4 color : COLOR;\n"
		"};\n"
		"cbuffer cbVertex{\n"
		"	float4x4 WVP;\n"
		"	float4x4 W;\n"
		"};\n"
		"cbuffer cbPixel{\n"
		"	float4 BaseColor;\n"
		"	float4 SunPosition;\n"
		"};\n"
		"struct VSOut{\n"
		"   float4 pos : SV_POSITION;\n"
		"	float2 uv : TEXCOORD0;\n"
		"	float4 vColor : COLOR0;\n"
		"	float3 normal : NORMAL0;\n"
		"	float4 fragPos : NORMAL1;\n"
		"};\n"
		"struct PSOut{\n"
		"    float4 color : SV_Target;\n"
		"};\n"
		"VSOut VSMain(VSIn input){\n"
		"   VSOut output;\n"
		"	output.pos   = mul(WVP, float4(input.position.x, input.position.y, input.position.z, 1.f));\n"
		"	output.uv.x    = input.uv.x;\n"
		"	output.uv.y    = input.uv.y;\n"
		"	output.vColor    = input.color;\n"
		"	output.normal    = mul((float3x3)W, input.normal);\n"
		"	output.fragPos    = mul(WVP, float4(input.position.x, input.position.y, input.position.z, 1.f));\n"
		"	return output;\n"
		"}\n"
		"PSOut PSMain(VSOut input){\n"
		"	float3 lightDir = normalize(SunPosition.xyz - input.fragPos.xyz);\n"
		"	float diff = max(dot(input.normal, lightDir), 0.0);\n"

		"    PSOut output;\n"
		"    output.color = tex2d_1.Sample(tex2D_sampler_1, input.uv) * BaseColor;\n"
		"	output.color.xyz = lerp(output.color.xyz, input.vColor.xyz, input.vColor.www);\n"
		"	output.color.xyz *= diff;\n"

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

	if (!D3D11_createConstantBuffer(sizeof(cbVertex), &m_cbVertex))
	{
		YY_PRINT_FAILED;
		return false;
	}

	if (!D3D11_createConstantBuffer(sizeof(cbPixel), &m_cbPixel))
	{
		YY_PRINT_FAILED;
		return false;
	}

	return true;
}

void D3D11ShaderStd::SetConstants(yyMaterial* material) {
	m_cbVertexData.WVP = *yyGetMatrix(yyMatrixType::WorldViewProjection);
	m_cbVertexData.W = *yyGetMatrix(yyMatrixType::World);
	m_cbPixelData.BaseColor = material->m_baseColor;
	m_cbPixelData.SunPosition = material->m_sunPos;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	D3D11_BUFFER_DESC d;
	g_d3d11->m_d3d11DevCon->Map(m_cbVertex, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	m_cbVertex->GetDesc(&d);
	memcpy(mappedResource.pData, &m_cbVertexData, d.ByteWidth);
	g_d3d11->m_d3d11DevCon->Unmap(m_cbVertex, 0);
	g_d3d11->m_d3d11DevCon->VSSetConstantBuffers(0, 1, &m_cbVertex);

	g_d3d11->m_d3d11DevCon->Map(m_cbPixel, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	m_cbPixel->GetDesc(&d);
	memcpy(mappedResource.pData, &m_cbPixelData, d.ByteWidth);
	g_d3d11->m_d3d11DevCon->Unmap(m_cbPixel, 0);
	g_d3d11->m_d3d11DevCon->PSSetConstantBuffers(0, 1, &m_cbPixel);
}