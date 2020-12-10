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
		gglBindFramebuffer(GL_FRAMEBUFFER, 0);
		gglDeleteFramebuffers(1, &m_FBO);
	}
	if( m_depthRBO ) gglDeleteRenderbuffers(1, &m_depthRBO);
	if( m_texture ) gglDeleteTextures(1, &m_texture);
}