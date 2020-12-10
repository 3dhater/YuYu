#ifndef YY_OPENGL_SHADER_SPRITE_H__
#define YY_OPENGL_SHADER_SPRITE_H__

class OpenGLShaderSprite
{
public:
	OpenGLShaderSprite();
	~OpenGLShaderSprite();

	GLuint m_program;
	GLuint m_VAO;

	GLint m_uniform_ProjMtx;
	GLint m_uniform_WorldMtx;
	GLint m_uniform_CameraPosition;
	GLint m_uniform_CameraScale;
	GLint m_uniform_uv1;
	GLint m_uniform_uv2;
	GLint m_uniform_flags;
	GLint m_uniform_Texture;

	bool init();
};

#endif