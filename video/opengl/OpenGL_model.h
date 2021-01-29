#ifndef YY_OPENGL_MODEL_H__
#define YY_OPENGL_MODEL_H__

#include "yy_material.h"
#include "yy_model.h"

class OpenGLModel
{
public:
	OpenGLModel();
	~OpenGLModel();
	GLuint m_VAO;
	GLuint m_vBuffer;
	GLuint m_iBuffer;
	GLsizei m_iCount;
	GLenum m_indexType;

	yyVertexType m_vertexType;
	yyMaterial m_material;
};

#endif