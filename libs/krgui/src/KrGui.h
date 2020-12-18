#ifndef __KK_KRGUI_H__
#define __KK_KRGUI_H__

#include "KrGuiCommon.h"
#include "KrGuiConfig.h"
#include "KrGuiForward.h"
#include "KrGuiVector.h"
#include "KrGuiCharInfo.h"
#include "KrGuiColors.h"
#include "KrGuiImage.h"
#include "KrGuiStyle.h"
#include "KrGuiTexture.h"
#include "KrGuiVertex.h"
#include "KrGuiDrawCommands.h"
#include "KrGuiDrawCommandsGroup.h"
#include "KrGuiFontGlyph.h"
#include "KrGuiFont.h"
#include "KrGuiGroup.h"
#include "KrGuiNodeEditorNodeSocket.h"
#include "KrGuiNodeEditorNode.h"
#include "KrGuiNodeEditorSocketConnection.h"
#include "KrGuiNodeEditor.h"
#include "KrGuiWindow.h"

#include "KrGui_dds.h"
#include "KrGui_string.h"
#include "KrGui_file_io.h"

namespace Kr
{
	namespace Gui
	{
		namespace _internal
		{
			inline bool pointInRect( int x, int y, const Vec4f& rect )
			{
				if(x>rect.x){if(x<rect.z){if(y>rect.y){if(y<rect.w){return true;}}}}
				return false;
			}
			inline bool pointInRect( float x, float y, const Vec4f& rect )
			{
				if(x>rect.x){if(x<rect.z){if(y>rect.y){if(y<rect.w){return true;}}}}
				return false;
			}
		}

		enum class GraphicsSystemType
		{
			OpenGL3
		};

		struct PopupMenuInformation
		{
			PopupMenuInformation()
			{
				m_isActive = false;
				m_isActive_ptr = nullptr;
				m_style_ptr = nullptr;
				m_subMenuActiveId = -2;
				m_subMenuActive = false;
				m_insideSubMenu = false;
				m_inRect = false; 
				m_addMenuItemCounter = 0;
				m_hoverItemId = -1;
				m_shortcutWidthMax = 0.f;
				m_id = -1;
				m_scrollValue = 0.f;
				m_scrollSpeed = 50.f;
				m_drawGroup = nullptr;
			}

			bool m_isActive;
			bool * m_isActive_ptr;
			Style* m_style_ptr;

			Vec2f m_lastItemPosition;


			int m_subMenuActiveId; // когда наводится на субменю то нужно запомнить его m_activeItemId
			//int  m_subMenuActiveIdLast = -1;
			
			bool m_subMenuActive;
			bool m_insideSubMenu;
			bool m_inRect;

			int m_addMenuItemCounter;
			int m_hoverItemId; // при наведении мышки на пункт нужно запомнить его номер
			//int m_currentItemId = -1;

			Vec2f m_position;
			Vec2f m_size; // must grow when add items
			Vec2f m_sizeEnd;
			float m_shortcutWidthMax;
			int m_id;

			float m_scrollValue;// try to scroll
			float m_scrollSpeed;

			DrawCommandsGroup* m_drawGroup;

			void reset()
			{
				m_size.set(0.f,0.f);
				m_sizeEnd.set(0.f,0.f);
				m_hoverItemId = -1;
				m_subMenuActiveId = -2;
				m_id = -1;
				m_isActive = false;
				m_subMenuActive = false;
				m_insideSubMenu = false;
				m_inRect = false;
				m_addMenuItemCounter = 0;
				m_hoverItemId = -1;
			}
		};

		struct MenuBarInformation
		{
			MenuBarInformation()
			{
				m_current = false;
				m_style = nullptr;
				m_useNewLine = true;
				m_inRect = false;
				m_enabled = true; // для выключения меню на совсем
				m_activated = false; // когда нажали
				m_insidePopupFind = false;
				m_insidePopup = false;
				m_activeItem = -1;
				m_itemCounter = 0;
			}

