#ifndef __KK_KRGUI_STYLE_H__
#define __KK_KRGUI_STYLE_H__

namespace Kr
{
	namespace Gui
	{
		struct Style
		{
			Style()
			{
				commonTextSpacing = 2; // for just text
				commonTextSpaceAddSize = 2;
				commonTextColor = ColorWhite;
				commonTextAlpha = 1.f;
				commonTextYOffset = 0.f; // move text up\down

											   // for buttons. gradient - up/down (only for rectangular buttons)
				buttonIdleColor1 = ColorLightGray; // not active button
				buttonIdleColor2 = ColorGray;
				buttonHoverColor1 = ColorDarkGray; // when mouse cursor over button
				buttonHoverColor2 = ColorDarkGray;
				buttonPushColor1 = ColorGray;      // when button pressed
				buttonPushColor2 = ColorLightGray;
				buttonDisabledColor1 = ColorLightGray;
				buttonDisabledColor2 = ColorLightGray;
				buttonTextSpacing = 2; // for text on buttons
				buttonTextSpaceAddSize = 2; // for text on buttons
				buttonTextIdleColor = ColorBlack; // for text on buttons
				buttonTextHoverColor = 0xFFB75403; //   when mouse cursor over button
				buttonTextPushColor = 0xFF7A3602; //   when button pressed
				buttonTextDisabledColor = ColorDarkGray; //   when button disabled
				buttonTextPositionAdd = Vec2f(0, 0);
				buttonBackgroundAlpha = 1.f; // for button BG
				buttonTextAlpha = 1.f; // for button text

											 // BG color for addRectangle
				rectangleIdleColor1 = ColorGray;
				rectangleIdleColor2 = ColorGray;
				rectangleHoverColor1 = ColorLightGray;
				rectangleHoverColor2 = ColorLightGray;

				nodeEditorColor1 = ColorGray;
				nodeEditorColor2 = ColorGray;
				nodeEditorNodeColor1 = ColorDarkGray;
				nodeEditorNodeColor2 = ColorDarkGray;
				nodeEditorNodeColorAlpha = 0.8f;

				nodeEditorSocketIdleColor = ColorLightCyan;

				// BG color for beginGroup
				groupColor1 = 0xFFA0A0A0;
				groupColor2 = 0xFFA0A0A0;
				groupHoverColor1 = ColorDarkGray;
				groupHoverColor2 = ColorDarkGray;
				groupBackgroundAlpha = 0.8f;

				popupColor1 = 0xFF515151;
				popupColor2 = 0xFF515151;
				popupAlpha = 0.82f;
				popupBorderIndent = Vec4f(2.f, 2.f, 2.f, 2.f);
				//float popupItemIndent = 3.f;


				menuBarHeight = 18.f;
				menuBarColor1 = 0xff555555;
				menuBarColor2 = 0xff666666;
				menuBarBGAlpha = 0.9f;
				menuBarTextSpacing = 2;
				menuBarTextSpaceAddSize = 2;
				menuBarTextBorderOffset = 12.f;
				menuBarTextYOffset = 2.f;
				menuBarHoverBGColor1 = 0xff007ACC;
				menuBarHoverBGColor2 = 0xff0070C0;
				menuBarHoverBGAlpha = 0.8f;
				menuBarTextHoverColor = 0xFFDDDDDD;
				menuBarTextColor = ColorWhite;
				menuBarDisabledTextColor = ColorGrey;

				menuItemHeight = 18.f;
				menuItemBGColor1 = 0xff555555;
				menuItemBGColor2 = 0xff555555;
				menuItemHoverBGColor1 = 0xff007ACC;
				menuItemHoverBGColor2 = 0xff0070C0;
				menuItemHoverBGAlpha = 0.8f;
				menuItemTextSpacing = 2;
				menuItemTextSpaceAddSize = 2;
				menuItemTextYOffset = 2.f;
				menuItemTextHoverColor = 0xFF111111;
				menuItemTextColor = ColorWhite;
				menuItemIconSize = Vec2f(menuItemHeight, menuItemHeight);
				menuItemIconSubmenuSize = Vec2f(menuItemHeight, menuItemHeight);
				menuItemIconTextOffset = 2.f;
				menuItemItemTextShortcutOffset = 40.f;

				separatorHeight = 5.f;
				separatorLineWidth = 1.f;
				separatorLineColor = 0xFF777777;
				separatorLineBorderOffset = 3.f;

				rangeSliderBgColor = 0xff797979;
				rangeSliderFgColor = ColorLightGray;

				textInputBgColor = 0xff000000;
				textInputTextColor = 0xffFFFFFF;
				textInputSelectedTextColor = 0xff000000;
				textInputSelectedBgColor = 0xff0000AA;

				checkboxBoxTextOffset = 5.f;
				checkboxCheckSymbol = '#';
				checkboxUncheckSymbol = '_';

				// for using this, you need to create special font, where 'symbol' must have own icon
				// see example in main.cpp
				/*char16_t menuItemIcon = 0;
				char16_t menuItemSubmenuIcon = 0;*/
				iconFont = nullptr;
			}

