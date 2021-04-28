#ifndef YY_OPENGL_MODEL_H__
#define YY_OPENGL_MODEL_H__

#include "yy_material.h"
#include "yy_model.h"

class OpenGLModel : public yyResourceImplementation
{
public:
	OpenGLModel();
	virtual ~OpenGLModel();

	virtual void Load(yyResourceData*);
	virtual void Unload();
	virtual void GetTextureSize(v2f*);
	virtual void GetTextureHandle(void**);
	virtual void MapModelForWriteVerts(u8** v_ptr);
	virtual void UnmapModelForWriteVerts();

	GLuint m_VAO;
	GLuint m_vBuffer;
	GLuint m_iBuffer;
	GLsizei m_iCount;
	GLenum m_indexType;

	yyVertexType m_vertexType;
	//yyMaterial m_material;
};

#endif