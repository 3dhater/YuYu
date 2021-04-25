#ifndef _YUYU_CFG_H_
#define _YUYU_CFG_H_

//set maximum gui elements
#define YY_MAX_GUI_ELEMENTS 0x100

#define YY_MAX_FONT_TEXTURES 20

#define yyPoolSetup_default_resourceCount 100
#define yyPoolSetup_default_materialCount 100 // every model have material so it must me at least equal
#define yyPoolSetup_default_modelCount yyPoolSetup_default_materialCount //  and it depends on YY_MAX_FONT_TEXTURES

#endif