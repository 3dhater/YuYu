#include "yy.h"

#include "vid_d3d11.h"

#include "d3d11_shader.h"
#include "d3d11_shader_Line3D.h"

#include "math/mat.h"

D3D11ShaderLine3D::D3D11ShaderLine3D()
{
}

D3D11ShaderLine3D::~D3D11ShaderLine3D()
{
}

bool D3D11ShaderLine3D::init()
{
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