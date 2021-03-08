#ifndef YY_OPENGL_SHADER_SPRITE2_H__
#define YY_OPENGL_SHADER_SPRITE2_H__

class OpenGLShaderSprite2
{
public:
	OpenGLShaderSprite2();
	~OpenGLShaderSprite2();

	GLuint m_program;
	GLuint m_VAO;

	GLint m_uniform_ProjMtx;
	GLint m_uniform_WorldMtx;
	GLint m_uniform_CameraPosition;
	GLint m_uniform_CameraScale;
	GLint m_uniform_Texture;

	bool init();
};

#endif