			float commonTextSpacing ; // for just text
			float commonTextSpaceAddSize ; 
			Vec4f commonTextColor ; 
			float commonTextAlpha ; 
			float commonTextYOffset ; // move text up\down

			// for buttons. gradient - up/down (only for rectangular buttons)
			Vec4f buttonIdleColor1 ; // not active button
			Vec4f buttonIdleColor2 ;
			Vec4f buttonHoverColor1 ; // when mouse cursor over button
			Vec4f buttonHoverColor2 ;
			Vec4f buttonPushColor1 ;      // when button pressed
			Vec4f buttonPushColor2 ;
			Vec4f buttonDisabledColor1 ;
			Vec4f buttonDisabledColor2 ;
			float buttonTextSpacing ; // for text on buttons
			float buttonTextSpaceAddSize ; // for text on buttons
			Vec4f buttonTextIdleColor    ; // for text on buttons
			Vec4f buttonTextHoverColor   ; //   when mouse cursor over button
			Vec4f buttonTextPushColor    ; //   when button pressed
			Vec4f buttonTextDisabledColor ; //   when button disabled
			Vec2f buttonTextPositionAdd ;
			float buttonBackgroundAlpha ; // for button BG
			float buttonTextAlpha ; // for button text

			// BG color for addRectangle
			Vec4f rectangleIdleColor1 ;
			Vec4f rectangleIdleColor2 ;
			Vec4f rectangleHoverColor1 ;
			Vec4f rectangleHoverColor2 ;

			Vec4f nodeEditorColor1 ;
			Vec4f nodeEditorColor2 ;
			Vec4f nodeEditorNodeColor1 ;
			Vec4f nodeEditorNodeColor2 ;
			float nodeEditorNodeColorAlpha ;

			Vec4f nodeEditorSocketIdleColor ;

			// BG color for beginGroup
			Vec4f groupColor1   ;
			Vec4f groupColor2   ;
			Vec4f groupHoverColor1 ;
			Vec4f groupHoverColor2 ;
			float groupBackgroundAlpha ;

			Vec4f popupColor1   ;
			Vec4f popupColor2   ;
			float popupAlpha    ;
			Vec4f popupBorderIndent ;
			//float popupItemIndent = 3.f;


			float menuBarHeight ;
			Vec4f menuBarColor1 ;
			Vec4f menuBarColor2 ;
			float menuBarBGAlpha ;
			float menuBarTextSpacing ;
			float menuBarTextSpaceAddSize ;
			float menuBarTextBorderOffset ;
			float menuBarTextYOffset ;
			Vec4f menuBarHoverBGColor1 ;
			Vec4f menuBarHoverBGColor2 ;
			float menuBarHoverBGAlpha  ;
			Vec4f menuBarTextHoverColor ;
			Vec4f menuBarTextColor ;
			Vec4f menuBarDisabledTextColor ;

			float menuItemHeight ;
			Vec4f menuItemBGColor1 ;
			Vec4f menuItemBGColor2 ;
			Vec4f menuItemHoverBGColor1 ;
			Vec4f menuItemHoverBGColor2 ;
			float menuItemHoverBGAlpha  ;
			float menuItemTextSpacing ;
			float menuItemTextSpaceAddSize ;
			float menuItemTextYOffset ;
			Vec4f menuItemTextHoverColor ;
			Vec4f menuItemTextColor ;
			Vec2f menuItemIconSize ;
			Vec2f menuItemIconSubmenuSize ;
			float menuItemIconTextOffset ;
			float menuItemItemTextShortcutOffset ;

			float separatorHeight ;
			float separatorLineWidth ;
			Vec4f separatorLineColor ;
			float separatorLineBorderOffset ;

			Vec4f rangeSliderBgColor ;
			Vec4f rangeSliderFgColor ;

			Vec4f textInputBgColor ;
			Vec4f textInputTextColor ;
			Vec4f textInputSelectedTextColor ;
			Vec4f textInputSelectedBgColor ;

			float    checkboxBoxTextOffset ;
			char16_t checkboxCheckSymbol   ;
			char16_t checkboxUncheckSymbol ;

			// for using this, you need to create special font, where 'symbol' must have own icon
			// see example in main.cpp
			/*char16_t menuItemIcon = 0;
			char16_t menuItemSubmenuIcon = 0;*/
			Font * iconFont ;
		};
	}
}

#endif