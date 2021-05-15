#include "yy.h"

#include "yy_video.h"
#include "yy_material.h"

#include "vid_d3d11.h"
#include "d3d11_shader.h"
#include "d3d11_shader_points.h"

#include "math/mat.h"

extern D3D11 * g_d3d11;

D3D11ShaderPoints::D3D11ShaderPoints(){
	m_cbVertex = 0;
	m_gShader = 0;
}

D3D11ShaderPoints::~D3D11ShaderPoints(){
	if (m_cbVertex)m_cbVertex->Release();
}

bool D3D11ShaderPoints::init(){
	const char * text =
		"struct VSIn{\n"
		"   float3 position : POSITION;\n"
		"   float4 color : COLOR;\n"
		"};\n"
		"cbuffer cbVertex{\n"
		"	float4x4 WVP;\n"
		"	float4 Eye;\n"
		"	float2 Viewport;\n"
		"	float2 Padding;\n"
		"};\n"
		"struct VSOut{\n"
		"   float4 pos : SV_POSITION;\n"
		"	float4 vColor : COLOR0;\n"
		"};\n"
		"struct PSOut{\n"
		"    float4 color : SV_Target;\n"
		"};\n"
		"VSOut VSMain(VSIn input){\n"
		"   VSOut output;\n"
		"	output.vColor    = input.color;\n"
		"	output.vColor.w    = 1.f;\n"
		

		"	output.pos = mul(WVP, float4(input.position,1.f));\n"
		"	output.pos.z    -= 0.0001f;\n"
		"	output.pos = output.pos / output.pos.w;\n"

		"	return output;\n"
		"}\n"
		"PSOut PSMain(VSOut input){\n"
		"   PSOut output;\n"
		"	output.color = input.vColor;\n"
		"   return output;\n"
		"}\n"
		"[maxvertexcount(4)]\n"
		"void GSMain(point VSOut input[1], inout TriangleStream<VSOut> TriStream ){\n"
		"	VSOut Out;\n"

		"	const float sz = 5.f;\n"
		"	float h = 2.f / Viewport.y;\n"
		"	float w = 2.f / Viewport.x;\n"
		"	float x = sz * w;\n"
		"	float y = sz * h;\n"
		"	float half_x = x * 0.5f;\n"
		"	float half_y = y * 0.5f;\n"
		"	const float4 sizes[4] = {\n"
		"		float4(-half_x, -half_y, 0, 0),\n"
		"		float4(-half_x, half_y, 0, 0),\n"
		"		float4(half_x, -half_y, 0, 0),\n"
		"		float4(half_x, half_y, 0, 0)\n"
		"	};\n"
		"	for (uint i = 0; i<4; ++i)\n"
		"	{\n"
		"		Out.pos =input[0].pos;\n"
		"		Out.pos += sizes[i];\n"
		"		Out.vColor = input[0].vColor;\n"
		"		TriStream.Append(Out);\n"
		"	}\n"

		"	TriStream.RestartStrip();\n"
		"}\n";
	if (!D3D11_createShaders(
		"vs_4_0",
		"ps_4_0",
		text,
		text,
		"VSMain",
		"PSMain",
		yyVertexType::Point,
		&this->m_vShader,
		&this->m_pShader,
		&this->m_vLayout))
	{
		YY_PRINT_FAILED;
		return false;
	}

	if (!D3D11_createGeometryShaders("gs_4_0", text, "GSMain", &m_gShader))
	{
		YY_PRINT_FAILED;
		return false;
	}

	if (!D3D11_createConstantBuffer(sizeof(cbVertex), &m_cbVertex))
	{
		YY_PRINT_FAILED;
		return false;
	}

	return true;
}

void D3D11ShaderPoints::SetConstants(yyMaterial* material){
	m_cbVertexData.WVP = *yyGetMatrix(yyMatrixType::WorldViewProjection);
	m_cbVertexData.Eye = *yyGetEyePosition();
	m_cbVertexData.Viewport = g_d3d11->m_viewportSize;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	D3D11_BUFFER_DESC d;
	g_d3d11->m_d3d11DevCon->Map(m_cbVertex, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	m_cbVertex->GetDesc(&d);
	memcpy(mappedResource.pData, &m_cbVertexData, d.ByteWidth);
	g_d3d11->m_d3d11DevCon->Unmap(m_cbVertex, 0);
	g_d3d11->m_d3d11DevCon->VSSetConstantBuffers(0, 1, &m_cbVertex);
	g_d3d11->m_d3d11DevCon->GSSetConstantBuffers(0, 1, &m_cbVertex);
}



D3D11ShaderPointsAnimated::D3D11ShaderPointsAnimated() {
	m_cbVertex = 0;
}
D3D11ShaderPointsAnimated::~D3D11ShaderPointsAnimated() {
	if (m_cbVertex)m_cbVertex->Release();
}
void D3D11ShaderPointsAnimated::SetConstants(yyMaterial* material) {
	m_cbVertexData.WVP = *yyGetMatrix(yyMatrixType::WorldViewProjection);
	memcpy(m_cbVertexData.Bones, yyGetBoneMatrix(0)->getPtr(), YY_MAX_BONES * sizeof(Mat4));

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	D3D11_BUFFER_DESC d;

	g_d3d11->m_d3d11DevCon->Map(m_cbVertex, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	m_cbVertex->GetDesc(&d);
	memcpy(mappedResource.pData, &m_cbVertexData, d.ByteWidth);
	g_d3d11->m_d3d11DevCon->Unmap(m_cbVertex, 0);
	g_d3d11->m_d3d11DevCon->VSSetConstantBuffers(0, 1, &m_cbVertex);
}
bool D3D11ShaderPointsAnimated::init() {
	const char * text =
		"struct VSIn{\n"
		"   float3 position : POSITION;\n"
		"   float4 color : COLOR;\n"
		"	float4 weights : WEIGHTS;\n"
		"	uint4 bones : BONES;\n"
		"};\n"
		"cbuffer cbVertex{\n"
		"	float4x4 WVP;\n"
		"	float4x4 Bones[250];\n"
		"};\n"
		"struct VSOut{\n"
		"   float4 pos : SV_POSITION;\n"
		"	float4 vColor : COLOR0;\n"
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
		"	output.vColor    = input.color;\n"
		"	return output;\n"
		"}\n"
		"PSOut PSMain(VSOut input){\n"
		"   PSOut output;\n"
		"	output.color = input.vColor;\n"
		"   return output;\n"
		"}\n";
	if (!D3D11_createShaders(
		"vs_4_0",
		"ps_4_0",
		text,
		text,
		"VSMain",
		"PSMain",
		yyVertexType::AnimatedPoint,
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

	return true;
}