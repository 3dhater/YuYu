#ifndef YY_OPENGL_SHADER_SPRITE_H__
#define YY_OPENGL_SHADER_SPRITE_H__

class OpenGLShaderSprite
{
public:
	OpenGLShaderSprite();
	~OpenGLShaderSprite();

	GLuint m_program = 0;
	GLuint m_VAO = 0;

	GLint m_uniform_ProjMtx = 0;
	GLint m_uniform_WorldMtx = 0;
	GLint m_uniform_CameraPosition = 0;
	GLint m_uniform_CameraScale = 0;
	GLint m_uniform_uv1 = 0;
	GLint m_uniform_uv2 = 0;
	GLint m_uniform_flags = 0;
	GLint m_uniform_Texture = 0;

	bool init();
};

#endif