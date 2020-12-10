#ifndef _YUYU_TYPES_H_
#define _YUYU_TYPES_H_

typedef char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;
typedef unsigned long long u64;
typedef float f32;
typedef double f64;

#define YY_MAKEFOURCC( ch0, ch1, ch2, ch3 )\
	((u32)(u8)(ch0)|((u32)(u8)(ch1)<<8)|\
	((u32)(u8)(ch2)<<16)|((u32)(u8)(ch3)<<24))

#define BIT(x)0x1<<(x)

#endif