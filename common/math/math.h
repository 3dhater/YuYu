#ifndef _GAME_MATH_H_
#define _GAME_MATH_H_

namespace math
{
	const f32 PI = static_cast<f32>(3.14159265358979323846);
	const f32 PIPI = 6.2831853f;

	YY_FORCE_INLINE f32 degToRad(f32 degrees)
	{
		return degrees * (PI / 180.f);
	}

	YY_FORCE_INLINE f32 radToDeg(f32 radians)
	{
		return radians * (180.f / PI);
	}

	YY_FORCE_INLINE f32 radians(f32 degrees)
	{
		return degrees * static_cast<f32>(0.01745329251994329576923690768489);
	}

	YY_FORCE_INLINE f64 radians(f64 degrees)
	{
		return degrees * static_cast<f64>(0.01745329251994329576923690768489);
	}

	YY_FORCE_INLINE f32 degrees(f32 radians)
	{
		return radians * static_cast<f32>(57.295779513082320876798154814105);
	}

	YY_FORCE_INLINE f64 degrees(f64 radians)
	{
		return radians * static_cast<f64>(57.295779513082320876798154814105);
	}

	YY_FORCE_INLINE f32 clamp(f32 x, f32 min, f32 max)
	{
		if (x < min)
			return min;
		else if (x > max)
			return max;
		return x;
	}

	template<typename T>
	T lerp(const T& a, const T& b, const f32 t)
	{
		return (T)(a * (1.f - t)) + (b * t);
	}

	YY_FORCE_INLINE f32 get_0_1(f32 base, f32 val)
	{
		return val * (1.f / base);
	}
}

#endif