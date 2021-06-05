#include "yy.h"

#include "OpenGL.h"

#include "OpenGL_shader.h"
#include "OpenGL_shader_sprite.h"

#include "math/mat.h"
extern Mat4 g_guiProjectionMatrix;

OpenGLShaderSprite::OpenGLShaderSprite()
	:
	m_programSprite(0),
	m_programBillboard(0),
	m_uniform_ProjMtx(0),
	m_uniform_WorldMtx(0),
	m_uniform_CameraPosition(0),
	m_uniform_CameraScale(0),
	m_uniform_uv1(0),
	m_uniform_uv2(0),
	m_uniform_flags(0),
	m_uniform_Texture(0)
{
}

OpenGLShaderSprite::~OpenGLShaderSprite(){
	if(m_programSprite)
		glDeleteProgram(m_programSprite);
}

bool OpenGLShaderSprite::init(){
	{
		const char * text_v =
			"#version 330\n"
			"layout(location = 0) in vec2 Position;\n"
			"layout(location = 1) in vec2 UV;\n"
			"uniform mat4 WVP;\n"
			"uniform vec2 uv1;\n"
			"uniform vec2 uv2;\n"
			"uniform int flags;\n"
			"out vec2 out_UV;\n"
			"#define FLAG_INVERT_X 1\n"
			"#define FLAG_INVERT_Y 2\n"
			"void main(){\n"
			"    gl_Position = WVP * vec4(Position.x, 0.f - Position.y, 0.f, 1.f);\n"
			"    out_UV = UV;\n"
			"    if((flags & FLAG_INVERT_X) == FLAG_INVERT_X){\n"
			"	    if((flags & FLAG_INVERT_Y) == FLAG_INVERT_Y){\n"
			"		   switch(gl_VertexID){\n"
			"			 case 2:\n"
			"				out_UV.x = uv1.x;\n"
			"				out_UV.y = uv2.y;\n"
			"			 break;\n"
			"			case 3:\n"
			"				out_UV.x = uv1.x;\n"
			"				out_UV.y = uv1.y;\n"
			"		    break;\n"
			"			case 0:\n"
			"				out_UV.x = uv2.x;\n"
			"				out_UV.y = uv1.y;\n"
			"			 break;\n"
			"			case 1:\n"
			"				out_UV.x = uv2.x;\n"
			"				out_UV.y = uv2.y;\n"
			"		    break;\n"
			"			}\n"
			"		}else{\n"
			"			switch(gl_VertexID){\n"
			"			 case 3:\n"
			"				out_UV.x = uv1.x;\n"
			"				out_UV.y = uv2.y;\n"
			"			 break;\n"
			"		    case 2:\n"
			"				out_UV.x = uv1.x;\n"
			"				out_UV.y = uv1.y;\n"
			"		    break;\n"
			"			 case 1:\n"
			"				out_UV.x = uv2.x;\n"
			"				out_UV.y = uv1.y;\n"
			"			 break;\n"
			"		    case 0:\n"
			"				out_UV.x = uv2.x;\n"
			"				out_UV.y = uv2.y;\n"
			"		    break;\n"
			"			}\n"
			"		}\n"
			"    }else if((flags & FLAG_INVERT_Y) == FLAG_INVERT_Y){\n"
			"	   switch(gl_VertexID){\n"
			"		 case 1:\n"
			"			out_UV.x = uv1.x;\n"
			"			out_UV.y = uv2.y;\n"
			"		 break;\n"
			"	    case 0:\n"
			"			out_UV.x = uv1.x;\n"
			"			out_UV.y = uv1.y;\n"
			"	    break;\n"
			"		case 3:\n"
			"			out_UV.x = uv2.x;\n"
			"			out_UV.y = uv1.y;\n"
			"		 break;\n"
			"	    case 2:\n"
			"			out_UV.x = uv2.x;\n"
			"			out_UV.y = uv2.y;\n"
			"	    break;\n"
			"		}\n"
			"    }else{\n"
			"	   switch(gl_VertexID){\n"
			"		 case 0:\n"
			"			out_UV.x = uv1.x;\n"
			"			out_UV.y = uv2.y;\n"
			"		 break;\n"
			"	    case 1:\n"
			"			out_UV.x = uv1.x;\n"
			"			out_UV.y = uv1.y;\n"
			"	    break;\n"
			"		 case 2:\n"
			"			out_UV.x = uv2.x;\n"
			"			out_UV.y = uv1.y;\n"
			"		 break;\n"
			"	    case 3:\n"
			"			out_UV.x = uv2.x;\n"
			"			out_UV.y = uv2.y;\n"
			"	    break;\n"
			"		}\n"
			"    }\n"
			"}\n";
		const char * text_f =
			"#version 330\n"
			"in vec2 out_UV;\n"
			"uniform sampler2D Texture;\n"
			"out vec4 Out_Color;\n"
			"void main(){\n"
			"    Out_Color = texture(Texture,out_UV) * vec4(1.f,1.f,1.f,1.f);\n"
			"    if(Out_Color.a < 0.1) discard;\n"
			"}\n";
		if (!createShader(text_v, text_f, nullptr, m_programBillboard))
			return false;

		glUseProgram(m_programBillboard);
		m_uniform_WVP = glGetUniformLocation(m_programBillboard, "WVP");
		m_uniform_uv1_b = glGetUniformLocation(m_programBillboard, "uv1");
		m_uniform_uv2_b = glGetUniformLocation(m_programBillboard, "uv2");
		m_uniform_flags_b = glGetUniformLocation(m_programBillboard, "flags");
		glUniform1i(glGetUniformLocation(m_programBillboard, "Texture"), 0);
	}
	{
		const char * text_v =
			"#version 330\n"
			"layout(location = 0) in vec2 Position;\n"
			"layout(location = 1) in vec2 UV;\n"
			"uniform mat4 ProjMtx;\n"
			"uniform mat4 World;\n"
			"uniform vec2 CameraPosition;\n"
			"uniform vec2 CameraScale;\n"
			"uniform vec2 uv1;\n"
			"uniform vec2 uv2;\n"
			"uniform int flags;\n"
			"out vec2 out_UV;\n"
			"#define FLAG_INVERT_X 1\n"
			"#define FLAG_INVERT_Y 2\n"
			"void main(){\n"
			"    gl_Position = (ProjMtx * World) * vec4(Position.x, Position.y, 1.f, 1.f);\n"
			"    vec4 camPos = ProjMtx * vec4(CameraPosition.x, CameraPosition.y, 1.f, 1.f);"
			"    gl_Position.x -= camPos.x;\n"
			"    gl_Position.y -= camPos.y;\n"
			"    gl_Position.x *= CameraScale.x;\n"
			"    gl_Position.y *= CameraScale.y;\n"
			"    out_UV = UV;\n"
			"    if((flags & FLAG_INVERT_X) == FLAG_INVERT_X){\n"
			"	    if((flags & FLAG_INVERT_Y) == FLAG_INVERT_Y){\n"
			"		   switch(gl_VertexID){\n"
			"			 case 2:\n"
			"				out_UV.x = uv1.x;\n"
			"				out_UV.y = uv2.y;\n"
			"			 break;\n"
			"			case 3:\n"
			"				out_UV.x = uv1.x;\n"
			"				out_UV.y = uv1.y;\n"
			"		    break;\n"
			"			case 0:\n"
			"				out_UV.x = uv2.x;\n"
			"				out_UV.y = uv1.y;\n"
			"			 break;\n"
			"			case 1:\n"
			"				out_UV.x = uv2.x;\n"
			"				out_UV.y = uv2.y;\n"
			"		    break;\n"
			"			}\n"
			"		}else{\n"
			"			switch(gl_VertexID){\n"
			"			 case 3:\n"
			"				out_UV.x = uv1.x;\n"
			"				out_UV.y = uv2.y;\n"
			"			 break;\n"
			"		    case 2:\n"
			"				out_UV.x = uv1.x;\n"
			"				out_UV.y = uv1.y;\n"
			"		    break;\n"
			"			 case 1:\n"
			"				out_UV.x = uv2.x;\n"
			"				out_UV.y = uv1.y;\n"
			"			 break;\n"
			"		    case 0:\n"
			"				out_UV.x = uv2.x;\n"
			"				out_UV.y = uv2.y;\n"
			"		    break;\n"
			"			}\n"
			"		}\n"
			"    }else if((flags & FLAG_INVERT_Y) == FLAG_INVERT_Y){\n"
			"	   switch(gl_VertexID){\n"
			"		 case 1:\n"
			"			out_UV.x = uv1.x;\n"
			"			out_UV.y = uv2.y;\n"
			"		 break;\n"
			"	    case 0:\n"
			"			out_UV.x = uv1.x;\n"
			"			out_UV.y = uv1.y;\n"
			"	    break;\n"
			"		case 3:\n"
			"			out_UV.x = uv2.x;\n"
			"			out_UV.y = uv1.y;\n"
			"		 break;\n"
			"	    case 2:\n"
			"			out_UV.x = uv2.x;\n"
			"			out_UV.y = uv2.y;\n"
			"	    break;\n"
			"		}\n"
			"    }else{\n"
			"	   switch(gl_VertexID){\n"
			"		 case 0:\n"
			"			out_UV.x = uv1.x;\n"
			"			out_UV.y = uv2.y;\n"
			"		 break;\n"
			"	    case 1:\n"
			"			out_UV.x = uv1.x;\n"
			"			out_UV.y = uv1.y;\n"
			"	    break;\n"
			"		 case 2:\n"
			"			out_UV.x = uv2.x;\n"
			"			out_UV.y = uv1.y;\n"
			"		 break;\n"
			"	    case 3:\n"
			"			out_UV.x = uv2.x;\n"
			"			out_UV.y = uv2.y;\n"
			"	    break;\n"
			"		}\n"
			"    }\n"
			"}\n";
		const char * text_f =
			"#version 330\n"
			"in vec2 out_UV;\n"
			"uniform sampler2D Texture;\n"
			"out vec4 Out_Color;\n"
			"void main(){\n"
			"    Out_Color = texture(Texture,out_UV) * vec4(1.f,1.f,1.f,1.f);\n"
			"    if(Out_Color.a < 0.1) discard;\n"
			"}\n";
		if (!createShader(text_v, text_f, nullptr, m_programSprite))
			return false;

		glUseProgram(m_programSprite);
		m_uniform_ProjMtx = glGetUniformLocation(m_programSprite, "ProjMtx");
		m_uniform_WorldMtx = glGetUniformLocation(m_programSprite, "World");
		m_uniform_CameraPosition = glGetUniformLocation(m_programSprite, "CameraPosition");
		m_uniform_CameraScale = glGetUniformLocation(m_programSprite, "CameraScale");
		m_uniform_uv1 = glGetUniformLocation(m_programSprite, "uv1");
		m_uniform_uv2 = glGetUniformLocation(m_programSprite, "uv2");
		m_uniform_flags = glGetUniformLocation(m_programSprite, "flags");

		glUniform1i(glGetUniformLocation(m_programSprite, "Texture"), 0);
	}
	

	return true;
}