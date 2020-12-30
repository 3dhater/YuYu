#include "yy.h"

#include "OpenGL.h"

#include "OpenGL_shader.h"
#include "OpenGL_shader_simple.h"

#include "math/mat.h"

OpenGLShaderSimple::OpenGLShaderSimple()
{
	m_program = 0;
	m_VAO = 0;
	m_uniform_WVP = 0;
}

OpenGLShaderSimple::~OpenGLShaderSimple()
{
	if( m_VAO )
		gglDeleteVertexArrays(1,&m_VAO);
	if( m_program )
		gglDeleteProgram(m_program);
}

bool OpenGLShaderSimple::init()
{
	const char * text_v = 
		"#version 130\n"
		"in vec3 inputPosition;\n"
		"in vec2 inputTexCoord;\n"
		"in vec3 inputNormal;\n"
		"in vec3 inputBinormal;\n"
		"in vec3 inputTangent;\n"
		"out vec2 texCoord;\n"
		"uniform mat4 WVP;\n"
		"void main(){\n"
		"gl_Position = WVP * vec4(inputPosition.xyz,1.0f);\n"
		"texCoord.x = inputTexCoord.x;\n"
		"texCoord.y = (1.f - inputTexCoord.y);\n"
		"}\n";
	const char * text_f =
		"#version 130\n"
		"in vec2 texCoord;\n"
		"uniform sampler2D diffuseTexture;\n"
		"out vec4 color;\n"
		"void main(){\n"
		"color = texture(diffuseTexture, texCoord);\n"
		"}\n";
	if( !createShader(text_v, text_f, nullptr, m_program) )
		return false;

	glUseProgram(m_program);
	m_uniform_WVP = glGetUniformLocation(m_program, "WVP");
	
	glUniform1i(glGetUniformLocation(m_program, "diffuseTexture"), 0); 

	glGenVertexArrays(1, &m_VAO);

	return true;
}