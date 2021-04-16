#ifndef _YY_COLOR_H__
#define _YY_COLOR_H__

#include <cmath>

// I don't know how to call it
#define YY_ColorDivider 0.00392156862745f

class yyColorBytes
{
public:
	yyColorBytes()
	:
		r(0),
		g(0),
		b(0),
		a(255)
	{}
	yyColorBytes(u8 R, u8 G, u8 B, u8 A)
	:
		r(R),
		g(G),
		b(B),
		a(A)
	{}

	yyColorBytes( u32 uint_data ) // 0xFF112233 ARGB
	{
		r = static_cast<u8>( uint_data >> 16u );
		g = static_cast<u8>( uint_data >> 8u );
		b = static_cast<u8>( uint_data );
		a = static_cast<u8>( uint_data >> 24u );
	}

	u8 r,g,b,a;

	bool	operator==( const yyColorBytes& v ) const 
	{
		if( r != v.r ) return false;
		if( g != v.g ) return false;
		if( b != v.b ) return false;
		if( a != v.a ) return false;
		return true;
	}

	bool	operator!=( const yyColorBytes& v ) const 
	{
		if( r != v.r ) return true;
		if( g != v.g ) return true;
		if( b != v.b ) return true;
		if( a != v.a ) return true;
		return false;
	}
};

class yyColor
{
public:
	
	f32	m_data[ 4u ];

	yyColor()
	{
		m_data[ 0u ] = m_data[ 1u ] = m_data[ 2u ] = 0.f;
		m_data[ 3u ] = 1.f;
	}

	yyColor( f32 v )
	{
		m_data[ 0u ] = m_data[ 1u ] = m_data[ 2u ] = v;
		m_data[ 3u ] = 1.f;
	}

	yyColor( f32 r, f32 g, f32 b, f32 a = 1.f )
	{
		m_data[ 0u ] = r;
		m_data[ 1u ] = g;
		m_data[ 2u ] = b;
		m_data[ 3u ] = a;
	}

	yyColor( s32 r, s32 g, s32 b, s32 a = 255 )
	{
		this->setAsByteAlpha( a );
		this->setAsByteRed( r );
		this->setAsByteGreen( g );
		this->setAsByteBlue( b );
	}

	yyColor( u32 uint_data ) // 0xFF112233 ARGB
	{
		setAsInteger( uint_data );
	}

	v4f getV4f()const { return v4f(m_data[0], m_data[1], m_data[2], m_data[3]); }

	const f32 * data() const { return &m_data[ 0u ]; }

	const f32 getRed() const   { return m_data[ 0u ]; }
	const f32 getGreen() const { return m_data[ 1u ]; }
	const f32 getBlue() const  { return m_data[ 2u ]; }
	const f32 getAlpha() const { return m_data[ 3u ]; }

	const u8 getAsByteRed() const   { return static_cast<u8>( m_data[ 0u ] * 255. ); }
	const u8 getAsByteGreen() const { return static_cast<u8>( m_data[ 1u ] * 255. ); }
	const u8 getAsByteBlue() const  { return static_cast<u8>( m_data[ 2u ] * 255. ); }
	const u8 getAsByteAlpha() const { return static_cast<u8>( m_data[ 3u ] * 255. ); }
	
	bool	operator==( const yyColor& v ) const 
	{
		if( m_data[0] != v.m_data[0] ) return false;
		if( m_data[1] != v.m_data[1] ) return false;
		if( m_data[2] != v.m_data[2] ) return false;
		if( m_data[3] != v.m_data[3] ) return false;
		return true;
	}

	bool	operator!=( const yyColor& v ) const 
	{
		if( m_data[0] != v.m_data[0] ) return true;
		if( m_data[1] != v.m_data[1] ) return true;
		if( m_data[2] != v.m_data[2] ) return true;
		if( m_data[3] != v.m_data[3] ) return true;
		return false;
	}

