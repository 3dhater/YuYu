#ifndef _GAME_MAT_H_
#define _GAME_MAT_H_

#include "Math/vec.h"
#include "Math/quat.h"

class Mat4
{
public:

	v4f m_data[ 4u ]; //< components

	Mat4()
	{
		identity();
	}

	Mat4( const Mat4& m )
	{
		*this = m;
	}

	Mat4( const Quat& q )
	{
		identity();
		setRotation(q);
	}

	Mat4( f32 v )
	{
		fill( v );
	}

	Mat4( const v4f& x, const v4f& y, const v4f& z, const v4f& w ){
		m_data[ 0u ] = x;
		m_data[ 1u ] = y;
		m_data[ 2u ] = z;
		m_data[ 3u ] = w;
	}

	void		fill( f32 v ){
		m_data[ 0u ].set( v );
		m_data[ 1u ].set( v );
		m_data[ 2u ].set( v );
		m_data[ 3u ].set( v );
	}

	void		zero(){
		fill( 0.f );
	}


	void		identity(){
		auto * p = this->getPtr();
		p[ 0u ] = 1.f;
		p[ 1u ] = 0.f;
		p[ 2u ] = 0.f;
		p[ 3u ] = 0.f;

		p[ 4u ] = 0.f;
		p[ 5u ] = 1.f;
		p[ 6u ] = 0.f;
		p[ 7u ] = 0.f;

		p[ 8u ] = 0.f;
		p[ 9u ] = 0.f;
		p[ 10u ] = 1.f;
		p[ 11u ] = 0.f;

		p[ 12u ] = 0.f;
		p[ 13u ] = 0.f;
		p[ 14u ] = 0.f;
		p[ 15u ] = 1.f;

	}
	
	void setValue(f32 xx, f32 xy, f32 xz,
				  f32 yx, f32 yy, f32 yz,
				  f32 zx, f32 zy, f32 zz)
	{
		m_data[0].set(xx, xy, xz, 0.f);
		m_data[1].set(yx, yy, yz, 0.f);
		m_data[2].set(zx, zy, zz, 0.f);
	}

	void setRotation(const Quat& q)
	{
		f32 d = q.length2();
		f32 s = 2.0f / d;
		f32 xs = q.x * s, ys = q.y * s, zs = q.z * s;
		f32 wx = q.w * xs, wy = q.w * ys, wz = q.w * zs;
		f32 xx = q.x * xs, xy = q.x * ys, xz = q.x * zs;
		f32 yy = q.y * ys, yz = q.y * zs, zz = q.z * zs;
		setValue(
			1.0f - (yy + zz), xy - wz, xz + wy,
			xy + wz, 1.0f - (xx + zz), yz - wx,
			xz - wy, yz + wx, 1.0f - (xx + yy));
	}

	f32 * getPtr(){ return reinterpret_cast<f32*>(&m_data); }
	f32 * getPtrConst()const{ return (f32*)&m_data; }
		
	v4f&       operator[]( u32 i ){assert(i<=3); return m_data[ i ]; }
	const v4f& operator[]( u32 i ) const {assert(i<=3); return m_data[ i ]; }

		//	add
		// \param m: other matrix
		// \return new matrix
	Mat4 operator+( const Mat4& m ) const {
		Mat4 out = *this;

		out[ 0u ] += m[ 0u ];
		out[ 1u ] += m[ 1u ];
		out[ 2u ] += m[ 2u ];
		out[ 3u ] += m[ 3u ];

		return out;
	}

		//	substract
		// \param m: other matrix
		// \return new matrix
	Mat4 operator-( const Mat4& m ) const {
		Mat4 out = *this;

		out[ 0u ] -= m[ 0u ];
		out[ 1u ] -= m[ 1u ];
		out[ 2u ] -= m[ 2u ];
		out[ 3u ] -= m[ 3u ];

		return out;
	}

