#include "yy.h"

#include "OpenGL.h"

#include "OpenGL_shader.h"
#include "OpenGL_shader_sprite2.h"

#include "math/mat.h"
extern Mat4 g_guiProjectionMatrix;

OpenGLShaderSprite2::OpenGLShaderSprite2()
	:
	m_program(0),
	m_VAO(0),
	m_uniform_ProjMtx(0),
	m_uniform_WorldMtx(0),
	m_uniform_CameraPosition(0),
	m_uniform_CameraScale(0),
	m_uniform_Texture(0)
{
}

OpenGLShaderSprite2::~OpenGLShaderSprite2()
{
	if( m_VAO )
		gglDeleteVertexArrays(1,&m_VAO);
	if( m_program )
		gglDeleteProgram(m_program);
}

bool OpenGLShaderSprite2::init()
{
	const char * text_v =
		"#version 330\n"
		"layout(location = 0) in vec2 Position;\n"
		"layout(location = 1) in vec2 UV;\n"
		"uniform mat4 ProjMtx;\n"
		"uniform mat4 World;\n"
		"uniform vec2 CameraPosition;\n"
		"uniform vec2 CameraScale;\n"
		"uniform int flags;\n"
		"out vec2 out_UV;\n"
		"#define FLAG_INVERT_X 1\n"
		"#define FLAG_INVERT_Y 2\n"
		"void main(){\n"
		"    gl_Position = (ProjMtx * World) * vec4(Position.x, Position.y, 1.f, 1.f);\n"
		"    vec4 camPos = ProjMtx * vec4(CameraPosition.x, CameraPosition.y, 1.f, 1.f);"
		"    gl_Position.x -= camPos.x;\n"
		"    gl_Position.y -= camPos.y;\n"
		"    gl_Position.x *= CameraScale.x;\n"
		"    gl_Position.y *= CameraScale.y;\n"
		"    out_UV = UV;\n"
		"}\n";
	const char * text_f = 
		"#version 330\n" 
		"in vec2 out_UV;\n"
		"uniform sampler2D Texture;\n"
		"out vec4 Out_Color;\n"
		"void main(){\n"
		"    Out_Color = texture(Texture,out_UV) * vec4(1.f,1.f,1.f,1.f);\n"
		"    if(Out_Color.a < 0.1) discard;\n"
		"}\n";
	if( !createShader(text_v, text_f, nullptr, m_program) )
		return false;

	glUseProgram(m_program);
	m_uniform_ProjMtx = glGetUniformLocation(m_program, "ProjMtx");
	m_uniform_WorldMtx = glGetUniformLocation(m_program, "World");
	m_uniform_CameraPosition = glGetUniformLocation(m_program, "CameraPosition");
	m_uniform_CameraScale = glGetUniformLocation(m_program, "CameraScale");
	
	glUniform1i(glGetUniformLocation(m_program, "Texture"), 0); 

	glGenVertexArrays(1, &m_VAO);

	return true;
}