			Vec4f m_rect;
			bool m_current;
			Style* m_style;
			bool m_useNewLine;
			bool m_inRect;
			bool m_enabled; // для выключения меню на совсем
			bool m_activated; // когда нажали
			bool m_insidePopupFind;
			bool m_insidePopup;
			int m_activeItem;
			int m_itemCounter;
		};

		class GuiSystem
		{
			//std::vector<DrawCommands> m_drawCommands;
			size_t m_drawCommandGroups_capacity;
			size_t m_drawCommandGroups_size;
			std::vector<DrawCommandsGroup*> m_drawCommandGroups;
			DrawCommandsGroup* m_drawGroupMain; // must be for objects outside nodeEditor
			DrawCommandsGroup* m_currentDrawGroup;
			//DrawCommandsGroup* m_prevDrawGroup= nullptr;

			NodeEditorNode * m_topNode;
			bool m_blockInputGlobal;
			bool m_blockInputGlobal_old;
			bool m_disableBlockInputGlobal;

			Texture * m_whiteTexture;

			// get new command or old(already allocated)
			DrawCommands* _getDrawCommand()
			{
				return m_currentDrawGroup->_getDrawCommand();
			}
			void _clearCommands()
			{
				m_currentDrawGroup->m_priority = 0;
				m_drawCommandGroups_size = 1; 
				for( size_t i = 0, sz = m_drawCommandGroups.size(); i < sz; ++i )
				{
					m_drawCommandGroups[ i ]->_clear();
				}
			}
			void _setNewDrawGroup(bool setMain)
			{
				//m_prevDrawGroup = m_currentDrawGroup;
				auto prev_dg = m_currentDrawGroup;
				if( setMain )
				{
					m_currentDrawGroup = m_drawGroupMain;
				}
				else
				{
					++m_drawCommandGroups_size;
					if( m_drawCommandGroups_size > m_drawCommandGroups_capacity )
					{
						++m_drawCommandGroups_capacity;
						m_drawCommandGroups.push_back(new DrawCommandsGroup);
					}
					m_currentDrawGroup = m_drawCommandGroups[m_drawCommandGroups_size-1];
					m_currentDrawGroup->m_priority = ++m_drawGroupPriorityCounter;
				}

				m_currentDrawGroup->m_prevDrawGroup = prev_dg;
			}
			void _setPrevDrawGroup()
			{
				auto prev_dg = m_currentDrawGroup->m_prevDrawGroup;
				m_currentDrawGroup = prev_dg;
			}

			std::vector<Font*> m_fontCache;
			Font * m_defaultFont;
			Font * m_currentFont;

			GraphicsSystemType m_gsType;

			Style m_defaultStyle;

			bool _init(GraphicsSystemType, const char* defaultFontDir, const char* defaultFontFilename);

			bool   m_doWindowInput;
			bool   m_IsShift;
			bool   m_IsAlt;
			bool   m_IsEnter;
			bool   m_IsCtrl;
			bool   m_IsEsc;
			
			bool   m_pauseMove;
			int m_drawGroupPriorityCounter;

			//float m_maxItemHeightOnCurrentRow = 0;
			float m_maxItemHeightOnCurrentRow[256];
			unsigned int   m_maxItemHeightOnCurrentRowCount;


			int m_uniqueIdCounter;
			int m_pressedItemIdLMB;
			int m_pressedItemIdMMB;
			int m_pressedItemIdRMB;
		
			Window* m_currentWindow;

			// `last` means last added item
			int m_lastCursorHoverItemId; // if mouse hover on item, then remember ID of this item
			int m_lastCursorMoveItemId; // if mouse move in area of item
			int m_lastDisabledItemId;
			int m_lastItemId;
			int m_lastKeyboardInputItemId;
			int m_lastKeyboardInputItemIdExit;

