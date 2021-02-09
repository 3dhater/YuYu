#ifndef _YY_FILE_BUFFER_H_
#define _YY_FILE_BUFFER_H_

#include "strings/utils.h"
#include <cassert>

class yyFileBuffer
{
	u8* m_data;
	u32 m_dataSize;
	u32 m_cursorPosition;
public:
	yyFileBuffer()
	{
		m_data = 0;
		clear();
	}
	~yyFileBuffer()
	{
		clear();
	}

	void clear()
	{
		if (m_data)
			yyMemFree(m_data);
		m_data = 0;
		m_dataSize = 0;
		m_cursorPosition = 0;
	}

	bool initFromFile(const char* path)
	{
		FILE* f = fopen(path, "rb");
		if (!f)
		{
			YY_PRINT_FAILED;
			return false;
		}

		fseek(f, 0, SEEK_END);
		m_dataSize = ftell(f);
		fseek(f, 0, SEEK_SET);

		m_data = (u8*)yyMemAlloc(m_dataSize);
		fread(m_data, m_dataSize, 1, f);
		fclose(f);
	}

	void read(void* _Buffer, size_t _ElementSize)
	{
		u8 * ptr = (u8*)_Buffer;
		for (size_t i = 0; i < _ElementSize; ++i)
		{
			ptr[i] = m_data[m_cursorPosition + i];
		}
		m_cursorPosition += _ElementSize;
	}

	enum SeekPos
	{
		SeekPos_Begin,
		SeekPos_Current,
		SeekPos_End
	};

	void seek(long _Offset, SeekPos seekPos)
	{
		switch (seekPos)
		{
		case yyFileBuffer::SeekPos_Begin:
			m_cursorPosition = _Offset;
			break;
		case yyFileBuffer::SeekPos_Current:
			m_cursorPosition += _Offset;
			break;
		case yyFileBuffer::SeekPos_End:
			m_cursorPosition = m_dataSize;
			break;
		}
	}

	u32 tell() { return m_cursorPosition; }

	u8 getNextSymbol()
	{
		u8 chr = m_data[m_cursorPosition];
		++m_cursorPosition;
		return chr;
	}

	bool isEOF() { return m_cursorPosition == m_dataSize; }
};

#endif