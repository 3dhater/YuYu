#ifndef YY_OPENGL_SHADER_GUI_H__
#define YY_OPENGL_SHADER_GUI_H__

class OpenGLShaderGUI
{
public:
	OpenGLShaderGUI();
	~OpenGLShaderGUI();

	GLuint m_program = 0;
	GLuint m_VAO = 0;

	GLint m_uniform_ProjMtx = 0;
	GLint m_uniform_Texture = 0;

	bool init();
};

#endif