			int m_currentGroupInRectId;
			int m_lastGroupInRectId;
			int m_lastDrawPointY;

			float m_guiZoom;
			float m_guiZoom_old; // для запоминания m_guiZoom

			Vec2f m_drawPointer;
			Vec2f m_cursorCoordsOld;
			Vec4f m_OSWindowClientRect;
			Vec2f m_OSWindowClientSize;


			Vec4f m_currentClipRect;
			Vec4f m_currentNodeClipRect;

			Vec4f m_firstColor;
			Vec4f m_secondColor;
			Vec4f m_textColor;

			//bool       m_useParentClipRect = false;
			//Vec4f      m_parentClipRect;
			Vec4f        m_parentClipRects[256];
			unsigned int m_parentClipRectCount;

			Gui::Group   m_groupInfo[256]; // информация о всех группах за текущий фрейм
			unsigned int m_groupInfoCount; // нужно обнулять каждый новый фрейм
			Gui::Group*  m_groupList[256]; // тут хранятся вложенные группы
			unsigned int m_groupListCurrent; // прибавлять каждый beginGroup, уменьшать в endGroup

			//Group*   m_currentGroup = nullptr;
			NodeEditor* m_currentNodeEditor;

			//float*              m_parentScrollValue = nullptr;
			float               m_scrollMultipler;
		
			void _addRectangle(const Vec4f& clipRect, const Vec4f& buildRect, const Vec4f& rounding = Vec4f() );
			void _addPictureBox(const Vec4f& clipRect, const Vec4f& buildRect, unsigned long long textureAddressOrID);
			void _addText(const Vec4f& clipRect, const Vec4f& textRect, const char16_t* text, int numOfChars, float spacing, float spaceAdd, bool centerOnZoom);

			std::u16string m_inputTextBuffer;
			std::u16string m_inputTextBuffer_out;

			bool m_nextItemIgnoreInput;

			enum mouseButton
			{
				LMB,
				MMB,
				RMB
			};
			void _updateMouseInputInRectMoveHover()
			{
				if(m_disableInput) return;
				if( m_mouseDelta.x == 0 && m_mouseDelta.y == 0 ) // mouse not move
					m_lastCursorHoverItemId = m_uniqueIdCounter; // then hover
				else // mouse move
					m_lastCursorMoveItemId = m_uniqueIdCounter;
			}
			void _updateMouseInputInRect(mouseButton button)
			{
				if(m_disableInput) return;
				if( m_doWindowInput )
				{
					_updateMouseInputInRectMoveHover();

					switch (button)
					{
					case GuiSystem::LMB:
						if( m_mouseIsLMB_firstClick ) // check when button pressed first time
						{
							// if nobody clicked before and this item not disabled, then this item will be `pressed`
							if( m_pressedItemIdLMB == 0 && m_lastDisabledItemId != m_uniqueIdCounter ) // нажатие (и удержание)
								m_pressedItemIdLMB = m_uniqueIdCounter;
						}
						break;
					case GuiSystem::MMB:
						if( m_mouseIsMMB_firstClick ) // check when button pressed first time
						{
							// if nobody clicked before and this item not disabled, then this item will be `pressed`
							if( m_pressedItemIdMMB == 0 && m_lastDisabledItemId != m_uniqueIdCounter ) // нажатие (и удержание)
								m_pressedItemIdMMB = m_uniqueIdCounter;
						}
						break;
					case GuiSystem::RMB:
						if( m_mouseIsRMB_firstClick ) // check when button pressed first time
						{
							// if nobody clicked before and this item not disabled, then this item will be `pressed`
							if( m_pressedItemIdRMB == 0 && m_lastDisabledItemId != m_uniqueIdCounter ) // нажатие (и удержание)
								m_pressedItemIdRMB = m_uniqueIdCounter;
						}
						break;
					}

				}
			}
			void _updateMouseInput(mouseButton button)
			{
				if(m_disableInput) return;
				if( _internal::pointInRect( m_cursorCoords.x, m_cursorCoords.y, m_currentClipRect ) )
				{
					m_currentWindow->m_currentItemId_inRect = m_uniqueIdCounter;
					_updateMouseInputInRect(button);
				}
			}
			void _checkMouseHover()
			{
				if(m_disableInput) return;
				if( _internal::pointInRect( m_cursorCoords.x, m_cursorCoords.y, m_currentClipRect ) )
				{
					m_currentWindow->m_currentItemId_inRect = m_uniqueIdCounter;
					if( m_mouseDelta.x == 0 && m_mouseDelta.y == 0 ) // mouse not move
						m_lastCursorHoverItemId = m_uniqueIdCounter; // then hover
					else // mouse move
						m_lastCursorMoveItemId = m_uniqueIdCounter;
				}
			}

