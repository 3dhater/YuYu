#ifndef _GAME_VEC_H_
#define _GAME_VEC_H_

#include <cmath>
#include <limits>
#include <cassert>

#define Infinity std::numeric_limits<f32>::infinity()
#define Epsilon std::numeric_limits<f32>::epsilon()

enum class yyVectorComponent
{
	x,y,z,w
};

struct v4i
{
	v4i():x(0),y(0),z(0),w(0){}
	v4i(s32 _x, s32 _y, s32 _z, s32 _w):x(_x),y(_y),z(_z),w(_w){}
	void set(s32 _x, s32 _y, s32 _z, s32 _w) { x = _x; y = _y; z = _z; w = _w; }
	s32* data() { return &x; }
	s32 x, y, z, w;
};

struct v3f;
struct v4f
{
	v4f():x(0.f),y(0.f),z(0.f),w(0.f){}
	v4f(const v3f& v);
	v4f(f32 _v):x(_v),y(_v),z(_v),w(_v){}
	v4f(f32 _x, f32 _y, f32 _z, f32 _w):x(_x),y(_y),z(_z),w(_w){}
	f32 x, y, z, w;

	const f32* cdata()const{return &x;}
	f32* data(){return &x;}
	f32 operator[]( s32 index )const{assert( ( index >= 0 ) && ( index < 4 ) );return ( &x )[ index ];}
	f32& operator[]( s32 index ) {assert( ( index >= 0 ) && ( index < 4 ) );return ( &x )[ index ];}

	void set(f32 _x, f32 _y, f32 _z, f32 _w){x=_x;y=_y;z=_z;w=_w;}
	void set(f32 val){x=y=z=w=val;}
	void operator+=( const v4f& v ){x += v.x;y += v.y;z += v.z;w += v.w;}
	void operator-=( const v4f& v ){x -= v.x;y -= v.y;z -= v.z;w -= v.w;}
	void operator*=( const v4f& v ){x *= v.x;y *= v.y;z *= v.z;w *= v.w;}
	void operator/=( const v4f& v ){x /= v.x;y /= v.y;z /= v.z;w /= v.w;}
	
	v4f operator+(const v4f& v)const{v4f r;r.x=x+v.x;r.y=y+v.y;r.z=z+v.z;r.w=w+v.w;return r;}
	v4f operator-(const v4f& v)const{v4f r;r.x=x-v.x;r.y=y-v.y;r.z=z-v.z;r.w=w-v.w;return r;}
	v4f operator*(const v4f& v)const{v4f r;r.x=x*v.x;r.y=y*v.y;r.z=z*v.z;r.w=w*v.w;return r;}
	v4f operator/(const v4f& v)const{v4f r;r.x=x/v.x;r.y=y/v.y;r.z=z/v.z;r.w=w/v.w;return r;}

	v4f operator-()const{v4f r;r.x=-x;r.y=-y;r.z=-z;r.w=-w;return r;}

	v4f& operator=(const v3f& v);


	f32	dot(const v4f& V2)const{return (x*V2.x)+(y*V2.y)+(z*V2.z)+(w*V2.w);}
	f32	dot()const{return (x*x)+(y*y)+(z*z)+(w*w);}
	void	normalize2()
	{
		f32 len = std::sqrt(dot());
		if(len > 0)
			len = 1.0f/len;
		x *= len; y *= len; z *= len; w *= len;
	}
	void cross2( const v4f& a, v4f& out )const{
		out.x = (y * a.z) - (z * a.y);
		out.y = (z * a.x) - (x * a.z);
		out.z = (x * a.y) - (y * a.x);
	}
	f32 lengthSqrt()const{return(x*x)+(y*y)+(z*z);}
	f32 length()const{return std::sqrt(lengthSqrt());}
	f32 distance( const v4f& from )const{return v4f(x-from.x,y-from.y,z-from.z,1.f).length();}
	bool operator==(const v4f& v)const{
		if(x != v.x)return false;
		if(y != v.y)return false;
		if(z != v.z)return false;
		if(w != v.w)return false;
		return true;
	}
};
YY_FORCE_INLINE v4f operator*(const f32& s, const v4f& v){return v * s;}

struct v2i
{
	v2i():x(0),y(0){}
	v2i(const v2i& o):x(o.x),y(o.y){}
	v2i(s32 X, s32 Y):x(X),y(Y){}
	s32 x, y;
	v2i operator/(const v2i& v)const{v2i r;r.x=x/v.x;r.y=y/v.y;return r;}
	v2i operator/(int v)const { v2i r; r.x = x / v; r.y = y / v; return r; }
	v2i operator*(f32 v)const{v2i r;r.x=(s32)((f32)x*v);r.y=(s32)((f32)y/v);return r;}
	v2i operator*(const v2i& v)const { v2i r; r.x = x * v.x; r.y = y * v.y; return r; }
	v2i operator-(const v2i& v)const { v2i r; r.x = x - v.x; r.y = y - v.y; return r; }
	v2i operator+(const v2i& v)const { v2i r; r.x = x + v.x; r.y = y + v.y; return r; }
};

