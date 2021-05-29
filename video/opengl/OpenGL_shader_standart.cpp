#include "yy.h"

#include "OpenGL.h"

#include "OpenGL_shader.h"
#include "OpenGL_shader_standart.h"

#include "math/mat.h"

OpenGLShaderStd::OpenGLShaderStd(){
	m_program = 0;
	m_VAO = 0;
	m_uniform_WVP = 0;
}

OpenGLShaderStd::~OpenGLShaderStd(){
	if( m_VAO )
		glDeleteVertexArrays(1,&m_VAO);

	if( m_program )
		glDeleteProgram(m_program);
}

bool OpenGLShaderStd::init(){
	const char * text_v =
		"#version 330\n"
		"layout(location = 0) in vec3 inputPosition;\n"
		"layout(location = 1) in vec2 inputTexCoord;\n"
		"layout(location = 2) in vec3 inputNormal;\n"
		"layout(location = 3) in vec3 inputBinormal;\n"
		"layout(location = 4) in vec3 inputTangent;\n"
		"layout(location = 5) in vec4 inputColor;\n"
		"out vec2 texCoord;\n"
		"out vec4 vertColor;\n"
		"out vec3 normal;\n"
		"out vec3 fragPos;\n"
		"uniform mat4 WVP;\n"
		"uniform mat4 W;\n"
		"void main(){\n"
		"vertColor = inputColor;\n"
		"gl_Position = WVP * vec4(inputPosition.xyz,1.0f);\n"
		"normal = mat3(W) * inputNormal;\n"
		"fragPos = vec3(W * vec4(inputPosition.xyz, 1.0));\n"
		"texCoord.x = inputTexCoord.x;\n"
		"texCoord.y = inputTexCoord.y;\n"
		"}\n";
	const char * text_f =
		"#version 330\n"
		"in vec2 texCoord;\n"
		"in vec4 vertColor;\n"
		"in vec3 normal;\n"
		"in vec3 fragPos;\n"
		"uniform sampler2D diffuseTexture;\n"
		"uniform vec4 BaseColor;\n"
		"uniform vec3 SunLightPos;\n"
		"out vec4 color;\n"
		"void main(){\n"
		"	vec3 lightDir = normalize(SunLightPos - fragPos);\n"
		"	float diff = max(dot(normal, lightDir), 0.0);\n"
		"	color = texture(diffuseTexture, texCoord) * BaseColor;\n"
		"	color.xyz = mix(color.xyz, vertColor.xyz, vertColor.www);\n"
		"	color.xyz *= diff;\n"
		"}\n";
	if (!createShader(text_v, text_f, nullptr, m_program))
		return false;

	glUseProgram(m_program);
	m_uniform_WVP = glGetUniformLocation(m_program, "WVP");
	m_uniform_W = glGetUniformLocation(m_program, "W");
	m_uniform_BaseColor = glGetUniformLocation(m_program, "BaseColor");
	m_uniform_SunLightPos = glGetUniformLocation(m_program, "SunLightPos");
	
	glUniform1i(glGetUniformLocation(m_program, "diffuseTexture"), 0); 

	glGenVertexArrays(1, &m_VAO);

	return true;
}