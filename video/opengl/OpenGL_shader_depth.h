#ifndef YY_OPENGL_SHADER_DEPTH_H__
#define YY_OPENGL_SHADER_DEPTH_H__

class OpenGLShaderDepth
{
public:
	OpenGLShaderDepth();
	~OpenGLShaderDepth();

	GLuint m_program;
	GLuint m_VAO;
	GLint m_uniform_World;
	GLint m_uniform_LightView;
	GLint m_uniform_LightProjection;

	bool init();
};

#endif