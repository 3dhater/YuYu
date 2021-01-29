#include "yy.h"

#include "OpenGL.h"

#include "OpenGL_model.h"

OpenGLModel::OpenGLModel()
	:
	m_VAO(0),
	m_vBuffer(0),
	m_iBuffer(0),
	m_iCount(0),
	m_vertexType(yyVertexType::Model)
{
}

OpenGLModel::~OpenGLModel()
{
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glDeleteBuffers(1, &m_vBuffer);
	glDeleteBuffers(1, &m_iBuffer);
	glDeleteVertexArrays(1,&m_VAO);
}

