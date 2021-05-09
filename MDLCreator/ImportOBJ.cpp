#include "yy.h"
#include "yy_model.h"
#include "math\math.h"
#include "scene\common.h"
#include "scene\mdl_object.h"

#include "MDLCreator_common.h"
#include "SceneObject.h"

extern SceneObject* g_sceneObject;

enum class OBJFaceType 
{
	p,
	pu,
	pun,
	pn
};

struct OBJSimpleArr 
{
	s32 data[0x100];
	u32 sz = 0;
	void push_back(s32 v) { data[sz++] = v; }
	u32 size() { return sz; }
	void reset() { sz = 0; }
};

struct OBJFace 
{
	OBJSimpleArr p, u, n;
	OBJFaceType ft = OBJFaceType::pun;
	void reset() 
	{
		ft = OBJFaceType::pun;
		p.reset();
		u.reset();
		n.reset();
	}
};

u8 * OBJNextLine(u8 * ptr);
u8 * OBJSkipSpaces(u8 * ptr);
u8 * OBJReadVec2(u8 * ptr, v4f& vec2);
u8 * OBJReadFloat(u8 * ptr, f32& value);
u8 * OBJReadVec3(u8 * ptr, v4f& vec3);
u8 * OBJReadFace(u8 * ptr, OBJFace& f, char * s);
u8 * OBJReadWord(u8 * ptr, yyStringA& str);

