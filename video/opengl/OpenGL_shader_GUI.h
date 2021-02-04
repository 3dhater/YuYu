#ifndef YY_OPENGL_SHADER_GUI_H__
#define YY_OPENGL_SHADER_GUI_H__

class OpenGLShaderGUI
{
public:
	OpenGLShaderGUI();
	~OpenGLShaderGUI();

	GLuint m_program;
	GLuint m_VAO;

	GLint m_uniform_ProjMtx;
	GLint m_uniform_Offset;
	GLint m_uniform_Texture;
	GLint m_uniform_Color;

	bool init();
};

#endif