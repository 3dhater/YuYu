#ifndef _YY_STRING_H_
#define _YY_STRING_H_

#include "common/allocator.h"

#include <cstring>
#include <sstream>
#include <cstdarg>

#include <vector>

#pragma warning( disable : 4996 )



template<typename char_type, typename allocator>
class yyString_base
{
		
	typedef char_type* pointer;
	typedef const char_type* const_pointer;
	typedef char_type& reference ;
	typedef const char_type&  const_reference;
	typedef yyString_base this_type;
	typedef const yyString_base& this_const_reference;
	
	allocator m_allocator;

	u32 m_stringWordSize;

	pointer m_data;
	u32     m_allocated;
	u32     m_size;

	void reallocate( u32 new_allocated )
	{
		//char_type * new_data = (char_type*)malloc( new_allocated * sizeof(char_type) );
		char_type * new_data = (char_type*)m_allocator.allocate( new_allocated * sizeof(char_type) );
		if( m_data )
		{
			std::memcpy( new_data, m_data, m_size * sizeof( char_type ) );
		//	free( m_data );
			m_allocator.free( m_data );
		}
		else
		{
			std::memset(new_data, 0, new_allocated);
		}
		m_data = new_data;
		m_allocated = new_allocated;
	}

	template<typename other_type>
	u32 getlen( const other_type* str )
	{
		unsigned int len = 0u;
		if(str[0]!=0)
		{
			const other_type* p = &str[ 0u ];
			while( (u32)*p++ ) 
				len++;
		}
		return len;
	}

	template<typename dst_type, typename src_type>
	void copy( dst_type * dst, src_type* src ) const
	{
		while( (u32)*src )
		{
			*dst = static_cast<dst_type>(*src);
			dst++;
			src++;
		}
	}


public:
	typedef char_type value_type;
	
	yyString_base()
		: m_stringWordSize(16), m_data(nullptr), m_allocated(16), m_size(0)
	{
		reallocate( m_allocated );
	}

	template<typename other_type>
	yyString_base( const other_type * str )
		: m_stringWordSize(16), m_data(nullptr), m_allocated(16), m_size(0)
	{
		reallocate( m_allocated );
		assign( str );
	}

	yyString_base( this_const_reference str )
		: m_stringWordSize(16), m_data(nullptr), m_allocated(16), m_size(0)
	{
		reallocate( m_allocated );
		assign( str );
	}

	yyString_base( this_type&& str )
		: m_stringWordSize(16), m_data(nullptr), m_allocated(16), m_size(0)
	{
		reallocate( m_allocated );
		assign( str );
	}

	yyString_base( char_type c )
		: m_stringWordSize(16), m_data(nullptr), m_allocated(16), m_size(0)
	{
		u32 new_size = 1u;
		reallocate( (new_size + 1u) + m_stringWordSize );
		m_data[ 0u ] = c;
		m_size = new_size;
		m_data[ m_size ] = static_cast<char_type>(0x0);
	}

	~yyString_base()
	{
		if( m_data )
		{
			m_allocator.free( m_data );
			//free( m_data );
		}
	}

	void setWordSize( u32 v )
	{
		m_stringWordSize = v;
	}

	void reserve( u32 size )
	{
		if( size > m_allocated )
		{
			reallocate( size );
			m_data[ m_size ] = 0;
		}
	}

	template<typename other_type>
	void assign( other_type str )
	{
		m_size = 0u;
		m_data[ m_size ] = static_cast<char_type>(0x0);
		append( str );
	}

	void assign( this_const_reference str )
	{
		m_size = 0u;
		m_data[ m_size ] = static_cast<char_type>(0x0);
		append( str );
	}

	template<typename other_type>
	void append( const other_type * str )
	{
		u32 new_size = getlen( str ) + m_size;

		if( (new_size + 1u) > m_allocated )
			reallocate( (new_size + 1u) + m_stringWordSize );

		copy( &m_data[m_size], str );

		m_size = new_size;
		m_data[ m_size ] = static_cast<char_type>(0x0);
	}

	void append( this_const_reference str )
	{
		append( str.data() );
	}

