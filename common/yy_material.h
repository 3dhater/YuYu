#ifndef _YY_MATERIAL_H__
#define _YY_MATERIAL_H__

enum class yyMaterialType
{
	Simple,
	Default,
	SimpleTransparentBlend,

};

struct yyMaterial
{
	yyMaterial()
	{
		m_wireframe = false;
		m_cullBackFace = false;
		m_type = yyMaterialType::Simple;
		m_opacity = 0.5f;
	}
	yyMaterialType m_type;
	bool m_wireframe;
	bool m_cullBackFace;
	f32 m_opacity;
};

#endif