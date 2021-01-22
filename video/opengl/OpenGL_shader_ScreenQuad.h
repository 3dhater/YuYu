#ifndef YY_OPENGL_SHADER_SCREENQUAD_H__
#define YY_OPENGL_SHADER_SCREENQUAD_H__

class OpenGLShaderScreenQuad 
{
public:
	OpenGLShaderScreenQuad();
	~OpenGLShaderScreenQuad();

	GLuint m_program;
	GLuint m_VAO;

	GLint m_uniform_Texture;

	bool init();
};

#endif