		//	multiplication
		// \param m: other matrix
		// \return new matrix
	Mat4 operator*( const Mat4& m ) const
	{
		return Mat4(
			m_data[ 0u ] * m[ 0u ].x + m_data[ 1u ] * m[ 0u ].y + m_data[ 2u ] * m[ 0u ].z + m_data[ 3u ] * m[ 0u ].w,
			m_data[ 0u ] * m[ 1u ].x + m_data[ 1u ] * m[ 1u ].y + m_data[ 2u ] * m[ 1u ].z + m_data[ 3u ] * m[ 1u ].w,
			m_data[ 0u ] * m[ 2u ].x + m_data[ 1u ] * m[ 2u ].y + m_data[ 2u ] * m[ 2u ].z + m_data[ 3u ] * m[ 2u ].w,
			m_data[ 0u ] * m[ 3u ].x + m_data[ 1u ] * m[ 3u ].y + m_data[ 2u ] * m[ 3u ].z + m_data[ 3u ] * m[ 3u ].w
		);
	}

	// возможно тут нужно по другому.
	v4f operator*( const v4f& v ) const {
		return v4f
		(
			v.x * m_data[0].x + v.y * m_data[1].x + v.z * m_data[2].x + v.w * m_data[2].x,
			v.x * m_data[0].y + v.y * m_data[1].y + v.z * m_data[2].y + v.w * m_data[2].y,
			v.x * m_data[0].z + v.y * m_data[1].z + v.z * m_data[2].z + v.w * m_data[2].z,
			v.x * m_data[0].w + v.y * m_data[1].w + v.z * m_data[2].w + v.w * m_data[2].w
		);
	}

		//	divide
		// \param m: other matrix
		// \return new matrix
	Mat4 operator/( const Mat4& m ) const {
		Mat4 out = *this;

		out[ 0u ] /= m[ 0u ];
		out[ 1u ] /= m[ 1u ];
		out[ 2u ] /= m[ 2u ];
		out[ 3u ] /= m[ 3u ];

		return out;
	}

		//	add
		// \param m: other matrix
		// \return this matrix
	Mat4& operator+=( const Mat4& m ){
		m_data[ 0u ] += m[ 0u ];
		m_data[ 1u ] += m[ 1u ];
		m_data[ 2u ] += m[ 2u ];
		m_data[ 3u ] += m[ 3u ];
		return *this;
	}

		//	substract
		// \param m: other matrix
		// \return this matrix
	Mat4& operator-=( const Mat4& m ){
		m_data[ 0u ] -= m[ 0u ];
		m_data[ 1u ] -= m[ 1u ];
		m_data[ 2u ] -= m[ 2u ];
		m_data[ 3u ] -= m[ 3u ];
		return *this;
	}

		//	multiplication
		// \param m: other matrix
		// \return this matrix
	Mat4& operator*=( const Mat4& m ){
		(*this) = (*this) * m;
		return *this;
	}

		//	divide
		// \param m: other matrix
		// \return this matrix
	Mat4& operator/=( const Mat4& m ){
		m_data[ 0u ] /= m[ 0u ];
		m_data[ 1u ] /= m[ 1u ];
		m_data[ 2u ] /= m[ 2u ];
		m_data[ 3u ] /= m[ 3u ];
		return *this;
	}

		//	transpose
	void transpose()
	{
		Mat4 tmp;
		tmp[ 0u ].x = this->m_data[ 0u ].x; //0
		tmp[ 0u ].y = this->m_data[ 1u ].x; //1
		tmp[ 0u ].z = this->m_data[ 2u ].x; //2
		tmp[ 0u ].w = this->m_data[ 3u ].x; //3

		tmp[ 1u ].x = this->m_data[ 0u ].y; //4
		tmp[ 1u ].y = this->m_data[ 1u ].y; //5
		tmp[ 1u ].z = this->m_data[ 2u ].y; //6
		tmp[ 1u ].w = this->m_data[ 3u ].y; //7

		tmp[ 2u ].x = this->m_data[ 0u ].z; //8
		tmp[ 2u ].y = this->m_data[ 1u ].z; //9
		tmp[ 2u ].z = this->m_data[ 2u ].z; //10
		tmp[ 2u ].w = this->m_data[ 3u ].z; //11

		tmp[ 3u ].x = this->m_data[ 0u ].w; //12
		tmp[ 3u ].y = this->m_data[ 1u ].w; //13
		tmp[ 3u ].z = this->m_data[ 2u ].w; //14
		tmp[ 3u ].w = this->m_data[ 3u ].w; //15
		this->m_data[ 0u ] = tmp[ 0u ];
		this->m_data[ 1u ] = tmp[ 1u ];
		this->m_data[ 2u ] = tmp[ 2u ];
		this->m_data[ 3u ] = tmp[ 3u ];
	}

