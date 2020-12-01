#include "yy.h"

#include "OpenGL.h"

#include "OpenGL_shader.h"
#include "OpenGL_shader_sprite.h"

#include "math/mat.h"
extern Mat4 g_guiProjectionMatrix;

OpenGLShaderSprite::OpenGLShaderSprite()
{

}

OpenGLShaderSprite::~OpenGLShaderSprite()
{
	if( m_VAO )
		gglDeleteVertexArrays(1,&m_VAO);
	if( m_program )
		gglDeleteProgram(m_program);
}

bool OpenGLShaderSprite::init()
{
	const char * text_v = 
		"#version 130\n"
		"in vec3 Position;\n"
		"in vec2 UV;\n"
		"uniform mat4 ProjMtx;\n"
		"uniform vec4 SpritePosition;\n"
		"out vec2 out_UV;\n"
		"void main(){\n"
		"    gl_Position = ProjMtx * vec4(Position.xyz + SpritePosition.xyz,1);\n"
		"    out_UV = UV;\n"
		"}\n";
	const char * text_f = 
		"#version 130\n" 
		"in vec2 out_UV;\n"
		"uniform sampler2D Texture;\n"
		"out vec4 Out_Color;\n"
		"void main(){\n"
		"    Out_Color = texture(Texture,out_UV) * vec4(1.f,1.f,1.f,1.f);\n"
		"}\n";
	if( !createShader(text_v, text_f, nullptr, m_program) )
		return false;

	glUseProgram(m_program);
	m_uniform_ProjMtx = glGetUniformLocation(m_program, "ProjMtx");
	m_uniform_SpritePosition = glGetUniformLocation(m_program, "SpritePosition");
	
	glUniform1i(glGetUniformLocation(m_program, "Texture"), 0); 

	glGenVertexArrays(1, &m_VAO);

	return true;
}