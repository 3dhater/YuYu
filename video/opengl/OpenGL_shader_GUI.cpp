#include "yy.h"

#include "OpenGL.h"

#include "OpenGL_shader.h"
#include "OpenGL_shader_GUI.h"

#include "math/mat.h"
extern Mat4 g_guiProjectionMatrix;

OpenGLShaderGUI::OpenGLShaderGUI()
	:
	m_program(0),
	m_VAO(0),
	m_uniform_ProjMtx(0),
	m_uniform_Offset(0),
	m_uniform_Texture(0),
	m_uniform_Color(0)
{
}

OpenGLShaderGUI::~OpenGLShaderGUI()
{
	if( m_VAO )
		gglDeleteVertexArrays(1,&m_VAO);
	if( m_program )
		gglDeleteProgram(m_program);
}

bool OpenGLShaderGUI::init()
{
	const char * text_v = 
		"#version 330\n"
		"layout(location = 0) in vec2 Position;\n"
		"layout(location = 1) in vec2 UV;\n"
		"uniform mat4 ProjMtx;\n"
		"uniform vec2 Offset;\n"
		"out vec2 out_UV;\n"
		"void main(){\n"
		"    gl_Position = ProjMtx * vec4(Position.xy + Offset,0,1);\n"
		"    out_UV = UV;\n"
		"}\n";
	const char * text_f = 
		"#version 330\n" 
		"in vec2 out_UV;\n"
		"uniform sampler2D Texture;\n"
		"uniform vec4 Color;\n"
		"out vec4 Out_Color;\n"
		"void main(){\n"
		"    Out_Color = texture(Texture,out_UV) * Color;\n"
		"}\n";
	if( !createShader(text_v, text_f, nullptr, m_program) )
		return false;

	glUseProgram(m_program);
	m_uniform_ProjMtx = glGetUniformLocation(m_program, "ProjMtx");
	m_uniform_Offset = glGetUniformLocation(m_program, "Offset");
	m_uniform_Color = glGetUniformLocation(m_program, "Color");

	glUniform1i(glGetUniformLocation(m_program, "Texture"), 0); 

	glGenVertexArrays(1, &m_VAO);

	return true;
}