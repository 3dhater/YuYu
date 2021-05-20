#ifndef _YY_STRING_UTILS_H_
#define _YY_STRING_UTILS_H_

#include "strings/string.h"

namespace util
{
template<typename Type>
inline void stringToLower( Type& str )
{
	if (!str.size()) return;
	u32 sz = str.size();
	for( u32 i = 0u; i < sz; ++i )
	{
		auto c = str[ i ];
		if( c <= 'Z' && c >= 'A' )
			str[ i ] += 32;
	}
}
//	abc -> cba
template<typename Type>
inline void stringFlip( Type& str )
{
	if (!str.size()) return;
	Type flippedStr;
	for( u32 i = str.size() - 1u; i >= 0u; --i )
	{
		flippedStr += str[ i ];
		if( !i ) break;
	}
	str = flippedStr;
}
template<typename Type>
inline void stringFlipSlash( Type& str )
{
	if (!str.size()) return;
	u32 sz = str.size();
	for( u32 i = 0u; i < sz; ++i )
	{
		if( str[ i ] == '\\' )
			str[ i ] = '/';
	}
}
template<typename Type>
inline void stringFlipSlashBackSlash( Type& str )
{
	if (!str.size()) return;
	u32 sz = str.size();
	for( u32 i = 0u; i < sz; ++i )
	{
		if( str[ i ] == '/' )
			str[ i ] = '\\';
	}
}
template<typename Type>
inline void stringPopBackBefore( Type& str, s8 c )
{
	if( str.size() )
		str.pop_back();
	if( str.size() )
	{
		for( u32 i = str.size() - 1u; i >= 0u; --i )
		{
			if( str[ i ] == (Type::value_type)c ) 
				break;
			else 
				str.pop_back();
			if( !i ) 
				break;
		}
	}
}
template<typename Type>
inline Type stringGetExtension( const Type& str, bool addDot )
{
	Type ret;
	for( u32 i = str.size() - 1u; i >= 0u; --i )
	{
		auto c = str[ i ];
		if( c == '/' || c == '.' )
			break;
		else ret += c;
		if( !i ) break;
	}
	if(addDot && ret.size())
	{
		ret += ".";
	}
	stringFlip<Type>( ret );
	return ret;
}

YY_FORCE_INLINE void getLine( std::u16string& outText, const char16_t * inText )
{
	outText.clear();
	while(true)
	{
		auto ch = *inText;
		if(ch == '\n' || ch == 0)
			break;

		outText += ch;
		inText++;
	}
}
YY_FORCE_INLINE void getLine( yyString& outText, const char16_t * inText )
{
	outText.clear();
	while(true)
	{
		auto ch = *inText;
		if(ch == '\n' || ch == 0)
			break;

		outText += ch;
		inText++;
	}
}

YY_FORCE_INLINE char16_t * nextLine( char16_t * ptr )
{
	while( *ptr )
	{
		if( *ptr == '\n' )
		{
			ptr++;
			return ptr;
		}
		ptr++;
	}
	return ptr;
}
YY_FORCE_INLINE bool is_space(char16_t c)
{
	switch (c)
	{
	case ' ':
	case '\t':
	case '\r':
	case '\n':
		return true;
	default:
		break;
	}
	return false;
}
YY_FORCE_INLINE bool is_space(char c)
{
	switch (c)
	{
	case ' ':
	case '\t':
	case '\r':
	case '\n':
		return true;
	default:
		break;
	}
	return false;
}
YY_FORCE_INLINE bool is_space(wchar_t c)
{
	switch (c)
	{
	case L' ':
	case L'\t':
	case L'\r':
	case L'\n':
		return true;
	default:
		break;
	}
	return false;
}

YY_FORCE_INLINE bool is_digit(char16_t c)
{
	switch (c)
	{
	case '0': case '1': case '2': case '3': case '4': case '5':
	case '6': case '7': case '8': case '9':
		return true;
	default:
		break;
	}
	return false;
}
YY_FORCE_INLINE bool is_digit(char c)
{
	switch (c)
	{
	case '0': case '1': case '2': case '3': case '4': case '5':
	case '6': case '7': case '8': case '9':
		return true;
	default:
		break;
	}
	return false;
}
YY_FORCE_INLINE bool is_digit(wchar_t c)
{
	switch (c)
	{
	case L'0': case L'1': case L'2': case L'3': case L'4': case L'5':
	case L'6': case L'7': case L'8': case L'9':
		return true;
	default:
		break;
	}
	return false;
}
YY_FORCE_INLINE bool is_alpha(char16_t c)
{
	return (c >= 0x41 && c <= 0x5A) || (c >= 0x61 && c <= 0x7A);
}
YY_FORCE_INLINE bool is_alpha(char c)
{
	return (c >= 0x41 && c <= 0x5A) || (c >= 0x61 && c <= 0x7A);
}
YY_FORCE_INLINE bool is_alpha(wchar_t c)
{
	return (c >= 0x41 && c <= 0x5A) || (c >= 0x61 && c <= 0x7A);
}
template<typename char_type>
size_t str_len(const char_type* str)
{
	size_t len = 0u;
	if(str[0]!=0)
	{
		const char_type* p = &str[ 0u ];
		while( (int)*p++ ) 
			len++;
	}
	return len;
}

template<typename char_type>
s32 to_int(const char_type* str, s32 size = -1)
{
	size_t len = size;
	if( size == -1 )
		len = str_len(str);
	int result = 0;
	int mul_val = 1;
	bool is_neg = str[0] == (char_type)'-';
	for(size_t i = 0, last = len-1; i < len; ++i)
	{
		int char_value = (int)str[last] - 0x30;
		if( char_value >= 0 && char_value <= 9 )
		{
			result += char_value * mul_val;
			mul_val *= 10;
		}
		--last;
	}
	if( is_neg )
		result = -result;
	return result;
}
const float string_to_float_table[17] = 
{
	0.f,
	0.1f,
	0.01f,
	0.001f,
	0.0001f,
	0.00001f,
	0.000001f,
	0.0000001f,
	0.00000001f,
	0.000000001f,
	0.0000000001f,
	0.00000000001f,
	0.000000000001f,
	0.0000000000001f,
	0.00000000000001f,
	0.000000000000001f,
	0.0000000000000001f,
};

template<typename char_type>
float to_float(const char_type* str)
{
	float result = 0.f;
	bool is_negative = *str == (char_type)'-';

	if(is_negative)
		++str;

	int i = 0;
	while(*str >= (char_type)'0' && *str <= (char_type)'9')
	{
		i *= 10;
		i += *str - (char_type)'0';
		++str;
	}
	result = (float)i;
				
	i = 0;

	if(*str == (char_type)'.')
		++str;

	int part_2_count = 0;
	while(*str >= (char_type)'0' && *str <= (char_type)'9')
	{
		i *= 10;
		i += *str - (char_type)'0';
		++str;
		++part_2_count;
	}
	result += (float)i * string_to_float_table[part_2_count];

	return is_negative ? -result : result;
}

template<typename input_string_type, typename output_string_type>
void utf16_to_utf8(input_string_type* utf16, output_string_type* utf8)
{
	u32 sz = utf16->size();
	for( u32 i = 0u; i < sz; ++i )
	{
		input_string_type::value_type ch16 = utf16->operator[]( i );
		if( ch16 < 0x80 )
		{
			*utf8 += (output_string_type::value_type)ch16;
		}
		else if( ch16 < 0x800 )
		{
			*utf8 += (output_string_type::value_type)((ch16>>6)|0xc0);
			*utf8 += (output_string_type::value_type)((ch16&0x3f)|0x80);
		}
	}
}

template<typename output_string_type>
void utf8_to_utf16(yyStringA* utf8, output_string_type* utf16)
{
	auto utf8_data = utf8->data();
	while(*utf8_data)
	{
		unsigned char utf8_char = *utf8_data;
		++utf8_data;

		if(utf8_char >= 0 && utf8_char < 0x80) // one byte
		{
			utf16->append((output_string_type::value_type)utf8_char);
		}
		else if(utf8_char >= 0xC0 && utf8_char < 0xE0) // 2
		{
			unsigned char utf8_char2 = *utf8_data;
			if(utf8_char2)
			{
				++utf8_data;

				output_string_type::value_type char16 = 0;
				char16 = utf8_char;
				char16 ^= 0xC0;
				char16 <<= 6;
				char16 |= (utf8_char2^0x80);
		
				utf16->append(char16);
			}
		}
		else if(utf8_char >= 0xE0 && utf8_char < 0xF0) // 3
		{
			unsigned char utf8_char2 = *utf8_data;
			if(utf8_char2)
			{
				++utf8_data;
				unsigned char utf8_char3 = *utf8_data;
				if(utf8_char3)
				{
					++utf8_data;

					output_string_type::value_type char16 = 0;
					char16 = utf8_char;
					char16 ^= 0xE0;
					char16 <<= 12;
					char16 |= (utf8_char2^0x80) << 6;
					char16 |= (utf8_char3^0x80);
		
					utf16->append(char16);
				}
			}
		}
	}
}

}


#endif