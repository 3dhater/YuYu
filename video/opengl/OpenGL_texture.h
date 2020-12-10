#ifndef YY_OPENGL_TEXTURE_H__
#define YY_OPENGL_TEXTURE_H__

class OpenGLTexture
{
public:
	OpenGLTexture();
	~OpenGLTexture();
	
	GLuint m_texture;
	GLuint m_depthRBO;
	GLuint m_FBO;

	u32 m_h;
	u32 m_w;
};

#endif