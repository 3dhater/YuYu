#include "yy.h"
#include "containers/fixed_fifo.h"

#include "engine.h"

#include <chrono>
#include <thread>

extern Engine * g_engine;


void yyBackgroundWorkerFunction()
{
	auto s = std::chrono::milliseconds(300);

	while(true)
	{
		for(int i = 0, sz = g_engine->m_workerCommands.m_size; i < sz; ++i)
		{
			auto & obj = g_engine->m_workerCommands.get();
			
			switch (obj.m_type)
			{
			case BackgroundWorkerCommands::type::ExitThread:
				return;
			case BackgroundWorkerCommands::type::LoadSource:
			{
				BackgroundWorkerResults res;
				res.m_type = BackgroundWorkerResults::type::LoadSource;
				res.m_id   = obj.m_id;
				res.m_resource = obj.m_resource;
				res.m_resource->LoadSource();
				yyLogWriteInfo("Async LoadSource\n");
				g_engine->m_workerResults.put(res);
			}break;
			default:
				break;
			}
		}

		std::this_thread::sleep_for(s);
	}
}