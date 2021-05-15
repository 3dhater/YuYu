#ifndef YY_D3D11_SHADER_H__
#define YY_D3D11_SHADER_H__

#include "strings\string.h"
#include "yy_model.h"


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
	ID3D11InputLayout** il);
bool D3D11_createConstantBuffer(u32 byteSize, ID3D11Buffer**);
bool D3D11_createGeometryShaders(const char* target,
	const s8 * shaderText,
	const s8 * entryPoint,
	ID3D11GeometryShader** gs);
#endif