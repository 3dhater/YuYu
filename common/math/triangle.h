#ifndef _GAME_TRIANGLE_H_
#define _GAME_TRIANGLE_H_

#include "math\ray.h"
struct yyTriangle
{
	v3f v1;
	v3f v2;
	v3f v3;
	v3f normal1;
	v3f normal2;
	v3f normal3;
	v4f e1;
	v4f e2;
	v3f t1;
	v3f t2;
	v3f t3;

	void update()
	{
		e1 = v4f(v2.x - v1.x,
			v2.y - v1.y,
			v2.z - v1.z,
			0.f);
		e2 = v4f(v3.x - v1.x,
			v3.y - v1.y,
			v3.z - v1.z,
			0.f);
		//	e1.cross(e2, faceNormal);
	}

	bool rayTest_MT(const yyRay& ray, bool withBackFace, f32& T, f32& U, f32& V, f32& W)
	{
		v4f pvec;
		ray.m_direction.cross2(e2, pvec);
		f32 det = e1.dot(pvec);

		if (withBackFace)
		{
			if (std::fabs(det) < Epsilon)
				return false;
		}
		else
		{
			if (det < Epsilon && det > -Epsilon)
				return false;
		}

		v4f tvec(
			ray.m_origin.x - v1.x,
			ray.m_origin.y - v1.y,
			ray.m_origin.z - v1.z,
			0.f);

		f32 inv_det = 1.f / det;
		U = tvec.dot(pvec) * inv_det;

		if (U < 0.f || U > 1.f)
			return false;

		v4f  qvec;
		tvec.cross2(e1, qvec);
		V = ray.m_direction.dot(qvec) * inv_det;

		if (V < 0.f || U + V > 1.f)
			return false;

		// T is length from origin to intersection point
		T = e2.dot(qvec) * inv_det;

		if (T < Epsilon) return false;

		W = 1.f - U - V;
		return true;
	}
};


#endif