	void normalize()
	{
		f32 len = std::sqrt(( m_data[ 0 ] * m_data[ 0 ] ) + (m_data[ 1 ] * m_data[ 1 ] ) + ( m_data[ 2 ] * m_data[ 2 ] ) );
		if(len > 0)
		{
			len = 1.0f/len;
		}
		m_data[0] *= len;
		m_data[1] *= len;
		m_data[2] *= len;
	}

	// 0xff112233
	u32 getAsInteger()
	{
		return YY_MAKEFOURCC(
			this->getAsByteBlue(),
			this->getAsByteGreen(),
			this->getAsByteRed(),
			this->getAsByteAlpha()
		);
	}

	void setAlpha( f32 v ){ m_data[ 3u ] = v;	}
	void setRed( f32 v )  { m_data[ 0u ] = v;	}
	void setGreen( f32 v ){ m_data[ 1u ] = v;	}
	void setBlue( f32 v ) { m_data[ 2u ] = v;	}

	void setAsByteAlpha( s32 v ){ m_data[ 3u ] = static_cast<f32>(v) * YY_ColorDivider; }
	void setAsByteRed( s32 v )  { m_data[ 0u ] = static_cast<f32>(v) * YY_ColorDivider; }
	void setAsByteGreen( s32 v ){ m_data[ 1u ] = static_cast<f32>(v) * YY_ColorDivider; }
	void setAsByteBlue( s32 v ) { m_data[ 2u ] = static_cast<f32>(v) * YY_ColorDivider; }

	void setAsInteger( u32 v )
	{
		this->setAsByteRed( static_cast<u8>( v >> 16u ) );
		this->setAsByteGreen( static_cast<u8>( v >> 8u ) );
		this->setAsByteBlue( static_cast<u8>( v ) );
		this->setAsByteAlpha( static_cast<u8>( v >> 24u ) );
	}

	void set( f32 r, f32 g, f32 b, f32 a = 1. )
	{
		setAlpha( a );
		setRed( r );
		setGreen( g );
		setBlue( b );
	}

	void set( f32 v, f32 a = 1. )
	{
		setAlpha( a );
		setRed( v );
		setGreen( v );
		setBlue( v );
	}
};

	//	HTML colors
