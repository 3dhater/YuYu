#include "yy.h"

#include "vid_d3d11.h"

#include "D3D11_model.h"

extern D3D11 * g_d3d11;

D3D11Model::D3D11Model()
	:
	m_lockedResource(0),
	m_vBuffer(0),
	m_iBuffer(0),
	m_stride(0),
	m_iCount(0),
	m_indexType(DXGI_FORMAT_R16_UINT),
	m_vertexType(yyVertexType::Model)
{
	YY_DEBUG_PRINT_FUNC;
}

D3D11Model::~D3D11Model(){
	YY_DEBUG_PRINT_FUNC;
	Unload();
}

void D3D11Model::GetTextureHandle(void**) {}
void D3D11Model::GetTextureSize(v2f*) {}
void D3D11Model::MapModelForWriteVerts(u8** v_ptr) {
	static D3D11_MAPPED_SUBRESOURCE mapData;
	auto hr = g_d3d11->m_d3d11DevCon->Map(
		m_vBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&mapData
	);
	if (FAILED(hr))
	{
		yyLogWriteError("Can not lock D3D11 render model buffer. Code : %u\n", hr);
		return;
	}
	*v_ptr = (u8*)mapData.pData;
	m_lockedResource = m_vBuffer;
}
void D3D11Model::UnmapModelForWriteVerts() {
	g_d3d11->m_d3d11DevCon->Unmap(m_lockedResource, 0);
	m_lockedResource = nullptr;
}

void D3D11Model::Load(yyResourceData* modelData) {
	yyModel * model = (yyModel *)modelData->m_source;

	m_material = model->m_material;
	m_vertexType = model->m_vertexType;

	D3D11_BUFFER_DESC	vbd, ibd;

	ZeroMemory(&vbd, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&ibd, sizeof(D3D11_BUFFER_DESC));

	vbd.Usage = D3D11_USAGE_DEFAULT;
	//vbd.Usage = D3D11_USAGE_DYNAMIC;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	ibd.Usage = D3D11_USAGE_DEFAULT;
	//ibd.Usage = D3D11_USAGE_DYNAMIC;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	//vbd.CPUAccessFlags = 0;
	//ibd.CPUAccessFlags = 0; //D3D11_CPU_ACCESS_WRITE

	D3D11_SUBRESOURCE_DATA	vData, iData;
	ZeroMemory(&vData, sizeof(D3D11_SUBRESOURCE_DATA));
	ZeroMemory(&iData, sizeof(D3D11_SUBRESOURCE_DATA));
	HRESULT	hr = 0;


	vbd.ByteWidth = model->m_stride * model->m_vCount;
	vData.pSysMem = &model->m_vertices[0];

	hr = g_d3d11->m_d3d11Device->CreateBuffer(&vbd, &vData, &m_vBuffer);
	if (FAILED(hr))
	{
		yyLogWriteError("Can't create Direct3D 11 vertex buffer [%u]\n", hr);
		YY_PRINT_FAILED;
		return;
	}


	u32 index_sizeof = sizeof(u16);
	m_indexType = DXGI_FORMAT_R16_UINT;
	if (model->m_indexType == yyMeshIndexType::u32)
	{
		m_indexType = DXGI_FORMAT_R32_UINT;
		index_sizeof = sizeof(u32);
	}
	ibd.ByteWidth = index_sizeof * model->m_iCount;
	iData.pSysMem = &model->m_indices[0];

	m_iCount = model->m_iCount;
	m_stride = model->m_stride;

	hr = g_d3d11->m_d3d11Device->CreateBuffer(&ibd, &iData, &m_iBuffer);
	if (FAILED(hr))
	{
		yyLogWriteError("Can't create Direct3D 11 index buffer [%u]\n", hr);
		YY_PRINT_FAILED;
		return;
	}

	return;
}

void D3D11Model::Unload() {
	if (m_vBuffer) {
		m_vBuffer->Release();
		m_vBuffer = 0;
	}
	if (m_iBuffer) {
		m_iBuffer->Release();
		m_iBuffer = 0;
	}
}