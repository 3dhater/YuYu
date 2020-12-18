#ifndef __KK_KRGUI_NODEEDITORNODESOCKET_H__
#define __KK_KRGUI_NODEEDITORNODESOCKET_H__

namespace Kr
{
	namespace Gui
	{
		struct NodeEditorNodeSocket
		{
			NodeEditorNodeSocket()
			{
				userData = nullptr;
				userDataSize = 0;
				style = nullptr;
				// (0,0) = center of the node
				position = Vec2f(0, 0);
				size = Vec2f(10, 10);
				id = -1;
				flags = 0; // user flags
							   // read only
							   // позиция относительно всего экрана
				_realPosition = Vec2f(0, 0);
				parentNode = nullptr;
				rounding = Vec4f(5.f, 5.f, 5.f, 5.f);
			}

			void * userData ;
			unsigned long long userDataSize ;

			Style * style ;

			// (0,0) = center of the node
			Vec2f position;
			Vec2f size;
			int id;
			int flags; // user flags

			// read only
			// позиция относительно всего экрана
			Vec2f _realPosition;
			NodeEditorNode * parentNode;

			Vec4f rounding;
		};
	}
}

#endif