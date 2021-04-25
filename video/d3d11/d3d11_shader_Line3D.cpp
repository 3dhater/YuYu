#include "yy.h"

#include "vid_d3d11.h"

#include "d3d11_shader.h"
#include "d3d11_shader_Line3D.h"

#include "math/mat.h"

extern D3D11 * g_d3d11;

D3D11ShaderLine3D::D3D11ShaderLine3D(){
	YY_DEBUG_PRINT_FUNC;
	m_cb = 0;
}

D3D11ShaderLine3D::~D3D11ShaderLine3D(){
	YY_DEBUG_PRINT_FUNC;
}

bool D3D11ShaderLine3D::init(){
	YY_DEBUG_PRINT_FUNC;
	const char * text =
		"struct VSIn{\n"
		"	uint vertexID : SV_VertexID;\n"
		"};\n"
		"cbuffer cbVertex  : register(b0) {\n"
		"	float4x4 VP;\n"
		"	float4 P1;\n"
		"	float4 P2;\n"
		"	float4 Color;\n"
		"};\n"
		"struct VSOut{\n"
		"   float4 pos : SV_POSITION;\n"
		"   float4 color : COLOR0;\n"
		"};\n"
		"struct PSOut{\n"
		"    float4 color : SV_Target;\n"
		"};\n"

		"VSOut VSMain(VSIn input){\n"
		"float4 vertices[2] = {\n"
			"float4( P1.xyz, 1.0),\n"
			"float4( P2.xyz, 1.0)\n"
			"};\n"
		"   VSOut output;\n"
		"	output.pos   = mul(VP, vertices[input.vertexID]);\n"
		"	output.color = Color;\n"
		"	return output;\n"
		"}\n"
		"PSOut PSMain(VSOut input){\n"
		"    PSOut output;\n"
		"    output.color = input.color;\n"
		"    return output;\n"
		"}\n";
	if (!D3D11_createShaders(
		"vs_4_0",
		"ps_4_0",
		text,
		text,
		"VSMain",
		"PSMain",
		yyVertexType::Null,
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
	/*const char * text_v = 
		"#version 130\n"
		"uniform mat4 ProjMtx;\n"
		"uniform vec4 P1;\n"
		"uniform vec4 P2;\n"
		"uniform vec4 Color;\n"
		"out vec4 out_color;\n"
		"void main(){\n"
		"vec4 vertices[2] = vec4[2](\n"
									"vec4( P1.xyz, 1.0),\n"
									"vec4( P2.xyz, 1.0)\n"
										");\n"
		"gl_Position = ProjMtx * vertices[gl_VertexID];\n"
		"out_color = Color;\n"
		"}\n";
	const char * text_f = 
		"#version 130\n" 
		"in vec4 out_color;\n"
		"out vec4 color;\n"
		"void main(){\n"
		  "color = vec4(out_color.xyz, 1.0);\n"
		"}\n";
	if( !createShader(text_v, text_f, nullptr, m_program) )
		return false;

	glUseProgram(m_program);
	m_uniform_ProjMtx = glGetUniformLocation(m_program, "ProjMtx");
	m_uniform_P1 = glGetUniformLocation(m_program, "P1");
	m_uniform_P2 = glGetUniformLocation(m_program, "P2");
	m_uniform_Color = glGetUniformLocation(m_program, "Color");
	
	glGenVertexArrays(1, &m_VAO);*/

	return true;
}

void D3D11ShaderLine3D::SetData(const v4f& p1, const v4f& p2, const yyColor& color, const Mat4& projMat){
	m_cbData.P1 = p1;
	m_cbData.P2 = p2;
	m_cbData.Color = color;
	m_cbData.VP = projMat; // g_d3d11->m_matrixViewProjection;
}

void D3D11ShaderLine3D::SetConstants(yyMaterial* material){

	g_d3d11->m_d3d11DevCon->VSSetConstantBuffers(0, 1, &m_cb);
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	g_d3d11->m_d3d11DevCon->Map(m_cb, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	D3D11_BUFFER_DESC d;
	m_cb->GetDesc(&d);
	memcpy(mappedResource.pData, &m_cbData, d.ByteWidth);
	g_d3d11->m_d3d11DevCon->Unmap(m_cb, 0);
}