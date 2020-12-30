#include "yy.h"

#include "OpenGL.h"

#include "OpenGL_shader.h"
#include "OpenGL_shader_standart.h"

#include "math/mat.h"

OpenGLShaderStd::OpenGLShaderStd()
{
	m_program = 0;
	m_VAO = 0;
	m_uniform_WVP = 0;
}

OpenGLShaderStd::~OpenGLShaderStd()
{
	if( m_VAO )
		gglDeleteVertexArrays(1,&m_VAO);
	if( m_program )
		gglDeleteProgram(m_program);
}

bool OpenGLShaderStd::init()
{
	const char * text_v = 
		"#version 130\n"
		"in vec3 inputPosition;\n"
		"in vec2 inputTexCoord;\n"
		"in vec3 inputNormal;\n"
		"in vec3 inputBinormal;\n"
		"in vec3 inputTangent;\n"
		"out vec2 texCoord;\n"
		"out vec3 normal;\n"
		"out vec4 viewPosition;\n"
		"uniform mat4 WVP;\n"
		"uniform mat4 W;\n"
		"uniform mat4 LightView;\n"
		"uniform mat4 LightProjection;\n"
		"void main(){\n"
		"gl_Position = WVP * vec4(inputPosition.xyz,1.0f);\n"
		"texCoord.x = inputTexCoord.x;\n"
		"texCoord.y = 1.f - inputTexCoord.y;\n"
		"normal = mat3(W) * inputNormal;\n"
		"normal = normalize(normal);\n"
		"viewPosition = W * vec4(inputPosition.xyz,1.0f);\n"
		"viewPosition = LightView * vec4(viewPosition.xyz,1.0f);\n"
		"viewPosition = LightProjection * vec4(viewPosition.xyz,1.0f);\n"
		"}\n";
	const char * text_f =
		"#version 130\n"
		"in vec2 texCoord;\n"
		"in vec3 normal;\n"
		"in vec4 viewPosition;\n"
		"out vec4 color;\n"
		"uniform vec3 sunDir;\n"
		"uniform vec3 ambientColor;\n"
		"uniform float selfLight;\n"
		"uniform sampler2D diffuseTexture;\n"
		"uniform sampler2D shadowDepthTexture;\n"
		"void main(){\n"

		"vec2 depthCoords = vec2(0.f,0.f);\n"
		"depthCoords.x = viewPosition.x / viewPosition.w / 2.f + 0.5f;\n"
		"depthCoords.y = viewPosition.y / viewPosition.w / 2.f + 0.5f;\n"

		"vec3 diffuseColor = vec3(1.f,1.f,1.f);\n"
		"vec3 ambient = ambientColor * diffuseColor;\n"
		"float lightIntensity = clamp(dot(normal, sunDir), selfLight, 1.0f);\n"
		"vec4 diffuse_texture = texture(diffuseTexture,texCoord);\n"
		"if(diffuse_texture.w < 1.f){\n"
		"discard;\n"
		"}\n"
		"color = diffuse_texture * lightIntensity;\n"

		"if((clamp(depthCoords.x,0.0f,1.0f) == depthCoords.x) && (clamp(depthCoords.y,0.1f,1.0f) == depthCoords.y)){\n"
		"    float xOffset = 1.f / 2048.f;\n"
		"    float lightPercent = 0.f;\n"
		"    float lightDepthValue = viewPosition.z;\n"
		"    lightDepthValue = lightDepthValue - 0.005;\n"
		"    for (float y = -0.5 ; y <= 0.5; y +=1.0) {\n"
		"        for (float x = -0.5 ; x <= 0.5; x+=1.0) {\n"
		"		    	float texel = texture(shadowDepthTexture, vec2(depthCoords.x + x * xOffset, depthCoords.y + y * xOffset)).z;\n"
		"			    if(texel < lightDepthValue) lightPercent += 1.0;\n"
		"        }\n"
		"    }\n"
		"    lightPercent /= 4.f;\n"
		"    color *= 1.0 - (lightPercent * selfLight);\n"
		"}\n"

		"color *= 1.3;\n"

		"color.xyz = ambient * color.xyz;\n"
		"color.w = diffuse_texture.w;\n"
		"}\n";
	if( !createShader(text_v, text_f, nullptr, m_program) )
		return false;

	glUseProgram(m_program);
	m_uniform_WVP = glGetUniformLocation(m_program, "WVP");
	m_uniform_W = glGetUniformLocation(m_program, "W");
	m_uniform_LightView = glGetUniformLocation(m_program, "LightView");
	m_uniform_LightProjection = glGetUniformLocation(m_program, "LightProjection");
	m_uniform_sunDir = glGetUniformLocation(m_program, "sunDir");
	m_uniform_ambientColor = glGetUniformLocation(m_program, "ambientColor");
	m_uniform_selfLight = glGetUniformLocation(m_program, "selfLight");
	//m_uniform_diffuseTexture = glGetUniformLocation(m_program, "diffuseTexture");
	
	glUniform1i(glGetUniformLocation(m_program, "diffuseTexture"), 0); 
	glUniform1i(glGetUniformLocation(m_program, "shadowDepthTexture"), 1);

	glGenVertexArrays(1, &m_VAO);

	return true;
}