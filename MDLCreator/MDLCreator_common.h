#ifndef __MDLCREATOR_COMMON_H_
#define __MDLCREATOR_COMMON_H_

void ImportSMD(yyMDLObject* object, const char* file);
void ImportOBJ(yyMDLObject* object, const char* file);
void ImportAssimp(yyMDLObject* object, const char* fileName);

yyStringA AnimationCheckName(yyMDLObject* object, yyStringA name);
yyStringA ModelCheckName(yyMDLObject* object, yyStringA name);

// для того чтобы настраивать модель в редакторе
// например, можно изменить позицию
// При сохранении нужно будет учитывать эти данные
struct LayerInfo
{
	LayerInfo()
	{
		m_texturePathTextBufferSize = 512;
		m_texturePathTextBuffer.reserve(m_texturePathTextBufferSize);
	}
	v3f m_offset;
	yyStringA m_texturePathTextBuffer;
	u32 m_texturePathTextBufferSize;
};

#endif