	//https://www.scratchapixel.com/lessons/mathematics-physics-for-computer-graphics/matrix-inverse
	bool invert()
	{
		Mat4 mat; 
		auto ptr = this->getPtr();
		for(unsigned column = 0; column < 4; ++column) 
		{ 
			// Swap row in case our pivot point is not working
			auto column_data = m_data[column].data();
			if(column_data[column] == 0)
			{ 
				unsigned big = column; 
				for (unsigned row = 0; row < 4; ++row) 
				{
					auto row_data = m_data[row].data();
					auto big_data = m_data[big].data();
					if(fabs(row_data[column]) > fabs(big_data[column]))
						big = row; 
				}
				// Print this is a singular matrix, return identity ?
				if (big == column)
#ifdef _YUYU_H_
					yyLogWriteWarning("Singular matrix\n");
#else
					fprintf(stderr, "Singular matrix\n");
#endif
				// Swap rows                               
				else for (unsigned j = 0; j < 4; ++j) 
				{ 
					auto big_data = m_data[big].data();
					std::swap(column_data[j], big_data[j]); 

					auto other_column_data = mat.m_data[column].data();
					auto other_big_data = mat.m_data[big].data();
					std::swap(other_column_data[j], other_big_data[j]); 
				} 
			} 

			// Set each row in the column to 0  
			for (unsigned row = 0; row < 4; ++row)
			{ 
				if (row != column) 
				{ 
					auto row_data = m_data[row].data();
					f32 coeff = row_data[column] / column_data[column]; 
					if (coeff != 0) 
					{ 
						for (unsigned j = 0; j < 4; ++j) 
						{ 
							row_data[j] -= coeff * column_data[j]; 

							auto other_row_data = mat.m_data[row].data();
							auto other_column_data = mat.m_data[column].data();
							other_row_data[j] -= coeff * other_column_data[j]; 
						} 
						// Set the element to 0 for safety
						row_data[column] = 0; 
					} 
				} 
			}
		} 

		// Set each element of the diagonal to 1
		for (unsigned row = 0; row < 4; ++row) 
		{ 
			for (unsigned column = 0; column < 4; ++column) 
			{ 
				auto other_row_data = mat.m_data[row].data();
				auto row_data = m_data[row].data();
				other_row_data[column] /= row_data[row]; 
			} 
		} 

		*this = mat;
		return true;
	}

