﻿#ifndef _YY_STRING_H_
#define _YY_STRING_H_

#include "common/allocator.h"

#include <cstring>
#include <sstream>
#include <cstdarg>

#include <vector>

#pragma warning( disable : 4996 )

const size_t yyStringWordSize = 16;

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
	
	pointer m_data;
	size_t     m_allocated;
	size_t     m_size;

	void reallocate( size_t new_allocated )
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
	size_t getlen( const other_type* str )
	{
		size_t len = 0u;
		if(str[0]!=0)
		{
			const other_type* p = &str[ 0u ];
			while( (size_t)*p++ ) 
				len++;
		}
		return len;
	}

	template<typename dst_type, typename src_type>
	void copy( dst_type * dst, src_type* src ) const
	{
		while( (size_t)*src )
		{
			*dst = static_cast<dst_type>(*src);
			dst++;
			src++;
		}
	}


public:
	typedef char_type value_type;
	
	yyString_base()
		:m_data(nullptr), m_allocated(yyStringWordSize), m_size(0)
	{
		reallocate( m_allocated );
	}

	template<typename other_type>
	yyString_base( const other_type * str )
		: m_data(nullptr), m_allocated(yyStringWordSize), m_size(0)
	{
		reallocate( m_allocated );
		assign( str );
	}

	yyString_base( this_const_reference str )
		: m_data(nullptr), m_allocated(yyStringWordSize), m_size(0)
	{
		reallocate( m_allocated );
		assign( str );
	}

	yyString_base( this_type&& str )
		: m_data(nullptr), m_allocated(yyStringWordSize), m_size(0)
	{
		reallocate( m_allocated );
		assign( str );
	}

	yyString_base( char_type c )
		: m_data(nullptr), m_allocated(yyStringWordSize), m_size(0)
	{
		size_t new_size = 1u;
		reallocate( (new_size + 1u) + yyStringWordSize);
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

	void reserve( size_t size )
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
		if(str)
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
		size_t new_size = getlen( str ) + m_size;

		if( (new_size + 1u) > m_allocated )
			reallocate( (new_size + 1u) + yyStringWordSize);

		copy( &m_data[m_size], str );

		m_size = new_size;
		m_data[ m_size ] = static_cast<char_type>(0x0);
	}

	void append( this_const_reference str )
	{
		append( str.data() );
	}

	template<typename other_type>
	void insert(const other_type * str, size_t where) {
		size_t str_len = getlen(str);
		size_t new_size = str_len + m_size;
		if ((new_size + 1u) > m_allocated)
			reallocate((new_size + 1u) + yyStringWordSize);

		size_t i = m_size;
		while (i >= where)
		{
			m_data[i + str_len] = m_data[i];
			if (i == 0)
				break;
			--i;
		}
		for (size_t i = 0; i < str_len; ++i)
		{
			m_data[i + where] = str[i];
		}
		m_size = new_size;
		m_data[m_size] = 0;
	}

	size_t capacity() { return m_allocated; }

	void insert(char_type c, size_t where) {
		size_t new_size = m_size + 1u;
		if ((new_size + 1u) > m_allocated)
			reallocate((new_size + 1u) + yyStringWordSize);

		size_t i = m_size;
		while (i >= where)
		{
			auto next = i + 1;
			if (next < m_allocated)
				m_data[next] = m_data[i];

			if (i == 0)
				break;
			--i;
		}
		m_data[where] = c;
		m_size = new_size;
		m_data[m_size] = 0;
	}

	void push_back( char_type c ){
		size_t new_size = m_size + 1u;
		if( (new_size + 1u) > m_allocated )
			reallocate( (new_size + 1u) + yyStringWordSize);
		m_data[ m_size ] = c;
		m_size = new_size;
		m_data[ m_size ] = 0;
	}
	void append( char_type c ){
		size_t new_size = m_size + 1u;
		if( (new_size + 1u) > m_allocated )
			reallocate( (new_size + 1u) + yyStringWordSize);
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
	const size_t size() const { return m_size; }

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

	this_type operator+( size_t num )
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

	const_reference operator[]( size_t i ) const 
	{
		return m_data[ i ];
	}

	reference operator[]( size_t i )
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

		const size_t sz = other.size();
		const auto * ptr = other.data();
		for( size_t i = 0u; i < sz; ++i )
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

		const size_t sz = other.size();
		const auto * ptr = other.data();
		for( size_t i = 0u; i < sz; ++i )
		{
			if( ptr[ i ] != m_data[ i ]  ) 
				return true;
		}

		return false;
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
			if( m_allocated > (m_size + yyStringWordSize) )
			{
				reallocate( m_size + 1u );
				m_data[ m_size ] = static_cast<char_type>(0x0);
			}
		}
	}

	void setSize( size_t size )
	{
		m_size = size;
	}

	void erase( size_t begin, size_t end )
	{
		size_t numCharsToDelete = end - begin + 1u; // delete first char: 0 - 0 + 1
		size_t next = end + 1u;
		for( size_t i = begin; i < m_size; ++i )
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

	bool is_space( size_t index )
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
		for(size_t i = 0; i < m_size; ++i)
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
		size_t sz = utf16.size();
		for( size_t i = 0u; i < sz; ++i )
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
		size_t sz = string.size();
		for( size_t i = 0; i < sz; ++i ){
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