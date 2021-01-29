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
		"#version 330\n"
		"layout(location = 0) in vec3 inputPosition;\n"
		"layout(location = 1) in vec2 inputTexCoord;\n"
		"layout(location = 2) in vec3 inputNormal;\n"
		"layout(location = 3) in vec3 inputBinormal;\n"
		"layout(location = 4) in vec3 inputTangent;\n"
		"out vec2 texCoord;\n"
		"uniform mat4 WVP;\n"
		"void main(){\n"
		"gl_Position = WVP * vec4(inputPosition.xyz,1.0f);\n"
		"texCoord.x = inputTexCoord.x;\n"
		"texCoord.y = inputTexCoord.y;\n"
		"}\n";
	const char * text_f =
		"#version 330\n"
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

// ===============================================================================================
OpenGLShaderSimpleAnimated::OpenGLShaderSimpleAnimated()
{
	m_program = 0;
	m_VAO = 0;
	m_uniform_WVP = 0;
	m_uniform_World = 0;
	m_uniform_Bones = 0;
}

OpenGLShaderSimpleAnimated::~OpenGLShaderSimpleAnimated()
{
	if (m_VAO)
		gglDeleteVertexArrays(1, &m_VAO);
	if (m_program)
		gglDeleteProgram(m_program);
}

bool OpenGLShaderSimpleAnimated::init()
{
	const char * text_v =
		"#version 330\n"
		"layout(location = 0) in vec3 inputPosition;\n"
		"layout(location = 1) in vec2 inputTexCoord;\n"
		"layout(location = 2) in vec3 inputNormal;\n"
		"layout(location = 3) in vec3 inputBinormal;\n"
		"layout(location = 4) in vec3 inputTangent;\n"
		"layout(location = 5) in vec4 inputWeights;\n"
		"layout(location = 6) in uvec4 inputBones;\n"
		"out vec2 texCoord;\n"
		"uniform mat4 WVP;\n"
		"uniform mat4 World;\n"
		"uniform mat4 Bones[100];\n"
		"void main(){\n"
		"	mat4 BoneTransform = Bones[inputBones[0]] * inputWeights[0];\n"
		"	BoneTransform     += Bones[inputBones[1]] * inputWeights[1];\n"
		"	BoneTransform     += Bones[inputBones[2]] * inputWeights[2];\n"
		"	BoneTransform     += Bones[inputBones[3]] * inputWeights[3];\n"
		"	vec4 vertexPosition = BoneTransform * vec4(inputPosition.xyz,1.0f);\n"
		"	gl_Position = WVP * vertexPosition;\n"
		"	texCoord.x = inputTexCoord.x;\n"
		"	texCoord.y = inputTexCoord.y;\n"
		"}\n";
	const char * text_f =
		"#version 330\n"
		"in vec2 texCoord;\n"
		"uniform sampler2D diffuseTexture;\n"
		"out vec4 color;\n"
		"void main(){\n"
		"	color = texture(diffuseTexture, texCoord);\n"
		"}\n";
	if (!createShader(text_v, text_f, nullptr, m_program))
		return false;

	glUseProgram(m_program);
	m_uniform_WVP = glGetUniformLocation(m_program, "WVP");
	m_uniform_World = glGetUniformLocation(m_program, "World");
	m_uniform_Bones = glGetUniformLocation(m_program, "Bones");

	glUniform1i(glGetUniformLocation(m_program, "diffuseTexture"), 0);

	glGenVertexArrays(1, &m_VAO);

	return true;
}