	bool invert2() {
		Mat4 r0, r1, r2, r3;
		f32 a, det, invDet;
		f32* mat = reinterpret_cast<f32 *>(this);

		det = mat[0 * 4 + 0] * mat[1 * 4 + 1] - mat[0 * 4 + 1] * mat[1 * 4 + 0];
		if (std::abs(det) < 1e-14) {
			return false;
		}
		v4f dd;

		invDet = 1.0f / det;

		r0[0][0] = mat[1 * 4 + 1] * invDet;
		r0[0][1] = -mat[0 * 4 + 1] * invDet;
		r0[1][0] = -mat[1 * 4 + 0] * invDet;
		r0[1][1] = mat[0 * 4 + 0] * invDet;
		r1[0][0] = r0[0][0] * mat[0 * 4 + 2] + r0[0][1] * mat[1 * 4 + 2];
		r1[0][1] = r0[0][0] * mat[0 * 4 + 3] + r0[0][1] * mat[1 * 4 + 3];
		r1[1][0] = r0[1][0] * mat[0 * 4 + 2] + r0[1][1] * mat[1 * 4 + 2];
		r1[1][1] = r0[1][0] * mat[0 * 4 + 3] + r0[1][1] * mat[1 * 4 + 3];
		r2[0][0] = mat[2 * 4 + 0] * r1[0][0] + mat[2 * 4 + 1] * r1[1][0];
		r2[0][1] = mat[2 * 4 + 0] * r1[0][1] + mat[2 * 4 + 1] * r1[1][1];
		r2[1][0] = mat[3 * 4 + 0] * r1[0][0] + mat[3 * 4 + 1] * r1[1][0];
		r2[1][1] = mat[3 * 4 + 0] * r1[0][1] + mat[3 * 4 + 1] * r1[1][1];
		r3[0][0] = r2[0][0] - mat[2 * 4 + 2];
		r3[0][1] = r2[0][1] - mat[2 * 4 + 3];
		r3[1][0] = r2[1][0] - mat[3 * 4 + 2];
		r3[1][1] = r2[1][1] - mat[3 * 4 + 3];

		det = r3[0][0] * r3[1][1] - r3[0][1] * r3[1][0];
		if (std::abs(det) < 1e-14) {
			return false;
		}

		invDet = 1.0f / det;

		a = r3[0][0];
		r3[0][0] = r3[1][1] * invDet;
		r3[0][1] = -r3[0][1] * invDet;
		r3[1][0] = -r3[1][0] * invDet;
		r3[1][1] = a * invDet;
		r2[0][0] = mat[2 * 4 + 0] * r0[0][0] + mat[2 * 4 + 1] * r0[1][0];
		r2[0][1] = mat[2 * 4 + 0] * r0[0][1] + mat[2 * 4 + 1] * r0[1][1];
		r2[1][0] = mat[3 * 4 + 0] * r0[0][0] + mat[3 * 4 + 1] * r0[1][0];
		r2[1][1] = mat[3 * 4 + 0] * r0[0][1] + mat[3 * 4 + 1] * r0[1][1];
		mat[2 * 4 + 0] = r3[0][0] * r2[0][0] + r3[0][1] * r2[1][0];
		mat[2 * 4 + 1] = r3[0][0] * r2[0][1] + r3[0][1] * r2[1][1];
		mat[3 * 4 + 0] = r3[1][0] * r2[0][0] + r3[1][1] * r2[1][0];
		mat[3 * 4 + 1] = r3[1][0] * r2[0][1] + r3[1][1] * r2[1][1];
		mat[0 * 4 + 0] = r0[0][0] - r1[0][0] * mat[2 * 4 + 0] - r1[0][1] * mat[3 * 4 + 0];
		mat[0 * 4 + 1] = r0[0][1] - r1[0][0] * mat[2 * 4 + 1] - r1[0][1] * mat[3 * 4 + 1];
		mat[1 * 4 + 0] = r0[1][0] - r1[1][0] * mat[2 * 4 + 0] - r1[1][1] * mat[3 * 4 + 0];
		mat[1 * 4 + 1] = r0[1][1] - r1[1][0] * mat[2 * 4 + 1] - r1[1][1] * mat[3 * 4 + 1];
		mat[0 * 4 + 2] = r1[0][0] * r3[0][0] + r1[0][1] * r3[1][0];
		mat[0 * 4 + 3] = r1[0][0] * r3[0][1] + r1[0][1] * r3[1][1];
		mat[1 * 4 + 2] = r1[1][0] * r3[0][0] + r1[1][1] * r3[1][0];
		mat[1 * 4 + 3] = r1[1][0] * r3[0][1] + r1[1][1] * r3[1][1];
		mat[2 * 4 + 2] = -r3[0][0];
		mat[2 * 4 + 3] = -r3[0][1];
		mat[3 * 4 + 2] = -r3[1][0];
		mat[3 * 4 + 3] = -r3[1][1];

		return true;
	}
	
	void setTranslation(const v3f& v) { m_data[3].set(v.x, v.y, v.z, 1.f); }
	void setTranslation(const v4f& v) { m_data[3].set(v.x, v.y, v.z, 1.f); }
};

class Mat3
{
public:
	v3f m_data[3];

	Mat3()
	{
		identity();
	}

	Mat3(const Mat3& m)
	{
		*this = m;
	}

	Mat3(const Quat& q)
	{
		setRotation(q);
	}

	Mat3(f32 v)
	{
		fill(v);
	}

	Mat3(const v3f& x, const v3f& y, const v3f& z) {
		m_data[0u] = x;
		m_data[1u] = y;
		m_data[2u] = z;
	}

	void		fill(f32 v) {
		m_data[0u].set(v);
		m_data[1u].set(v);
		m_data[2u].set(v);
	}

	void		zero() {
		fill(0.f);
	}


	void		identity() {
		auto * p = this->getPtr();
		p[0] = 1.f;
		p[1] = 0.f;
		p[2] = 0.f;

		p[3] = 0.f;
		p[4] = 1.f;
		p[5] = 0.f;

		p[6] = 0.f;
		p[7] = 0.f;
		p[8] = 1.f;
	}

