#include "yy.h"
#include "vid_d3d11.h"

#include "D3D11_shader.h"
#include "D3D11_shader_GUI.h"

#include "math/mat.h"
extern Mat4 g_guiProjectionMatrix;

D3D11ShaderGUI::D3D11ShaderGUI()
	:
	m_cbVertex(0),
	m_cbPixel(0)
{
}

D3D11ShaderGUI::~D3D11ShaderGUI()
{
	if (m_cbVertex) m_cbVertex->Release();
	if (m_cbPixel) m_cbPixel->Release();
}

bool D3D11ShaderGUI::init()
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
		"	float2 Offset;\n"
		"	float2 Padding;\n"
		"};\n"
		"cbuffer cbPixel{\n"
		"	float4 Color;\n"
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
		"	output.pos   = mul(ProjMtx, float4(input.position.x + Offset.x, input.position.y + Offset.y, 0.0f, 1.f));\n"
		"	output.uv    = input.uv;\n"
		"	return output;\n"
		"}\n"
		"PSOut PSMain(VSOut input){\n" 
		"    PSOut output;\n"
		"    output.color = tex2d_1.Sample(tex2D_sampler_1, input.uv) * Color;\n"
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