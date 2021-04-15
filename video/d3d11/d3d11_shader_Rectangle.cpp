#include "yy.h"
#include "vid_d3d11.h"

#include "D3D11_shader.h"
#include "d3d11_shader_Rectangle.h"

#include "math/mat.h"
extern Mat4 g_guiProjectionMatrix;
extern D3D11 * g_d3d11;

D3D11ShaderRectangle::D3D11ShaderRectangle()
	:
	m_cbVertex(0)
{
}

D3D11ShaderRectangle::~D3D11ShaderRectangle(){
	if (m_cbVertex) m_cbVertex->Release();
}

bool D3D11ShaderRectangle::init(){
	const char * text = 
		"cbuffer cbVertex{\n"
		"	float4x4 ProjMtx;\n"
		"	float4 Corners;\n"
		"	float4 Color1;\n"
		"	float4 Color2;\n"
		"};\n"
		"struct VSIn{\n"
		"	uint vertex_id : SV_VertexID;"
		"};\n"
		"struct VSOut{\n"
		"   float4 pos : SV_POSITION;\n"
		"	float4 color : COLOR0;\n"
		"};\n"
		"struct PSOut{\n"
		"    float4 color : SV_Target;\n"
		"};\n"
		"VSOut VSMain(VSIn input){\n"
		"float4 vertices[6] = {\n"
									"float4( Corners.z,  Corners.w,  0, 1.0),\n"
                                    "float4( Corners.x,  Corners.w,  0, 1.0),\n"
                                    "float4( Corners.z,  Corners.y,  0, 1.0),\n"
									"float4( Corners.x,  Corners.w,  0, 1.0),\n"
									"float4( Corners.x,  Corners.y,  0, 1.0),\n"
									"float4( Corners.z,  Corners.y, 0,  1.0)\n"
										"};\n"
		"   VSOut output;\n"
		"	output.pos   = mul(ProjMtx, vertices[input.vertex_id]);\n"
		"switch( input.vertex_id ){\n"
		"case 0:output.color = Color2;break;\n"
		"case 1:output.color = Color2;break;\n"
		"case 3:output.color = Color2;break;\n"
		"default:output.color = Color1;break;\n"
		"}\n"
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

	if (!D3D11_createConstantBuffer(sizeof(cbVertex), &m_cbVertex))
	{
		YY_PRINT_FAILED;
		return false;
	}

	return true;
}

void D3D11ShaderRectangle::SetConstants(yyMaterial* material) {
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	D3D11_BUFFER_DESC d;
	g_d3d11->m_d3d11DevCon->Map(m_cbVertex, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	m_cbVertex->GetDesc(&d);
	memcpy(mappedResource.pData, &m_cbVertex_impl, d.ByteWidth);
	g_d3d11->m_d3d11DevCon->Unmap(m_cbVertex, 0);
	g_d3d11->m_d3d11DevCon->VSSetConstantBuffers(0, 1, &m_cbVertex);
}
