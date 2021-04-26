#ifndef _YUYU_CFG_H_
#define _YUYU_CFG_H_

//set maximum gui elements
#define YY_MAX_GUI_ELEMENTS 0x100

#define YY_MAX_FONT_TEXTURES 20

#define yyPoolSetup_default_resourceCount 100
#define yyPoolSetup_default_materialCount 100 // every model have material so it must me at least equal
//  and it depends on YY_MAX_FONT_TEXTURES, because if font have 20 textures
//   then text can have 1-20 models
#define yyPoolSetup_default_modelCount yyPoolSetup_default_materialCount 

#endif