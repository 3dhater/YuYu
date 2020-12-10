#ifndef YY_OPENGL_SHADER_LINE3D_H__
#define YY_OPENGL_SHADER_LINE3D_H__

class OpenGLShaderLine3D
{
public:
	OpenGLShaderLine3D();
	~OpenGLShaderLine3D();

	GLuint m_program;
	GLuint m_VAO;

	GLint m_uniform_ProjMtx;
	GLint m_uniform_P1;
	GLint m_uniform_P2;
	GLint m_uniform_Color;

	bool init();
};

#endif