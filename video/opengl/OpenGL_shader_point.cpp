#include "yy.h"

#include "OpenGL.h"

#include "OpenGL_shader.h"
#include "OpenGL_shader_point.h"

#include "math/mat.h"

OpenGLShaderPoint::OpenGLShaderPoint(){
	m_program = 0;
	m_VAO = 0;
	m_uniform_W = 0;
	m_uniform_P = 0;
	m_uniform_V = 0;
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
		"layout(location = 1) in vec3 inputWorldPosition;\n"
		"layout(location = 2) in vec4 inputColor;\n"
		"out vec4 vertColor;\n"
		"uniform mat4 W;\n"
		"uniform mat4 P;\n"
		"uniform mat4 V;\n"
		"void main(){\n"
		"	vertColor = inputColor;\n"

		"	mat4 V2 = V;\n"
		"	V2[3] = vec4(0,0,0,1.f);\n"

		"	mat4 W2 = W * inverse(V2);\n"
		"	W2[3].y = -W2[3].y;\n"
		"	W2[3].xyz += inputWorldPosition;\n"
		"	W2[3].w = 1.f;\n"

		"	gl_Position = (P * V * W2) * vec4(inputPosition.xyz,1.f);\n"
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
	m_uniform_W = glGetUniformLocation(m_program, "W");
	m_uniform_P = glGetUniformLocation(m_program, "P");
	m_uniform_V = glGetUniformLocation(m_program, "V");
	
	glUniform1i(glGetUniformLocation(m_program, "diffuseTexture"), 0); 
	glGenVertexArrays(1, &m_VAO);

	return true;
}

// ===============================================================================================
OpenGLShaderPointAnimated::OpenGLShaderPointAnimated(){
	m_program = 0;
	m_VAO = 0;
	m_uniform_W = 0;
	m_uniform_P = 0;
	m_uniform_V = 0;
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
		"layout(location = 2) in vec3 inputWorldPosition;\n"
		"layout(location = 3) in vec4 inputWeights;\n"
		"layout(location = 4) in uvec4 inputBones;\n"
		"out vec4 vertColor;\n"
		"uniform mat4 W;\n"
		"uniform mat4 P;\n"
		"uniform mat4 V;\n"
		"uniform mat4 Bones[250];\n"
		"void main(){\n"
		"	vertColor = inputColor;\n"

		"	vec4 inPos = vec4(inputPosition.xyz,1.0f);\n"

		"	mat4 BoneTransform = Bones[inputBones.x] * inputWeights.x;\n"
		"	BoneTransform     += Bones[inputBones.y] * inputWeights.y;\n"
		"	BoneTransform     += Bones[inputBones.z] * inputWeights.z;\n"
		"	BoneTransform     += Bones[inputBones.w] * inputWeights.w;\n"
		"	vec4 outPos = BoneTransform * inPos;\n"
		//"	gl_Position = WVP * outPos;\n"
		"	mat4 V2 = V;\n"
		"	V2[3] = vec4(0,0,0,1.f);\n"

		"	mat4 W2 = W * inverse(V2);\n"
		"	W2[3].y = -W2[3].y;\n"
		"	W2[3].xyz += inputWorldPosition;\n"
		"	W2[3].w = 1.f;\n"

		"	gl_Position = (P * V * W2) * vec4(inputPosition.xyz,1.f);\n"
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
	m_uniform_W = glGetUniformLocation(m_program, "W");
	m_uniform_P = glGetUniformLocation(m_program, "P");
	m_uniform_V = glGetUniformLocation(m_program, "V");
	m_uniform_Bones = glGetUniformLocation(m_program, "Bones");

	glUniform1i(glGetUniformLocation(m_program, "diffuseTexture"), 0);

	glGenVertexArrays(1, &m_VAO);

	return true;
}