	void setValue(f32 xx, f32 xy, f32 xz,
		f32 yx, f32 yy, f32 yz,
		f32 zx, f32 zy, f32 zz)
	{
		m_data[0].set(xx, xy, xz);
		m_data[1].set(yx, yy, yz);
		m_data[2].set(zx, zy, zz);
	}

	void setRotation(const Quat& q)
	{
		f32 d = q.length2();
		f32 s = 2.0f / d;
		f32 xs = q.x * s, ys = q.y * s, zs = q.z * s;
		f32 wx = q.w * xs, wy = q.w * ys, wz = q.w * zs;
		f32 xx = q.x * xs, xy = q.x * ys, xz = q.x * zs;
		f32 yy = q.y * ys, yz = q.y * zs, zz = q.z * zs;
		setValue(
			1.0f - (yy + zz), xy - wz, xz + wy,
			xy + wz, 1.0f - (xx + zz), yz - wx,
			xz - wy, yz + wx, 1.0f - (xx + yy));
	}

	f32 * getPtr() { return reinterpret_cast<f32*>(&m_data); }
	f32 * getPtrConst()const { return (f32*)&m_data; }

	v3f&       operator[](u32 i) { assert(i <= 3); return m_data[i]; }
	const v3f& operator[](u32 i) const { assert(i <= 3); return m_data[i]; }
};

namespace math
{

		//	create perspective matrix for left hand coordinate system
		// \param in_out: projection matrix
		// \param FOV: field of view
		// \param aspect: aspect ratio
		// \param Near: near clip plane
		// \param Far: far clip plane
	YY_FORCE_INLINE void  makePerspectiveLHMatrix( Mat4& out, f32 FOV, f32 aspect,
		f32 Near, f32 Far)
	{
		f32 S	=	std::sin( 0.5f * FOV );
		f32 C	=	std::cos( 0.5f * FOV );
		f32 H = C / S;
		f32 W = H / aspect;
		out[ 0 ] = v4f( W, 0.f, 0.f, 0.f );
		out[ 1 ] = v4f( 0.f, H, 0.f, 0.f );
		out[ 2 ] = v4f( 0.f, 0.f, Far/(Far-Near), 1.f );
		out[ 3 ] = v4f( 0.f, 0.f, -out[2].z * Near, 0.f );
	}

		//	create perspective matrix for right hand coordinate system
		// \param in_out: projection matrix
		// \param FOV: field of view
		// \param aspect: aspect ratio
		// \param Near: near clip plane
		// \param Far: far clip plane
	YY_FORCE_INLINE void  makePerspectiveRHMatrix( Mat4& out, f32 FOV, f32 aspect,
		f32 Near, f32 Far)
	{
		f32 S	=	std::sin( 0.5f * FOV );
		f32 C	=	std::cos( 0.5f * FOV );
		f32 H = C / S;
		f32 W = H / aspect;
		out[ 0u ] = v4f( W, 0.f, 0.f, 0.f );
		out[ 1u ] = v4f( 0.f, H, 0.f, 0.f );
		out[ 2u ] = v4f( 0.f, 0.f, Far / (Near - Far), -1.f );
		out[ 3u ] = v4f( 0.f, 0.f, out[ 2u ].z * Near, 0.f );
	}

		//	create orthogonal matrix for right hand coordinate system
		// \param in_out: projection matrix
		// \param width: width of viewport
		// \param height: height of viewport
		// \param Near: near clip plane
		// \param Far: far clip plane
	YY_FORCE_INLINE void  makeOrthoRHMatrix( Mat4& out, f32 width, f32 height,
		f32 Near, f32 Far )
	{
		out[ 0u ] = v4f( 2.f / width, 0.f, 0.f, 0.f );
		out[ 1u ] = v4f( 0.f, 2.f / height, 0.f, 0.f );
		out[ 2u ] = v4f( 0.f, 0.f, 1.f / (Near - Far), 0.f );
		out[ 3u ] = v4f( 0.f, 0.f, out[ 2u ].z * Near, 1.f );
	}

