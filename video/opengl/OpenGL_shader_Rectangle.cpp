#include "yy.h"

#include "OpenGL.h"

#include "OpenGL_shader.h"
#include "OpenGL_shader_Rectangle.h"

#include "math/mat.h"
extern Mat4 g_guiProjectionMatrix;

OpenGLShaderRectangle::OpenGLShaderRectangle()
	:
	m_program(0),
	m_VAO(0),
	m_uniform_ProjMtx(0),
	m_uniform_Color1(0),
	m_uniform_Color2(0),
	m_uniform_Corners(0)
{
	YY_DEBUG_PRINT_FUNC;
}

OpenGLShaderRectangle::~OpenGLShaderRectangle(){
	YY_DEBUG_PRINT_FUNC;
	if( m_VAO )
		gglDeleteVertexArrays(1,&m_VAO);
	if( m_program )
		gglDeleteProgram(m_program);
}

bool OpenGLShaderRectangle::init(){
	YY_DEBUG_PRINT_FUNC;
	const char * text_v =
		"#version 330\n"
		"uniform mat4 ProjMtx;\n"
		"uniform vec4 Corners;\n"
		"uniform vec4 Color1;\n"
		"uniform vec4 Color2;\n"
		"out vec4 out_color;\n"
		"void main(){\n"
			"vec4 vertices[6] = vec4[6](\n"
									"vec4( Corners.z,  Corners.w,  0, 1.0),\n"
                                    "vec4( Corners.x,  Corners.w,  0, 1.0),\n"
                                    "vec4( Corners.z,  Corners.y,  0, 1.0),\n"
									"vec4( Corners.x,  Corners.w,  0, 1.0),\n"
									"vec4( Corners.x,  Corners.y,  0, 1.0),\n"
									"vec4( Corners.z,  Corners.y, 0,  1.0)\n"
										");\n"
		"gl_Position = ProjMtx * vertices[gl_VertexID];\n"
		"switch( gl_VertexID ){\n"
		"case 0:out_color = Color2;break;\n"
		"case 1:out_color = Color2;break;\n"
		"case 3:out_color = Color2;break;\n"
		"default:out_color = Color1;break;\n"
		"}\n"
		"}\n";
	const char * text_f = 
		"#version 330\n" 
		"in vec4 out_color;\n"
		"out vec4 color;\n"
		"void main(){\n"
		"    color = vec4(out_color.xyz, 1.f);\n"
		"}\n";
	if( !createShader(text_v, text_f, nullptr, m_program) )
		return false;

	glUseProgram(m_program);
	m_uniform_ProjMtx = glGetUniformLocation(m_program, "ProjMtx");
	m_uniform_Color1 = glGetUniformLocation(m_program, "Color1");
	m_uniform_Color2 = glGetUniformLocation(m_program, "Color2");
	m_uniform_Corners = glGetUniformLocation(m_program, "Corners");
	glGenVertexArrays(1, &m_VAO);

	return true;
}