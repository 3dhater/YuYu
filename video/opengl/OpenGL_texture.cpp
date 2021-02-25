#include "yy.h"

#include "OpenGL.h"

#include "OpenGL_texture.h"

OpenGLTexture::OpenGLTexture()
	:
	m_texture(0),
	m_depthRBO(0),
	m_FBO(0),
	m_h(0),
	m_w(0)
{
}

OpenGLTexture::~OpenGLTexture()
{
	if( m_FBO )
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &m_FBO);
	}
	if( m_depthRBO ) glDeleteRenderbuffers(1, &m_depthRBO);
	if( m_texture ) glDeleteTextures(1, &m_texture);
}