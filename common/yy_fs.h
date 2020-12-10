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
	class path
	{
		void _checkBackSlash()
		{
			if(m_data.size())
			{
				util::stringFlipSlashBackSlash(m_data);
			}
		}
	public:
		path(){}
		path(const wchar_t* char16_text)
		{
			m_data = char16_text;
			_checkBackSlash();
		}
		path(const char16_t* char16_text)
		{
			m_data = (const wchar_t*)char16_text;
			_checkBackSlash();
		}
		path(const char* char_text)
		{
			yyStringA str(char_text);
			util::utf8_to_utf16(&str, &m_data);
			_checkBackSlash();
		}
		~path()
		{
		}

		bool has_extension()
		{
			if(m_data.size())
			{
				for( u32 i = m_data.size() - 1u; i >= 0u; --i )
				{
					auto c = m_data[ i ];
					if( c == '\\' || c == '.' )
						break;
					else return true;
					if( !i ) break;
				}
			}
			return false;
		}

		path extension()
		{
			path result;
			for( u32 i = m_data.size() - 1u; i >= 0u; --i )
			{
				auto c = m_data[ i ];
				if( c == '/' || c == '.' || c == '\\' )
					break;
				else result.m_data += c;
				if( !i ) break;
			}
			result.m_data += L'.';
			util::stringFlip(result.m_data);
			return result;
		}

		bool operator==(const path& other){ return m_data == other.m_data; }
		bool operator!=(const path& other){ return m_data != other.m_data; }
		
		std::string generic_string()
		{
			std::string result;
			util::utf16_to_utf8(&m_data, &result);
			return result;
		}

		yyStringW m_data;
	};

	YY_FORCE_INLINE bool exists(const path& p)
	{
		bool result = false;
#ifdef YY_PLATFORM_WINDOWS
		DWORD dwAttrib = GetFileAttributes( p.m_data.c_str() );
		result = (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)); // dir
		if(!result)
		{
			WIN32_FIND_DATA FindFileData;
			HANDLE handle = FindFirstFile( p.m_data.c_str(), &FindFileData ); // file
			int found = handle != INVALID_HANDLE_VALUE;
			if( found ){
				FindClose( handle );
				return true;
			}
		}
		else
			return true;
#else
#error Need implement
#endif

#ifdef YY_PLATFORM_WINDOWS
#else
#error Need implement
#endif
		return false;
	}

	YY_FORCE_INLINE size_t file_size(const path& p)
	{
		std::ifstream f( p.m_data.c_str(), std::ios::binary | std::ios::ate);
		return (size_t)f.tellg();
	}

	YY_FORCE_INLINE path current_path()
	{
		path result;
#ifdef YY_PLATFORM_WINDOWS
		TCHAR szFileName[MAX_PATH];
		GetModuleFileName( NULL, szFileName, MAX_PATH );
		result.m_data = szFileName;
		//util::stringFlipSlash(result.m_data);
		util::stringPopBackBefore(result.m_data, '\\' );
#else
#error Need implement
#endif
		return result;
	}

//	class directory_iterator;
	class directory_entry
	{
		void _check()
		{
			if(m_path.m_data.size())
			{
				if( !exists(m_path) )
				{
					if(m_path.m_data[m_path.m_data.size()-1] == '\\')
						m_path.m_data.pop_back();
				}
			}
		}
		friend class directory_iterator;
	public:
		directory_entry()
		{
	//		m_dirIt = nullptr;
			m_endSearch = false;
		}
		~directory_entry(){}

		bool operator==(const directory_entry& other)const
		{ 
			return other.m_endSearch == this->m_endSearch;
		}
		bool operator!=(const directory_entry& other)const 
		{
			return other.m_endSearch != this->m_endSearch;
		}
		bool operator==(directory_entry* other)const
		{
			return other->m_endSearch == this->m_endSearch;
		}
		bool operator!=(directory_entry* other)const
		{
			return other->m_endSearch != this->m_endSearch;
		}

		path m_path;
		const path& path()
		{
			return m_path;
		}


		bool m_endSearch;
	//	directory_iterator* m_dirIt;
	};

	class directory_iterator
	{
		class Iterator
		{
			friend class ConstIterator;
			directory_entry* m_dirEntry;
			directory_iterator* m_dirIt;
		public:
			Iterator() :m_dirIt(0) {}
			Iterator(directory_entry* o, directory_iterator* di) { m_dirEntry = o; m_dirIt = di; }
			~Iterator() {}
			Iterator& operator ++() { m_dirIt->_nextEntry(); return *this; }
			Iterator& operator --() { return *this; }
			Iterator  operator ++(s32) { Iterator tmp = *this; return tmp; }
			Iterator  operator --(s32) { Iterator tmp = *this;  return tmp; }
			bool operator ==(const Iterator&      other) const { return *m_dirEntry == *other.m_dirEntry; }
			bool operator !=(const Iterator&      other) const { return *m_dirEntry != *other.m_dirEntry; }
			directory_entry & operator * () { return *m_dirEntry; }
			directory_entry * operator ->() { return m_dirEntry; }
		};

		directory_entry m_begin;
		directory_entry m_end;

		Iterator m_it_begin;
		Iterator m_it_end;

#ifdef YY_PLATFORM_WINDOWS
		HANDLE		hFind;
#endif

		path m_path;

		directory_iterator(){}

		friend class directory_entry;
		void _nextEntry()
		{
#ifdef YY_PLATFORM_WINDOWS
			WIN32_FIND_DATA ffd;
			if( FindNextFile( hFind, &ffd ) == FALSE )
			{
				m_begin.m_endSearch = true;
			}
			else
			{
				m_begin.m_path = ffd.cFileName;
				m_begin._check();
			}
#else
#error Need implement
#endif
		}

	public:

		directory_iterator(const path& p)
		{
			m_end.m_endSearch = true;

			m_it_begin = Iterator(&m_begin, this);
			m_it_end = Iterator(&m_end, this);
			m_path = p;
#ifdef YY_PLATFORM_WINDOWS
			hFind = 0;
#endif
		}

		~directory_iterator()
		{
#ifdef YY_PLATFORM_WINDOWS
			if( hFind )
				FindClose( hFind );
#endif
		}

		Iterator begin()
		{
#ifdef YY_PLATFORM_WINDOWS
			static bool firstCall = false;
			if(!firstCall)
			{
				WIN32_FIND_DATA ffd;
				firstCall = true;
				m_path.m_data += "*";
				hFind = FindFirstFile( (wchar_t*)m_path.m_data.c_str(), &ffd );
				if( INVALID_HANDLE_VALUE == hFind )
				{
					DWORD error = GetLastError();
					yyLogWriteError( "Can't scan dir. Error code [%u].\n", error );
					return Iterator();
				}
				
				m_begin.m_path = ffd.cFileName;
				m_begin._check();
			}
#else
#error Need implement
#endif
			return m_it_begin;
		}
		Iterator end(){return m_it_end;}

	};
}


#endif