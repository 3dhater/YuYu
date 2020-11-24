﻿#ifndef _YUYU_TYPES_H_
#define _YUYU_TYPES_H_

using s8 = char;
using u8 = unsigned char;
using s16 = short;
using u16 = unsigned short;
using s32 = int;
using u32 = unsigned int;
using s64 = long long;
using u64 = unsigned long long;
using f32 = float;
using f64 = double;

#define YY_MAKEFOURCC( ch0, ch1, ch2, ch3 )\
	((u32)(u8)(ch0)|((u32)(u8)(ch1)<<8)|\
	((u32)(u8)(ch2)<<16)|((u32)(u8)(ch3)<<24))

#define BIT(x)0x1<<(x)

#endif