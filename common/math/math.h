#ifndef _GAME_MATH_H_
#define _GAME_MATH_H_

namespace math
{
	const f32 PI = static_cast<f32>(3.14159265358979323846);
	const f32 PIPI = 6.2831853f;

	YY_FORCE_INLINE
	bool pointInRect(float x, float y, const v4f& rect)
	{
		if (x>rect.x) { if (x<rect.z) { if (y>rect.y) { if (y<rect.w) { return true; } } } }
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
}

#endif