#ifndef YY_OPENGL_MODEL_H__
#define YY_OPENGL_MODEL_H__

class OpenGLMeshBuffer
{
public:
	OpenGLMeshBuffer();
	~OpenGLMeshBuffer();
	GLuint m_VAO     = 0;
	GLuint m_vBuffer = 0;
	GLuint m_iBuffer = 0;
	GLsizei m_iCount = 0;
};

class OpenGLModel
{
public:
	OpenGLModel();
	~OpenGLModel();
	yyArraySmall<OpenGLMeshBuffer*> m_meshBuffers;
};

#endif