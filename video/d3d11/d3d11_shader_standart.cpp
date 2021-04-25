#include "yy.h"

#include "yy_video.h"

#include "vid_d3d11.h"
#include "d3d11_shader.h"
#include "d3d11_shader_standart.h"

#include "math/mat.h"

D3D11ShaderStd::D3D11ShaderStd(){
	YY_DEBUG_PRINT_FUNC;
}

D3D11ShaderStd::~D3D11ShaderStd(){
	YY_DEBUG_PRINT_FUNC;
}

bool D3D11ShaderStd::init(){
	YY_DEBUG_PRINT_FUNC;
	//const char * text_v = 
	//	"#version 130\n"
	//	"in vec3 inputPosition;\n"
	//	"in vec2 inputTexCoord;\n"
	//	"in vec3 inputNormal;\n"
	//	"in vec3 inputBinormal;\n"
	//	"in vec3 inputTangent;\n"
	//	"out vec2 texCoord;\n"
	//	"uniform mat4 WVP;\n"
	//	"void main(){\n"
	//	"gl_Position = WVP * vec4(inputPosition.xyz,1.0f);\n"
	//	"texCoord.x = inputTexCoord.x;\n"
	//	"texCoord.y = 1.f - inputTexCoord.y;\n"
	//	"}\n";
	//const char * text_f = 
	//	"#version 130\n" 
	//	"in vec2 texCoord;\n"
	//	"out vec4 color;\n"
	//	"uniform sampler2D diffuseTexture;\n"
	//	"void main(){\n"
	//	"vec4 diffuse_texture = texture(diffuseTexture,texCoord);\n"
	//	  "color = diffuse_texture * vec4(1.f,1.f,1.f,1.f);\n"
	//	"}\n";
	//if( !createShader(text_v, text_f, nullptr, m_program) )
	//	return false;

	//glUseProgram(m_program);
	//m_uniform_WVP = glGetUniformLocation(m_program, "WVP");
	////m_uniform_diffuseTexture = glGetUniformLocation(m_program, "diffuseTexture");
	//
	//glUniform1i(glGetUniformLocation(m_program, "diffuseTexture"), 0); 

	//glGenVertexArrays(1, &m_VAO);

	return true;
}