			Vec2f* m_currentNodePosition;

			// элементы в узлах добавлются в центр узла. чтобы установить элемент на нужную позицию
			// нужно вызвать метод ...
			Vec2f  m_currentNodeContentOffset;
			//bool m_nexButtonIsNodeSocket = false;
			NodeEditorNode* m_nodeEditorPickedNode;
			NodeEditorNodeSocket* m_nodeEditorPickedSocket;

			PopupMenuInformation m_popupMenuZero;
			PopupMenuInformation m_popupMenuInfo[KRGUI_NUM_OF_POPUPS];
			unsigned int m_popupMenuInfoCount ; // add 1 every popupMenuBegin. do not --m_activePopupMenuSize
			PopupMenuInformation* m_popupMenuActive[KRGUI_POPUP_DEPTH]; // те, которые активированы
			unsigned int m_popupMenuActiveCount; // если 0 значит нет активного окна
			Vec4f m_popupMenuLastItemRect; // просто устанавливается на в каждом addMenuItem
			Vec2f m_popupMenuPosition;
			void _resetAllPopupMenu();
			bool _addMenuItem(const char16_t* text, const char16_t* shortcutText, char16_t iconFontSymbolLeft, char16_t iconFontSymbolRight1, char16_t iconFontSymbolRight2, bool * check, bool isSubMenu );

			MenuBarInformation m_currentMenuBar;
			//bool _beginMenuBarItem(const char16_t* text);

			void _checkStyle(Style** style);
			void _newId();
			void _checkSize(Vec2f* size);
			void _checkNewLine(float);
			void _setCurrentClipRect(const Vec2f& position, const Vec2f& size);
			void _checkParentClipRect();
			void _checkParentClipRect(Vec4f& rect);
			void _checkItemHeight(float size_y);
			void _checkZoomPosition(Vec2f* point);

			float m_deltaTime;

			bool m_disableInput;

			friend GuiSystem* CreateSystem( GraphicsSystemType type, const char* defaultFontDir, const char* defaultFontFileName );
			friend void GraphicsSystem_OpenGL3Draw(GuiSystem*);

