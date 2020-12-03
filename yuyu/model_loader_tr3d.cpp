#include "yy.h"

#include "yy_model.h"

#include "io/file_io.h"

#include <iostream>
#include <fstream>
#include <filesystem>

#include "yy_ptr.h"

struct Game_TR3DHeaderMeshHeader
{
	u32 m_dataComressSize_v = 0;
	u32 m_dataComressSize_i = 0;
	u32 m_dataDecomressSize_v = 0;
	u32 m_dataDecomressSize_i = 0;
	u32 m_vCount   = 0;
	u32 m_iCount   = 0;
	u32 m_stride   = 0;
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
constexpr u32 Game_TR3DMagic = YY_MAKEFOURCC('T', 'R', '3', 'D');
constexpr u32 Game_TR3DNameSize = 36;
#pragma pack(1)
struct Game_TR3DHeader
{
	u32 m_magic   = Game_TR3DMagic;
	u32 m_meshCount = 0;
	char m_name[Game_TR3DNameSize];
};
#pragma pack()

yyModel* ModelLoader_TR3D(const std::filesystem::path& p)
{
	auto file_size = std::filesystem::file_size(p);

	if( file_size < sizeof(u32) + sizeof(Game_TR3DHeader) )
	{
		YY_PRINT_FAILED;
		return nullptr;
	}

	std::ifstream in(p.c_str(), std::ios::binary);
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

	for( u32 i = 0; i < main_header.m_meshCount; ++i )
	{
		Game_TR3DHeaderMeshHeader meshhead;
		yyPtr<yyMeshBuffer> new_mesh = yyCreate<yyMeshBuffer>();

		in.read((char*)&meshhead, sizeof(Game_TR3DHeaderMeshHeader));

		new_mesh.m_data->m_iCount = meshhead.m_iCount;
		new_mesh.m_data->m_vCount = meshhead.m_vCount;
		new_mesh.m_data->m_stride = meshhead.m_stride;

		u8* compressed_v = nullptr;
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
		}

		compressed_v = (u8*)yyMemAlloc(meshhead.m_dataComressSize_v);
		compressed_i = (u8*)yyMemAlloc(meshhead.m_dataComressSize_i);
						
		in.read((char*)compressed_v, meshhead.m_dataComressSize_v);
		in.read((char*)compressed_i, meshhead.m_dataComressSize_i);
		u32 outsize = 0;
		new_mesh.m_data->m_vertices = yyDecompressData(compressed_v, meshhead.m_dataComressSize_v, outsize, yyCompressType::ZStd);
		new_mesh.m_data->m_indices = (u16*)yyDecompressData(compressed_i, meshhead.m_dataComressSize_i, outsize, yyCompressType::ZStd);

		yyMemFree(compressed_v);
		yyMemFree(compressed_i);

		if(!new_mesh.m_data->m_vertices)
		{
			YY_PRINT_FAILED;
			return nullptr;
		}
		if(!new_mesh.m_data->m_indices)
		{
			YY_PRINT_FAILED;
			return nullptr;
		}

		new_model.m_data->m_meshBuffers.push_back(new_mesh.m_data);
		new_mesh.m_data = nullptr;
	}
	if(new_model.m_data->m_meshBuffers.size())
	{
		auto ret = new_model.m_data;
		new_model.m_data = nullptr;
		return ret;
	}

	return nullptr;
}
