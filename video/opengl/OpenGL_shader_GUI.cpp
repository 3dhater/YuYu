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
	m_uniform_Texture(0)
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
		"#version 130\n"
		"in vec3 Position;\n"
		"in vec2 UV;\n"
		"uniform mat4 ProjMtx;\n"
		"out vec2 out_UV;\n"
		"void main(){\n"
		"    gl_Position = ProjMtx * vec4(Position.xyz,1);\n"
		"    out_UV = UV;\n"
		"}\n";
	const char * text_f = 
		"#version 130\n" 
		"in vec2 out_UV;\n"
		"uniform sampler2D Texture;\n"
		"out vec4 Out_Color;\n"
		"void main(){\n"
		"    Out_Color = texture(Texture,out_UV) * vec4(1.f,1.f,1.f,1.f);\n"
		"}\n";
	if( !createShader(text_v, text_f, nullptr, m_program) )
		return false;

	glUseProgram(m_program);
	m_uniform_ProjMtx = glGetUniformLocation(m_program, "ProjMtx");
	
	glUniform1i(glGetUniformLocation(m_program, "Texture"), 0); 

	glGenVertexArrays(1, &m_VAO);

	return true;
}