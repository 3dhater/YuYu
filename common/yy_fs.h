#ifndef _YUYU_FS_H_
#define _YUYU_FS_H_

#include "strings/string.h"
#include "strings/utils.h"
#include "containers\list.h"

#include <iostream>

namespace yy_fs
{
	class path;
	class directory_iterator;
	class directory_entry;
}

class YY_API yyFileSystemInternal {
public:
	yyFileSystemInternal();
	~yyFileSystemInternal();

	static bool exists(const yy_fs::path& p);
	static size_t file_size(const yy_fs::path& p);
	static yy_fs::path current_path();
	static bool create_directory(const yy_fs::path& p);

	static void directory_iterator_next_entry(yy_fs::directory_iterator* di);
	static bool directory_iterator_begin(yy_fs::directory_iterator* di);
	static void directory_entry_check(yy_fs::directory_entry* de);
};

namespace yy_fs
{
	class path
	{
		void _checkBackSlash()
		{
			if(string_type.size())
			{
				util::stringFlipSlashBackSlash(string_type);
			}
		}


	public:

		typedef wchar_t value_type;
		yyString_base<value_type,yyDefault_allocator> string_type;

		path(){}
		path(const wchar_t* char16_text)
		{
			string_type = char16_text;
			_checkBackSlash();
		}
		path(const char16_t* char16_text)
		{
			string_type = (const wchar_t*)char16_text;
			_checkBackSlash();
		}
		path(const char* char_text)
		{
			yyStringA str(char_text);
			util::utf8_to_utf16(&str, &string_type);
			_checkBackSlash();
		}
		~path()
		{
		}

		bool has_extension()
		{
			bool dot = false;
			if(string_type.size())
			{

				for( u32 i = string_type.size() - 1u; i >= 0u; --i )
				{
					auto c = string_type[ i ];

					if (c == '/' || c == '\\')
					{
						return dot;
					}

					if (c == '.')
						dot = true;

					if( !i ) break;
				}
			}
			return dot;
		}

		path extension()
		{
			path result;
			for( u32 i = string_type.size() - 1u; i >= 0u; --i )
			{
				auto c = string_type[ i ];
				if( c == '/' || c == '.' || c == '\\' )
					break;
				else result.string_type += c;
				if( !i ) break;
			}
			result.string_type += L'.';
			util::stringFlip(result.string_type);
			util::stringToLower(result.string_type);
			return result;
		}

		path filename()
		{
			path result;
			for (u32 i = string_type.size() - 1u; i >= 0u; --i)
			{
				auto c = string_type[i];
				if (c == '/' || c == '\\')
					break;
				else result.string_type += c;
				if (!i) break;
			}
			util::stringFlip(result.string_type);
			util::stringToLower(result.string_type);
			return result;
		}
		
		path parent_path()
		{
			path result;
			result.string_type = string_type;
			util::stringFlipSlashBackSlash(result.string_type);
			util::stringPopBackBefore(result.string_type, '\\');
			if (result.string_type.size())
			{
				if (result.string_type[result.string_type.size() - 1] == (value_type)'\\')
					result.string_type.pop_back();
			}
			return result;
		}

		bool operator==(const path& other){ return string_type == other.string_type; }
		bool operator!=(const path& other){ return string_type != other.string_type; }
		
		std::string generic_string() const
		{
			std::string result;
			util::utf16_to_utf8(&string_type, &result);
			return result;
		}
		std::u16string generic_u16string() const
		{
			std::u16string result;
			result.reserve(string_type.size());
			auto data = string_type.data();
			for (size_t i = 0, sz = string_type.size(); i < sz; ++i)
			{
				result += (std::u16string::value_type)string_type[i];
			}
			return result;
		}


		friend std::wostream& operator<<(std::wostream& os, const yy_fs::path& dt);
		friend std::ostream& operator<<(std::ostream& os, const yy_fs::path& dt);


	};

	YY_FORCE_INLINE std::wostream& operator<<(std::wostream& os, const yy_fs::path& dt){
		os << dt.string_type.data();
		return os;
	}
	YY_FORCE_INLINE std::ostream& operator<<(std::ostream& os, const yy_fs::path& dt){
		os << dt.generic_string().data();
		return os;
	}

	YY_FORCE_INLINE bool exists(const path& p){
		return yyFileSystemInternal::exists(p);
	}

	YY_FORCE_INLINE size_t file_size(const path& p){
		return yyFileSystemInternal::file_size(p);
	}

	YY_FORCE_INLINE path current_path(){
		return yyFileSystemInternal::current_path();
	}

	YY_FORCE_INLINE bool create_directory(const path& p){
		return yyFileSystemInternal::create_directory(p);
	}

	class directory_entry
	{
		friend class directory_iterator;
		friend class yyFileSystemInternal;
	public:
		directory_entry(){
			m_endSearch = false;
		}
		~directory_entry(){}

		bool operator==(const directory_entry& other)const{ 
			return other.m_endSearch == this->m_endSearch;
		}
		bool operator!=(const directory_entry& other)const{
			return other.m_endSearch != this->m_endSearch;
		}
		bool operator==(directory_entry* other)const{
			return other->m_endSearch == this->m_endSearch;
		}
		bool operator!=(directory_entry* other)const{
			return other->m_endSearch != this->m_endSearch;
		}

		path m_path;
		const path& path()const{return m_path;}

		bool m_endSearch;
	};

	class directory_iterator
	{
	protected:
		bool m_isRecursiveIterator;

		class Iterator
		{
			friend class ConstIterator;
			directory_entry* m_dirEntry;
			directory_iterator* m_dirIt;
		public:
			Iterator() :m_dirIt(0) {}
			Iterator(directory_entry* o, directory_iterator* di) { m_dirEntry = o; m_dirIt = di; }
			~Iterator() {}
			Iterator& operator ++() { yyFileSystemInternal::directory_iterator_next_entry(m_dirIt); return *this; }
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

		path m_path;
		path m_startPath;

		yyListFast<yyStringW> m_subDirs;

		directory_iterator(){}

		friend class directory_entry;
		friend class yyFileSystemInternal;

	public:

		directory_iterator(const path& p){
			m_end.m_endSearch = true;
			m_isRecursiveIterator = false;
			m_it_begin = Iterator(&m_begin, this);
			m_it_end = Iterator(&m_end, this);
			m_path = p;
			m_startPath = p;
		}

		virtual ~directory_iterator(){}

		Iterator begin(){
			if (yyFileSystemInternal::directory_iterator_begin(this))
			{
				return m_it_begin;
			}
			else
			{
				return Iterator();
			}
		}

		Iterator end(){return m_it_end;}

	};

	class recursive_directory_iterator : public directory_iterator
	{
	public:

		recursive_directory_iterator(const path& p)
			:
			directory_iterator(p)
		{
			m_isRecursiveIterator = true;
		}

		~recursive_directory_iterator(){}
	};
}


#endif