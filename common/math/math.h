#ifndef _GAME_MATH_H_
#define _GAME_MATH_H_

namespace math
{
	const f32 PI = static_cast<f32>(3.14159265358979323846);
	const f32 PIHalf = static_cast<f32>(3.14159265358979323846 * 0.5);
	const f32 PIPlusHalf = static_cast<f32>(3.14159265358979323846 + PIHalf);
	const f32 PIPI = 6.2831853f;
	const v4f v4fFltMax = v4f(FLT_MAX);
	const v4f v4fFltMaxNeg = v4f(-FLT_MAX);

	namespace _internal
	{
		/*
		https://stackoverflow.com/questions/6235847/how-to-generate-nan-infinity-and-infinity-in-ansi-c
		unsigned int p = 0x7F800000; // 0xFF << 23
		unsigned int n = 0xFF800000; // 0xFF8 << 20
		unsigned int pnan = 0x7F800001; // or anything above this up to 0x7FFFFFFF
		unsigned int nnan = 0xFF800001; // or anything above this up to 0xFFFFFFFF

		float positiveInfinity = *(float *)&p;
		float negativeInfinity = *(float *)&n;
		float positiveNaN = *(float *)&pnan;
		float negativeNaN = *(float *)&nnan;
		*/
		YY_FORCE_INLINE
			f32 get_infinity()
		{
			unsigned int p = 0x7F800000;
			f32 inf = *(float *)&p;
			return inf;
		}
	}

	//#define YY_INFINITY HUGE_VAL
	const f32 YY_INFINITY = _internal::get_infinity();

	YY_FORCE_INLINE
		bool isinf(f32 v)
	{
		return v == YY_INFINITY;
	}

	YY_FORCE_INLINE
		bool pointInRect(float x, float y, const v4f& rect)
	{
		if (x >= rect.x) { if (x <= rect.z) { if (y >= rect.y) { if (y <= rect.w) { return true; } } } }
		return false;
	}

	YY_FORCE_INLINE
		f32 degToRad(f32 degrees)
	{
		return degrees * (PI / 180.f);
	}

	YY_FORCE_INLINE
		f32 radToDeg(f32 radians)
	{
		return radians * (180.f / PI);
	}

	YY_FORCE_INLINE
		f32 clamp(f32 x, f32 min, f32 max)
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

	// assimp
	YY_FORCE_INLINE
		Quat slerp(const Quat& pStart, const Quat& pEnd, const f32 pFactor)
	{
		Quat pOut;
		f32 cosom = pStart.x * pEnd.x + pStart.y * pEnd.y + pStart.z * pEnd.z + pStart.w * pEnd.w;

		// adjust signs (if necessary)
		Quat end = pEnd;
		if (cosom < static_cast<f32>(0.0))
		{
			cosom = -cosom;
			end.x = -end.x;   // Reverse all signs
			end.y = -end.y;
			end.z = -end.z;
			end.w = -end.w;
		}

		// Calculate coefficients
		f32 sclp, sclq;
		if ((static_cast<f32>(1.0) - cosom) > static_cast<f32>(0.0001)) // 0.0001 -> some epsillon
		{
			// Standard case (slerp)
			f32 omega, sinom;
			omega = std::acos(cosom); // extract theta from dot product's cos theta
			sinom = std::sin(omega);
			sclp = std::sin((static_cast<f32>(1.0) - pFactor) * omega) / sinom;
			sclq = std::sin(pFactor * omega) / sinom;
		}
		else
		{
			// Very close, do linear interp (because it's faster)
			sclp = static_cast<f32>(1.0) - pFactor;
			sclq = pFactor;
		}

		pOut.x = sclp * pStart.x + sclq * end.x;
		pOut.y = sclp * pStart.y + sclq * end.y;
		pOut.z = sclp * pStart.z + sclq * end.z;
		pOut.w = sclp * pStart.w + sclq * end.w;
		return pOut;
	}

	YY_FORCE_INLINE
		f32 get_0_1(f32 base, f32 val)
	{
		return val * (1.f / base);
	}

	YY_FORCE_INLINE
	v2f worldToScreen(const Mat4& VP, const v4f& point3D, const v2f& viewportSize, const v2f& offset)
	{
		v4f point = point3D;
		point.w = 1.f;

		point = math::mul(point, VP);

		return v2f
		(
			offset.x + (viewportSize.x * 0.5f + point.x * viewportSize.x * 0.5f / point.w)
			,
			offset.y + (viewportSize.y - (viewportSize.y * 0.5f + point.y * viewportSize.y * 0.5f / point.w))
		);
	}

	YY_FORCE_INLINE
	v2f screenToClient(const v2f& screen_coord, const v4f& client_rect)
	{
		return v2f(
			screen_coord.x - client_rect.x,
			screen_coord.y - client_rect.y
		);
	}
}

#endif