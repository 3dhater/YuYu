#ifndef YY_OPENGL_TEXTURE_H__
#define YY_OPENGL_TEXTURE_H__

class OpenGLTexture : public yyResourceImplementation
{
public:
	OpenGLTexture();
	virtual ~OpenGLTexture();

	virtual void Load(yyResourceData*);
	virtual void Unload();
	virtual void GetTextureSize(v2f*);
	virtual void GetTextureHandle(void**);
	virtual void MapModelForWriteVerts(u8** v_ptr);
	virtual void UnmapModelForWriteVerts();
	
	GLuint m_texture;
	GLuint m_depthRBO;
	GLuint m_FBO;

	u32 m_h;
	u32 m_w;
};

#endif