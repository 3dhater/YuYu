#include "demo.h"
#include "demo_example.h"

DemoExample::DemoExample()
{
	m_guiTextTitle = 0;
	m_gpu = yyGetVideoDriverAPI();
}

DemoExample::~DemoExample()
{

}