		public:
			GuiSystem()
			{
				for (int i = 0; i < KRGUI_POPUP_DEPTH; ++i)
				{
					m_popupMenuActive[i] = nullptr;
				}

				m_drawCommandGroups_capacity = 1; // 1 because minimum with m_drawGroupMain
				m_drawCommandGroups_size = 1;
				m_drawGroupMain = nullptr;
				m_currentDrawGroup = nullptr;
				m_topNode = nullptr;
				m_blockInputGlobal = false;
				m_blockInputGlobal_old = false;
				m_disableBlockInputGlobal = false;
				m_whiteTexture = nullptr;
				m_defaultFont = nullptr;
				m_currentFont = nullptr;
				m_gsType = GraphicsSystemType::OpenGL3;
				m_doWindowInput = false;
				m_IsShift = false;
				m_IsAlt = false;
				m_IsEnter = false;
				m_IsCtrl = false;
				m_IsEsc = false;
				m_pauseMove = false;
				m_drawGroupPriorityCounter = 0;
				m_maxItemHeightOnCurrentRowCount = 0;
				m_uniqueIdCounter = 0;
				m_pressedItemIdLMB = 0;
				m_pressedItemIdMMB = 0;
				m_pressedItemIdRMB = 0;
				m_currentWindow = nullptr;
				m_lastCursorHoverItemId = 0; // if mouse hover on item, then remember ID of this item
				m_lastCursorMoveItemId = 0; // if mouse move in area of item
				m_lastDisabledItemId = 0;
				m_lastItemId = 0;
				m_lastKeyboardInputItemId = 0;
				m_lastKeyboardInputItemIdExit = 0;
				m_currentGroupInRectId = 0;
				m_lastGroupInRectId = 0;
				m_lastDrawPointY = 0;
				m_guiZoom = 1.f;
				m_guiZoom_old = 1.f; // для запоминания m_guiZoom
				m_parentClipRectCount = 0;
				m_groupInfoCount = 0; // нужно обнулять каждый новый фрейм
				m_groupListCurrent = 0; // прибавлять каждый beginGroup, уменьшать в endGroup
				m_currentNodeEditor = nullptr;
				m_scrollMultipler = 1.f;
				m_nextItemIgnoreInput = false;
				m_currentNodePosition = nullptr;
				m_nodeEditorPickedNode = nullptr;
				m_nodeEditorPickedSocket = nullptr;
				m_popupMenuInfoCount = 0;
				m_popupMenuActiveCount = 0;
				m_deltaTime = 0.f;
				m_disableInput = false;


				m_isEnterTextMode = false;
				m_mouseIsLMB_up = false;
				m_mouseIsLMB = false;
				m_mouseIsLMB_old = false;
				m_mouseIsLMB_firstClick = false;
				m_mouseIsMMB_up = false;
				m_mouseIsMMB = false;
				m_mouseIsMMB_old = false;
				m_mouseIsMMB_firstClick = false;
				m_mouseIsRMB_up = false;
				m_mouseIsRMB = false;
				m_mouseIsRMB_old = false;
				m_mouseIsRMB_firstClick = false;
			}
			~GuiSystem();

			// prepare GUI for new frame
			void newFrame( Window * guiWindow, float deltaTime );
			void endFrame();

			// set new window for drawing
			void switchWindow( Window * guiWindow );

			// draw all
			void render();

			// игнорировать ввод следующего элемента
			void setNextItemIgnoreInput(){m_nextItemIgnoreInput=true;}

			Font* createFontFromTexture(
				unsigned long long * textureArray, 
				int textureCount, 
				int textureWidth, 
				int textureHeight
			);
			Font* createFontFromDDS_RGBA8(
				const char16_t* fileName
			);
			Font* createFontFromSystem(
				const char16_t* fontName,
				unsigned int fontSize,
				bool bold,
				bool italic,
				bool aa,
				bool alpha,
				bool saveToFile = false,
				const char * fileDir = nullptr,
				const char * fileName = nullptr
			);
			Font* createFontFromFile(const char * dir, const char * fileName);
			void setCurrentFont(Font* font); // nullptr - set default
			Font* getCurrentFont(){return m_currentFont;}
			
			/*
			* rounding
			x-----w
			|     |
			|     |
			y-----z
			*/
			bool addButton( const char16_t* text = nullptr, Style* style = nullptr, const Vec2f& size = Vec2f(), bool enabled = true, bool textToCenter = true, const Vec4f& rounding = Vec4f() );
			bool addButtonSymbol( char16_t iconSymbol, Style* style = nullptr, const Vec2f& size = Vec2f(), bool enabled = true, bool textToCenter = true, const Vec4f& rounding = Vec4f() );
			// style: button... checkbox...
			bool addCheckBox(bool * v, Style* style, const char16_t* text = nullptr, bool placeTextOnRightSide = true,  bool enabled = true);

