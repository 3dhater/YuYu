#ifndef YY_OPENGL_SHADER_POINT_H__
#define YY_OPENGL_SHADER_POINT_H__

class OpenGLShaderPoint
{
public:
	OpenGLShaderPoint();
	~OpenGLShaderPoint();

	GLuint m_program;
	GLuint m_VAO;
	GLint m_uniform_W;
	GLint m_uniform_P;
	GLint m_uniform_V;
	GLint m_uniform_Eye;

	bool init();
};
class OpenGLShaderPointAnimated
{
public:
	OpenGLShaderPointAnimated();
	~OpenGLShaderPointAnimated();

	GLuint m_program;
	GLuint m_VAO;
	GLint m_uniform_W;
	GLint m_uniform_P;
	GLint m_uniform_V;
	GLint m_uniform_Bones;

	bool init();
};

#endif