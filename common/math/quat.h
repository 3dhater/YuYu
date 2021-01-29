#ifndef _GAME_QUAT_H_
#define _GAME_QUAT_H_
#include <cmath>
#include "Math/vec.h"

class Quat
{
public:

	f32 x, y, z, w;

	Quat()
	{
		identity();
	}

	Quat( const Quat& o ):
		x(o.x),
		y(o.y),
		z(o.z),
		w(o.w)
	{
	}

	Quat( f32 i1, f32 i2, f32 i3, f32 real )
		:
		x(i1),
		y(i2),
		z(i3),
		w(real)
	{
	}

	Quat(const v3f& _axis, const f32& _angle)
	{
		setRotation(_axis, _angle);
	}
		//	create quaternion from Euler angles
	Quat( f32 Euler_x, f32 Euler_y, f32 Euler_z )
	{
		set( Euler_x, Euler_y, Euler_z );
	}
		//	create quaternion from Euler angles
	Quat( const v4f& v ){ set( v ); }

	f32* data(){return &x;}
		//	get quaternion in kkVector4f form
	v4f	get() const { return v4f( x, y, z, w ); }

		//	get real part
	f32	getReal() const { return w; }

		//	set quaternion with components
		//	\param i1: imagine1, first component
		//	\param i2: imagine2, second component
		//	\param i3: imagine3, third component
		//	\param real: real part, fourth component
	void set( f32 i1, f32 i2, f32 i3, f32 real ){
		x = i1;
		y = i2;
		z = i3;
		w = real;
	}

		//	set quaternion from Euler angles
	void set( const v4f& v ){ set( v.x, v.y, v.z ); }

		//	set quaternion from Euler angles
		// \param Euler_x: x angle
		// \param Euler_y: y angle
		// \param Euler_z: z angle
	void set( f32 Euler_x, f32 Euler_y, f32 Euler_z ){
		Euler_x *= 0.5f;
		Euler_y *= 0.5f;
		Euler_z *= 0.5f;
		f32 c1 = cos( Euler_x );
		f32 c2 = cos( Euler_y );
		f32 c3 = cos( Euler_z );
		f32 s1 = sin( Euler_x );
		f32 s2 = sin( Euler_y );
		f32 s3 = sin( Euler_z );
		w	=	(c1 * c2 * c3) + (s1 * s2 * s3);
		x	=	(s1 * c2 * c3) - (c1 * s2 * s3);
		y	=	(c1 * s2 * c3) + (s1 * c2 * s3);
		z	=	(c1 * c2 * s3) - (s1 * s2 * c3);
	}

	void setRotation(const v3f& axis, const f32& _angle)
	{
		f32 d = axis.length();
		f32 s = sin(_angle * 0.5f) / d;
		x = axis.x * s;
		y = axis.y * s;
		z = axis.z * s;
		w = cos(_angle * 0.5f);
	}

		//	set real part
		//	\param r: real part
	void setReal( f32 r ){
		w = r;
	}

		//	reset quaternion
	void identity(){
		x = 0.f;
		y = 0.f;
		z = 0.f;
		w = 1.f;
	}

		//	assing other
		//	\return this quaternion
	Quat& operator=( const Quat& o ){
		x = o.x;
		y = o.y;
		z = o.z;
		w = o.w;
		return *this;
	}

		//	multiplication with other
		//	\param q: other quaternion
		//	\return new quaternion
	Quat operator*( const Quat& q )const{
		return Quat(
			w * q.x + x * q.w + y * q.z - z * q.y,
			w * q.y + y * q.w + z * q.x - x * q.z,
			w * q.z + z * q.w + x * q.y - y * q.x,
			w * q.w - x * q.x - y * q.y - z * q.z);
	}

		//	multiplication with other
		//	\param q: other quaternion
	void operator*=( const Quat& q ){
		x = w * q.x + x * q.w + y * q.z - z * q.y;
		y = w * q.y + y * q.w + z * q.x - x * q.z;
		z = w * q.z + z * q.w + x * q.y - y * q.x;
		w = w * q.w - x * q.x - y * q.y - z * q.z;
	}

		//	compare
		//	\param q: other quaternion
		// \return \b true if not equal
	bool operator!=( const Quat& q )const{
		if( x != q.x ) return true;
		if( y != q.y ) return true;
		if( z != q.z ) return true;
		if( w != q.w ) return true;
		return false;
	}

		//	compare
		//	\param q: other quaternion
		// \return \b true if equal
	bool operator==( const Quat& q )const{
		if( x != q.x ) return false;
		if( y != q.y ) return false;
		if( z != q.z ) return false;
		if( w != q.w ) return false;
		return true;
	}

		//	add other
		//	\param q: other quaternion
		// \return new quaternion
	Quat operator+( const Quat& o ) const {
		return Quat(
			x + o.x,
			y + o.y,
			z + o.z,
			w + o.w );
	}

		//	substract other
		//	\param q: other quaternion
		// \return new quaternion
	Quat operator-( const Quat& o ) const {
		return Quat(
			x - o.x,
			y - o.y,
			z - o.z,
			w - o.w );
	}

		//	invert
		// \return new quaternion
	Quat operator-(){
		x = -x;
		y = -y;
		z = -z;
		return Quat( x, y, z, w );
	}

	f32 operator[]( s32 index ) const 
	{
		assert( ( index >= 0 ) && ( index < 4 ) );
		return ( &x )[ index ];
	}

	f32& operator[]( s32 index ) 
	{
		assert( ( index >= 0 ) && ( index < 4 ) );
		return ( &x )[ index ];
	}

		//	length
		// \return length
	f32	getLength(){return std::sqrt( x*x+y*y+z*z+w*w );}
	f32 length2() const{return dot(*this);}

	Quat operator*(f32 s) const
	{
		return Quat(s*x, s*y, s*z, s*w);
	}

	Quat lerp(Quat q1, Quat q2, f32 time)
	{
		const f32 scale = 1.0f - time;
		return (*this = (q1*scale) + (q2*time));
	}

	f32 dot(const Quat& q2) const
	{
		return (x * q2.x) + (y * q2.y) + (z * q2.z) + (w * q2.w);
	}

	Quat slerp(Quat q1, Quat q2, f32 time, f32 threshold) 
	{
		f32 angle = q1.dot(q2);
		// make sure we use the short rotation
		if (angle < 0.0f)
		{
			q1 = q1 * -1.0f;
			angle *= -1.0f;
		}

		if (angle <= (1-threshold)) // spherical interpolation
		{
			const f32 theta = acosf(angle);
			const f32 invsintheta = 1.f / (sinf(theta));
			const f32 scale = sinf(theta * (1.0f-time)) * invsintheta;
			const f32 invscale = sinf(theta * time) * invsintheta;
			return (*this = (q1*scale) + (q2*invscale));
		}
		else // linear interploation
			return lerp(q1,q2,time);
	}

		//	normalize
		// \return normalized quaternion
	Quat& normalize(){
		f32 len = this->getLength();
		if( len ){
			f32 len2 = 1.f / len;
			x *= len2;
			y *= len2;
			z *= len2;
			w *= len2;
		}
		return *this;
	}

		//	invert
	void invert()
	{
		x = -x;
		y = -y;
		z = -z;
	}

};

#endif