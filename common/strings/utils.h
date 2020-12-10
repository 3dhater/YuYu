#ifndef _YY_STRING_UTILS_H_
#define _YY_STRING_UTILS_H_

#include "strings/string.h"

namespace util
{
template<typename Type>
inline void stringToLower( Type& str )
{
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
	Type flippedStr;
	for( u32 i = str.size() - 1u; i >= 0u; --i )
	{
		flippedStr += str[ i ];
		if( !i ) break;
	}
	str = flippedStr;
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
YY_FORCE_INLINE s32 to_int(const char16_t* str, s32 size = -1)
{
	size_t len = size;
	if( size == -1 )
		len = str_len(str);
	int result = 0;
	int mul_val = 1;
	bool is_neg = str[0]=='-';
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
YY_FORCE_INLINE float to_float(const char16_t* str)
{
	float result = 0.f;
	bool is_negative = *str == '-';

	if(is_negative)
		++str;

	int i = 0;
	while(*str >= '0' && *str <= '9')
	{
		i *= 10;
		i += *str - '0';
		++str;
	}
	result = (float)i;
				
	i = 0;

	if(*str == '.')
		++str;

	int part_2_count = 0;
	while(*str >= '0' && *str <= '9')
	{
		i *= 10;
		i += *str - '0';
		++str;
		++part_2_count;
	}
	result += (float)i * string_to_float_table[part_2_count];

	return is_negative ? -result : result;
}
}


#endif