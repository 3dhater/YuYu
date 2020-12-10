#include "yy.h"

#include "OpenGL.h"

#include "OpenGL_shader.h"
#include "OpenGL_shader_Line3D.h"

#include "math/mat.h"

OpenGLShaderLine3D::OpenGLShaderLine3D()
{
	m_program = 0;
	m_VAO = 0;
	m_uniform_ProjMtx = 0;
	m_uniform_P1 = 0;
	m_uniform_P2 = 0;
	m_uniform_Color = 0;
}

OpenGLShaderLine3D::~OpenGLShaderLine3D()
{
	if( m_VAO )
		glDeleteVertexArrays(1,&m_VAO);
	if( m_program )
		glDeleteProgram(m_program);
}

bool OpenGLShaderLine3D::init()
{
	const char * text_v = 
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
	
	glGenVertexArrays(1, &m_VAO);

	return true;
}