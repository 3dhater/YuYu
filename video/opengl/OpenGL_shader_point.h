#ifndef YY_OPENGL_SHADER_POINT_H__
#define YY_OPENGL_SHADER_POINT_H__

class OpenGLShaderPoint
{
public:
	OpenGLShaderPoint();
	~OpenGLShaderPoint();

	GLuint m_program;
	GLuint m_VAO;
	GLint m_uniform_WVP;

	bool init();
};
class OpenGLShaderPointAnimated
{
public:
	OpenGLShaderPointAnimated();
	~OpenGLShaderPointAnimated();

	GLuint m_program;
	GLuint m_VAO;
	GLint m_uniform_WVP;
	GLint m_uniform_Bones;

	bool init();
};

#endif