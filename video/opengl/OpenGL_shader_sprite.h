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
	GLint m_uniform_SpritePosition = 0;
	GLint m_uniform_Texture = 0;

	bool init();
};

#endif