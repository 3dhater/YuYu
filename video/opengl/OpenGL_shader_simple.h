#ifndef YY_OPENGL_SHADER_SIMPLE_H__
#define YY_OPENGL_SHADER_SIMPLE_H__

class OpenGLShaderSimple
{
public:
	OpenGLShaderSimple();
	~OpenGLShaderSimple();

	GLuint m_program;
	GLuint m_VAO;
	GLint m_uniform_WVP;

	bool init();
};

#endif