const yyColor ColorAliceBlue            = 0xffF0F8FF;
const yyColor ColorAntiqueWhite         = 0xffFAEBD7;
const yyColor ColorAqua                 = 0xff00FFFF;
const yyColor ColorAquamarine           = 0xff7FFFD4;
const yyColor ColorAzure                = 0xffF0FFFF;
const yyColor ColorBeige                = 0xffF5F5DC;
const yyColor ColorBisque               = 0xffFFE4C4;
const yyColor ColorBlack                = 0xff000000;
const yyColor ColorBlanchedAlmond       = 0xffFFEBCD;
const yyColor ColorBlue                 = 0xff0000FF;
const yyColor ColorBlueViolet           = 0xff8A2BE2;
const yyColor ColorBrown                = 0xffA52A2A;
const yyColor ColorBurlyWood            = 0xffDEB887;
const yyColor ColorCadetBlue            = 0xff5F9EA0;
const yyColor ColorChartreuse           = 0xff7FFF00;
const yyColor ColorChocolate            = 0xffD2691E;
const yyColor ColorCoral                = 0xffFF7F50;
const yyColor ColorCornflowerBlue       = 0xff6495ED;
const yyColor ColorCornsilk             = 0xffFFF8DC;
const yyColor ColorCrimson              = 0xffDC143C;
const yyColor ColorCyan                 = 0xff00FFFF;
const yyColor ColorDarkBlue             = 0xff00008B;
const yyColor ColorDarkCyan             = 0xff008B8B;
const yyColor ColorDarkGoldenRod        = 0xffB8860B;
const yyColor ColorDarkGray             = 0xffA9A9A9;
const yyColor ColorDarkGrey             = 0xffA9A9A9;
const yyColor ColorDarkGreen            = 0xff006400;
const yyColor ColorDarkKhaki            = 0xffBDB76B;
const yyColor ColorDarkMagenta          = 0xff8B008B;
const yyColor ColorDarkOliveGreen       = 0xff556B2F;
const yyColor ColorDarkOrange           = 0xffFF8C00;
const yyColor ColorDarkOrchid           = 0xff9932CC;
const yyColor ColorDarkRed              = 0xff8B0000;
const yyColor ColorDarkSalmon           = 0xffE9967A;
const yyColor ColorDarkSeaGreen         = 0xff8FBC8F;
const yyColor ColorDarkSlateBlue        = 0xff483D8B;
const yyColor ColorDarkSlateGray        = 0xff2F4F4F;
const yyColor ColorDarkSlateGrey        = 0xff2F4F4F;
const yyColor ColorDarkTurquoise        = 0xff00CED1;
const yyColor ColorDarkViolet           = 0xff9400D3;
const yyColor ColorDeepPink             = 0xffFF1493;
const yyColor ColorDeepSkyBlue          = 0xff00BFFF;
const yyColor ColorDimGray              = 0xff696969;
const yyColor ColorDimGrey              = 0xff696969;
const yyColor ColorDodgerBlue           = 0xff1E90FF;
const yyColor ColorFireBrick            = 0xffB22222;
const yyColor ColorFloralWhite          = 0xffFFFAF0;
const yyColor ColorForestGreen          = 0xff228B22;
const yyColor ColorFuchsia              = 0xffFF00FF;
const yyColor ColorGainsboro            = 0xffDCDCDC;
const yyColor ColorGhostWhite           = 0xffF8F8FF;
const yyColor ColorGold                 = 0xffFFD700;
const yyColor ColorGoldenRod            = 0xffDAA520;
const yyColor ColorGray                 = 0xff808080;
const yyColor ColorGrey                 = 0xff808080;
const yyColor ColorGreen                = 0xff008000;
const yyColor ColorGreenYellow          = 0xffADFF2F;
const yyColor ColorHoneyDew             = 0xffF0FFF0;
const yyColor ColorHotPink              = 0xffFF69B4;
const yyColor ColorIndianRed            = 0xffCD5C5C;
const yyColor ColorIndigo               = 0xff4B0082;
const yyColor ColorIvory                = 0xffFFFFF0;
const yyColor ColorKhaki                = 0xffF0E68C;
const yyColor ColorLavender             = 0xffE6E6FA;
const yyColor ColorLavenderBlush        = 0xffFFF0F5;
const yyColor ColorLawnGreen            = 0xff7CFC00;
const yyColor ColorLemonChiffon         = 0xffFFFACD;
const yyColor ColorLightBlue            = 0xffADD8E6;
const yyColor ColorLightCoral           = 0xffF08080;
const yyColor ColorLightCyan            = 0xffE0FFFF;
const yyColor ColorLightGoldenRodYellow = 0xffFAFAD2;
const yyColor ColorLightGray            = 0xffD3D3D3;
const yyColor ColorLightGrey            = 0xffD3D3D3;
const yyColor ColorLightGreen           = 0xff90EE90;
const yyColor ColorLightPink            = 0xffFFB6C1;
const yyColor ColorLightSalmon          = 0xffFFA07A;
const yyColor ColorLightSeaGreen        = 0xff20B2AA;
const yyColor ColorLightSkyBlue         = 0xff87CEFA;
const yyColor ColorLightSlateGray       = 0xff778899;
const yyColor ColorLightSlateGrey       = 0xff778899;
const yyColor ColorLightSteelBlue       = 0xffB0C4DE;
const yyColor ColorLightYellow          = 0xffFFFFE0;
const yyColor ColorLime                 = 0xff00FF00;
const yyColor ColorLimeGreen            = 0xff32CD32;
const yyColor ColorLinen                = 0xffFAF0E6;
const yyColor ColorMagenta              = 0xffFF00FF;
const yyColor ColorMaroon               = 0xff800000;
const yyColor ColorMediumAquaMarine     = 0xff66CDAA;
const yyColor ColorMediumBlue           = 0xff0000CD;
const yyColor ColorMediumOrchid         = 0xffBA55D3;
const yyColor ColorMediumPurple         = 0xff9370DB;
const yyColor ColorMediumSeaGreen       = 0xff3CB371;
const yyColor ColorMediumSlateBlue      = 0xff7B68EE;
const yyColor ColorMediumSpringGreen    = 0xff00FA9A;
const yyColor ColorMediumTurquoise      = 0xff48D1CC;
const yyColor ColorMediumVioletRed      = 0xffC71585;
const yyColor ColorMidnightBlue         = 0xff191970;
const yyColor ColorMintCream            = 0xffF5FFFA;
const yyColor ColorMistyRose            = 0xffFFE4E1;
const yyColor ColorMoccasin             = 0xffFFE4B5;
const yyColor ColorNavajoWhite          = 0xffFFDEAD;
const yyColor ColorNavy                 = 0xff000080;
const yyColor ColorOldLace              = 0xffFDF5E6;
const yyColor ColorOlive                = 0xff808000;
const yyColor ColorOliveDrab            = 0xff6B8E23;
const yyColor ColorOrange               = 0xffFFA500;
const yyColor ColorOrangeRed            = 0xffFF4500;
const yyColor ColorOrchid               = 0xffDA70D6;
const yyColor ColorPaleGoldenRod        = 0xffEEE8AA;
const yyColor ColorPaleGreen            = 0xff98FB98;
const yyColor ColorPaleTurquoise        = 0xffAFEEEE;
const yyColor ColorPaleVioletRed        = 0xffDB7093;
const yyColor ColorPapayaWhip           = 0xffFFEFD5;
const yyColor ColorPeachPuff            = 0xffFFDAB9;
const yyColor ColorPeru                 = 0xffCD853F;
const yyColor ColorPink                 = 0xffFFC0CB;
const yyColor ColorPlum                 = 0xffDDA0DD;
const yyColor ColorPowderBlue           = 0xffB0E0E6;
const yyColor ColorPurple               = 0xff800080;
const yyColor ColorRebeccaPurple        = 0xff663399;
const yyColor ColorRed                  = 0xffFF0000;
const yyColor ColorRosyBrown            = 0xffBC8F8F;
const yyColor ColorRoyalBlue            = 0xff4169E1;
const yyColor ColorSaddleBrown          = 0xff8B4513;
const yyColor ColorSalmon               = 0xffFA8072;
const yyColor ColorSandyBrown           = 0xffF4A460;
const yyColor ColorSeaGreen             = 0xff2E8B57;
const yyColor ColorSeaShell             = 0xffFFF5EE;
const yyColor ColorSienna               = 0xffA0522D;
const yyColor ColorSilver               = 0xffC0C0C0;
const yyColor ColorSkyBlue              = 0xff87CEEB;
const yyColor ColorSlateBlue            = 0xff6A5ACD;
const yyColor ColorSlateGray            = 0xff708090;
const yyColor ColorSlateGrey            = 0xff708090;
const yyColor ColorSnow                 = 0xffFFFAFA;
const yyColor ColorSpringGreen          = 0xff00FF7F;
const yyColor ColorSteelBlue            = 0xff4682B4;
const yyColor ColorTan                  = 0xffD2B48C;
const yyColor ColorTeal                 = 0xff008080;
const yyColor ColorThistle              = 0xffD8BFD8;
const yyColor ColorTomato               = 0xffFF6347;
const yyColor ColorTurquoise            = 0xff40E0D0;
const yyColor ColorViolet               = 0xffEE82EE;
const yyColor ColorWheat                = 0xffF5DEB3;
const yyColor ColorWhite                = 0xffffffff;
const yyColor ColorWhiteSmoke           = 0xffF5F5F5;
const yyColor ColorYellow               = 0xffFFFF00;
const yyColor ColorYellowGreen          = 0xff9ACD32;

#endif