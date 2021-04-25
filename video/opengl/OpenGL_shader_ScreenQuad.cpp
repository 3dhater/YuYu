#include "yy.h"

#include "OpenGL.h"

#include "OpenGL_shader.h"
#include "OpenGL_shader_ScreenQuad.h"

#include "math/mat.h"
extern Mat4 g_guiProjectionMatrix;

OpenGLShaderScreenQuad::OpenGLShaderScreenQuad()
	:
	m_program(0),
	m_VAO(0),
	m_uniform_Texture(0)
{
	YY_DEBUG_PRINT_FUNC;
}

OpenGLShaderScreenQuad::~OpenGLShaderScreenQuad(){
	YY_DEBUG_PRINT_FUNC;
	if( m_VAO )
		gglDeleteVertexArrays(1,&m_VAO);
	if( m_program )
		gglDeleteProgram(m_program);
}

bool OpenGLShaderScreenQuad::init(){
	YY_DEBUG_PRINT_FUNC;
	const char * text_v =
		"#version 330\n"
		"layout(location = 0) in vec2 Position;\n"
		"layout(location = 1) in vec2 UV;\n"
		"out vec2 out_UV;\n"
		"void main(){\n"
		"    gl_Position = vec4(Position.xy,0,1);\n"
		"    out_UV = UV;\n"
		"}\n";
	const char * text_f = 
		"#version 330\n" 
		"in vec2 out_UV;\n"
		"uniform sampler2D Texture;\n"
		"out vec4 Out_Color;\n"
		"void main(){\n"
		"    Out_Color = texture(Texture,out_UV);\n"
		"}\n";
	if( !createShader(text_v, text_f, nullptr, m_program) )
		return false;

	glUseProgram(m_program);
	glUniform1i(glGetUniformLocation(m_program, "Texture"), 0); 
	glGenVertexArrays(1, &m_VAO);

	return true;
}