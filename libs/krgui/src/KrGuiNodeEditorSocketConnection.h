#ifndef __KK_KRGUI_NODEEDITORSOCKETCONNECTION_H__
#define __KK_KRGUI_NODEEDITORSOCKETCONNECTION_H__

namespace Kr
{
	namespace Gui
	{
		struct NodeEditorSocketConnection
		{
			NodeEditorSocketConnection()
			{
				first = nullptr;
				second = nullptr;
				node1 = nullptr;
				node2 = nullptr;
			}

			NodeEditorNodeSocket * first;
			NodeEditorNodeSocket * second;

			// можно попробовать удалять ненужные connection делая проверку по node
			NodeEditorNode * node1;
			NodeEditorNode * node2;
		};
	}
}

#endif