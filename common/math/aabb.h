#ifndef _GAME_AABB_H_
#define _GAME_AABB_H_


#include "Math/vec.h"
#include "Math/ray.h"
#include "Math/math.h"

#include <float.h>
#include <algorithm>

#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

	// Oriented Bounding Box
class Obb
{
public:
	Obb(){ }
	v4f v1, v2, v3, v4, v5, v6, v7, v8;			
};
		// Axis-Aligned Bounding Box
class Aabb
{
public:

	Aabb()
		:
		m_min(math::v4fFltMax),
		m_max(math::v4fFltMaxNeg)
	{}
	Aabb( const v4f& min, const v4f& max ):m_min( min ), m_max( max ){ }

	/*
	Transforming Axis-Aligned Bounding Boxes
	by Jim Arvo
	from "Graphics Gems", Academic Press, 1990
	*/
	void transform(Aabb* original, Mat4* matrix, v4f* position) {
		float  a, b;
		float  Amin[3], Amax[3];
		float  Bmin[3], Bmax[3];
		int    i, j;

		/*Copy box A into a min array and a max array for easy reference.*/
		Amin[0] = original->m_min.x;  Amax[0] = original->m_max.x;
		Amin[1] = original->m_min.y;  Amax[1] = original->m_max.y;
		Amin[2] = original->m_min.z;  Amax[2] = original->m_max.z;

		/* Take care of translation by beginning at T. */
		Bmin[0] = Bmax[0] = position->x;
		Bmin[1] = Bmax[1] = position->y;
		Bmin[2] = Bmax[2] = position->z;

		/* Now find the extreme points by considering the product of the */
		/* min and max with each component of M.  */
		for (i = 0; i < 3; i++)
		{
			for (j = 0; j < 3; j++)
			{
				// mimi: I swapped j and i
				a = (float)(matrix->m_data[j][i] * Amin[j]);
				b = (float)(matrix->m_data[j][i] * Amax[j]);
				if (a < b)
				{
					Bmin[i] += a;
					Bmax[i] += b;
				}
				else
				{
					Bmin[i] += b;
					Bmax[i] += a;
				}
			}
		}
		/* Copy the result into the new box. */
		m_min.x = Bmin[0];  m_max.x = Bmax[0];
		m_min.y = Bmin[1];  m_max.y = Bmax[1];
		m_min.z = Bmin[2];  m_max.z = Bmax[2];
	}

	void add( const v4f& point )
	{
		if( point.x < m_min.x ) m_min.x = point.x;
		if( point.y < m_min.y ) m_min.y = point.y;
		if( point.z < m_min.z ) m_min.z = point.z;

		if( point.x > m_max.x ) m_max.x = point.x;
		if( point.y > m_max.y ) m_max.y = point.y;
		if( point.z > m_max.z ) m_max.z = point.z;
	}

	void add( const v3f& point )
	{
		if( point.x < m_min.x ) m_min.x = point.x;
		if( point.y < m_min.y ) m_min.y = point.y;
		if( point.z < m_min.z ) m_min.z = point.z;

		if( point.x > m_max.x ) m_max.x = point.x;
		if( point.y > m_max.y ) m_max.y = point.y;
		if( point.z > m_max.z ) m_max.z = point.z;
	}

	void add( const Aabb& box )
	{
		if( box.m_min.x < m_min.x ) m_min.x = box.m_min.x;
		if( box.m_min.y < m_min.y ) m_min.y = box.m_min.y;
		if( box.m_min.z < m_min.z ) m_min.z = box.m_min.z;

		if( box.m_max.x > m_max.x ) m_max.x = box.m_max.x;
		if( box.m_max.y > m_max.y ) m_max.y = box.m_max.y;
		if( box.m_max.z > m_max.z ) m_max.z = box.m_max.z;
	}

	bool rayTest( const yyRay& r )
	{
		float t1 = (m_min.x - r.m_origin.x)*r.m_invDir.x;
		float t2 = (m_max.x - r.m_origin.x)*r.m_invDir.x;
		float t3 = (m_min.y - r.m_origin.y)*r.m_invDir.y;
		float t4 = (m_max.y - r.m_origin.y)*r.m_invDir.y;
		float t5 = (m_min.z - r.m_origin.z)*r.m_invDir.z;
		float t6 = (m_max.z - r.m_origin.z)*r.m_invDir.z;

		float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
		float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));
		
		if (tmax < 0 || tmin > tmax) return false;

		return true;
	}

	void center( v4f& v ) const
	{
		v = v4f( m_min + m_max );
		v *= 0.5f;
	}

	float radius() {
		return m_min.distance(m_max) * 0.5f;
	}

	void extent( v4f& v ) { v = v4f( m_max - m_min ); }
	bool isEmpty() const { 
		return ((m_min == math::v4fFltMax) && (m_max == math::v4fFltMaxNeg))
			|| (m_min == m_max);
	}
	void reset() { m_min = math::v4fFltMax; m_max = math::v4fFltMaxNeg; }

	v4f m_min;
	v4f m_max;
};

#endif