			void addRectangle( Style* style, const Vec2f& size, float Alpha = 1.f, const Vec4f& rounding = Vec4f() );

			// return true wher curser in area
			bool addSensorArea( const Vec2f& size );

			void addText(Style* style, const char16_t* fmt, ...);
			void addText(const char16_t* text, Style* style = nullptr);
			void addText(const char* text, Style* style = nullptr);

			void addPictureBox(const Vec2f& size, unsigned long long textureAddress, float Alpha = 1.f, const Vec4f& color = ColorWhite );

			// add range slider like in Blender
			// if isHorizontal != true then without text
			bool addRangeSlider( float minimum, float maximum, float * value, const Vec2f& size,
				bool isHorizontal = true,
				float speed = 1.f, Style* style = nullptr,
				const Vec4f& rounding = Vec4f() );
			bool addRangeSliderInt( int minimum, int maximum, int * value, const Vec2f& size,
				bool isHorizontal = true,
				float speed = 1.f, Style* style = nullptr,
				const Vec4f& rounding = Vec4f() );

			bool addValueSelector( float * value, const Vec2f& size,
				bool isHorizontal = true,
				float speed = 1.f, Style* style = nullptr,
				const Vec4f& rounding = Vec4f() );
			bool addValueSelectorLimit( float minim, float maxim, float * value, const Vec2f& size,
				bool isHorizontal = true,
				float speed = 1.f, Style* style = nullptr,
				const Vec4f& rounding = Vec4f() );

			// true when quit editing
			bool addTextInput( 
				const char16_t* text,
				const Vec2f& size,
				bool(*filter)(char16_t),
				Style* style = nullptr,
				const Vec4f& rounding = Vec4f() );
			const char16_t* getTextInputResult()
			{
				return m_inputTextBuffer_out.c_str();
			}

			bool addTextInputPopup(const Vec2f& size, char16_t* buf, size_t buf_size, size_t char_limit, bool(*filter)(char16_t), Style* style = nullptr );

			//bool beginGroup( Group * group );
			bool beginGroup(const Vec2f& size, bool * expandCollapse = nullptr, Style* style = nullptr);
			void setCurrentGroupContentHeight(float);
			Gui::Group* getCurrentGroup();
			void endGroup();

			bool beginButtonGroup( const char16_t* text, Style* style = nullptr, const Vec2f& size = Vec2f(), bool enabled = true, bool textToCenter = true, const Vec4f& rounding = Vec4f() );
			void endButtonGroup();

			void drawLine( const Vec2f& begin, const Vec2f& end, float size, const Vec4f& color1, const Vec4f& color2 );

			bool beginNodeEditor( NodeEditor * nodeEditor );
			bool beginNode( NodeEditorNode * node );
			void setCurrentNodeContentOffset(const Vec2f& offset)
			{
				m_currentNodeContentOffset = offset;
			}
			void endNode();
			void nodeEditorZoomIn(NodeEditor * nodeEditor, float);
			void nodeEditorZoomOut(NodeEditor * nodeEditor, float);
			void endNodeEditor();

			// move draw point on new line (+= m_max_item_height_on_current_row + offset)
			void newLine(float offset = 0.f);

			// text lenght in pixels
			int getTextLen( const char16_t * text, Vec2f * out_size, float spacing, float spaceAdd );
			float getTextMaxCharHeight( const char16_t * text );

			bool isLastItemCursorHover();
			bool isLastItemCursorMove();
			bool isLastItemPressed();
			bool isLastItemPressedOnce();
			bool isLastItemDisabled();
			bool isLastItemKeyboardInput();
			bool isLastItemKeyboardInputExit();

			void setScrollMultipler( float v ){m_scrollMultipler = v;}

			void movePause(bool);
			void moveLeftRight(float offset);