void ImportOBJ(yyMDLObject* object, const char* fileName)
{
	FILE* file = fopen(fileName, "rb");
	auto file_size = (size_t)yy_fs::file_size(fileName);

	yyArray<u8> file_byte_array;
	file_byte_array.reserve((u32)file_size + 2);
	file_byte_array.setSize((u32)file_size + 2);

	u8 * ptr = file_byte_array.data();
	fread(ptr, 1, file_size, file);
	fclose(file);

	ptr[(u32)file_size] = ' ';
	ptr[(u32)file_size + 1] = 0;

	bool groupBegin = false;
	bool isModel = false;
	bool grpFound = false;

	v4f tcoords;
	v4f pos;
	v4f norm;

	yyArray<v4f> position;
	yyArray<v4f> uv;
	yyArray<v4f> normal;

	position.reserve(0xffff);
	uv.reserve(0xffff);
	normal.reserve(0xffff);
	
	position.setAddMemoryValue(0xffff);
	uv.setAddMemoryValue(0xffff);
	normal.setAddMemoryValue(0xffff);

	yyArray<yyVertexModel> modelVerts;
	modelVerts.reserve(0xffff);
	modelVerts.setAddMemoryValue(0xffff);
	yyArray<u32> modelInds;
	modelInds.reserve(0xffff);
	modelInds.setAddMemoryValue(0xffff);

	yyStringA name_word;

	OBJFace f;
	char s[0xff];
	
	yyModel* newModel = yyMegaAllocator::CreateModel();

	v4i last_counter;
	
	while (*ptr)
	{
		switch (*ptr)
		{
		case '#':
		case 's':
		case 'l':
		case 'u'://usemtl
		case 'c'://curv
		case 'm'://mtllib
		case 'p'://parm
		case 'd'://deg
		case 'e'://end
			ptr = OBJNextLine(ptr);
			break;
		case 'v':
		{
			++ptr;
			if (groupBegin)
				groupBegin = false;
			switch (*ptr)
			{
			case 't':
				ptr = OBJReadVec2(++ptr, tcoords);
				uv.push_back(tcoords);
				++last_counter.y;
				break;
			case 'n':
				ptr = OBJReadVec3(++ptr, norm);
				normal.push_back(norm);
				++last_counter.z;
				break;
			default:
				ptr = OBJReadVec3(ptr, pos);
				position.push_back(pos);
				++last_counter.x;

				newModel->m_aabb.add(pos);

				break;
			}
		}break;
		case 'f':
		{
			isModel = true;
			f.reset();
			ptr = OBJReadFace(++ptr, f, s);

			for (u32 sz2 = f.p.size() - 2, i2 = 0; i2 < sz2; ++i2)
			{
				auto index1 = 0;
				auto index2 = 1 + i2;
				auto index3 = 2 + i2;
				auto pos_index1 = f.p.data[index1];
				auto pos_index2 = f.p.data[index2];
				auto pos_index3 = f.p.data[index3];

					// if inds < 0
				if (pos_index1 < 0) pos_index1 = last_counter.x + pos_index1 + 1;
				if (pos_index2 < 0) pos_index2 = last_counter.x + pos_index2 + 1;
				if (pos_index3 < 0) pos_index3 = last_counter.x + pos_index3 + 1;

				yyVertexModel newVertex1;
				yyVertexModel newVertex2;
				yyVertexModel newVertex3;

				newVertex1.Position = position[pos_index1];
				newVertex2.Position = position[pos_index2];
				newVertex3.Position = position[pos_index3];

				if (f.ft == OBJFaceType::pu || f.ft == OBJFaceType::pun)
				{
					auto uv_index1 = f.u.data[index1];
					auto uv_index2 = f.u.data[index2];
					auto uv_index3 = f.u.data[index3];
					if (uv_index1 < 0) uv_index1 = last_counter.y + uv_index1 + 1;
					if (uv_index2 < 0) uv_index2 = last_counter.y + uv_index2 + 1;
					if (uv_index3 < 0) uv_index3 = last_counter.y + uv_index3 + 1;

					auto u1 = uv[uv_index1];
					auto u2 = uv[uv_index2];
					auto u3 = uv[uv_index3];
					newVertex1.TCoords.set(u1.x, 1.f - u1.y);
					newVertex2.TCoords.set(u2.x, 1.f - u2.y);
					newVertex3.TCoords.set(u3.x, 1.f - u3.y);
				}

				if (f.ft == OBJFaceType::pn || f.ft == OBJFaceType::pun)
				{
					auto nor_index1 = f.n.data[index1];
					auto nor_index2 = f.n.data[index2];
					auto nor_index3 = f.n.data[index3];
					if (nor_index1 < 0) nor_index1 = last_counter.z + nor_index1 + 1;
					if (nor_index2 < 0) nor_index2 = last_counter.z + nor_index2 + 1;
					if (nor_index3 < 0) nor_index3 = last_counter.z + nor_index3 + 1;

					newVertex1.Normal = normal[nor_index1];
					newVertex2.Normal = normal[nor_index2];
					newVertex3.Normal = normal[nor_index3];
				}

				u32 vIndex1 = 0xffffffff;
				u32 vIndex2 = 0xffffffff;
				u32 vIndex3 = 0xffffffff;
				yyVertexModel* v1_ptr = &newVertex1;
				yyVertexModel* v2_ptr = &newVertex2;
				yyVertexModel* v3_ptr = &newVertex3;
				for (u32 k = 0, ksz = modelVerts.size(); k < ksz; ++k)
				{
					auto _v = &modelVerts[k];
					if (_v->Position == v1_ptr->Position
						&& _v->Normal == v1_ptr->Normal
						&& _v->TCoords == v1_ptr->TCoords) { v1_ptr = _v; vIndex1 = k; }
					if (_v->Position == v2_ptr->Position
						&& _v->Normal == v2_ptr->Normal
						&& _v->TCoords == v2_ptr->TCoords) { v2_ptr = _v; vIndex2 = k; }
					if (_v->Position == v3_ptr->Position
						&& _v->Normal == v3_ptr->Normal
						&& _v->TCoords == v3_ptr->TCoords) { v3_ptr = _v; vIndex3 = k; }
				}

				if (vIndex1 == 0xffffffff) { vIndex1 = modelVerts.size(); modelVerts.push_back(*v1_ptr); }
				if (vIndex2 == 0xffffffff) { vIndex2 = modelVerts.size(); modelVerts.push_back(*v2_ptr); }
				if (vIndex3 == 0xffffffff) { vIndex3 = modelVerts.size(); modelVerts.push_back(*v3_ptr); }

				modelInds.push_back(vIndex1);
				modelInds.push_back(vIndex2);
				modelInds.push_back(vIndex3);
			}

		}break;
		case 'o':
		case 'g':
		{
			if (!groupBegin)
				groupBegin = true;
			else
			{
				ptr = OBJNextLine(ptr);
				break;
			}

			yyStringA tmp_word;
			ptr = OBJReadWord(++ptr, tmp_word);
			if (tmp_word.size())
			{
				if (!name_word.size())
					name_word = tmp_word;
			}

			grpFound = true;
		}break;
		default:
			++ptr;
			break;
		}
	}

	newModel->m_vertices = (u8*)yyMemAlloc(modelVerts.size() * sizeof(yyVertexModel));
	memcpy(newModel->m_vertices, modelVerts.data(), modelVerts.size() * sizeof(yyVertexModel));

	if (modelInds.size() / 3 > 21845)
	{
		newModel->m_indexType = yyMeshIndexType::u32;
		newModel->m_indices = (u8*)yyMemAlloc(modelInds.size() * sizeof(u32));
		memcpy(newModel->m_indices, modelInds.data(), modelInds.size() * sizeof(u32));
	}
	else
	{
		newModel->m_indexType = yyMeshIndexType::u16;
		newModel->m_indices = (u8*)yyMemAlloc(modelInds.size() * sizeof(u16));
		u16 * i_ptr = (u16*)newModel->m_indices;
		for (u32 i = 0, sz = modelInds.size(); i < sz; ++i)
		{
			*i_ptr = (u16)modelInds[i];
			++i_ptr;
		}
	}

	newModel->m_name = ModelCheckName(object, name_word );
	newModel->m_vCount = modelVerts.size();
	newModel->m_iCount = modelInds.size();
	newModel->m_stride = sizeof(yyVertexModel);
	newModel->m_vertexType = yyVertexType::Model;
	

	yyMDLLayer* newLayer = yyCreate<yyMDLLayer>();
	newLayer->m_model = newModel;

	newLayer->m_meshGPU = yyCreateModel(newModel);
	newLayer->m_meshGPU->Load();

	object->m_mdl->m_layers.push_back(newLayer);
	g_sceneObject->m_layerInfo.push_back(LayerInfo());
}

