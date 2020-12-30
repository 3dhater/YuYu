#include "yy.h"

#include "OpenGL.h"

#include "OpenGL_shader.h"
#include "OpenGL_shader_depth.h"

#include "math/mat.h"

OpenGLShaderDepth::OpenGLShaderDepth()
{
	m_program = 0;
	m_VAO = 0;
	m_uniform_World = 0;
	m_uniform_LightView = 0;
	m_uniform_LightProjection = 0;
}

OpenGLShaderDepth::~OpenGLShaderDepth()
{
	if( m_VAO )
		gglDeleteVertexArrays(1,&m_VAO);
	if( m_program )
		gglDeleteProgram(m_program);
}

bool OpenGLShaderDepth::init()
{
	const char * text_v = 
		"#version 130\n"
		"in vec3 inputPosition;\n"
		"in vec2 inputTexCoord;\n"
		"in vec3 inputNormal;\n"
		"in vec3 inputBinormal;\n"
		"in vec3 inputTangent;\n"
		"out vec2 texCoord;\n"
		"out vec4 vertexPosition;\n"
		"uniform mat4 World;\n"
		"uniform mat4 LightView;\n"
		"uniform mat4 LightProjection;\n"
		"void main(){\n"
		"vertexPosition = (LightProjection * LightView * World) * vec4(inputPosition.xyz,1.0f);\n"
		"gl_Position = vertexPosition;\n"
		"texCoord.x = inputTexCoord.x;\n"
		"texCoord.y = 1.f - inputTexCoord.y;\n"
		"}\n";
	const char * text_f =
		"#version 130\n"
		"in vec2 texCoord;\n"
		"in vec4 vertexPosition;\n"
		"out vec4 color;\n"
		"uniform sampler2D diffuseTexture;\n"
		"void main(){\n"
		
		"vec4 diffuse_texture = texture(diffuseTexture,texCoord);\n"
		"if(diffuse_texture.w < 1.f){\n"
		"discard;\n"
		"}\n"

		"color = vec4(vertexPosition.x, vertexPosition.y, vertexPosition.z, 1.f);\n"
		"}\n";
	if( !createShader(text_v, text_f, nullptr, m_program) )
		return false;

	glUseProgram(m_program);
	m_uniform_World = glGetUniformLocation(m_program, "World");
	m_uniform_LightView = glGetUniformLocation(m_program, "LightView");
	m_uniform_LightProjection = glGetUniformLocation(m_program, "LightProjection");
	
	glUniform1i(glGetUniformLocation(m_program, "diffuseTexture"), 0);

	glGenVertexArrays(1, &m_VAO);

	return true;
}