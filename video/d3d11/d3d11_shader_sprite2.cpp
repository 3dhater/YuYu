#include "yy.h"

#include "vid_d3d11.h"

#include "d3d11_shader.h"
#include "d3d11_shader_sprite2.h"

#include "math/mat.h"
extern Mat4 g_guiProjectionMatrix;
extern D3D11 * g_d3d11;


D3D11ShaderSprite2::D3D11ShaderSprite2()
	:
	m_cb(0)
{
}

D3D11ShaderSprite2::~D3D11ShaderSprite2()
{
	if (m_cb) m_cb->Release();
}

void D3D11ShaderSprite2::SetConstants(yyMaterial* material)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	g_d3d11->m_d3d11DevCon->Map(m_cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	D3D11_BUFFER_DESC d;
	m_cb->GetDesc(&d);
	memcpy(mappedResource.pData, &m_structCB, d.ByteWidth);
	g_d3d11->m_d3d11DevCon->Unmap(m_cb, 0);
}

bool D3D11ShaderSprite2::init()
{
	const char * text =
		"Texture2D tex2d_1;\n"
		"SamplerState tex2D_sampler_1;\n"
		"struct VSIn{\n"
		"   float2 position : POSITION;\n"
		"	float2 uv : TEXCOORD;\n"
		"};\n"

		"cbuffer cbVertex{\n"
		"	float4x4 ProjMtx;\n"
		"	float4x4 World;\n"
		"	float4 CameraPositionScale;\n"
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
"	output.pos   = mul(World, float4(input.position.x, input.position.y, 0.0f, 1.f));\n"
"	output.pos   = mul(ProjMtx, output.pos);\n"
"   float4 camPos = mul(ProjMtx, float4(CameraPositionScale.x, CameraPositionScale.y, 0.f, 1.f));"

"   output.pos.x -= camPos.x;\n"
"   output.pos.y -= camPos.y;\n"
"   output.pos.x *= CameraPositionScale.z;\n"
"   output.pos.y *= CameraPositionScale.w;\n"
"	output.uv    = input.uv;\n"

"	return output;\n"
"}\n"

"PSOut PSMain(VSOut input){\n"
"    PSOut output;\n"
"    output.color = tex2d_1.Sample(tex2D_sampler_1, input.uv);\n"
"    if(output.color.a < 0.1) discard;\n"
		"    return output;\n"
		"}\n";

		if (!D3D11_createShaders(
			"vs_4_0",
			"ps_4_0",
			text,
			text,
			"VSMain",
			"PSMain",
			yyVertexType::GUI,
			&this->m_vShader,
			&this->m_pShader,
			&this->m_vLayout))
		{
			YY_PRINT_FAILED;
			return false;
		}

		if (!D3D11_createConstantBuffer(sizeof(D3D11ShaderSprite2::cbVertex), &m_cb))
		{
			YY_PRINT_FAILED;
			return false;
		}

	return true;
}