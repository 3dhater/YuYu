#ifndef _GAME_RAY_H_
#define _GAME_RAY_H_

#include "Math/mat.h"
#include "Math/vec.h"
inline int max_dim(const v4f& v)
{
    return (v.x > v.y) ? ((v.x > v.z) 
		? 0 : 2) : ((v.y > v.z) ? 1 : 2);
}

//const f32 g_tMax = Infinity;

class yyRay
{
public:
	yyRay()
	:
		m_kz(0),
		m_kx(0),
		m_ky(0),
		m_Sx(0.f),
		m_Sy(0.f),
		m_Sz(0.f)
	{}
	~yyRay(){}

	v4f m_origin;
	v4f m_end;
	v4f m_direction;
	v4f m_invDir;

	s32 m_kz;
	s32 m_kx;
	s32 m_ky;

	f32 m_Sx;
	f32 m_Sy;
	f32 m_Sz;

	void createFrom2DCoords(const v2f& coord, const v4f& rc, const v2f& rc_sz, const Mat4& VPinv)
	{
		v2f point;
		point.x = coord.x - rc.x;
		point.y = coord.y - rc.y;

		// координаты курсора от -1 до +1
		float pt_x = (point.x / rc_sz.x) * 2.f - 1.f;
		float pt_y = -(point.y / rc_sz.y) * 2.f + 1.f;

		//                                           0.f - for d3d
		m_origin = math::mul(v4f(pt_x, pt_y, -1.f, 1.f), VPinv);
		m_end = math::mul(v4f(pt_x, pt_y, 1.f, 1.f), VPinv);

		m_origin.w = 1.0f / m_origin.w;
		m_origin.x *= m_origin.w;
		m_origin.y *= m_origin.w;
		m_origin.z *= m_origin.w;

		m_end.w = 1.0f / m_end.w;
		m_end.x *= m_end.w;
		m_end.y *= m_end.w;
		m_end.z *= m_end.w;

		update();
	}

	f32 distanceToLine(const v4f& lineP0, const v4f& lineP1)
	{
		v4f u = m_end - m_origin;
		v4f v = lineP1 - lineP0;
		v4f w = m_origin - lineP0;
		u.w = 0.f;
		v.w = 0.f;
		w.w = 0.f;
		f32 a = u.dot();
		f32 b = u.dot(v);
		f32 c = v.dot();
		f32 d = u.dot(w);
		f32 e = v.dot(w);
		f32 D = a*c - b*b;
		f32 sc, tc;

		if( D < Epsilon )
		{
			sc = 0.f;
			tc = (b>c ? d/b : e/c);
		}
		else
		{
			sc = (b*e - c*d) / D;
			tc = (a*e - b*d) / D;
		}

		v4f dP = w + (sc*u) - (tc*v);
		dP.w = 0.f;
		return std::sqrt(dP.dot());
	}

	void update()
	{
		m_direction.x = m_end.x - m_origin.x;
		m_direction.y = m_end.y - m_origin.y;
		m_direction.z = m_end.z - m_origin.z;
		m_direction.normalize2();
		m_direction.w = 1.f;

		m_invDir.x = 1.f / m_direction.x;
		m_invDir.y = 1.f / m_direction.y;
		m_invDir.z = 1.f / m_direction.z;
		m_invDir.w = 1.f / m_direction.w;
	
		m_kz = max_dim
		(
			v4f
			(
				std::abs(m_direction.x),
				std::abs(m_direction.y),
				std::abs(m_direction.z),
				1.f
			)
		);
		
		m_kx = m_kz + 1;
		if( m_kx == 3 )
			m_kx = 0;
		
		m_ky = m_kx + 1;
		if( m_ky == 3 )
			m_ky = 0;

		auto dir_data = m_direction.data();
		if( dir_data[m_kz] )
			std::swap(m_kx, m_ky);

		m_Sx = dir_data[m_kx] / dir_data[m_kz];
		m_Sy = dir_data[m_ky] / dir_data[m_kz];
		m_Sz = 1.f / dir_data[m_kz];
	}
};


#endif