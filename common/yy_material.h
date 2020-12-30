#ifndef _YY_MATERIAL_H__
#define _YY_MATERIAL_H__

enum class yyMaterialType
{
	Simple,
	Default,
	SimpleTransparentBlend,
	Terrain,
	Depth,
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
		m_ambientColor.set(1.f, 1.f, 1.f);
		m_selfLight = 0.3f;
	}
	yyMaterialType m_type;
	bool m_wireframe;
	bool m_cullBackFace;
	f32 m_opacity;

	v3f m_sunDir;        
	v3f m_ambientColor;
	f32 m_selfLight;  // уровень освещённости 0.f...1.f
};

#endif