u8 * OBJNextLine(u8 * ptr)
{
	while (*ptr)
	{
		if (*ptr == '\n')
		{
			ptr++;
			return ptr;
		}
		ptr++;
	}
	return ptr;
}

u8 * OBJReadVec2(u8 * ptr, v4f& vec2)
{
	ptr = OBJSkipSpaces(ptr);
	f32 x, y;
	if (*ptr == '\n')
	{
		ptr++;
	}
	else
	{
		ptr = OBJReadFloat(ptr, x);
		ptr = OBJSkipSpaces(ptr);
		ptr = OBJReadFloat(ptr, y);
		ptr = OBJNextLine(ptr);
		vec2.x = x;
		vec2.y = y;
	}
	return ptr;
}

u8 * OBJSkipSpaces(u8 * ptr)
{
	while (*ptr)
	{
		if (*ptr != '\t' && *ptr != ' ')
			break;
		ptr++;
	}
	return ptr;
}

u8 * OBJReadFloat(u8 * ptr, f32& value)
{
	char str[32u];
	memset(str, 0, 32);
	char * p = &str[0u];
	while (*ptr) {
		if (!isdigit(*ptr) && (*ptr != '-') && (*ptr != '+')
			&& (*ptr != 'e') && (*ptr != 'E') && (*ptr != '.')) break;
		*p = *ptr;
		++p;
		++ptr;
	}
	value = (f32)atof(str);
	return ptr;
}

u8 * OBJReadVec3(u8 * ptr, v4f& vec3) {
	ptr = OBJSkipSpaces(ptr);
	f32 x, y, z;
	if (*ptr == '\n') {
		ptr++;
	}
	else {
		ptr = OBJReadFloat(ptr, x);
		ptr = OBJSkipSpaces(ptr);
		ptr = OBJReadFloat(ptr, y);
		ptr = OBJSkipSpaces(ptr);
		ptr = OBJReadFloat(ptr, z);
		ptr = OBJNextLine(ptr);
		vec3.x = x;
		vec3.y = y;
		vec3.z = z;
	}
	return ptr;
}

u8 * OBJSkipSpace(u8 * ptr) {
	while (*ptr) {
		if (*ptr != ' ' && *ptr != '\t') break;
		ptr++;
	}
	return ptr;
}

u8 * OBJGetInt(u8 * p, s32& i)
{
	char str[8u];
	memset(str, 0, 8);
	char * pi = &str[0u];

	while (*p)
	{
		/*if( *p == '-' )
		{
		++p;
		continue;
		}*/

		if (!isdigit(*p) && *p != '-') break;


		*pi = *p;
		++pi;
		++p;
	}
	i = atoi(str);
	return p;
}

u8 * OBJReadFace(u8 * ptr, OBJFace& f, char * s) {
	ptr = OBJSkipSpaces(ptr);
	if (*ptr == '\n')
	{
		ptr++;
	}
	else
	{
		while (true)
		{
			s32 p = 1;
			s32 u = 1;
			s32 n = 1;

			ptr = OBJGetInt(ptr, p);

			if (*ptr == '/')
			{
				ptr++;
				if (*ptr == '/')
				{
					ptr++;
					f.ft = OBJFaceType::pn;
					ptr = OBJGetInt(ptr, n);
				}
				else
				{
					ptr = OBJGetInt(ptr, u);
					if (*ptr == '/')
					{
						ptr++;
						f.ft = OBJFaceType::pun;
						ptr = OBJGetInt(ptr, n);
					}
					else
					{
						f.ft = OBJFaceType::pu;
					}
				}
			}
			else
			{
				f.ft = OBJFaceType::p;
			}
			f.n.push_back(n - 1);
			f.u.push_back(u - 1);
			f.p.push_back(p - 1);
			ptr = OBJSkipSpace(ptr);

			if (*ptr == '\r')
				break;
			else if (*ptr == '\n')
				break;
		}
	}
	return ptr;
}

u8 * OBJReadWord(u8 * ptr, yyStringA& str)
{
	ptr = OBJSkipSpaces(ptr);
	str.clear();
	while (*ptr)
	{
		if (isspace(*ptr))
			break;
		str += (char)*ptr;
		ptr++;
	}
	return ptr;
}
