#include "yy.h"
#include "yy_image.h"
#include "yy_material.h"
#include "yy_model.h"

#include "engine.h"
extern yyEngine * g_engine;

#include <cstdlib>
#include <cassert>

extern "C"
{
	YY_API void * YY_C_DECL yyMemAlloc(size_t size){
		assert(size != 0);
		return std::malloc(static_cast<size_t>(size));
	}

	YY_API void * YY_C_DECL yyMemRealloc(void * ptr, size_t size){
		return std::realloc(ptr,static_cast<size_t>(size));
	}

	YY_API void   YY_C_DECL yyMemFree(void* ptr){
		assert(ptr != nullptr);
		std::free(ptr);
	}
}

yyMegaAllocator::yyMegaAllocator() {
}

yyMegaAllocator::~yyMegaAllocator() {

}

yyMaterial* yyMegaAllocator::CreateMaterial() {
	auto ptr = g_engine->m_poolMaterial.GetObject();
	new(ptr)yyMaterial();
	//printf(">>> get [%i] - freeCount[%i]\n", g_engine->m_poolMaterial.GetUsedCount(),
	//	g_engine->m_poolMaterial.GetFreeCount());
	return ptr;
}
yyResource* yyMegaAllocator::CreateResource() {
	auto ptr = g_engine->m_poolResource.GetObject();
	new(ptr)yyResourceImpl();
	return ptr;
}
yyModel* yyMegaAllocator::CreateModel() {
	auto ptr = g_engine->m_poolModel.GetObject();
	new(ptr)yyModel();
	/*printf(">>> get [%i] - freeCount[%i]\n", g_engine->m_poolModel.GetUsedCount(),
		g_engine->m_poolModel.GetFreeCount());*/
	return ptr;
}

void yyMegaAllocator::Destroy(yyMaterial* ptr) {
	assert(ptr);
	ptr->~yyMaterial();
	g_engine->m_poolMaterial.PutBack(ptr);
	//printf(">>> put [%i] - freeCount[%i]\n", g_engine->m_poolMaterial.GetUsedCount(),
	//	g_engine->m_poolMaterial.GetFreeCount());
}
void yyMegaAllocator::Destroy(yyResource* ptr) {
	assert(ptr);
	ptr->~yyResource();
	g_engine->m_poolResource.PutBack((yyResourceImpl*)ptr);
}
void yyMegaAllocator::Destroy(yyModel* ptr) {
	assert(ptr);
	ptr->~yyModel();
	g_engine->m_poolModel.PutBack(ptr);
	/*printf(">>> put [%i] - freeCount[%i]\n", g_engine->m_poolModel.GetUsedCount(),
		g_engine->m_poolModel.GetFreeCount());*/
}