	void push_back( char_type c ){
		u32 new_size = m_size + 1u;
		if( (new_size + 1u) > m_allocated )
			reallocate( (new_size + 1u) + m_stringWordSize );
		m_data[ m_size ] = c;
		m_size = new_size;
		m_data[ m_size ] = 0;
	}
	void append( char_type c ){
		u32 new_size = m_size + 1u;
		if( (new_size + 1u) > m_allocated )
			reallocate( (new_size + 1u) + m_stringWordSize );
		m_data[ m_size ] = c;
		m_size = new_size;
		m_data[ m_size ] = 0;
	}

	void append( s32 c )
	{
		char buf[32u];
		::std::sprintf( buf, "%i", c );
		append( buf );
	}

	void append( u32 c )
	{
		char buf[32u];
		::std::sprintf( buf, "%u", c );
		append( buf );
	}

	void append( u64 c )
	{
		char buf[32u];
		::std::sprintf( buf, "%llu", c );
		append( buf );
	}

	void append( f64 c )
	{
		char buf[32u];
		::std::sprintf( buf, "%.12f", c );
		append( buf );
	}

	void append( f32 c )
	{
		char buf[32u];
		::std::sprintf( buf, "%.4f", c );
		append( buf );
	}

	yyString_base<char,allocator> to_stringA() const
	{
		yyString_base<char,allocator> ret;
		ret.reserve(m_size+1);
		this->copy(ret.data(),m_data);
		ret.setSize(m_size);
		ret.data()[m_size]=0;
		return ret;
	}

	yyString_base<char16_t,allocator> to_string() const
	{
		yyString_base<char16_t,allocator> ret;
		ret.reserve(m_size);
		ret = m_data;
		return ret;
	}

	const_pointer c_str() const { return m_data; }
	pointer data() const { return m_data; }
	const u32 size() const { return m_size; }

	this_type& operator=( this_const_reference str )
	{
		assign( str );
		return *this;
	}

	this_type& operator=( this_type&& str )
	{
		assign( str );
		return *this;
	}

	
	template<typename other_type>
	this_type& operator=( other_type * str )
	{
		assign( str );
		return *this;
	}

	this_type operator+( const_pointer str )
	{
		this_type ret( *this );
		ret.append( str );
		return ret;
	}

	this_type operator+( this_const_reference str )
	{
		return operator+(str.data());
	}

	this_type operator+( u32 num )
	{
		this_type r( *this );
		r.append( num );
		return r;
	}

	pointer begin() const 
	{
		return m_data;
	}

	pointer end() const 
	{
		return (m_data+(m_size));
	}

	const_reference operator[]( u32 i ) const 
	{
		return m_data[ i ];
	}

	reference operator[]( u32 i )
	{
		return m_data[ i ];
	}

	void operator+=( s32 i )
	{
		append( i );
	}

	void operator+=( u32 i )
	{
		append( i );
	}
	void operator+=( u64 i )
	{
		append( i );
	}
	void operator+=( f32 i )
	{
		append( i );
	}
	void operator+=( f64 i )
	{
		append( i );
	}

	void operator+=( char_type c )
	{
		append( c );
	}

	template<typename other_type>
	void operator+=( other_type * str )
	{
		append( str );
	}

	void operator+=( this_const_reference str )
	{
		append( str );
	}

	bool operator==( this_const_reference other ) const 
	{
		if( other.size() != m_size ) 
			return false;

		const u32 sz = other.size();
		const auto * ptr = other.data();
		for( u32 i = 0u; i < sz; ++i )
		{
			if( ptr[ i ] != m_data[ i ]  )
				return false;
		}

		return true;
	}

	bool operator!=( this_const_reference other ) const 
	{
		if( other.size() != m_size )
			return true;

		const u32 sz = other.size();
		const auto * ptr = other.data();
		for( u32 i = 0u; i < sz; ++i )
		{
			if( ptr[ i ] == m_data[ i ]  ) 
				return false;
		}

		return true;
	}

	void clear()
	{
		m_size = 0u;
		m_data[ m_size ] = 0;
	}

	void pop_back()
	{
		if( m_size )
		{
			--m_size;
			m_data[ m_size ] = 0;
		}
	}

	char_type pop_back_return()
	{
		char_type r = '?';
		if( m_size )
		{
			--m_size;
			r = m_data[ m_size ];
			m_data[ m_size ] = 0;
		}
		return r;
	}

	void shrink_to_fit()
	{
		if( m_size )
		{
			if( m_allocated > (m_size + m_stringWordSize) )
			{
				reallocate( m_size + 1u );
				m_data[ m_size ] = static_cast<char_type>(0x0);
			}
		}
	}

