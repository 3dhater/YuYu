#ifndef YY_OPENGL_TEXTURE_H__
#define YY_OPENGL_TEXTURE_H__

class OpenGLTexture
{
public:
	OpenGLTexture();
	~OpenGLTexture();
	
	GLuint m_texture   = 0;
	GLuint m_depthRBO = 0;
	GLuint m_FBO = 0;

	u32 m_h = 0;
	u32 m_w = 0;
};

#endif