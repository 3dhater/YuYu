#ifndef __KK_KRGUI_NODEEDITORNODE_H__
#define __KK_KRGUI_NODEEDITORNODE_H__

namespace Kr
{
	namespace Gui
	{
		struct NodeEditorNode
		{
			NodeEditorNode()
			{
				m_id = -1;
				m_userData = nullptr;
				m_userDataSize = 0;
				m_rounding = Vec4f(5.f, 5.f, 5.f, 5.f);
				m_parentDrawGroup = nullptr;

				// for z-ordering
				m_z_position = -1; // read-only

				m_inputRect_add = Vec4f(10.f, 10.f, 10.f, 10.f); // add area for m_inputRect (f.e. for sockets)
			}

			std::vector<NodeEditorNodeSocket> m_sockets;
			int m_id;
			Vec2f m_position;
			Vec2f m_size;
		
			void * m_userData;
			unsigned long long m_userDataSize;
		

			Vec4f m_rounding;

			DrawCommandsGroup* m_parentDrawGroup;

			// for z-ordering
			int m_z_position; // read-only

			Vec4f m_inputRect_add;
			Vec4f m_inputRect; // read-only

			void addSocket(const Vec2f& position, const Vec2f& size, int id, int userFlags, Style * style = nullptr)
			{
				NodeEditorNodeSocket newSocket;
				newSocket.size = size;
				newSocket.id   = id;
				newSocket.position = position;
				newSocket.parentNode = this;
				newSocket.style = style;
				newSocket.flags = userFlags;

				m_sockets.push_back(newSocket);
			}

			void addSocket(const NodeEditorNodeSocket& newSocket )
			{
				m_sockets.push_back(newSocket);
			}
		};
	}
}

#endif