	// based on XNA math library
	YY_FORCE_INLINE void makeMatrixOrthographicOffCenterRH(
		Mat4& out,
		f32 ViewLeft,
		f32 ViewRight,
		f32 ViewBottom,
		f32 ViewTop,
		f32 NearZ,
		f32 FarZ
	)
	{
		f32    ReciprocalWidth;
		f32    ReciprocalHeight;

		ReciprocalWidth = 1.0f / (ViewRight - ViewLeft);
		ReciprocalHeight = 1.0f / (ViewTop - ViewBottom);

		out.m_data[0].set(ReciprocalWidth + ReciprocalWidth, 0.0f, 0.0f, 0.0f);
		out.m_data[1].set(0.0f, ReciprocalHeight + ReciprocalHeight, 0.0f, 0.0f);
		out.m_data[2].set(0.0f, 0.0f, 1.0f / (NearZ - FarZ), 0.0f);
		out.m_data[3].set(-(ViewLeft + ViewRight) * ReciprocalWidth,
			-(ViewTop + ViewBottom) * ReciprocalHeight,
			out.m_data[2].z * NearZ,
			1.0f);
	}
		
		//	create \a look \a at matrix for right hand coordinate system
		// \param in_out: view matrix
		// \param eye: camera position
		// \param center: camera target
		// \param up: up vector
	YY_FORCE_INLINE void  makeLookAtRHMatrix( Mat4& out, const v4f& eye, const v4f& center, const v4f& up )
	{
		v4f f( center - eye );
		f.normalize2();
			
		v4f s;
		f.cross2( up, s );
		s.normalize2();

		v4f u;
		s.cross2( f, u );

		out.identity();

		out[ 0u ].x = s.x;
		out[ 1u ].x = s.y;
		out[ 2u ].x = s.z;
		out[ 0u ].y = u.x;
		out[ 1u ].y = u.y;
		out[ 2u ].y = u.z;
		out[ 0u ].z = -f.x;
		out[ 1u ].z = -f.y;
		out[ 2u ].z = -f.z;
		out[ 3u ].x = -s.dot( eye );
		out[ 3u ].y = -u.dot( eye );
		out[ 3u ].z = f.dot( eye );
	}

	YY_FORCE_INLINE void  makeLookAtLHMatrix( const v4f& eye, const v4f& center,const v4f& up, Mat4& out)
	{
		v4f f( center - eye );
		f.normalize2();
			
		v4f s;
		f.cross2( up, s );
		s.normalize2();

		v4f u;
		s.cross2( f, u );

		out.identity();

		out[ 0u ].x = s.x;
		out[ 1u ].x = s.y;
		out[ 2u ].x = s.z;
		out[ 0u ].y = u.x;
		out[ 1u ].y = u.y;
		out[ 2u ].y = u.z;
		out[ 0u ].z = f.x;
		out[ 1u ].z = f.y;
		out[ 2u ].z = f.z;
		out[ 3u ].x = -s.dot( eye );
		out[ 3u ].y = -u.dot( eye );
		out[ 3u ].z = -f.dot( eye );
	}

	YY_FORCE_INLINE void makeTranslationMatrix( const v4f& position, Mat4& out )
	{
		out[ 3u ].x = position.x;
		out[ 3u ].y = position.y;
		out[ 3u ].z = position.z;
	}

	YY_FORCE_INLINE void makeScaleMatrix( const v4f& scale, Mat4& out )
	{
		out[ 0u ].x = scale.x;
		out[ 1u ].y = scale.y;
		out[ 2u ].z = scale.z;
	}
	
		//Vector-matrix product 
	YY_FORCE_INLINE v4f mul( const v4f& vec, const Mat4& mat )
	{
		return v4f
		(
			mat[ 0u ].x * vec.x + mat[ 1u ].x * vec.y + mat[ 2u ].x * vec.z + mat[ 3u ].x * vec.w,
			mat[ 0u ].y * vec.x + mat[ 1u ].y * vec.y + mat[ 2u ].y * vec.z + mat[ 3u ].y * vec.w,
			mat[ 0u ].z * vec.x + mat[ 1u ].z * vec.y + mat[ 2u ].z * vec.z + mat[ 3u ].z * vec.w,
			mat[ 0u ].w * vec.x + mat[ 1u ].w * vec.y + mat[ 2u ].w * vec.z + mat[ 3u ].w * vec.w
		);
	}
	YY_FORCE_INLINE v3f mul( const v3f& vec, const Mat4& mat )
	{
		return v3f
		(
			mat[ 0u ].x * vec.x + mat[ 1u ].x * vec.y + mat[ 2u ].x * vec.z + mat[ 3u ].x,
			mat[ 0u ].y * vec.x + mat[ 1u ].y * vec.y + mat[ 2u ].y * vec.z + mat[ 3u ].y,
			mat[ 0u ].z * vec.x + mat[ 1u ].z * vec.y + mat[ 2u ].z * vec.z + mat[ 3u ].z
		);
	}
	YY_FORCE_INLINE v3f mulBasis( const v3f& vec, const Mat4& mat )
	{
		return v3f
		(
			mat[ 0u ].x * vec.x + mat[ 1u ].x * vec.y + mat[ 2u ].x * vec.z,
			mat[ 0u ].y * vec.x + mat[ 1u ].y * vec.y + mat[ 2u ].y * vec.z,
			mat[ 0u ].z * vec.x + mat[ 1u ].z * vec.y + mat[ 2u ].z * vec.z
		);
	}
	
