#ifndef YY_OPENGL_SHADER_RECT_H__
#define YY_OPENGL_SHADER_RECT_H__

class OpenGLShaderRectangle
{
public:
	OpenGLShaderRectangle();
	~OpenGLShaderRectangle();

	GLuint m_program;
	GLuint m_VAO;

	GLint m_uniform_ProjMtx;
	GLint m_uniform_Corners;
	GLint m_uniform_Color1;
	GLint m_uniform_Color2;

	bool init();
};

#endif