			void setDrawPosition( float x, float y, bool zeroHeight = true ); // directly set where to start to draw next item
			const Vec2f& getDrawPosition();

			GraphicsSystemType getGSType(){return m_gsType;}

			Texture* getWhiteTexture(){return m_whiteTexture;}

			// fileName = "out"
			// +
			// extesionName = ".dds"
			// =
			// out.dds
			void saveImageToFile(Image*, const char* fileName, const char* extesionName );
			bool loadImageFromFile(Image*, const char* fileName );

			// If you need main menu bar, draw it first
			// You can draw menu bar in other places, set customPosition
			bool menuBarBegin( bool enabled = true, Style* style = nullptr, bool useNewLine = true);
			// return true if in rect
			bool menuBarEnd(float * out_menuHeight = nullptr);
			bool menuBarMenu(const char16_t* text);
			//void menuBarEndMenu();

			void setNexPopupPosition(float x, float y);

			void addSeparator(Style* style = nullptr);

			bool popupMenuBegin( bool * active, Style* style = nullptr );
			bool popupMenuEnd(const Vec4f& rounding = Vec4f());
			bool addMenuItem(const char16_t* text, const char16_t* shortcutText, char16_t iconFontSymbol = 0, char16_t iconFontSubmenuSymbol = 0 );
			bool addMenuItemCheck(const char16_t* text, const char16_t* shortcutText, bool* check, char16_t iconFontSymbol = 0, char16_t iconFontCheckSymbol = 0, char16_t iconFontUncheckSymbol = 0 );
			bool beginMenu(const char16_t* text, char16_t iconFontSymbol = 0, char16_t iconFontSubmenuSymbol = 0);
			bool endMenu();

			const Vec4f& getLastClipRect();

			static int m_wheel_delta;
			static Vec2f m_mouseDelta;
			static Vec2f m_cursorCoords;
			static bool  m_IsLeft;
			static bool  m_IsRight;
			static bool  m_IsDelete;
			static bool  m_IsBackspace;
			static bool  m_IsHome;
			static bool  m_IsEnd;
			static bool   m_IsA;
			static bool   m_IsX ;
			static bool   m_IsC  ;
			static bool   m_IsV   ;
			static char16_t  m_character;
			static bool  m_IsLMBDouble;

			bool   m_isEnterTextMode;
			bool   m_mouseIsLMB_up;
			bool   m_mouseIsLMB;
			bool   m_mouseIsLMB_old;
			bool   m_mouseIsLMB_firstClick;
			bool   m_mouseIsMMB_up;
			bool   m_mouseIsMMB;
			bool   m_mouseIsMMB_old;
			bool   m_mouseIsMMB_firstClick;
			bool   m_mouseIsRMB_up;
			bool   m_mouseIsRMB;
			bool   m_mouseIsRMB_old;
			bool   m_mouseIsRMB_firstClick;

			float getZoom(){return m_guiZoom;}
			void setZoom(float v){m_guiZoom = v;}
			void disableInput(){m_disableInput = true;}
			void enableInput(){m_disableInput = false;}
			private:
				bool _popupMenuBegin( bool * active, Style* style, const Vec2f& positionLT);
		};

		const float PI = 3.1415926f;
		inline float DegToRad( float degrees ){return degrees * ( PI / 180.f );}
		inline float RadToDeg( float radians ){return radians * ( 180.f / PI );}

		inline GuiSystem* CreateSystem( GraphicsSystemType type, const char* defaultFontDir = "../res/fonts/noto/", const char* defaultFontFileName = "notosans.txt" )
		{
			GuiSystem* ptr = new GuiSystem;
			if( ptr )
			{
				if( !ptr->_init(type, defaultFontDir, defaultFontFileName) )
				{
					delete ptr;
					ptr = nullptr;
				}
			}
			return ptr;
		}
	
		void CopyToClipboard( const char16_t* str);
		std::u16string GetTextFromClipboard();
	}
}

#endif