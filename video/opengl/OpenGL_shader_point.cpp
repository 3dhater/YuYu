#include "yy.h"

#include "OpenGL.h"

#include "OpenGL_shader.h"
#include "OpenGL_shader_point.h"

#include "math/mat.h"

OpenGLShaderPoint::OpenGLShaderPoint(){
	m_program = 0;
	m_VAO = 0;
	m_uniform_WVP = 0;
}

OpenGLShaderPoint::~OpenGLShaderPoint(){
	if( m_VAO )
		glDeleteVertexArrays(1,&m_VAO);
	if( m_program )
		glDeleteProgram(m_program);
}

bool OpenGLShaderPoint::init(){
	const char * text_v =
		"#version 330\n"
		"layout(location = 0) in vec3 inputPosition;\n"
		"layout(location = 1) in vec4 inputColor;\n"
		"out vec4 vertColor;\n"
		"uniform mat4 WVP;\n"
		"void main(){\n"
		"	vertColor = inputColor;\n"
		"	gl_PointSize = 5.0;\n"
		"	gl_Position = WVP * vec4(inputPosition.xyz ,1.f);\n"
		"	gl_Position.z -= 0.0001f;\n"
		"}\n";
	const char * text_f =
		"#version 330\n"
		"in vec4 vertColor;\n"
		"out vec4 color;\n"
		"void main(){\n"
		"	color = vertColor;\n"
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
OpenGLShaderPointAnimated::OpenGLShaderPointAnimated(){
	m_program = 0;
	m_VAO = 0;
	m_uniform_WVP = 0;
	m_uniform_Bones = 0;
}

OpenGLShaderPointAnimated::~OpenGLShaderPointAnimated(){
	if (m_VAO)
		glDeleteVertexArrays(1, &m_VAO);
	if (m_program)
		glDeleteProgram(m_program);
}

bool OpenGLShaderPointAnimated::init(){
	const char * text_v =
		"#version 330\n"
		"layout(location = 0) in vec3 inputPosition;\n"
		"layout(location = 1) in vec4 inputColor;\n"
		"layout(location = 2) in vec4 inputWeights;\n"
		"layout(location = 3) in uvec4 inputBones;\n"
		"out vec4 vertColor;\n"
		"uniform mat4 WVP;\n"
		"uniform mat4 Bones[250];\n"
		"void main(){\n"
		"	vertColor = inputColor;\n"

		"	vec4 inPos = vec4(inputPosition.xyz,1.0f);\n"

		"	mat4 BoneTransform = Bones[inputBones.x] * inputWeights.x;\n"
		"	BoneTransform     += Bones[inputBones.y] * inputWeights.y;\n"
		"	BoneTransform     += Bones[inputBones.z] * inputWeights.z;\n"
		"	BoneTransform     += Bones[inputBones.w] * inputWeights.w;\n"
		"	vec4 outPos = BoneTransform * inPos;\n"
		"	gl_Position = WVP * outPos;\n"
		"	gl_PointSize = 2.0;\n"
		//"	gl_Position.z -= 0.0001f;\n"
		"}\n";
	const char * text_f =
		"#version 330\n"
		"in vec4 vertColor;\n"
		"out vec4 color;\n"
		"void main(){\n"
		"	color = vertColor;\n"
		"}\n";
	if (!createShader(text_v, text_f, nullptr, m_program))
		return false;

	glUseProgram(m_program);
	m_uniform_WVP = glGetUniformLocation(m_program, "WVP");
	m_uniform_Bones = glGetUniformLocation(m_program, "Bones");

	glUniform1i(glGetUniformLocation(m_program, "diffuseTexture"), 0);

	glGenVertexArrays(1, &m_VAO);

	return true;
}
