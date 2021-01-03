#include "yy.h"

#include "yy_model.h"

#include "io/file_io.h"

#include <iostream>
#include <fstream>
#include <filesystem>

#include "yy_ptr.h"
#include "yy_fs.h"

struct Game_TR3DHeaderMeshHeader
{
	Game_TR3DHeaderMeshHeader()
		:
		m_dataComressSize_v(0),
		m_dataComressSize_i(0),
		m_dataDecomressSize_v(0),
		m_dataDecomressSize_i(0),
		m_vCount(0),
		m_iCount(0),
		m_stride(0)
	{}

	u32 m_dataComressSize_v;
	u32 m_dataComressSize_i;
	u32 m_dataDecomressSize_v;
	u32 m_dataDecomressSize_i;
	u32 m_vCount   ;
	u32 m_iCount   ;
	u32 m_stride   ;
};
/*
* Структура файла
*	Game_TR3DHeader
*		Game_TR3DHeaderMeshHeader
*			меш
*		Game_TR3DHeaderMeshHeader
*			меш
*		Game_TR3DHeaderMeshHeader
*			меш
*/
const u32 Game_TR3DMagic = YY_MAKEFOURCC('T', 'R', '3', 'D');
const u32 Game_TR3DNameSize = 36;
#pragma pack(1)
struct Game_TR3DHeader
{
	Game_TR3DHeader()
		:
		m_magic(Game_TR3DMagic),
		m_meshCount(0)
	{}
	u32 m_magic;
	u32 m_meshCount;
	char m_name[Game_TR3DNameSize];
};
#pragma pack()

yyModel* ModelLoader_TR3D(const char* p)
{
	//auto file_size = std::filesystem::file_size(p);
	auto file_size = yyFS::file_size(p);

	if( file_size < sizeof(u32) + sizeof(Game_TR3DHeader) )
	{
		YY_PRINT_FAILED;
		return nullptr;
	}

	std::ifstream in(p, std::ios::binary);
	if(!in)
	{
		YY_PRINT_FAILED;
		return nullptr;
	}

	Game_TR3DHeader main_header;
	in.read((char*)&main_header, sizeof(Game_TR3DHeader));
	if( in.gcount() < sizeof(Game_TR3DHeader) )
	{
		YY_PRINT_FAILED;
		return nullptr;
	}

	if(main_header.m_magic != Game_TR3DMagic)
	{
		yyLogWriteWarning("Can't read TR3D: Bad magic\n");
		return nullptr;
	}

	yyPtr<yyModel> new_model = yyCreate<yyModel>();
	if(!new_model.m_data)
	{
		YY_PRINT_FAILED;
		return nullptr;
	}

	for( u32 i = 0; i < 1; ++i )
	{
		Game_TR3DHeaderMeshHeader meshhead;
		yyModel * newModel = yyCreate<yyModel>();
		
		newModel->m_vertexType = yyVertexType::Model;

		in.read((char*)&meshhead, sizeof(Game_TR3DHeaderMeshHeader));

		newModel->m_iCount = meshhead.m_iCount;
		newModel->m_vCount = meshhead.m_vCount;
		newModel->m_stride = meshhead.m_stride;

		/*u8* compressed_v = nullptr;
		u8* compressed_i = nullptr;
		if(!meshhead.m_dataComressSize_i)
		{
			YY_PRINT_FAILED;
			return nullptr;
		}
		if(!meshhead.m_dataComressSize_v)
		{
			YY_PRINT_FAILED;
			return nullptr;
		}*/

		//compressed_v = (u8*)yyMemAlloc(meshhead.m_dataComressSize_v);
		//compressed_i = (u8*)yyMemAlloc(meshhead.m_dataComressSize_i);
						
		//in.read((char*)compressed_v, meshhead.m_dataComressSize_v);
		//in.read((char*)compressed_i, meshhead.m_dataComressSize_i);
		//u32 outsize = 0;
		//new_mesh.m_data->m_vertices = yyDecompressData(compressed_v, meshhead.m_dataComressSize_v, outsize, yyCompressType::ZStd);
		//new_mesh.m_data->m_indices = (u8*)yyDecompressData(compressed_i, meshhead.m_dataComressSize_i, outsize, yyCompressType::ZStd);

		newModel->m_vertices = (u8*)yyMemAlloc(meshhead.m_dataDecomressSize_v);
		newModel->m_indices = (u8*)yyMemAlloc(meshhead.m_dataDecomressSize_i);
		in.read((char*)newModel->m_vertices, meshhead.m_dataDecomressSize_v);
		in.read((char*)newModel->m_indices, meshhead.m_dataDecomressSize_i);
		//u32 outsize = 0;
		//new_mesh.m_data->m_vertices = yyDecompressData(compressed_v, meshhead.m_dataComressSize_v, outsize, yyCompressType::ZStd);
		//new_mesh.m_data->m_indices = (u8*)yyDecompressData(compressed_i, meshhead.m_dataComressSize_i, outsize, yyCompressType::ZStd);

		auto vertsPtr = newModel->m_vertices;
		for (u32 o = 0; o < meshhead.m_vCount; ++o)
		{
			f32 * f32ptr = (f32*)vertsPtr;
			
			v3f v;
			v.x = *f32ptr; ++f32ptr;
			v.y = *f32ptr; ++f32ptr;
			v.z = *f32ptr; ++f32ptr;
			newModel->m_aabb.add(v);

			vertsPtr += meshhead.m_stride;
		}

		//yyMemFree(compressed_v);
		//yyMemFree(compressed_i);

		if(!newModel->m_vertices)
		{
			YY_PRINT_FAILED;
			return nullptr;
		}
		if(!newModel->m_indices)
		{
			YY_PRINT_FAILED;
			return nullptr;
		}

		return newModel;
	}

	return nullptr;
}
