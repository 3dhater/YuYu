#ifndef YY_OPENGL_SHADER_STD_H__
#define YY_OPENGL_SHADER_STD_H__

class OpenGLShaderStd
{
public:
	OpenGLShaderStd();
	~OpenGLShaderStd();

	GLuint m_program = 0;
	GLuint m_VAO = 0;

	GLint m_uniform_WVP = 0;
//	GLint m_uniform_diffuseTexture = 0;

	bool init();
};

#endif