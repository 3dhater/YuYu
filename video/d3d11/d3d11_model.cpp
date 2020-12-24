#include "yy.h"

#include "vid_d3d11.h"

#include "D3D11_model.h"

D3D11MeshBuffer::D3D11MeshBuffer()
	:
	m_lockedResource(0),
	m_vBuffer(0),
	m_iBuffer(0),
	m_stride(0),
	m_iCount(0),
	m_indexType(DXGI_FORMAT_R16_UINT)
{
}

D3D11MeshBuffer::~D3D11MeshBuffer()
{
	if (m_vBuffer) m_vBuffer->Release();
	if (m_iBuffer) m_iBuffer->Release();
}


D3D11Model::D3D11Model()
{
}

D3D11Model::~D3D11Model()
{
	for(u16 i = 0, sz = m_meshBuffers.size(); i < sz; ++i)
	{
		yyDestroy( m_meshBuffers[i] );
	}
}