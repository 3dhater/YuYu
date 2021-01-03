#ifndef YY_OPENGL_MODEL_H__
#define YY_OPENGL_MODEL_H__

#include "yy_material.h"

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

	yyMaterial m_material;
};

#endif