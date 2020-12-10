#ifndef _YUYU_FS_H_
#define _YUYU_FS_H_

#include "strings/string.h"
#include "strings/utils.h"

#ifdef YY_PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#error Need implement
#endif

#include <fstream>

namespace yyFS
{
	YY_FORCE_INLINE bool existsFile(const char* fileName)
	{
#ifdef YY_PLATFORM_WINDOWS
		WIN32_FIND_DATAA FindFileData;
		HANDLE handle = FindFirstFileA( fileName, &FindFileData );
		int found = handle != INVALID_HANDLE_VALUE;
		if( found ){
			FindClose( handle );
			return true;
		}
#else
#error Need implement
#endif
		return false;
	}
	YY_FORCE_INLINE bool existsDir(const char* dir)
	{
#ifdef YY_PLATFORM_WINDOWS
		DWORD dwAttrib = GetFileAttributesA( dir );
		return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
#else
#error Need implement
#endif
		return false;
	}

	YY_FORCE_INLINE size_t file_size(const char* file)
	{
		std::ifstream f( file, std::ios::binary | std::ios::ate);
		return f.tellg();
	}
}


#endif