	YY_FORCE_INLINE Quat matToQuat( const Mat4& mat )
	{
		Quat q;
		f32 trace = mat[ 0 ].x + mat[ 1 ].y + mat[ 2 ].z;
		if( trace > 0.0f )
		{

			f32 t = trace + 1.0f;
			f32 s = std::sqrt( t ) * 0.5f;

			q.w = s * t;
			q.x = ( mat[ 2 ].y - mat[ 1 ].z ) * s;
			q.y = ( mat[ 0 ].z - mat[ 2 ].x ) * s;
			q.z = ( mat[ 1 ].x - mat[ 0 ].y ) * s;

		}
		else 
		{
			static s32	next[ 3 ] = { 1, 2, 0 };
			s32 i = 0;
			if( mat[ 1 ][ 1 ] > mat[ 0 ][ 0 ] ) 
				i = 1;
			if ( mat[ 2 ][ 2 ] > mat[ i ][ i ] ) 
				i = 2;
			s32 j = next[ i ];
			s32 k = next[ j ];

			f32 t = ( mat[ i ][ i ] - ( mat[ j ][ j ] + mat[ k ][ k ] ) ) + 1.0f;
			f32 s = std::sqrt( t ) * 0.5f;

			q[i] = s * t;
			q[3] = ( mat[ k ][ j ] - mat[ j ][ k ] ) * s;
			q[j] = ( mat[ j ][ i ] + mat[ i ][ j ] ) * s;
			q[k] = ( mat[ k ][ i ] + mat[ i ][ k ] ) * s;
		}
		return q;
	}


	/*YY_FORCE_INLINE v3f mul( const v3f& vec, const Mat4& mat )
	{
		return v3f
		(
			mat[ 0u ].x * vec.x + mat[ 1u ].x * vec.y + mat[ 2u ].x * vec.z + mat[ 3u ].x,
			mat[ 0u ].y * vec.x + mat[ 1u ].y * vec.y + mat[ 2u ].y * vec.z + mat[ 3u ].y,
			mat[ 0u ].z * vec.x + mat[ 1u ].z * vec.y + mat[ 2u ].z * vec.z + mat[ 3u ].z
		);
	}*/

		//Matrix-vector product
	/*YY_FORCE_INLINE v4f mul( const Mat4& mat, const v4f& vec )
	{
		return v4f
		(
			mat[ 0u ].x * vec.x + mat[ 0u ].y * vec.y + mat[ 0u ].z * vec.z + mat[ 0u ].w * vec.w,
			mat[ 1u ].x * vec.x + mat[ 1u ].y * vec.y + mat[ 1u ].z * vec.z + mat[ 1u ].w * vec.w,
			mat[ 2u ].x * vec.x + mat[ 2u ].y * vec.y + mat[ 2u ].z * vec.z + mat[ 2u ].w * vec.w,
			mat[ 3u ].x * vec.x + mat[ 3u ].y * vec.y + mat[ 3u ].z * vec.z + mat[ 3u ].w * vec.w
		);
	}
	YY_FORCE_INLINE v3f mul( const Mat4& mat, const v3f& vec )
	{
		return v3f
		(
			mat[ 0u ].x * vec.x + mat[ 0u ].y * vec.y + mat[ 0u ].z * vec.z + mat[ 0u ].w,
			mat[ 1u ].x * vec.x + mat[ 1u ].y * vec.y + mat[ 1u ].z * vec.z + mat[ 1u ].w,
			mat[ 2u ].x * vec.x + mat[ 2u ].y * vec.y + mat[ 2u ].z * vec.z + mat[ 2u ].w
		);
	}*/

}//end math::

#endif