struct v2f
{
	v2f():x(0.f),y(0.f){}
	v2f(f32 _x, f32 _y) :x(_x), y(_y) {}
	v2f(f32 val):x(val),y(val){}
	f32 x, y;
	void set(f32 _x, f32 _y){x=_x;y=_y;}
	v2f operator*(const v2f& v)const { v2f r; r.x = x * v.x; r.y = y * v.y; return r; }
	v2f operator-(const v2f& v)const { v2f r; r.x = x - v.x; r.y = y - v.y; return r; }
	v2f operator+(const v2f& v)const { v2f r; r.x = x + v.x; r.y = y + v.y; return r; }
	v2f operator/(const v2f& v)const { v2f r; r.x = x / v.x; r.y = y / v.y; return r; }
	v2f& operator=(const v2i& v) { x = (f32)v.x; y = (f32)v.y; return *this; }
	void operator+=(const v2f& v) { x += v.x; y += v.y; }
	void operator-=(const v2f& v) { x -= v.x; y -= v.y; }
	void operator*=(const v2f& v) { x *= v.x; y *= v.y; }
	void operator/=(const v2f& v) { x /= v.x; y /= v.y; }
	bool operator==(const v2f& v)const {if (x != v.x)return false;if (y != v.y)return false;return true;}
	f32 distance(const v2f& from)const { return v2f(x - from.x, y - from.y).length(); }
	f32 length()const { return std::sqrt(lengthSqrt()); }
	f32 lengthSqrt()const { return(x*x) + (y*y); }
};

struct v3i
{
	v3i() :x(0), y(0), z(0) {}
	v3i(const v3i& o) :x(o.x), y(o.y), z(o.z) {}
	v3i(s32 X, s32 Y, s32 Z) :x(X), y(Y), z(Z) {}
	s32 x, y, z;
	v3i operator/(const v3i& v)const { v3i r; r.x = x / v.x; r.y = y / v.y; r.z = z / v.z; return r; }
	v3i operator/(int v)const { v3i r; r.x = x / v; r.y = y / v; r.z = z / v; return r; }
};

struct v3f
{
	v3f():x(0.f),y(0.f),z(0.f){}
	v3f(f32 _v):x(_v),y(_v),z(_v){}
	v3f(f32 _x, f32 _y, f32 _z):x(_x),y(_y),z(_z){}
	f32 x,y,z;
	void set(f32 _x, f32 _y, f32 _z){x=_x;y=_y;z=_z;}
	void set(f32 val) { x = y = z = val; }
	f32* data(){return &x;}
	void operator+=( const v3f& v ){x += v.x;y += v.y;z += v.z;}
	void operator-=( const v3f& v ){x -= v.x;y -= v.y;z -= v.z;}
	void operator*=( const v3f& v ){x *= v.x;y *= v.y;z *= v.z;}
	void operator/=( const v3f& v ){x /= v.x;y /= v.y;z /= v.z;}
	f32 operator[]( s32 index )const{assert( ( index >= 0 ) && ( index < 3 ) );return ( &x )[ index ];}
	f32& operator[]( s32 index ) {assert( ( index >= 0 ) && ( index < 3 ) );return ( &x )[ index ];}
	v3f operator+(const v3f& v)const{v3f r;r.x=x+v.x;r.y=y+v.y;r.z=z+v.z;return r;}
	v3f operator-(const v3f& v)const{v3f r;r.x=x-v.x;r.y=y-v.y;r.z=z-v.z;return r;}
	v3f operator*(const v3f& v)const{v3f r;r.x=x*v.x;r.y=y*v.y;r.z=z*v.z;return r;}
	v3f operator/(const v3f& v)const{v3f r;r.x=x/v.x;r.y=y/v.y;r.z=z/v.z;return r;}
	v3f operator+(const v4f& v)const { v3f r; r.x = x + v.x; r.y = y + v.y; r.z = z + v.z; return r; }
	v3f& operator=(const v4f& v) { x = v.x; y = v.y; z = v.z; return *this; }
	void operator+=(const v4f& v) { x += v.x; y += v.y; z += v.z; }
	bool operator==(const v3f& v)const {
		if (x != v.x)return false;
		if (y != v.y)return false;
		if (z != v.z)return false;
		return true;
	}
	v3f cross( const v3f& a )const{
		v3f out;
		out.x = (y * a.z) - (z * a.y);
		out.y = (z * a.x) - (x * a.z);
		out.z = (x * a.y) - (y * a.x);
		return out;
	}
	void cross2( const v3f& a, v3f& out )const{
		out.x = (y * a.z) - (z * a.y);
		out.y = (z * a.x) - (x * a.z);
		out.z = (x * a.y) - (y * a.x);
	}
	void	normalize2()
	{
		f32 len = std::sqrt(dot());
		if(len > 0)
			len = 1.0f/len;
		x *= len; y *= len; z *= len;
	}
	f32	dot(const v3f& V2)const{return (x*V2.x)+(y*V2.y)+(z*V2.z);}
	f32	dot()const{return (x*x)+(y*y)+(z*z);}
	v3f operator-()const{v3f r;r.x = -x;r.y = -y;r.z = -z;return r;}
	f32 length() const { return std::sqrt(length2()); }
	f32 length2() const { return dot(*this); }
};

YY_FORCE_INLINE v4f& v4f::operator=(const v3f& v) { this->x = v.x; this->y = v.y; this->z = v.z; this->w = 1.f; return *this; }
YY_FORCE_INLINE v4f::v4f(const v3f& v):x(v.x), y(v.y), z(v.z),w(0.f) {}




#endif