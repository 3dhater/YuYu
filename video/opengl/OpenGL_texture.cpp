#include "yy.h"

#include "OpenGL.h"

#include "OpenGL_texture.h"

OpenGLTexture::OpenGLTexture()
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