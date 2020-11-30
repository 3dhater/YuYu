#include "yy.h"
#include "scene/common.h"

#include "engine.h"
extern Engine * g_engine;

extern "C"
{

YY_API yySceneObjectBase* YY_C_DECL yySceneGetRootObject()
{
	return g_engine->m_sceneRootObject;
}
YY_API yyCamera* YY_C_DECL yySceneGetActiveCamera()
{
	return g_engine->m_sceneActiveCamera;
}
YY_API void YY_C_DECL yySceneSetActiveCamera(yyCamera* c)
{
	g_engine->m_sceneActiveCamera = c;
}

}
