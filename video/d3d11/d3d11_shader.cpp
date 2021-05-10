#include "yy.h"
#include "vid_d3d11.h"
#include <d3dcompiler.h>
#include "D3D11_shader.h"

extern D3D11 * g_d3d11;

bool D3D11_createConstantBuffer(u32 byteSize, ID3D11Buffer** cb){
	YY_DEBUG_PRINT_FUNC;
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
	YY_DEBUG_PRINT_FUNC;
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

	if (vertexType != yyVertexType::Null)
	{
		D3D11_INPUT_ELEMENT_DESC vertexLayout[8];
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

		int ind = 0;
		switch (vertexType)
		{
		case yyVertexType::GUI:
			vertexLayoutSize = 2;
			ind = 0;
			vertexLayout[ind].SemanticName = "POSITION";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 0;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind = 1;
			vertexLayout[ind].SemanticName = "TEXCOORD";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 8;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;
			break;
		case yyVertexType::Model:
			vertexLayoutSize = 6;

			ind = 0;
			vertexLayout[ind].SemanticName = "POSITION";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 0;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind = 1;
			vertexLayout[ind].SemanticName = "TEXCOORD";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 12;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind = 2;
			vertexLayout[ind].SemanticName = "NORMAL";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 20;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind = 3;
			vertexLayout[ind].SemanticName = "BINORMAL";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 32;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind = 4;
			vertexLayout[ind].SemanticName = "TANGENT";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 44;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind = 5;
			vertexLayout[ind].SemanticName = "COLOR";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 56;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;
			break;
		case yyVertexType::AnimatedModel:
			vertexLayoutSize = 8;

			ind = 0;
			vertexLayout[ind].SemanticName = "POSITION";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 0;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind = 1;
			vertexLayout[ind].SemanticName = "TEXCOORD";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 12;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind = 2;
			vertexLayout[ind].SemanticName = "NORMAL";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 20;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind = 3;
			vertexLayout[ind].SemanticName = "BINORMAL";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 32;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind = 4;
			vertexLayout[ind].SemanticName = "TANGENT";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 44;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind = 5;
			vertexLayout[ind].SemanticName = "COLOR";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 56;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind = 6;
			vertexLayout[ind].SemanticName = "WEIGHTS";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 72;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind = 7;
			vertexLayout[ind].SemanticName = "BONES";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_UINT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 88;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			break;
		case yyVertexType::LineModel:
			vertexLayoutSize = 3;

			ind = 0;
			vertexLayout[ind].SemanticName = "POSITION";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 0;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind = 1;
			vertexLayout[ind].SemanticName = "NORMAL";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 12;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind = 2;
			vertexLayout[ind].SemanticName = "COLOR";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 24;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;
			break;
		case yyVertexType::AnimatedLineModel:
			vertexLayoutSize = 5;

			ind = 0;
			vertexLayout[ind].SemanticName = "POSITION";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 0;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind = 1;
			vertexLayout[ind].SemanticName = "NORMAL";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 12;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind = 2;
			vertexLayout[ind].SemanticName = "COLOR";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 24;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind = 3;
			vertexLayout[ind].SemanticName = "WEIGHTS";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 40;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;

			ind = 4;
			vertexLayout[ind].SemanticName = "BONES";
			vertexLayout[ind].SemanticIndex = 0;
			vertexLayout[ind].Format = DXGI_FORMAT_R32G32B32A32_UINT;
			vertexLayout[ind].InputSlot = 0;
			vertexLayout[ind].AlignedByteOffset = 56;
			vertexLayout[ind].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			vertexLayout[ind].InstanceDataStepRate = 0;
			break;
		default:
			yyLogWriteError("Unsupportex vertex type\n");
			return false;
		}

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
	}
	

	if (m_VsBlob)    m_VsBlob->Release();
	if (m_PsBlob)    m_PsBlob->Release();
	if (m_errorBlob) m_errorBlob->Release();

	return true;
}