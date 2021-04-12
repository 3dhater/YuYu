#ifndef YY_OPENGL_SHADER_LM_H__
#define YY_OPENGL_SHADER_LM_H__

class OpenGLShaderLineModel
{
public:
	OpenGLShaderLineModel();
	~OpenGLShaderLineModel();

	GLuint m_program;
	GLuint m_VAO;
	GLint m_uniform_WVP;
	GLint m_uniform_BaseColor;

	bool init();
};
class OpenGLShaderLineModelAnimated
{
public:
	OpenGLShaderLineModelAnimated();
	~OpenGLShaderLineModelAnimated();

	GLuint m_program;
	GLuint m_VAO;
	GLint m_uniform_WVP;
	GLint m_uniform_World;
	GLint m_uniform_Bones;
	GLint m_uniform_BaseColor;

	bool init();
};

#endif