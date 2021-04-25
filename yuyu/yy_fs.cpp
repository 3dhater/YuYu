#include "yy.h"
#include "yy_fs.h"

#ifdef YY_PLATFORM_WINDOWS
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#else
#error Need implement
#endif

#include <fstream>

#ifdef YY_PLATFORM_WINDOWS
HANDLE		g_hFind = 0;
#endif

yyFileSystemInternal::yyFileSystemInternal(){
	YY_DEBUG_PRINT_FUNC;
}
yyFileSystemInternal::~yyFileSystemInternal(){
	YY_DEBUG_PRINT_FUNC;
#ifdef YY_PLATFORM_WINDOWS
	if (g_hFind)
		FindClose(g_hFind);
#endif
}

bool yyFileSystemInternal::exists(const yy_fs::path& p) {
	bool result = false;
#ifdef YY_PLATFORM_WINDOWS
	DWORD dwAttrib = GetFileAttributes(p.string_type.c_str());
	result = (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)); // dir
	if (!result)
	{
		WIN32_FIND_DATA FindFileData;
		HANDLE handle = FindFirstFile(p.string_type.c_str(), &FindFileData); // file
		int found = handle != INVALID_HANDLE_VALUE;
		if (found) {
			FindClose(handle);
			return true;
		}
	}
	else
		return true;
#else
#error Need implement
#endif
	return false;
}

size_t yyFileSystemInternal::file_size(const yy_fs::path& p) {
	std::ifstream f(p.string_type.c_str(), std::ios::binary | std::ios::ate);
	return (size_t)f.tellg();
}

yy_fs::path yyFileSystemInternal::current_path(){
	yy_fs::path result;
#ifdef YY_PLATFORM_WINDOWS
	TCHAR szFileName[MAX_PATH];
	GetModuleFileName(NULL, szFileName, MAX_PATH);
	result.string_type = szFileName;
	util::stringPopBackBefore(result.string_type, '\\');
#else
#error Need implement
#endif
	return result;
}

bool yyFileSystemInternal::create_directory(const yy_fs::path& p) {
	yy_fs::path _p = p;
	util::stringFlipSlashBackSlash(_p.string_type);
#ifdef YY_PLATFORM_WINDOWS
	if (CreateDirectory(_p.string_type.c_str(), 0) != 0)
		return true;
#else
#error Need implement
#endif
	return false;
}

void yyFileSystemInternal::directory_iterator_next_entry(yy_fs::directory_iterator* di) {
#ifdef YY_PLATFORM_WINDOWS
	WIN32_FIND_DATA ffd;
	if (FindNextFile(g_hFind, &ffd) == FALSE)
	{
		if (di->m_isRecursiveIterator)
		{
			auto listHead = di->m_subDirs.head();
			if (listHead)
			{
				di->m_path.string_type = listHead->m_data.c_str();
				di->m_path.string_type += L"\\";
				if (g_hFind)
					FindClose(g_hFind);
				di->begin();
				di->m_subDirs.erase_node(listHead);
			}
			else
			{
				di->m_begin.m_endSearch = true;
			}
		}
		else
		{
			di->m_begin.m_endSearch = true;
		}
	}
	else
	{
		di->m_begin.m_path = di->m_startPath;
		di->m_begin.m_path.string_type += ffd.cFileName;
		yyFileSystemInternal::directory_entry_check(&di->m_begin);

		if (di->m_isRecursiveIterator)
		{
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
				&& (lstrcmpW(L"..", ffd.cFileName) != 0)
				)
			{
				di->m_subDirs.push_back(di->m_begin.m_path.string_type);
			}
		}
	}
#else
#error Need implement
#endif
}

bool yyFileSystemInternal::directory_iterator_begin(yy_fs::directory_iterator* di) {
	di->m_startPath = di->m_path;
#ifdef YY_PLATFORM_WINDOWS
	WIN32_FIND_DATA ffd;
	di->m_path.string_type += "*";
	g_hFind = FindFirstFile((wchar_t*)di->m_path.string_type.c_str(), &ffd);
	if (INVALID_HANDLE_VALUE == g_hFind)
	{
		DWORD error = GetLastError();
		yyLogWriteError("Can't scan dir. Error code [%u].\n", error);
		return false;
	}

	di->m_begin.m_path = ffd.cFileName;
	yyFileSystemInternal::directory_entry_check(&di->m_begin);
#else
#error Need implement
#endif
	return true;
}

void yyFileSystemInternal::directory_entry_check(yy_fs::directory_entry* de) {
	if (de->m_path.string_type.size())
	{
		if (!exists(de->m_path))
		{
			if (de->m_path.string_type[de->m_path.string_type.size() - 1] == '\\')
				de->m_path.string_type.pop_back();
		}
	}
}