	void setSize( u32 size )
	{
		m_size = size;
	}

	void erase( u32 begin, u32 end )
	{
		u32 numCharsToDelete = end - begin + 1u; // delete first char: 0 - 0 + 1
		u32 next = end + 1u;
		for( u32 i = begin; i < m_size; ++i )
		{
			if( next < m_size )
			{
				m_data[ i ] = m_data[ next ];
				++next;
			}
		}
		m_size -= numCharsToDelete;
		m_data[ m_size ] = static_cast<char_type>(0x0);
	}

	void pop_front()
	{
		erase( 0u, 0u );
	}

	bool is_space( u32 index )
	{
		if( m_data[ index ] == (char_type)' ' ) return true;
		if( m_data[ index ] == (char_type)'\r' ) return true;
		if( m_data[ index ] == (char_type)'\n' ) return true;
		if( m_data[ index ] == (char_type)'\t' ) return true;
		return false;
	}

	u64 get_hash()
	{
		if (!m_size)
			return 0;

		const int p = 31;
		const int m = 1e9 + 9;
		long long hash_value = 0;
		long long p_pow = 1;
		for(u32 i = 0; i < m_size; ++i)
		{
			char c = (char)m_data[i];
			hash_value = (hash_value + (c - 'a' + 1) * p_pow) % m;
			p_pow = (p_pow * p) % m;
		}
		return hash_value;
	}
};

typedef yyString_base<char16_t,yyDefault_allocator> yyString;
typedef yyString_base<wchar_t,yyDefault_allocator> yyStringW;
typedef yyString_base<char32_t,yyDefault_allocator> yyString32;
typedef yyString_base<char,yyDefault_allocator> yyStringA;

namespace util
{
	template<typename allocator>
	void _string_UTF16_to_UTF8( yyString_base<char16_t,allocator>& utf16, yyString_base<char,allocator>& utf8 )
	{
		u32 sz = utf16.size();
		for( u32 i = 0u; i < sz; ++i )
		{
			char16_t ch16 = utf16[ i ];
			if( ch16 < 0x80 )
			{
				utf8 += (char)ch16;
			}
			else if( ch16 < 0x800 )
			{
				utf8 += (char)((ch16>>6)|0xc0);
				utf8 += (char)((ch16&0x3f)|0x80);
			}
		}
	}

YY_FORCE_INLINE void string_UTF16_to_UTF8( yyString_base<char16_t,yyDefault_allocator>& utf16, yyString_base<char,yyDefault_allocator>& utf8)
	{
		_string_UTF16_to_UTF8<yyDefault_allocator>(utf16,utf8);
	}
	template<typename Type, typename allocator>
	void _stringGetWords( std::vector<yyString_base<Type,allocator>> * out_array,
		const yyString_base<Type,allocator>& string,
		bool add_space = false,
		bool add_tab = false,
		bool add_newLine = false )
	{
		yyString_base<Type,allocator> word;
		u32 sz = string.size();
		for( u32 i = 0; i < sz; ++i ){
			auto ch = string[ i ];
			if( ch < 256 && (u8)ch == ' ' ){
				if( word.size() ){
					out_array->push_back( word );
					word.clear();
				}
				if( add_space ){
					out_array->push_back( yyString_base<Type,allocator>( (Type)ch ) );
				}
			}else if( ch < 256 && (u8)ch == '\t' ){
				if( word.size() ){
					out_array->push_back( word );
					word.clear();
				}
				if( add_tab ){
					out_array->push_back( yyString_base<Type,allocator>( (Type)ch ) );
				}
			}else if( ch < 256 && (u8)ch == '\n' ){
				if( word.size() ){
					out_array->push_back( word );
					word.clear();
				}
				if( add_newLine ){
					out_array->push_back( yyString_base<Type,allocator>( (Type)ch ) );
				}
			}else{
				word += ch;
			}
		}
		if( word.size() ){
			out_array->push_back( word );
			word.clear();
		}
	}
	
	template<typename Type>
	void stringGetWords( std::vector<Type> * out_array,
		const Type& string,
		bool add_space = false,
		bool add_tab = false,
		bool add_newLine = false )
	{
			_stringGetWords( out_array, string, add_space, add_tab, add_newLine);
	}
}
#endif