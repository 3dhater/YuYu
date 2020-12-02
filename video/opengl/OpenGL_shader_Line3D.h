#ifndef YY_OPENGL_SHADER_LINE3D_H__
#define YY_OPENGL_SHADER_LINE3D_H__

class OpenGLShaderLine3D
{
public:
	OpenGLShaderLine3D();
	~OpenGLShaderLine3D();

	GLuint m_program = 0;
	GLuint m_VAO = 0;

	GLint m_uniform_ProjMtx = 0;
	GLint m_uniform_P1 = 0;
	GLint m_uniform_P2 = 0;
	GLint m_uniform_Color = 0;

	bool init();
};

#endif