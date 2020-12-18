#ifndef YY_OPENGL_MODEL_H__
#define YY_OPENGL_MODEL_H__

class OpenGLMeshBuffer
{
public:
	OpenGLMeshBuffer();
	~OpenGLMeshBuffer();
	GLuint m_VAO;
	GLuint m_vBuffer;
	GLuint m_iBuffer;
	GLsizei m_iCount;
	GLenum m_indexType;
};

class OpenGLModel
{
public:
	OpenGLModel();
	~OpenGLModel();
	yyArraySmall<OpenGLMeshBuffer*> m_meshBuffers;
};

#endif