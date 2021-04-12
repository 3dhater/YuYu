#ifndef _YY_MATERIAL_H__
#define _YY_MATERIAL_H__

#include "yy_color.h"

enum class yyMaterialType
{
	Simple,



	/*Default,
	SimpleTransparentBlend,
	Terrain,
	Depth,*/
};

struct yyMaterial
{
	yyMaterial()
	{
		m_wireframe = false;
		m_cullBackFace = false;
		m_type = yyMaterialType::Simple;
		m_opacity = 0.5f;

		m_sunDir.set(0.3f, 0.6f, 0.3f);
		m_ambientColor = ColorGray;
		m_baseColor = ColorWhite;
		m_selfLight = 0.3f;
		
		m_fogData.x = 0.5f;
		m_fogColor = ColorWhite;
	}
	yyMaterialType m_type;
	bool m_wireframe;
	bool m_cullBackFace;
	f32 m_opacity;

	yyColor m_fogColor;
	v4f m_fogData;
	void SetFogStart(f32 v) {
		m_fogData.x = v;
		m_fogData.y = 1.f - v;
		if (m_fogData.y == 0.f)
			m_fogData.y = 0.001f;
	}
	void SetFogColor(const yyColor& c) {
		m_fogColor = c;
	}

	yyColor m_baseColor;

	v3f m_sunDir;        
	yyColor m_ambientColor;
	f32 m_selfLight;  // уровень освещённости 0.f...1.f
};

#endif