#include "yy.h"
#include "vid_d3d11.h"
#include <d3dcompiler.h>
#include "D3D11_shader.h"

extern D3D11 * g_d3d11;

bool D3D11_createConstantBuffer(u32 byteSize, ID3D11Buffer** cb)
{
	D3D11_BUFFER_DESC mbd;
	memset(&mbd, 0, sizeof(mbd));
	mbd.Usage = D3D11_USAGE_DYNAMIC;
	mbd.ByteWidth = byteSize;
	mbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	mbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	mbd.MiscFlags = 0;
	mbd.StructureByteStride = 0;

	HRESULT hr = g_d3d11->m_d3d11Device->CreateBuffer(&mbd, 0, cb);
	if (FAILED(hr)) 
	{
		yyLogWriteError("Can't create constant buffer. Error code [%u]\n", hr);
		YY_PRINT_FAILED;
		return false;
	}
	return true;
}

bool D3D11_createShaders(
	const char* vertexTarget,
	const char* pixelTarget,
	const s8 * vertexShader,
	const s8 * pixelShader,
	const s8 * vertexEntryPoint,
	const s8 * pixelEntryPoint,
	yyVertexType vertexType,
	ID3D11VertexShader** vs,
	ID3D11PixelShader** ps,
	ID3D11InputLayout** il)
{
	ID3D10Blob*		m_VsBlob = nullptr;
	ID3D10Blob*		m_PsBlob = nullptr;
	ID3D10Blob*		m_errorBlob = nullptr;

	HRESULT hr = D3DCompile(
		vertexShader,
		strlen(vertexShader),
		0, 0, 0,
		vertexEntryPoint,
		vertexTarget,
		0,
		0,
		&m_VsBlob,
		&m_errorBlob
	);

	if (FAILED(hr)) 
	{
		s8 * message = (s8*)m_errorBlob->GetBufferPointer();
		yyLogWriteError("Vertex shader compile error: %s\n", message);
		YY_PRINT_FAILED;
		return false;
	}

	hr = D3DCompile(
		pixelShader,
		strlen(pixelShader),
		0, 0, 0,
		pixelEntryPoint,
		pixelTarget,
		0,
		0,
		&m_PsBlob,
		&m_errorBlob
	);

	if (FAILED(hr))
	{
		s8 * message = (s8*)m_errorBlob->GetBufferPointer();
		yyLogWriteError("Pixel shader compile error: %s\n", message);
		YY_PRINT_FAILED;
		return false;
	}

	hr = g_d3d11->m_d3d11Device->CreateVertexShader(
		m_VsBlob->GetBufferPointer(),
		m_VsBlob->GetBufferSize(),
		0,
		vs);
	if (FAILED(hr)) 
	{
		yyLogWriteError("Can't create vertex shader. Error code [%u]\n", hr);
		YY_PRINT_FAILED;
		return false;
	}

	hr = g_d3d11->m_d3d11Device->CreatePixelShader(
		m_PsBlob->GetBufferPointer(),
		m_PsBlob->GetBufferSize(),
		0,
		ps);
	if (FAILED(hr)) 
	{
		yyLogWriteError("Can't create pixel shader. Error code [%u]\n", hr);
		YY_PRINT_FAILED;
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC vertexLayout[5];
	u32 vertexLayoutSize = 0;
	/*
	LPCSTR SemanticName;
	UINT SemanticIndex;
	DXGI_FORMAT Format;
	UINT InputSlot;
	UINT AlignedByteOffset;
	D3D11_INPUT_CLASSIFICATION InputSlotClass;
	UINT InstanceDataStepRate;
	*/
	switch (vertexType)
	{
	case yyVertexType::GUI:
		vertexLayoutSize = 1;
		vertexLayout[0].SemanticName = "POSITION";
		vertexLayout[0].SemanticIndex = 0;
		vertexLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexLayout[0].InputSlot = 0;
		vertexLayout[0].AlignedByteOffset = 0;
		vertexLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		vertexLayout[0].InstanceDataStepRate = 0;
		
		vertexLayout[1].SemanticName = "TEXCOORD";
		vertexLayout[1].SemanticIndex = 0;
		vertexLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		vertexLayout[1].InputSlot = 0;
		vertexLayout[1].AlignedByteOffset = 8;
		vertexLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		vertexLayout[1].InstanceDataStepRate = 0;
		break;
	case yyVertexType::Model:
		vertexLayoutSize = 5;
		vertexLayout[0].SemanticName = "POSITION";
		vertexLayout[0].SemanticIndex = 0;
		vertexLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexLayout[0].InputSlot = 0;
		vertexLayout[0].AlignedByteOffset = 0;
		vertexLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		vertexLayout[0].InstanceDataStepRate = 0;

		vertexLayout[1].SemanticName = "TEXCOORD";
		vertexLayout[1].SemanticIndex = 0;
		vertexLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
		vertexLayout[1].InputSlot = 0;
		vertexLayout[1].AlignedByteOffset = 12;
		vertexLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		vertexLayout[1].InstanceDataStepRate = 0;

		vertexLayout[2].SemanticName = "NORMAL";
		vertexLayout[2].SemanticIndex = 0;
		vertexLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexLayout[2].InputSlot = 0;
		vertexLayout[2].AlignedByteOffset = 20;
		vertexLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		vertexLayout[2].InstanceDataStepRate = 0;

		vertexLayout[3].SemanticName = "BINORMAL";
		vertexLayout[3].SemanticIndex = 0;
		vertexLayout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexLayout[3].InputSlot = 0;
		vertexLayout[3].AlignedByteOffset = 32;
		vertexLayout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		vertexLayout[3].InstanceDataStepRate = 0;

		vertexLayout[4].SemanticName = "TANGENT";
		vertexLayout[4].SemanticIndex = 0;
		vertexLayout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		vertexLayout[4].InputSlot = 0;
		vertexLayout[4].AlignedByteOffset = 44;
		vertexLayout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		vertexLayout[4].InstanceDataStepRate = 0;
		break;
	default:
		yyLogWriteError("Unsupportex vertex type\n");
		return false;
	}
	//		layout.get()[i] = { "COLOR",     0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offset, D3D11_INPUT_PER_VERTEX_DATA, 0 };

	hr = g_d3d11->m_d3d11Device->CreateInputLayout(
		vertexLayout,
		vertexLayoutSize,
		m_VsBlob->GetBufferPointer(),
		m_VsBlob->GetBufferSize(),
		il);
	if (FAILED(hr)) 
	{
		yyLogWriteError("Can't create input layout. Error code [%u]\n", hr);
		YY_PRINT_FAILED;
		return false;
	}

	if (m_VsBlob)    m_VsBlob->Release();
	if (m_PsBlob)    m_PsBlob->Release();
	if (m_errorBlob) m_errorBlob->Release();

	return true;
}