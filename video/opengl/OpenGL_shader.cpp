#include "yy.h"

#include "OpenGL.h"

#include "OpenGL_shader.h"

#include "math/mat.h"

GLuint _createShader(GLenum type, const char * text);
GLuint _createShaderProgram(GLuint v, GLuint f, GLuint g);
bool _checkShader(GLuint shader);
bool _checkProgram(GLuint program);

bool createShader( const char * v, const char * f, const char * g, GLuint& out_program ){
	YY_DEBUG_PRINT_FUNC;
	auto shader_vert = _createShader(GL_VERTEX_SHADER,v);
	if(!_checkShader(shader_vert))
	{
		return false;
	}
	auto shader_frag = _createShader(GL_FRAGMENT_SHADER,f);
	if(!_checkShader(shader_frag))
	{
		return false;
	}

	GLuint shader_geom = 0;
	if( g )
	{
		shader_geom = _createShader(GL_GEOMETRY_SHADER,g);
		if(!_checkShader(shader_geom))
		{
			return false;
		}
	}

	out_program = _createShaderProgram(shader_vert,shader_frag, shader_geom);
	if(!_checkProgram(out_program))
	{
		return false;
	}

	gglDeleteShader(shader_vert);
    gglDeleteShader(shader_frag);
    if(shader_geom != 0)
		gglDeleteShader(shader_geom);
	return true;
}
bool _checkProgram(GLuint program){
	YY_DEBUG_PRINT_FUNC;
	GLint link_result = 0;
	gglGetProgramiv(program, GL_LINK_STATUS, &link_result);
	if (link_result == GL_FALSE)
	{
		GLint loglen=0;
		gglGetProgramiv(program, GL_INFO_LOG_LENGTH, &loglen);
		char * buf = new char[loglen+1];
		buf[loglen]=0;
		gglGetProgramInfoLog(program, loglen, NULL, buf);
		yyLogWriteError("Shader error: %s\n", buf);
		delete[] buf;
		return false;
	}
	return true;
}
GLuint _createShader(GLenum type, const char * text){
	YY_DEBUG_PRINT_FUNC;
	auto shader = gglCreateShader(type);
	gglShaderSource(shader, 1, &text, nullptr );
	gglCompileShader(shader);
	return shader;
}
GLuint _createShaderProgram(GLuint v, GLuint f, GLuint g){
	YY_DEBUG_PRINT_FUNC;
	auto program = gglCreateProgram();
	gglAttachShader(program, v);
	gglAttachShader(program, f);
	if( g )
		gglAttachShader(program, g);
	gglLinkProgram(program);
	return program;
}
bool _checkShader(GLuint shader){
	YY_DEBUG_PRINT_FUNC;
	GLint compile_result = 0;
	gglGetShaderiv(shader, GL_COMPILE_STATUS, &compile_result);
	if(compile_result == GL_FALSE)
	{
		GLint loglen=0;
		gglGetShaderiv(shader, GL_INFO_LOG_LENGTH, &loglen);
		char * buf = new char[loglen+1];
		buf[loglen]=0;
		gglGetShaderInfoLog(shader, loglen, NULL, buf);
		yyLogWriteError("Shader error: %s\n", buf);
		delete[] buf;
		return false;
	}
	return true;
}
