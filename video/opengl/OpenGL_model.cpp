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

OpenGLModel::~OpenGLModel(){
	Unload();
}

void OpenGLModel::Load(yyResourceData* rd) {
	yyModel * model = (yyModel *)rd->m_source;

	//m_material = *model->m_material;
	m_vertexType = model->m_vertexType;

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
	glGenBuffers(1, &m_vBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vBuffer);
	glBufferData(GL_ARRAY_BUFFER, model->m_vCount * model->m_stride, model->m_vertices, GL_DYNAMIC_DRAW);

	if (model->m_vertexType == yyVertexType::GUI)
	{
		// POSITION
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, false, model->m_stride, 0);

		// TexCoords
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (2 * sizeof(float)));
	}
	else if (model->m_vertexType == yyVertexType::Model)
	{
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, model->m_stride, 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (3 * sizeof(float)));

		// Normals
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (5 * sizeof(float)));

		// binormal
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (8 * sizeof(float)));

		// tangent
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (11 * sizeof(float)));

		// color
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (14 * sizeof(float)));
	}
	else if (model->m_vertexType == yyVertexType::AnimatedModel)
	{
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, model->m_stride, 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (5 * sizeof(float)));
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (8 * sizeof(float)));
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (11 * sizeof(float)));
		
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (14 * sizeof(float)));

		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (18 * sizeof(float)));
		glEnableVertexAttribArray(7);
		glVertexAttribIPointer(7, 4, GL_INT, model->m_stride, (unsigned char*)NULL + (22 * sizeof(float)));
	}
	else if (model->m_vertexType == yyVertexType::LineModel)
	{
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, model->m_stride, 0);
		
		// Normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (3 * sizeof(float)));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (6 * sizeof(float)));
	}
	else if (model->m_vertexType == yyVertexType::AnimatedLineModel)
	{
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, model->m_stride, 0);
		
		// Normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (3 * sizeof(float)));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (6 * sizeof(float)));

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (10 * sizeof(float)));

		glEnableVertexAttribArray(4);
		glVertexAttribIPointer(4, 3, GL_INT, model->m_stride, (unsigned char*)NULL + (14 * sizeof(float)));
	}
	else if (model->m_vertexType == yyVertexType::Point)
	{
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, model->m_stride, 0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (3 * sizeof(float)));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (6 * sizeof(float)));
	}
	else if (model->m_vertexType == yyVertexType::AnimatedPoint)
	{
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, false, model->m_stride, 0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 4, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (7 * sizeof(float)));

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, false, model->m_stride, (unsigned char*)NULL + (10 * sizeof(float)));
		glEnableVertexAttribArray(4);
		glVertexAttribIPointer(4, 4, GL_INT, model->m_stride, (unsigned char*)NULL + (15 * sizeof(float)));
	}


	glGenBuffers(1, &m_iBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iBuffer);

	u32 index_sizeof = sizeof(u16);
	m_indexType = GL_UNSIGNED_SHORT;
	if (model->m_indexType == yyMeshIndexType::u32)
	{
		m_indexType = GL_UNSIGNED_INT;
		index_sizeof = sizeof(u32);
	}

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, model->m_iCount * index_sizeof, model->m_indices, GL_DYNAMIC_DRAW);

	m_iCount = model->m_iCount;

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void OpenGLModel::Unload() {
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	if (m_vBuffer)
	{
		glDeleteBuffers(1, &m_vBuffer);
		m_vBuffer = 0;
	}
	if (m_iBuffer)
	{
		glDeleteBuffers(1, &m_iBuffer);
		m_iBuffer = 0;
	}
	if (m_VAO)
	{
		glDeleteVertexArrays(1, &m_VAO);
		m_VAO = 0;
	}
}

void OpenGLModel::GetTextureSize(v2f*) {}
void OpenGLModel::GetTextureHandle(void**) {}

void OpenGLModel::MapModelForWriteVerts(u8** v_ptr) {
	glBindBuffer(GL_ARRAY_BUFFER, m_vBuffer);
	*v_ptr = (u8*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
}

void OpenGLModel::UnmapModelForWriteVerts() {
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}