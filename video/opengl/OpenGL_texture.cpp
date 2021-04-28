#include "yy.h"
#include "yy_image.h"

#include "OpenGL.h"

#include "OpenGL_texture.h"

OpenGLTexture::OpenGLTexture()
	:
	m_texture(0),
	m_depthRBO(0),
	m_FBO(0),
	m_h(0),
	m_w(0)
{
}

OpenGLTexture::~OpenGLTexture(){
	Unload();
}

void OpenGLTexture::Load(yyResourceData* rd) {
	GLint minFilter = 0;
	GLint magFilter = 0;

	auto tf = rd->m_imageData->m_filter;
	switch (tf)	{
	default:
	case yyTextureFilter::CMP_PPP:
	case yyTextureFilter::PPP:
		minFilter = GL_NEAREST;
		magFilter = GL_NEAREST;
		break;
	case yyTextureFilter::CMP_PPL:
	case yyTextureFilter::PPL:
		minFilter = GL_NEAREST_MIPMAP_NEAREST;
		magFilter = GL_NEAREST;
		break;
	case yyTextureFilter::CMP_PLP:
	case yyTextureFilter::PLP:
		minFilter = GL_NEAREST_MIPMAP_NEAREST;
		magFilter = GL_LINEAR;
		break;
	case yyTextureFilter::CMP_PLL:
	case yyTextureFilter::PLL:
		minFilter = GL_NEAREST_MIPMAP_LINEAR;
		magFilter = GL_LINEAR;
		break;
	case yyTextureFilter::CMP_LPP:
	case yyTextureFilter::LPP:
		minFilter = GL_LINEAR_MIPMAP_NEAREST;
		magFilter = GL_NEAREST;
		break;
	case yyTextureFilter::CMP_LPL:
	case yyTextureFilter::LPL:
		minFilter = GL_NEAREST_MIPMAP_LINEAR;
		magFilter = GL_NEAREST;
		break;
	case yyTextureFilter::CMP_LLP:
	case yyTextureFilter::LLP:
		minFilter = GL_LINEAR_MIPMAP_NEAREST;
		magFilter = GL_LINEAR;
		break;
	case yyTextureFilter::ANISOTROPIC:
	case yyTextureFilter::CMP_ANISOTROPIC:
	case yyTextureFilter::CMP_LLL:
	case yyTextureFilter::LLL:
		minFilter = GL_LINEAR_MIPMAP_LINEAR;
		magFilter = GL_LINEAR;
		break;
	}

	GLint tam;
	switch (rd->m_imageData->m_addressMode)
	{
	case yyTextureAddressMode::Wrap:
	default:
		tam = GL_REPEAT;
		break;
	case yyTextureAddressMode::Mirror:
		tam = GL_MIRRORED_REPEAT;
		break;
	case yyTextureAddressMode::Clamp:
		tam = GL_CLAMP_TO_EDGE;
		break;
	case yyTextureAddressMode::Border:
		tam = GL_CLAMP_TO_BORDER;
		break;
	case yyTextureAddressMode::MirrorOnce:
		tam = GL_CLAMP_TO_BORDER;
		break;
	}

	GLint cmpFunc = 0;
	switch (rd->m_imageData->m_comparisonFunc)
	{
	case yyTextureComparisonFunc::Never:
		cmpFunc = GL_NEVER;
		break;
	case yyTextureComparisonFunc::Less:
		cmpFunc = GL_LESS;
		break;
	case yyTextureComparisonFunc::Equal:
		cmpFunc = GL_EQUAL;
		break;
	case yyTextureComparisonFunc::LessEqual:
		cmpFunc = GL_LEQUAL;
		break;
	case yyTextureComparisonFunc::Greater:
		cmpFunc = GL_GREATER;
		break;
	case yyTextureComparisonFunc::NotEqual:
		cmpFunc = GL_NOTEQUAL;
		break;
	case yyTextureComparisonFunc::GreaterEqual:
		cmpFunc = GL_GEQUAL;
		break;
	case yyTextureComparisonFunc::Always:
	default:
		cmpFunc = GL_ALWAYS;
		break;
	}

	if (rd->m_type == yyResourceType::Texture)
	{
		auto image = (yyImage*)rd->m_source;
		m_h = image->m_height;
		m_w = image->m_width;

		glGenTextures(1, &m_texture);
		glBindTexture(GL_TEXTURE_2D, m_texture);

		if (image->m_format == yyImageFormat::R8G8B8A8)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->m_width, image->m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image->m_data);
		}
		else
		{
			u32 format = 0;
			u32 blockSize = 16;

			switch (image->m_format)
			{
			case yyImageFormat::BC1:
				blockSize = 8;
				format = 0x83F1;
				break;
			case yyImageFormat::BC2:
				format = 0x83F2;
				break;
			case yyImageFormat::BC3:
				format = 0x83F3;
				break;
			default:
				break;
			}

			u32 offset = 0;

			auto width = image->m_width;
			auto height = image->m_width;
			// load the mipmaps 
			for (u32 level = 0; level < image->m_mipCount && (width || height); ++level)
			{
				u32 size = ((width + 3) / 4)*((height + 3) / 4)*blockSize;
				glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
					0, size, image->m_data + image->m_bitDataOffset + offset);

				offset += size;
				width /= 2;
				height /= 2;
			}
		}
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tam);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tam);
		if (tf > yyTextureFilter::ANISOTROPIC)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, cmpFunc);
			glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	}
	else
	{
		m_w = (u32)rd->m_imageData->m_size[0];
		m_h = (u32)rd->m_imageData->m_size[1];
		glGenFramebuffers(1, &m_FBO);
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

		glGenTextures(1, &m_texture);
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_w, m_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, tam);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, tam);
		if (tf > yyTextureFilter::ANISOTROPIC)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, cmpFunc);
		}
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
		glBindTexture(GL_TEXTURE_2D, 0);

		glGenRenderbuffers(1, &m_depthRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, m_depthRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_w, m_h);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRBO);
		glBindRenderbuffer(GL_RENDERBUFFER, 0);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);
		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depth, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}
void OpenGLTexture::Unload() {
	if (m_FBO)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &m_FBO);
		m_FBO = 0;
	}
	if (m_depthRBO) {
		glDeleteRenderbuffers(1, &m_depthRBO);
		m_depthRBO = 0;
	}
	if (m_texture) {
		glDeleteTextures(1, &m_texture);
		m_texture = 0;
	}
}

void OpenGLTexture::GetTextureSize(v2f* ptr) {
	ptr->x = (f32)m_w;
	ptr->y = (f32)m_h;
}
void OpenGLTexture::GetTextureHandle(void** ptr) {
	*ptr = &m_texture;
}
void OpenGLTexture::MapModelForWriteVerts(u8** v_ptr) {}
void OpenGLTexture::UnmapModelForWriteVerts() {}