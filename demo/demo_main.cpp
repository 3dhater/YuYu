#include "demo.h"

int main(int argc, char* argv[])
{
	const char * videoDriverType = "d3d11.yyvd"; // for example read name from .ini
	yyStringA videoDriverTypeStr = videoDriverType;
	for (int i = 0; i < argc; ++i)
	{
		if (strcmp(argv[i],"-vid")==0)
		{
			++i;
			if (i < argc)
			{
				videoDriverTypeStr = argv[i];
			}
		}
	}

	Demo* demo = new Demo;
	if (!demo->Init(videoDriverTypeStr.c_str()))
	{
		delete demo;
		return 1;
	}

	demo->MainLoop();
	
	delete demo;

	return 0;
}