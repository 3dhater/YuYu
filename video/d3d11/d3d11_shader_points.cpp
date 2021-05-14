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
}

D3D11ShaderPoints::~D3D11ShaderPoints(){
	if (m_cbVertex)m_cbVertex->Release();
}

bool D3D11ShaderPoints::init(){
	const char * text =
		"struct VSIn{\n"
		"   float3 position : POSITION;\n"
		"   float3 worldPosition : NORMAL;\n"
		"   float4 color : COLOR;\n"
		"};\n"
		"cbuffer cbVertex{\n"
		"	float4x4 W;\n"
		"	float4x4 V;\n"
		"	float4x4 Vi;\n"
		"	float4x4 P;\n"
		"	float4 Eye;\n"
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
		
		//"	output.pos   = mul(WVP, float4(input.position.x, input.position.y, input.position.z, 1.f));\n"

		/*"	mat4 V2 = V;\n"
		"	V2[3] = vec4(0,0,0,1.f);\n"
		"	mat4 W2 = W * inverse(V2);\n"
		"	W2[3].y = -W2[3].y;\n"
		"	W2[3].xyz += inputWorldPosition;\n"
		"	W2[3].w = 1.f;\n"
		"	gl_Position = (P * V * W2) * vec4(inputPosition.xyz,1.f);\n"*/

		"	float dist = distance(Eye, input.worldPosition);\n"
		//0,0001
		"	float s = dist * 0.01f;\n"
		"	float4x4 S;\n"
		"	S[0].x = s;\n"
		"	S[0].y = 0.f;\n"
		"	S[0].z = 0.f;\n"
		"	S[0].w = 0.f;\n"
		"	S[1].x = 0;\n"
		"	S[1].y = s;\n"
		"	S[1].z = 0.f;\n"
		"	S[1].w = 0.f;\n"
		"	S[2].x = 0;\n"
		"	S[2].y = 0.f;\n"
		"	S[2].z = s;\n"
		"	S[2].w = 0.f;\n"
		"	S[3].x = 0.f;\n"
		"	S[3].y = 0.f;\n"
		"	S[3].z = 0.f;\n"
		"	S[3].w = 1.f;\n"

		"	float4x4 W2 = mul(W,Vi);\n"
		"	W2 = mul(W2, S);\n"
		"	W2[1].w = -W2[1].w;\n"

		"	W2[0].w += input.worldPosition.x;\n"
		"	W2[1].w += input.worldPosition.y;\n"
		"	W2[2].w += input.worldPosition.z;\n"
		"	W2[3].w = 1.f;\n"

		"	output.pos = mul(W2, float4(input.position,1.f));\n"
		"	output.pos = mul(V, output.pos);\n"
		"	output.pos = mul(P, output.pos);\n"
		
		"	output.pos.z    -= 0.0001f;\n"

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
		yyVertexType::Point,
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

void D3D11ShaderPoints::SetConstants(yyMaterial* material){
	m_cbVertexData.W = *yyGetMatrix(yyMatrixType::World);
	m_cbVertexData.V = *yyGetMatrix(yyMatrixType::View);
	m_cbVertexData.Vi = *yyGetMatrix(yyMatrixType::ViewInvert);
	m_cbVertexData.P = *yyGetMatrix(yyMatrixType::Projection);
	m_cbVertexData.Eye = *yyGetEyePosition();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	D3D11_BUFFER_DESC d;
	g_d3d11->m_d3d11DevCon->Map(m_cbVertex, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	m_cbVertex->GetDesc(&d);
	memcpy(mappedResource.pData, &m_cbVertexData, d.ByteWidth);
	g_d3d11->m_d3d11DevCon->Unmap(m_cbVertex, 0);
	g_d3d11->m_d3d11DevCon->VSSetConstantBuffers(0, 1, &m_cbVertex);
}



D3D11ShaderPointsAnimated::D3D11ShaderPointsAnimated() {
	m_cbVertex = 0;
}
D3D11ShaderPointsAnimated::~D3D11ShaderPointsAnimated() {
	if (m_cbVertex)m_cbVertex->Release();
}
void D3D11ShaderPointsAnimated::SetConstants(yyMaterial* material) {
	m_cbVertexData.W = *yyGetMatrix(yyMatrixType::World);
	m_cbVertexData.V = *yyGetMatrix(yyMatrixType::View);
	m_cbVertexData.Vi = *yyGetMatrix(yyMatrixType::ViewInvert);
	m_cbVertexData.P = *yyGetMatrix(yyMatrixType::Projection);
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
		"Texture2D tex2d_1;\n"
		"SamplerState tex2D_sampler_1;\n"
		"struct VSIn{\n"
		"   float3 position : POSITION;\n"
		"   float4 color : COLOR;\n"
		"   float3 worldPosition : NORMAL;\n"
		"	float4 weights : WEIGHTS;\n"
		"	uint4 bones : BONES;\n"
		"};\n"
		"cbuffer cbVertex{\n"
		"	float4x4 W;\n"
		"	float4x4 V;\n"
		"	float4x4 Vi;\n"
		"	float4x4 P;\n"
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

		"	output.pos   = mul(P*V*W, outPos);\n"
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