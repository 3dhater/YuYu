﻿#include "yy.h"

#include "OpenGL.h"

#include "OpenGL_shader.h"
#include "OpenGL_shader_LineModel.h"

#include "math/mat.h"

OpenGLShaderLineModel::OpenGLShaderLineModel(){
	m_program = 0;
	m_VAO = 0;
	m_uniform_WVP = 0;
}

OpenGLShaderLineModel::~OpenGLShaderLineModel(){
	if( m_VAO )
		gglDeleteVertexArrays(1,&m_VAO);
	if( m_program )
		gglDeleteProgram(m_program);
}

bool OpenGLShaderLineModel::init(){
	const char * text_v =
		"#version 330\n"
		"layout(location = 0) in vec3 inputPosition;\n"
		"layout(location = 1) in vec3 inputNormal;\n"
		"layout(location = 2) in vec4 inputColor;\n"
		"out vec4 vertexColor;\n"
		"out vec4 vertexNormal;\n"
		"out vec3 viewDir;\n"
		"uniform mat4 WVP;\n"
		"uniform mat4 W;\n"
		"uniform mat4 V;\n"
		"uniform vec3 Eye;\n"
		"void main(){\n"
		"gl_Position = WVP * vec4(inputPosition,1.0f);\n"
		"gl_Position.z -= 0.00015f;\n"
		"vertexColor = inputColor;\n"
		"vertexNormal = normalize(W * vec4(inputNormal,1.f));\n"
		"viewDir = normalize(Eye - (mat3(W) * inputPosition));\n"
		"}\n";
	const char * text_f =
		"#version 330\n"
		"in vec4 vertexColor;\n"
		"in vec4 vertexNormal;\n"
		"in vec3 viewDir;\n"
		"uniform vec4 BaseColor;\n"
		"out vec4 color;\n"
		"void main(){\n"
		"color = vertexColor * BaseColor;\n"
	//	"float d = clamp(dot(vertexNormal.xyz, viewDir) + 0.3f, 0.3f, 1.f);\n"
	//	"color.xyz *= d;\n"
		"}\n";
	if( !createShader(text_v, text_f, nullptr, m_program) )
		return false;

	glUseProgram(m_program);
	m_uniform_WVP = glGetUniformLocation(m_program, "WVP");
	m_uniform_W = glGetUniformLocation(m_program, "W");
	m_uniform_V = glGetUniformLocation(m_program, "V");
	m_uniform_Eye = glGetUniformLocation(m_program, "Eye");
	m_uniform_BaseColor = glGetUniformLocation(m_program, "BaseColor");
	
	glGenVertexArrays(1, &m_VAO);

	return true;
}

// ===============================================================================================
OpenGLShaderLineModelAnimated::OpenGLShaderLineModelAnimated(){
	m_program = 0;
	m_VAO = 0;
	m_uniform_WVP = 0;
	m_uniform_World = 0;
	m_uniform_Bones = 0;
}

OpenGLShaderLineModelAnimated::~OpenGLShaderLineModelAnimated(){
	if (m_VAO)
		gglDeleteVertexArrays(1, &m_VAO);
	if (m_program)
		gglDeleteProgram(m_program);
}

bool OpenGLShaderLineModelAnimated::init(){
	const char * text_v =
		"#version 330\n"
		"layout(location = 0) in vec3 inputPosition;\n"
		"layout(location = 1) in vec3 inputNormal;\n"
		"layout(location = 2) in vec4 inputColor;\n"
		"layout(location = 3) in vec4 inputWeights;\n"
		"layout(location = 4) in uvec4 inputBones;\n"
		"out vec4 vertexColor;\n"
		"uniform mat4 WVP;\n"
		"uniform mat4 World;\n"
		"uniform mat4 Bones[255];\n"
		"void main(){\n"
		
		"	vec4 inPos = vec4(inputPosition.xyz,1.0f);\n"

		"	mat4 BoneTransform = Bones[inputBones.x] * inputWeights.x;\n"
		"	BoneTransform     += Bones[inputBones.y] * inputWeights.y;\n"
		"	BoneTransform     += Bones[inputBones.z] * inputWeights.z;\n"
		"	BoneTransform     += Bones[inputBones.w] * inputWeights.w;\n"
		"	vec4 outPos = BoneTransform * inPos;\n"
		
		/*"	vec4 outPos = inPos;\n"
		"	uint iBone = inputBones.x;\n"
		"	float fWeight = inputWeights.x;\n"
		"	mat4 m = Bones[iBone];\n"
		"	outPos += fWeight * (m * inPos);\n"*/


		"	gl_Position = WVP * outPos;\n"
		"vertexColor = inputColor;\n"
		"}\n";
	const char * text_f =
		"#version 330\n"
		"in vec4 vertexColor;\n"
		"uniform vec4 BaseColor;\n"
		"out vec4 color;\n"
		"void main(){\n"
		"	color = vertexColor * BaseColor;\n"
		"}\n";
	if (!createShader(text_v, text_f, nullptr, m_program))
		return false;

	glUseProgram(m_program);
	m_uniform_WVP = glGetUniformLocation(m_program, "WVP");
	m_uniform_World = glGetUniformLocation(m_program, "World");
	m_uniform_Bones = glGetUniformLocation(m_program, "Bones");
	m_uniform_BaseColor = glGetUniformLocation(m_program, "BaseColor");

	glGenVertexArrays(1, &m_VAO);

	return true;
}
