/*
	Данный файл создан для проекта `gost`.
	В нём будут находиться только те объявления, которые будут использоваться
	в плагинах.
*/

#ifndef __D3D11_H__ 
#define __D3D11_H__

#include "dxgi.h"
#include "d3dcommon.h"

#define	D3D11_SDK_VERSION	( 7 )

#ifndef FLT_MAX
#define FLT_MAX 3.402823466e+38F
#endif

typedef interface ID3D11Device ID3D11Device; 
typedef interface ID3D11DepthStencilState ID3D11DepthStencilState;
typedef interface ID3D11SamplerState ID3D11SamplerState;
typedef interface ID3D11BlendState ID3D11BlendState;
typedef interface ID3D11ClassLinkage ID3D11ClassLinkage;
typedef interface ID3D11DepthStencilView ID3D11DepthStencilView;
typedef interface ID3D11RenderTargetView ID3D11RenderTargetView;
typedef interface ID3D11Resource ID3D11Resource;
typedef interface ID3D11Texture1D ID3D11Texture1D;
typedef interface ID3D11Texture2D ID3D11Texture2D;
typedef interface ID3D11Texture3D ID3D11Texture3D;
typedef interface ID3D11Buffer ID3D11Buffer;
typedef interface ID3D11DeviceChild ID3D11DeviceChild;
typedef interface ID3D11View ID3D11View;
typedef interface ID3D11ClassInstance ID3D11ClassInstance;
typedef interface ID3D11ShaderResourceView ID3D11ShaderResourceView;
typedef interface ID3D11UnorderedAccessView ID3D11UnorderedAccessView;
typedef interface ID3D11InputLayout ID3D11InputLayout;
typedef interface ID3D11VertexShader ID3D11VertexShader;
typedef interface ID3D11GeometryShader ID3D11GeometryShader;
typedef interface ID3D11PixelShader ID3D11PixelShader;
typedef interface ID3D11HullShader ID3D11HullShader;
typedef interface ID3D11DomainShader ID3D11DomainShader;
typedef interface ID3D11ComputeShader ID3D11ComputeShader;
typedef interface ID3D11RasterizerState ID3D11RasterizerState;
typedef interface ID3D11Query ID3D11Query;
typedef interface ID3D11Asynchronous ID3D11Asynchronous;
typedef interface ID3D11Predicate ID3D11Predicate;
typedef interface ID3D11Counter ID3D11Counter;
typedef interface ID3D11DeviceContext ID3D11DeviceContext;
typedef interface ID3D11CommandList ID3D11CommandList;

using D3D11_SRV_DIMENSION = D3D_SRV_DIMENSION;
using D3D11_RECT = RECT;
using D3D11_PRIMITIVE_TOPOLOGY = D3D_PRIMITIVE_TOPOLOGY;
using D3D11_PRIMITIVE = D3D_PRIMITIVE;

enum D3D11_FEATURE{
	D3D11_FEATURE_THREADING	= 0,
	D3D11_FEATURE_DOUBLES	= ( D3D11_FEATURE_THREADING + 1 ) ,
	D3D11_FEATURE_FORMAT_SUPPORT	= ( D3D11_FEATURE_DOUBLES + 1 ) ,
	D3D11_FEATURE_FORMAT_SUPPORT2	= ( D3D11_FEATURE_FORMAT_SUPPORT + 1 ) ,
	D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS	= ( D3D11_FEATURE_FORMAT_SUPPORT2 + 1 ) ,
	D3D11_FEATURE_D3D11_OPTIONS	= ( D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS + 1 ) ,
	D3D11_FEATURE_ARCHITECTURE_INFO	= ( D3D11_FEATURE_D3D11_OPTIONS + 1 ) ,
	D3D11_FEATURE_D3D9_OPTIONS	= ( D3D11_FEATURE_ARCHITECTURE_INFO + 1 ) ,
	D3D11_FEATURE_SHADER_MIN_PRECISION_SUPPORT	= ( D3D11_FEATURE_D3D9_OPTIONS + 1 ) ,
	D3D11_FEATURE_D3D9_SHADOW_SUPPORT	= ( D3D11_FEATURE_SHADER_MIN_PRECISION_SUPPORT + 1 ) ,
	D3D11_FEATURE_D3D11_OPTIONS1	= ( D3D11_FEATURE_D3D9_SHADOW_SUPPORT + 1 ) ,
	D3D11_FEATURE_D3D9_SIMPLE_INSTANCING_SUPPORT	= ( D3D11_FEATURE_D3D11_OPTIONS1 + 1 ) ,
	D3D11_FEATURE_MARKER_SUPPORT	= ( D3D11_FEATURE_D3D9_SIMPLE_INSTANCING_SUPPORT + 1 ) ,
	D3D11_FEATURE_D3D9_OPTIONS1	= ( D3D11_FEATURE_MARKER_SUPPORT + 1 ) ,
	D3D11_FEATURE_D3D11_OPTIONS2	= ( D3D11_FEATURE_D3D9_OPTIONS1 + 1 ) ,
	D3D11_FEATURE_D3D11_OPTIONS3	= ( D3D11_FEATURE_D3D11_OPTIONS2 + 1 ) ,
	D3D11_FEATURE_GPU_VIRTUAL_ADDRESS_SUPPORT	= ( D3D11_FEATURE_D3D11_OPTIONS3 + 1 ) ,
	D3D11_FEATURE_D3D11_OPTIONS4	= ( D3D11_FEATURE_GPU_VIRTUAL_ADDRESS_SUPPORT + 1 ) ,
	D3D11_FEATURE_SHADER_CACHE	= ( D3D11_FEATURE_D3D11_OPTIONS4 + 1 ) 
};

enum D3D11_DSV_DIMENSION{
	D3D11_DSV_DIMENSION_UNKNOWN	= 0,
	D3D11_DSV_DIMENSION_TEXTURE1D	= 1,
	D3D11_DSV_DIMENSION_TEXTURE1DARRAY	= 2,
	D3D11_DSV_DIMENSION_TEXTURE2D	= 3,
	D3D11_DSV_DIMENSION_TEXTURE2DARRAY	= 4,
	D3D11_DSV_DIMENSION_TEXTURE2DMS	= 5,
	D3D11_DSV_DIMENSION_TEXTURE2DMSARRAY	= 6
};

enum D3D11_DEVICE_CONTEXT_TYPE{
	D3D11_DEVICE_CONTEXT_IMMEDIATE	= 0,
	D3D11_DEVICE_CONTEXT_DEFERRED	= ( D3D11_DEVICE_CONTEXT_IMMEDIATE + 1 ) 
};


enum D3D11_QUERY{
	D3D11_QUERY_EVENT	= 0,
	D3D11_QUERY_OCCLUSION	= ( D3D11_QUERY_EVENT + 1 ) ,
	D3D11_QUERY_TIMESTAMP	= ( D3D11_QUERY_OCCLUSION + 1 ) ,
	D3D11_QUERY_TIMESTAMP_DISJOINT	= ( D3D11_QUERY_TIMESTAMP + 1 ) ,
	D3D11_QUERY_PIPELINE_STATISTICS	= ( D3D11_QUERY_TIMESTAMP_DISJOINT + 1 ) ,
	D3D11_QUERY_OCCLUSION_PREDICATE	= ( D3D11_QUERY_PIPELINE_STATISTICS + 1 ) ,
	D3D11_QUERY_SO_STATISTICS	= ( D3D11_QUERY_OCCLUSION_PREDICATE + 1 ) ,
	D3D11_QUERY_SO_OVERFLOW_PREDICATE	= ( D3D11_QUERY_SO_STATISTICS + 1 ) ,
	D3D11_QUERY_SO_STATISTICS_STREAM0	= ( D3D11_QUERY_SO_OVERFLOW_PREDICATE + 1 ) ,
	D3D11_QUERY_SO_OVERFLOW_PREDICATE_STREAM0	= ( D3D11_QUERY_SO_STATISTICS_STREAM0 + 1 ) ,
	D3D11_QUERY_SO_STATISTICS_STREAM1	= ( D3D11_QUERY_SO_OVERFLOW_PREDICATE_STREAM0 + 1 ) ,
	D3D11_QUERY_SO_OVERFLOW_PREDICATE_STREAM1	= ( D3D11_QUERY_SO_STATISTICS_STREAM1 + 1 ) ,
	D3D11_QUERY_SO_STATISTICS_STREAM2	= ( D3D11_QUERY_SO_OVERFLOW_PREDICATE_STREAM1 + 1 ) ,
	D3D11_QUERY_SO_OVERFLOW_PREDICATE_STREAM2	= ( D3D11_QUERY_SO_STATISTICS_STREAM2 + 1 ) ,
	D3D11_QUERY_SO_STATISTICS_STREAM3	= ( D3D11_QUERY_SO_OVERFLOW_PREDICATE_STREAM2 + 1 ) ,
	D3D11_QUERY_SO_OVERFLOW_PREDICATE_STREAM3	= ( D3D11_QUERY_SO_STATISTICS_STREAM3 + 1 ) 
};

enum D3D11_CPU_ACCESS_FLAG{
	D3D11_CPU_ACCESS_WRITE	= 0x10000L,
	D3D11_CPU_ACCESS_READ	= 0x20000L
};
	
enum D3D11_CLEAR_FLAG{
	D3D11_CLEAR_DEPTH	= 0x1L,
	D3D11_CLEAR_STENCIL	= 0x2L
};
	
enum D3D11_COLOR_WRITE_ENABLE{
	D3D11_COLOR_WRITE_ENABLE_RED	= 1,
	D3D11_COLOR_WRITE_ENABLE_GREEN	= 2,
	D3D11_COLOR_WRITE_ENABLE_BLUE	= 4,
	D3D11_COLOR_WRITE_ENABLE_ALPHA	= 8,
	D3D11_COLOR_WRITE_ENABLE_ALL	= ( ( ( D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN )  | D3D11_COLOR_WRITE_ENABLE_BLUE )  | D3D11_COLOR_WRITE_ENABLE_ALPHA ) 
};

enum D3D11_BIND_FLAG{
	D3D11_BIND_VERTEX_BUFFER	= 0x1L,
	D3D11_BIND_INDEX_BUFFER	= 0x2L,
	D3D11_BIND_CONSTANT_BUFFER	= 0x4L,
	D3D11_BIND_SHADER_RESOURCE	= 0x8L,
	D3D11_BIND_STREAM_OUTPUT	= 0x10L,
	D3D11_BIND_RENDER_TARGET	= 0x20L,
	D3D11_BIND_DEPTH_STENCIL	= 0x40L,
	D3D11_BIND_UNORDERED_ACCESS	= 0x80L,
	D3D11_BIND_DECODER	= 0x200L,
	D3D11_BIND_VIDEO_ENCODER	= 0x400L
};

enum D3D11_FILTER{
	D3D11_FILTER_MIN_MAG_MIP_POINT	= 0,
	D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR	= 0x1,
	D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT	= 0x4,
	D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR	= 0x5,
	D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT	= 0x10,
	D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR	= 0x11,
	D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT	= 0x14,
	D3D11_FILTER_MIN_MAG_MIP_LINEAR	= 0x15,
	D3D11_FILTER_ANISOTROPIC	= 0x55,
	D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT	= 0x80,
	D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR	= 0x81,
	D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT	= 0x84,
	D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR	= 0x85,
	D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT	= 0x90,
	D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR	= 0x91,
	D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT	= 0x94,
	D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR	= 0x95,
	D3D11_FILTER_COMPARISON_ANISOTROPIC	= 0xd5,
	D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT	= 0x100,
	D3D11_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR	= 0x101,
	D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT	= 0x104,
	D3D11_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR	= 0x105,
	D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT	= 0x110,
	D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR	= 0x111,
	D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT	= 0x114,
	D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR	= 0x115,
	D3D11_FILTER_MINIMUM_ANISOTROPIC	= 0x155,
	D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT	= 0x180,
	D3D11_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR	= 0x181,
	D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT	= 0x184,
	D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR	= 0x185,
	D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT	= 0x190,
	D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR	= 0x191,
	D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT	= 0x194,
	D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR	= 0x195,
	D3D11_FILTER_MAXIMUM_ANISOTROPIC	= 0x1d5
};

enum D3D11_CREATE_DEVICE_FLAG{
	D3D11_CREATE_DEVICE_SINGLETHREADED	= 0x1,
	D3D11_CREATE_DEVICE_DEBUG	= 0x2,
	D3D11_CREATE_DEVICE_SWITCH_TO_REF	= 0x4,
	D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS	= 0x8,
	D3D11_CREATE_DEVICE_BGRA_SUPPORT	= 0x20,
	D3D11_CREATE_DEVICE_DEBUGGABLE	= 0x40,
	D3D11_CREATE_DEVICE_PREVENT_ALTERING_LAYER_SETTINGS_FROM_REGISTRY	= 0x80,
	D3D11_CREATE_DEVICE_DISABLE_GPU_TIMEOUT	= 0x100,
	D3D11_CREATE_DEVICE_VIDEO_SUPPORT	= 0x800
};
	
enum D3D11_TEXTURE_ADDRESS_MODE{
	D3D11_TEXTURE_ADDRESS_WRAP	= 1,
	D3D11_TEXTURE_ADDRESS_MIRROR	= 2,
	D3D11_TEXTURE_ADDRESS_CLAMP	= 3,
	D3D11_TEXTURE_ADDRESS_BORDER	= 4,
	D3D11_TEXTURE_ADDRESS_MIRROR_ONCE	= 5
};

enum D3D11_COMPARISON_FUNC{
	D3D11_COMPARISON_NEVER	= 1,
	D3D11_COMPARISON_LESS	= 2,
	D3D11_COMPARISON_EQUAL	= 3,
	D3D11_COMPARISON_LESS_EQUAL	= 4,
	D3D11_COMPARISON_GREATER	= 5,
	D3D11_COMPARISON_NOT_EQUAL	= 6,
	D3D11_COMPARISON_GREATER_EQUAL	= 7,
	D3D11_COMPARISON_ALWAYS	= 8
};

enum D3D11_COUNTER{
	D3D11_COUNTER_DEVICE_DEPENDENT_0	= 0x40000000
};

enum D3D11_FILL_MODE{
	D3D11_FILL_WIREFRAME	= 2,
	D3D11_FILL_SOLID	= 3
};

enum D3D11_CULL_MODE{
	D3D11_CULL_NONE	= 1,
	D3D11_CULL_FRONT	= 2,
	D3D11_CULL_BACK	= 3
};

enum D3D11_DEPTH_WRITE_MASK{
	D3D11_DEPTH_WRITE_MASK_ZERO	= 0,
	D3D11_DEPTH_WRITE_MASK_ALL	= 1
};

enum D3D11_INPUT_CLASSIFICATION{
	D3D11_INPUT_PER_VERTEX_DATA	= 0,
	D3D11_INPUT_PER_INSTANCE_DATA	= 1
};

enum D3D11_USAGE{
	D3D11_USAGE_DEFAULT	= 0,
	D3D11_USAGE_IMMUTABLE	= 1,
	D3D11_USAGE_DYNAMIC	= 2,
	D3D11_USAGE_STAGING	= 3
};

enum D3D11_UAV_DIMENSION{
	D3D11_UAV_DIMENSION_UNKNOWN	= 0,
	D3D11_UAV_DIMENSION_BUFFER	= 1,
	D3D11_UAV_DIMENSION_TEXTURE1D	= 2,
	D3D11_UAV_DIMENSION_TEXTURE1DARRAY	= 3,
	D3D11_UAV_DIMENSION_TEXTURE2D	= 4,
	D3D11_UAV_DIMENSION_TEXTURE2DARRAY	= 5,
	D3D11_UAV_DIMENSION_TEXTURE3D	= 8
};

enum D3D11_RTV_DIMENSION{
	D3D11_RTV_DIMENSION_UNKNOWN	= 0,
	D3D11_RTV_DIMENSION_BUFFER	= 1,
	D3D11_RTV_DIMENSION_TEXTURE1D	= 2,
	D3D11_RTV_DIMENSION_TEXTURE1DARRAY	= 3,
	D3D11_RTV_DIMENSION_TEXTURE2D	= 4,
	D3D11_RTV_DIMENSION_TEXTURE2DARRAY	= 5,
	D3D11_RTV_DIMENSION_TEXTURE2DMS	= 6,
	D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY	= 7,
	D3D11_RTV_DIMENSION_TEXTURE3D	= 8
};

enum D3D11_RESOURCE_DIMENSION{
	D3D11_RESOURCE_DIMENSION_UNKNOWN	= 0,
	D3D11_RESOURCE_DIMENSION_BUFFER	= 1,
	D3D11_RESOURCE_DIMENSION_TEXTURE1D	= 2,
	D3D11_RESOURCE_DIMENSION_TEXTURE2D	= 3,
	D3D11_RESOURCE_DIMENSION_TEXTURE3D	= 4
};

enum D3D11_BLEND{
	D3D11_BLEND_ZERO	= 1,
	D3D11_BLEND_ONE	= 2,
	D3D11_BLEND_SRC_COLOR	= 3,
	D3D11_BLEND_INV_SRC_COLOR	= 4,
	D3D11_BLEND_SRC_ALPHA	= 5,
	D3D11_BLEND_INV_SRC_ALPHA	= 6,
	D3D11_BLEND_DEST_ALPHA	= 7,
	D3D11_BLEND_INV_DEST_ALPHA	= 8,
	D3D11_BLEND_DEST_COLOR	= 9,
	D3D11_BLEND_INV_DEST_COLOR	= 10,
	D3D11_BLEND_SRC_ALPHA_SAT	= 11,
	D3D11_BLEND_BLEND_FACTOR	= 14,
	D3D11_BLEND_INV_BLEND_FACTOR	= 15,
	D3D11_BLEND_SRC1_COLOR	= 16,
	D3D11_BLEND_INV_SRC1_COLOR	= 17,
	D3D11_BLEND_SRC1_ALPHA	= 18,
	D3D11_BLEND_INV_SRC1_ALPHA	= 19
};

enum D3D11_MAP{
	D3D11_MAP_READ	= 1,
	D3D11_MAP_WRITE	= 2,
	D3D11_MAP_READ_WRITE	= 3,
	D3D11_MAP_WRITE_DISCARD	= 4,
	D3D11_MAP_WRITE_NO_OVERWRITE	= 5
};
	
enum D3D11_BLEND_OP{
	D3D11_BLEND_OP_ADD	= 1,
	D3D11_BLEND_OP_SUBTRACT	= 2,
	D3D11_BLEND_OP_REV_SUBTRACT	= 3,
	D3D11_BLEND_OP_MIN	= 4,
	D3D11_BLEND_OP_MAX	= 5
};


enum D3D11_COUNTER_TYPE{
	D3D11_COUNTER_TYPE_FLOAT32	= 0,
	D3D11_COUNTER_TYPE_UINT16	= ( D3D11_COUNTER_TYPE_FLOAT32 + 1 ) ,
	D3D11_COUNTER_TYPE_UINT32	= ( D3D11_COUNTER_TYPE_UINT16 + 1 ) ,
	D3D11_COUNTER_TYPE_UINT64	= ( D3D11_COUNTER_TYPE_UINT32 + 1 ) 
};
	
struct D3D11_COUNTER_INFO{
    D3D11_COUNTER LastDeviceDependentCounter;
    UINT NumSimultaneousCounters;
    UINT8 NumDetectableParallelUnits;
};
	
struct D3D11_RENDER_TARGET_BLEND_DESC{
    BOOL BlendEnable;
    D3D11_BLEND SrcBlend;
    D3D11_BLEND DestBlend;
    D3D11_BLEND_OP BlendOp;
    D3D11_BLEND SrcBlendAlpha;
    D3D11_BLEND DestBlendAlpha;
    D3D11_BLEND_OP BlendOpAlpha;
    UINT8 RenderTargetWriteMask;
};

struct D3D11_BLEND_DESC{
    BOOL AlphaToCoverageEnable;
    BOOL IndependentBlendEnable;
    D3D11_RENDER_TARGET_BLEND_DESC RenderTarget[ 8 ];
};
	
struct D3D11_TEXTURE1D_DESC{
    UINT Width;
    UINT MipLevels;
    UINT ArraySize;
    DXGI_FORMAT Format;
    D3D11_USAGE Usage;
    UINT BindFlags;
    UINT CPUAccessFlags;
    UINT MiscFlags;
};

struct D3D11_TEXTURE2D_DESC{
    UINT Width;
    UINT Height;
    UINT MipLevels;
    UINT ArraySize;
    DXGI_FORMAT Format;
    DXGI_SAMPLE_DESC SampleDesc;
    D3D11_USAGE Usage;
    UINT BindFlags;
    UINT CPUAccessFlags;
    UINT MiscFlags;
};

	
struct D3D11_BUFFER_DESC{
    UINT ByteWidth;
    D3D11_USAGE Usage;
    UINT BindFlags;
    UINT CPUAccessFlags;
    UINT MiscFlags;
    UINT StructureByteStride;
};

struct D3D11_BUFFER_UAV{
    UINT FirstElement;
    UINT NumElements;
    UINT Flags;
};

struct D3D11_TEX1D_UAV{
    UINT MipSlice;
};

struct D3D11_TEX1D_ARRAY_UAV{
    UINT MipSlice;
    UINT FirstArraySlice;
    UINT ArraySize;
};

struct D3D11_TEX2D_UAV{
    UINT MipSlice;
};

struct D3D11_TEX2D_ARRAY_UAV{
    UINT MipSlice;
    UINT FirstArraySlice;
    UINT ArraySize;
};

struct D3D11_TEX3D_UAV{
    UINT MipSlice;
    UINT FirstWSlice;
    UINT WSize;
};
	
struct D3D11_INPUT_ELEMENT_DESC{
    LPCSTR SemanticName;
    UINT SemanticIndex;
    DXGI_FORMAT Format;
    UINT InputSlot;
    UINT AlignedByteOffset;
    D3D11_INPUT_CLASSIFICATION InputSlotClass;
    UINT InstanceDataStepRate;
};

struct D3D11_SUBRESOURCE_DATA{
    const void *pSysMem;
    UINT SysMemPitch;
    UINT SysMemSlicePitch;
};

struct D3D11_MAPPED_SUBRESOURCE{
    void *pData;
    UINT RowPitch;
    UINT DepthPitch;
};

struct D3D11_SO_DECLARATION_ENTRY{
    UINT Stream;
    LPCSTR SemanticName;
    UINT SemanticIndex;
    BYTE StartComponent;
    BYTE ComponentCount;
    BYTE OutputSlot;
};
	
struct D3D11_COUNTER_DESC{
    D3D11_COUNTER Counter;
    UINT MiscFlags;
};
	
struct D3D11_TEX1D_DSV{
    UINT MipSlice;
};

struct D3D11_TEX2DMS_ARRAY_DSV{
    UINT FirstArraySlice;
    UINT ArraySize;
};

struct D3D11_TEX1D_ARRAY_DSV{
    UINT MipSlice;
    UINT FirstArraySlice;
    UINT ArraySize;
};

struct D3D11_TEX2D_DSV{
    UINT MipSlice;
};

struct D3D11_TEX2D_ARRAY_DSV{
    UINT MipSlice;
    UINT FirstArraySlice;
    UINT ArraySize;
};

struct D3D11_BUFFER_RTV{
	union{
		UINT FirstElement;
		UINT ElementOffset;
	};
	union{
		UINT NumElements;
		UINT ElementWidth;
	};
};

struct D3D11_TEX1D_RTV{
    UINT MipSlice;
};

struct D3D11_TEX1D_ARRAY_RTV{
    UINT MipSlice;
    UINT FirstArraySlice;
    UINT ArraySize;
};

struct D3D11_TEX2D_RTV{
    UINT MipSlice;
};
	
struct D3D11_TEX2DMS_DSV{
    UINT UnusedField_NothingToDefine;
};

struct D3D11_TEX2DMS_RTV{
    UINT UnusedField_NothingToDefine;
};

struct D3D11_TEX2D_ARRAY_RTV{
    UINT MipSlice;
    UINT FirstArraySlice;
    UINT ArraySize;
};

struct D3D11_TEX2DMS_ARRAY_RTV{
    UINT FirstArraySlice;
    UINT ArraySize;
};

struct D3D11_TEX3D_RTV{
    UINT MipSlice;
    UINT FirstWSlice;
    UINT WSize;
};
	
struct D3D11_TEXTURE3D_DESC{
    UINT Width;
    UINT Height;
    UINT Depth;
    UINT MipLevels;
    DXGI_FORMAT Format;
    D3D11_USAGE Usage;
    UINT BindFlags;
    UINT CPUAccessFlags;
    UINT MiscFlags;
};

struct D3D11_RENDER_TARGET_VIEW_DESC{
    DXGI_FORMAT Format;
    D3D11_RTV_DIMENSION ViewDimension;
    union{
        D3D11_BUFFER_RTV Buffer;
        D3D11_TEX1D_RTV Texture1D;
        D3D11_TEX1D_ARRAY_RTV Texture1DArray;
        D3D11_TEX2D_RTV Texture2D;
        D3D11_TEX2D_ARRAY_RTV Texture2DArray;
        D3D11_TEX2DMS_RTV Texture2DMS;
        D3D11_TEX2DMS_ARRAY_RTV Texture2DMSArray;
        D3D11_TEX3D_RTV Texture3D;
    };
};
	
struct D3D11_UNORDERED_ACCESS_VIEW_DESC{
    DXGI_FORMAT Format;
    D3D11_UAV_DIMENSION ViewDimension;
    union 
        {
        D3D11_BUFFER_UAV Buffer;
        D3D11_TEX1D_UAV Texture1D;
        D3D11_TEX1D_ARRAY_UAV Texture1DArray;
        D3D11_TEX2D_UAV Texture2D;
        D3D11_TEX2D_ARRAY_UAV Texture2DArray;
        D3D11_TEX3D_UAV Texture3D;
        } 	;
};
	
struct D3D11_DEPTH_STENCIL_VIEW_DESC{
    DXGI_FORMAT Format;
    D3D11_DSV_DIMENSION ViewDimension;
    UINT Flags;
    union{
        D3D11_TEX1D_DSV Texture1D;
        D3D11_TEX1D_ARRAY_DSV Texture1DArray;
        D3D11_TEX2D_DSV Texture2D;
        D3D11_TEX2D_ARRAY_DSV Texture2DArray;
        D3D11_TEX2DMS_DSV Texture2DMS;
        D3D11_TEX2DMS_ARRAY_DSV Texture2DMSArray;
    };
};

enum D3D11_STENCIL_OP{
	D3D11_STENCIL_OP_KEEP	= 1,
	D3D11_STENCIL_OP_ZERO	= 2,
	D3D11_STENCIL_OP_REPLACE	= 3,
	D3D11_STENCIL_OP_INCR_SAT	= 4,
	D3D11_STENCIL_OP_DECR_SAT	= 5,
	D3D11_STENCIL_OP_INVERT	= 6,
	D3D11_STENCIL_OP_INCR	= 7,
	D3D11_STENCIL_OP_DECR	= 8
};

struct D3D11_DEPTH_STENCILOP_DESC{
    D3D11_STENCIL_OP StencilFailOp;
    D3D11_STENCIL_OP StencilDepthFailOp;
    D3D11_STENCIL_OP StencilPassOp;
    D3D11_COMPARISON_FUNC StencilFunc;
};
	
struct D3D11_DEPTH_STENCIL_DESC{
	BOOL DepthEnable;
	D3D11_DEPTH_WRITE_MASK DepthWriteMask;
	D3D11_COMPARISON_FUNC DepthFunc;
	BOOL StencilEnable;
	UINT8 StencilReadMask;
	UINT8 StencilWriteMask;
	D3D11_DEPTH_STENCILOP_DESC FrontFace;
	D3D11_DEPTH_STENCILOP_DESC BackFace;
};

struct D3D11_BUFFER_SRV{
    union{
        UINT FirstElement;
        UINT ElementOffset;
    };
    union{
        UINT NumElements;
        UINT ElementWidth;
    };
};

struct D3D11_TEX1D_SRV{
    UINT MostDetailedMip;
    UINT MipLevels;
};

struct D3D11_QUERY_DESC{
    D3D11_QUERY Query;
    UINT MiscFlags;
};
	
struct D3D11_TEX1D_ARRAY_SRV{
    UINT MostDetailedMip;
    UINT MipLevels;
    UINT FirstArraySlice;
    UINT ArraySize;
};

struct D3D11_TEX2D_SRV{
    UINT MostDetailedMip;
    UINT MipLevels;
};

struct D3D11_TEX2D_ARRAY_SRV{
    UINT MostDetailedMip;
    UINT MipLevels;
    UINT FirstArraySlice;
    UINT ArraySize;
};

struct D3D11_TEX2DMS_SRV{
    UINT UnusedField_NothingToDefine;
};

struct D3D11_TEX2DMS_ARRAY_SRV{
    UINT FirstArraySlice;
    UINT ArraySize;
};
	
struct D3D11_TEX3D_SRV{
    UINT MostDetailedMip;
    UINT MipLevels;
};

struct D3D11_TEXCUBE_SRV{
    UINT MostDetailedMip;
    UINT MipLevels;
};

struct D3D11_TEXCUBE_ARRAY_SRV{
    UINT MostDetailedMip;
    UINT MipLevels;
    UINT First2DArrayFace;
    UINT NumCubes;
};

struct D3D11_BUFFEREX_SRV{
    UINT FirstElement;
    UINT NumElements;
    UINT Flags;
};

struct D3D11_BOX{
    UINT left;
    UINT top;
    UINT front;
    UINT right;
    UINT bottom;
    UINT back;
};
	
struct D3D11_VIEWPORT{
    FLOAT TopLeftX;
    FLOAT TopLeftY;
    FLOAT Width;
    FLOAT Height;
    FLOAT MinDepth;
    FLOAT MaxDepth;
};

struct D3D11_SHADER_RESOURCE_VIEW_DESC{
    DXGI_FORMAT Format;
    D3D11_SRV_DIMENSION ViewDimension;
    union{
        D3D11_BUFFER_SRV Buffer;
        D3D11_TEX1D_SRV Texture1D;
        D3D11_TEX1D_ARRAY_SRV Texture1DArray;
        D3D11_TEX2D_SRV Texture2D;
        D3D11_TEX2D_ARRAY_SRV Texture2DArray;
        D3D11_TEX2DMS_SRV Texture2DMS;
        D3D11_TEX2DMS_ARRAY_SRV Texture2DMSArray;
        D3D11_TEX3D_SRV Texture3D;
        D3D11_TEXCUBE_SRV TextureCube;
        D3D11_TEXCUBE_ARRAY_SRV TextureCubeArray;
        D3D11_BUFFEREX_SRV BufferEx;
    };
};
	
struct D3D11_SAMPLER_DESC{
    D3D11_FILTER Filter;
    D3D11_TEXTURE_ADDRESS_MODE AddressU;
    D3D11_TEXTURE_ADDRESS_MODE AddressV;
    D3D11_TEXTURE_ADDRESS_MODE AddressW;
    FLOAT MipLODBias;
    UINT MaxAnisotropy;
    D3D11_COMPARISON_FUNC ComparisonFunc;
    FLOAT BorderColor[ 4 ];
    FLOAT MinLOD;
    FLOAT MaxLOD;
};

struct D3D11_RASTERIZER_DESC{
    D3D11_FILL_MODE FillMode;
    D3D11_CULL_MODE CullMode;
    BOOL FrontCounterClockwise;
    INT DepthBias;
    FLOAT DepthBiasClamp;
    FLOAT SlopeScaledDepthBias;
    BOOL DepthClipEnable;
    BOOL ScissorEnable;
    BOOL MultisampleEnable;
    BOOL AntialiasedLineEnable;
};

EXTERN_C const IID IID_ID3D11DeviceChild;
MIDL_INTERFACE("1841e5c8-16b0-489b-bcc8-44cfb0d5deae")
ID3D11DeviceChild : public IUnknown{
public:
	virtual void STDMETHODCALLTYPE GetDevice( 
		_Outptr_  ID3D11Device **ppDevice) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetPrivateData( 
		_In_  REFGUID guid,
		_Inout_  UINT *pDataSize,
		_Out_writes_bytes_opt_( *pDataSize )  void *pData) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE SetPrivateData( 
		_In_  REFGUID guid,
		_In_  UINT DataSize,
		_In_reads_bytes_opt_( DataSize )  const void *pData) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface( 
		_In_  REFGUID guid,
		_In_opt_  const IUnknown *pData) = 0;
	
};

EXTERN_C const IID IID_ID3D11CommandList;    
MIDL_INTERFACE("a24bc4d1-769e-43f7-8013-98ff566c18e2")
ID3D11CommandList : public ID3D11DeviceChild{
public:
	virtual UINT STDMETHODCALLTYPE GetContextFlags( void) = 0;
};

EXTERN_C const IID IID_ID3D11DeviceContext;    
MIDL_INTERFACE("c0bfa96c-e089-44fb-8eaf-26f8796190da")
ID3D11DeviceContext : public ID3D11DeviceChild{
public:
	virtual void STDMETHODCALLTYPE VSSetConstantBuffers( 
		_In_range_( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		_In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers) = 0;
	
	virtual void STDMETHODCALLTYPE PSSetShaderResources( 
		_In_range_( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		_In_reads_opt_(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews) = 0;
	
	virtual void STDMETHODCALLTYPE PSSetShader( 
		_In_opt_  ID3D11PixelShader *pPixelShader,
		_In_reads_opt_(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
		UINT NumClassInstances) = 0;
	
	virtual void STDMETHODCALLTYPE PSSetSamplers( 
		_In_range_( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		_In_reads_opt_(NumSamplers)  ID3D11SamplerState *const *ppSamplers) = 0;
	
	virtual void STDMETHODCALLTYPE VSSetShader( 
		_In_opt_  ID3D11VertexShader *pVertexShader,
		_In_reads_opt_(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
		UINT NumClassInstances) = 0;
	
	virtual void STDMETHODCALLTYPE DrawIndexed( 
		_In_  UINT IndexCount,
		_In_  UINT StartIndexLocation,
		_In_  INT BaseVertexLocation) = 0;
	
	virtual void STDMETHODCALLTYPE Draw( 
		_In_  UINT VertexCount,
		_In_  UINT StartVertexLocation) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE Map( 
		_In_  ID3D11Resource *pResource,
		_In_  UINT Subresource,
		_In_  D3D11_MAP MapType,
		_In_  UINT MapFlags,
		_Out_opt_  D3D11_MAPPED_SUBRESOURCE *pMappedResource) = 0;
	
	virtual void STDMETHODCALLTYPE Unmap( 
		_In_  ID3D11Resource *pResource,
		_In_  UINT Subresource) = 0;
	
	virtual void STDMETHODCALLTYPE PSSetConstantBuffers( 
		_In_range_( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		_In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers) = 0;
	
	virtual void STDMETHODCALLTYPE IASetInputLayout( 
		_In_opt_  ID3D11InputLayout *pInputLayout) = 0;
	
	virtual void STDMETHODCALLTYPE IASetVertexBuffers( 
		_In_range_( 0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		_In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppVertexBuffers,
		_In_reads_opt_(NumBuffers)  const UINT *pStrides,
		_In_reads_opt_(NumBuffers)  const UINT *pOffsets) = 0;
	
	virtual void STDMETHODCALLTYPE IASetIndexBuffer( 
		_In_opt_  ID3D11Buffer *pIndexBuffer,
		_In_  DXGI_FORMAT Format,
		_In_  UINT Offset) = 0;
	
	virtual void STDMETHODCALLTYPE DrawIndexedInstanced( 
		_In_  UINT IndexCountPerInstance,
		_In_  UINT InstanceCount,
		_In_  UINT StartIndexLocation,
		_In_  INT BaseVertexLocation,
		_In_  UINT StartInstanceLocation) = 0;
	
	virtual void STDMETHODCALLTYPE DrawInstanced( 
		_In_  UINT VertexCountPerInstance,
		_In_  UINT InstanceCount,
		_In_  UINT StartVertexLocation,
		_In_  UINT StartInstanceLocation) = 0;
	
	virtual void STDMETHODCALLTYPE GSSetConstantBuffers( 
		_In_range_( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		_In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers) = 0;
	
	virtual void STDMETHODCALLTYPE GSSetShader( 
		_In_opt_  ID3D11GeometryShader *pShader,
		_In_reads_opt_(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
		UINT NumClassInstances) = 0;
	
	virtual void STDMETHODCALLTYPE IASetPrimitiveTopology( 
		_In_  D3D11_PRIMITIVE_TOPOLOGY Topology) = 0;
	
	virtual void STDMETHODCALLTYPE VSSetShaderResources( 
		_In_range_( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		_In_reads_opt_(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews) = 0;
	
	virtual void STDMETHODCALLTYPE VSSetSamplers( 
		_In_range_( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		_In_reads_opt_(NumSamplers)  ID3D11SamplerState *const *ppSamplers) = 0;
	
	virtual void STDMETHODCALLTYPE Begin( 
		_In_  ID3D11Asynchronous *pAsync) = 0;
	
	virtual void STDMETHODCALLTYPE End( 
		_In_  ID3D11Asynchronous *pAsync) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetData( 
		_In_  ID3D11Asynchronous *pAsync,
		_Out_writes_bytes_opt_( DataSize )  void *pData,
		_In_  UINT DataSize,
		_In_  UINT GetDataFlags) = 0;
	
	virtual void STDMETHODCALLTYPE SetPredication( 
		_In_opt_  ID3D11Predicate *pPredicate,
		_In_  BOOL PredicateValue) = 0;
	
	virtual void STDMETHODCALLTYPE GSSetShaderResources( 
		_In_range_( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		_In_reads_opt_(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews) = 0;
	
	virtual void STDMETHODCALLTYPE GSSetSamplers( 
		_In_range_( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		_In_reads_opt_(NumSamplers)  ID3D11SamplerState *const *ppSamplers) = 0;
	
	virtual void STDMETHODCALLTYPE OMSetRenderTargets( 
		_In_range_( 0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT )  UINT NumViews,
		_In_reads_opt_(NumViews)  ID3D11RenderTargetView *const *ppRenderTargetViews,
		_In_opt_  ID3D11DepthStencilView *pDepthStencilView) = 0;
	
	virtual void STDMETHODCALLTYPE OMSetRenderTargetsAndUnorderedAccessViews( 
		_In_  UINT NumRTVs,
		_In_reads_opt_(NumRTVs)  ID3D11RenderTargetView *const *ppRenderTargetViews,
		_In_opt_  ID3D11DepthStencilView *pDepthStencilView,
		_In_range_( 0, D3D11_1_UAV_SLOT_COUNT - 1 )  UINT UAVStartSlot,
		_In_  UINT NumUAVs,
		_In_reads_opt_(NumUAVs)  ID3D11UnorderedAccessView *const *ppUnorderedAccessViews,
		_In_reads_opt_(NumUAVs)  const UINT *pUAVInitialCounts) = 0;
	
	virtual void STDMETHODCALLTYPE OMSetBlendState( 
		_In_opt_  ID3D11BlendState *pBlendState,
		_In_opt_  const FLOAT BlendFactor[ 4 ],
		_In_  UINT SampleMask) = 0;
	
	virtual void STDMETHODCALLTYPE OMSetDepthStencilState( 
		_In_opt_  ID3D11DepthStencilState *pDepthStencilState,
		_In_  UINT StencilRef) = 0;
	
	virtual void STDMETHODCALLTYPE SOSetTargets( 
		_In_range_( 0, D3D11_SO_BUFFER_SLOT_COUNT)  UINT NumBuffers,
		_In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppSOTargets,
		_In_reads_opt_(NumBuffers)  const UINT *pOffsets) = 0;
	
	virtual void STDMETHODCALLTYPE DrawAuto( void) = 0;
	
	virtual void STDMETHODCALLTYPE DrawIndexedInstancedIndirect( 
		_In_  ID3D11Buffer *pBufferForArgs,
		_In_  UINT AlignedByteOffsetForArgs) = 0;
	
	virtual void STDMETHODCALLTYPE DrawInstancedIndirect( 
		_In_  ID3D11Buffer *pBufferForArgs,
		_In_  UINT AlignedByteOffsetForArgs) = 0;
	
	virtual void STDMETHODCALLTYPE Dispatch( 
		_In_  UINT ThreadGroupCountX,
		_In_  UINT ThreadGroupCountY,
		_In_  UINT ThreadGroupCountZ) = 0;
	
	virtual void STDMETHODCALLTYPE DispatchIndirect( 
		_In_  ID3D11Buffer *pBufferForArgs,
		_In_  UINT AlignedByteOffsetForArgs) = 0;
	
	virtual void STDMETHODCALLTYPE RSSetState( 
		_In_opt_  ID3D11RasterizerState *pRasterizerState) = 0;
	
	virtual void STDMETHODCALLTYPE RSSetViewports( 
		_In_range_(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE)  UINT NumViewports,
		_In_reads_opt_(NumViewports)  const D3D11_VIEWPORT *pViewports) = 0;
	
	virtual void STDMETHODCALLTYPE RSSetScissorRects( 
		_In_range_(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE)  UINT NumRects,
		_In_reads_opt_(NumRects)  const D3D11_RECT *pRects) = 0;
	
	virtual void STDMETHODCALLTYPE CopySubresourceRegion( 
		_In_  ID3D11Resource *pDstResource,
		_In_  UINT DstSubresource,
		_In_  UINT DstX,
		_In_  UINT DstY,
		_In_  UINT DstZ,
		_In_  ID3D11Resource *pSrcResource,
		_In_  UINT SrcSubresource,
		_In_opt_  const D3D11_BOX *pSrcBox) = 0;
	
	virtual void STDMETHODCALLTYPE CopyResource( 
		_In_  ID3D11Resource *pDstResource,
		_In_  ID3D11Resource *pSrcResource) = 0;
	
	virtual void STDMETHODCALLTYPE UpdateSubresource( 
		_In_  ID3D11Resource *pDstResource,
		_In_  UINT DstSubresource,
		_In_opt_  const D3D11_BOX *pDstBox,
		_In_  const void *pSrcData,
		_In_  UINT SrcRowPitch,
		_In_  UINT SrcDepthPitch) = 0;
	
	virtual void STDMETHODCALLTYPE CopyStructureCount( 
		_In_  ID3D11Buffer *pDstBuffer,
		_In_  UINT DstAlignedByteOffset,
		_In_  ID3D11UnorderedAccessView *pSrcView) = 0;
	
	virtual void STDMETHODCALLTYPE ClearRenderTargetView( 
		_In_  ID3D11RenderTargetView *pRenderTargetView,
		_In_  const FLOAT ColorRGBA[ 4 ]) = 0;
	
	virtual void STDMETHODCALLTYPE ClearUnorderedAccessViewUint( 
		_In_  ID3D11UnorderedAccessView *pUnorderedAccessView,
		_In_  const UINT Values[ 4 ]) = 0;
	
	virtual void STDMETHODCALLTYPE ClearUnorderedAccessViewFloat( 
		_In_  ID3D11UnorderedAccessView *pUnorderedAccessView,
		_In_  const FLOAT Values[ 4 ]) = 0;
	
	virtual void STDMETHODCALLTYPE ClearDepthStencilView( 
		_In_  ID3D11DepthStencilView *pDepthStencilView,
		_In_  UINT ClearFlags,
		_In_  FLOAT Depth,
		_In_  UINT8 Stencil) = 0;
	
	virtual void STDMETHODCALLTYPE GenerateMips( 
		_In_  ID3D11ShaderResourceView *pShaderResourceView) = 0;
	
	virtual void STDMETHODCALLTYPE SetResourceMinLOD( 
		_In_  ID3D11Resource *pResource,
		FLOAT MinLOD) = 0;
	
	virtual FLOAT STDMETHODCALLTYPE GetResourceMinLOD( 
		_In_  ID3D11Resource *pResource) = 0;
	
	virtual void STDMETHODCALLTYPE ResolveSubresource( 
		_In_  ID3D11Resource *pDstResource,
		_In_  UINT DstSubresource,
		_In_  ID3D11Resource *pSrcResource,
		_In_  UINT SrcSubresource,
		_In_  DXGI_FORMAT Format) = 0;
	
	virtual void STDMETHODCALLTYPE ExecuteCommandList( 
		_In_  ID3D11CommandList *pCommandList,
		BOOL RestoreContextState) = 0;
	
	virtual void STDMETHODCALLTYPE HSSetShaderResources( 
		_In_range_( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		_In_reads_opt_(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews) = 0;
	
	virtual void STDMETHODCALLTYPE HSSetShader( 
		_In_opt_  ID3D11HullShader *pHullShader,
		_In_reads_opt_(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
		UINT NumClassInstances) = 0;
	
	virtual void STDMETHODCALLTYPE HSSetSamplers( 
		_In_range_( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		_In_reads_opt_(NumSamplers)  ID3D11SamplerState *const *ppSamplers) = 0;
	
	virtual void STDMETHODCALLTYPE HSSetConstantBuffers( 
		_In_range_( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		_In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers) = 0;
	
	virtual void STDMETHODCALLTYPE DSSetShaderResources( 
		_In_range_( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		_In_reads_opt_(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews) = 0;
	
	virtual void STDMETHODCALLTYPE DSSetShader( 
		_In_opt_  ID3D11DomainShader *pDomainShader,
		_In_reads_opt_(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
		UINT NumClassInstances) = 0;
	
	virtual void STDMETHODCALLTYPE DSSetSamplers( 
		_In_range_( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		_In_reads_opt_(NumSamplers)  ID3D11SamplerState *const *ppSamplers) = 0;
	
	virtual void STDMETHODCALLTYPE DSSetConstantBuffers( 
		_In_range_( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		_In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers) = 0;
	
	virtual void STDMETHODCALLTYPE CSSetShaderResources( 
		_In_range_( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		_In_reads_opt_(NumViews)  ID3D11ShaderResourceView *const *ppShaderResourceViews) = 0;
	
	virtual void STDMETHODCALLTYPE CSSetUnorderedAccessViews( 
		_In_range_( 0, D3D11_1_UAV_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_1_UAV_SLOT_COUNT - StartSlot )  UINT NumUAVs,
		_In_reads_opt_(NumUAVs)  ID3D11UnorderedAccessView *const *ppUnorderedAccessViews,
		_In_reads_opt_(NumUAVs)  const UINT *pUAVInitialCounts) = 0;
	
	virtual void STDMETHODCALLTYPE CSSetShader( 
		_In_opt_  ID3D11ComputeShader *pComputeShader,
		_In_reads_opt_(NumClassInstances)  ID3D11ClassInstance *const *ppClassInstances,
		UINT NumClassInstances) = 0;
	
	virtual void STDMETHODCALLTYPE CSSetSamplers( 
		_In_range_( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		_In_reads_opt_(NumSamplers)  ID3D11SamplerState *const *ppSamplers) = 0;
	
	virtual void STDMETHODCALLTYPE CSSetConstantBuffers( 
		_In_range_( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		_In_reads_opt_(NumBuffers)  ID3D11Buffer *const *ppConstantBuffers) = 0;
	
	virtual void STDMETHODCALLTYPE VSGetConstantBuffers( 
		_In_range_( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		_Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers) = 0;
	
	virtual void STDMETHODCALLTYPE PSGetShaderResources( 
		_In_range_( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		_Out_writes_opt_(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews) = 0;
	
	virtual void STDMETHODCALLTYPE PSGetShader( 
		_Outptr_result_maybenull_  ID3D11PixelShader **ppPixelShader,
		_Out_writes_opt_(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
		_Inout_opt_  UINT *pNumClassInstances) = 0;
	
	virtual void STDMETHODCALLTYPE PSGetSamplers( 
		_In_range_( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		_Out_writes_opt_(NumSamplers)  ID3D11SamplerState **ppSamplers) = 0;
	
	virtual void STDMETHODCALLTYPE VSGetShader( 
		_Outptr_result_maybenull_  ID3D11VertexShader **ppVertexShader,
		_Out_writes_opt_(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
		_Inout_opt_  UINT *pNumClassInstances) = 0;
	
	virtual void STDMETHODCALLTYPE PSGetConstantBuffers( 
		_In_range_( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		_Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers) = 0;
	
	virtual void STDMETHODCALLTYPE IAGetInputLayout( 
		_Outptr_result_maybenull_  ID3D11InputLayout **ppInputLayout) = 0;
	
	virtual void STDMETHODCALLTYPE IAGetVertexBuffers( 
		_In_range_( 0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		_Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppVertexBuffers,
		_Out_writes_opt_(NumBuffers)  UINT *pStrides,
		_Out_writes_opt_(NumBuffers)  UINT *pOffsets) = 0;
	
	virtual void STDMETHODCALLTYPE IAGetIndexBuffer( 
		_Outptr_opt_result_maybenull_  ID3D11Buffer **pIndexBuffer,
		_Out_opt_  DXGI_FORMAT *Format,
		_Out_opt_  UINT *Offset) = 0;
	
	virtual void STDMETHODCALLTYPE GSGetConstantBuffers( 
		_In_range_( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		_Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers) = 0;
	
	virtual void STDMETHODCALLTYPE GSGetShader( 
		_Outptr_result_maybenull_  ID3D11GeometryShader **ppGeometryShader,
		_Out_writes_opt_(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
		_Inout_opt_  UINT *pNumClassInstances) = 0;
	
	virtual void STDMETHODCALLTYPE IAGetPrimitiveTopology( 
		_Out_  D3D11_PRIMITIVE_TOPOLOGY *pTopology) = 0;
	
	virtual void STDMETHODCALLTYPE VSGetShaderResources( 
		_In_range_( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		_Out_writes_opt_(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews) = 0;
	
	virtual void STDMETHODCALLTYPE VSGetSamplers( 
		_In_range_( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		_Out_writes_opt_(NumSamplers)  ID3D11SamplerState **ppSamplers) = 0;
	
	virtual void STDMETHODCALLTYPE GetPredication( 
		_Outptr_opt_result_maybenull_  ID3D11Predicate **ppPredicate,
		_Out_opt_  BOOL *pPredicateValue) = 0;
	
	virtual void STDMETHODCALLTYPE GSGetShaderResources( 
		_In_range_( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		_Out_writes_opt_(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews) = 0;
	
	virtual void STDMETHODCALLTYPE GSGetSamplers( 
		_In_range_( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		_Out_writes_opt_(NumSamplers)  ID3D11SamplerState **ppSamplers) = 0;
	
	virtual void STDMETHODCALLTYPE OMGetRenderTargets( 
		_In_range_( 0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT )  UINT NumViews,
		_Out_writes_opt_(NumViews)  ID3D11RenderTargetView **ppRenderTargetViews,
		_Outptr_opt_result_maybenull_  ID3D11DepthStencilView **ppDepthStencilView) = 0;
	
	virtual void STDMETHODCALLTYPE OMGetRenderTargetsAndUnorderedAccessViews( 
		_In_range_( 0, D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT )  UINT NumRTVs,
		_Out_writes_opt_(NumRTVs)  ID3D11RenderTargetView **ppRenderTargetViews,
		_Outptr_opt_result_maybenull_  ID3D11DepthStencilView **ppDepthStencilView,
		_In_range_( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - 1 )  UINT UAVStartSlot,
		_In_range_( 0, D3D11_PS_CS_UAV_REGISTER_COUNT - UAVStartSlot )  UINT NumUAVs,
		_Out_writes_opt_(NumUAVs)  ID3D11UnorderedAccessView **ppUnorderedAccessViews) = 0;
	
	virtual void STDMETHODCALLTYPE OMGetBlendState( 
		_Outptr_opt_result_maybenull_  ID3D11BlendState **ppBlendState,
		_Out_opt_  FLOAT BlendFactor[ 4 ],
		_Out_opt_  UINT *pSampleMask) = 0;
	
	virtual void STDMETHODCALLTYPE OMGetDepthStencilState( 
		_Outptr_opt_result_maybenull_  ID3D11DepthStencilState **ppDepthStencilState,
		_Out_opt_  UINT *pStencilRef) = 0;
	
	virtual void STDMETHODCALLTYPE SOGetTargets( 
		_In_range_( 0, D3D11_SO_BUFFER_SLOT_COUNT )  UINT NumBuffers,
		_Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppSOTargets) = 0;
	
	virtual void STDMETHODCALLTYPE RSGetState( 
		_Outptr_result_maybenull_  ID3D11RasterizerState **ppRasterizerState) = 0;
	
	virtual void STDMETHODCALLTYPE RSGetViewports( 
		_Inout_ /*_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE )*/   UINT *pNumViewports,
		_Out_writes_opt_(*pNumViewports)  D3D11_VIEWPORT *pViewports) = 0;
	
	virtual void STDMETHODCALLTYPE RSGetScissorRects( 
		_Inout_ /*_range(0, D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE )*/   UINT *pNumRects,
		_Out_writes_opt_(*pNumRects)  D3D11_RECT *pRects) = 0;
	
	virtual void STDMETHODCALLTYPE HSGetShaderResources( 
		_In_range_( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		_Out_writes_opt_(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews) = 0;
	
	virtual void STDMETHODCALLTYPE HSGetShader( 
		_Outptr_result_maybenull_  ID3D11HullShader **ppHullShader,
		_Out_writes_opt_(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
		_Inout_opt_  UINT *pNumClassInstances) = 0;
	
	virtual void STDMETHODCALLTYPE HSGetSamplers( 
		_In_range_( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		_Out_writes_opt_(NumSamplers)  ID3D11SamplerState **ppSamplers) = 0;
	
	virtual void STDMETHODCALLTYPE HSGetConstantBuffers( 
		_In_range_( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		_Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers) = 0;
	
	virtual void STDMETHODCALLTYPE DSGetShaderResources( 
		_In_range_( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		_Out_writes_opt_(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews) = 0;
	
	virtual void STDMETHODCALLTYPE DSGetShader( 
		_Outptr_result_maybenull_  ID3D11DomainShader **ppDomainShader,
		_Out_writes_opt_(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
		_Inout_opt_  UINT *pNumClassInstances) = 0;
	
	virtual void STDMETHODCALLTYPE DSGetSamplers( 
		_In_range_( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		_Out_writes_opt_(NumSamplers)  ID3D11SamplerState **ppSamplers) = 0;
	
	virtual void STDMETHODCALLTYPE DSGetConstantBuffers( 
		_In_range_( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		_Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers) = 0;
	
	virtual void STDMETHODCALLTYPE CSGetShaderResources( 
		_In_range_( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT - StartSlot )  UINT NumViews,
		_Out_writes_opt_(NumViews)  ID3D11ShaderResourceView **ppShaderResourceViews) = 0;
	
	virtual void STDMETHODCALLTYPE CSGetUnorderedAccessViews( 
		_In_range_( 0, D3D11_1_UAV_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_1_UAV_SLOT_COUNT - StartSlot )  UINT NumUAVs,
		_Out_writes_opt_(NumUAVs)  ID3D11UnorderedAccessView **ppUnorderedAccessViews) = 0;
	
	virtual void STDMETHODCALLTYPE CSGetShader( 
		_Outptr_result_maybenull_  ID3D11ComputeShader **ppComputeShader,
		_Out_writes_opt_(*pNumClassInstances)  ID3D11ClassInstance **ppClassInstances,
		_Inout_opt_  UINT *pNumClassInstances) = 0;
	
	virtual void STDMETHODCALLTYPE CSGetSamplers( 
		_In_range_( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT - StartSlot )  UINT NumSamplers,
		_Out_writes_opt_(NumSamplers)  ID3D11SamplerState **ppSamplers) = 0;
	
	virtual void STDMETHODCALLTYPE CSGetConstantBuffers( 
		_In_range_( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - 1 )  UINT StartSlot,
		_In_range_( 0, D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT - StartSlot )  UINT NumBuffers,
		_Out_writes_opt_(NumBuffers)  ID3D11Buffer **ppConstantBuffers) = 0;
	
	virtual void STDMETHODCALLTYPE ClearState( void) = 0;
	
	virtual void STDMETHODCALLTYPE Flush( void) = 0;
	
	virtual D3D11_DEVICE_CONTEXT_TYPE STDMETHODCALLTYPE GetType( void) = 0;
	
	virtual UINT STDMETHODCALLTYPE GetContextFlags( void) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE FinishCommandList( 
		BOOL RestoreDeferredContextState,
		_COM_Outptr_opt_  ID3D11CommandList **ppCommandList) = 0;
	
};

EXTERN_C const IID IID_ID3D11Asynchronous;    
MIDL_INTERFACE("4b35d0cd-1e15-4258-9c98-1b1333f6dd3b")
ID3D11Asynchronous : public ID3D11DeviceChild{
public:
	virtual UINT STDMETHODCALLTYPE GetDataSize( void) = 0;
};

EXTERN_C const IID IID_ID3D11Counter;
MIDL_INTERFACE("6e8c49fb-a371-4770-b440-29086022b741")
ID3D11Counter : public ID3D11Asynchronous{
public:
	virtual void STDMETHODCALLTYPE GetDesc( 
		_Out_  D3D11_COUNTER_DESC *pDesc) = 0;
};
	
EXTERN_C const IID IID_ID3D11Query;    
MIDL_INTERFACE("d6c00747-87b7-425e-b84d-44d108560afd")
ID3D11Query : public ID3D11Asynchronous{
public:
	virtual void STDMETHODCALLTYPE GetDesc( 
		_Out_  D3D11_QUERY_DESC *pDesc) = 0;
};

EXTERN_C const IID IID_ID3D11Predicate;    
MIDL_INTERFACE("9eb576dd-9f77-4d86-81aa-8bab5fe490e2")
ID3D11Predicate : public ID3D11Query{
public:
};

EXTERN_C const IID IID_ID3D11GeometryShader;
MIDL_INTERFACE("38325b96-effb-4022-ba02-2e795b70275c")
ID3D11GeometryShader : public ID3D11DeviceChild{
public:
};
	
EXTERN_C const IID IID_ID3D11InputLayout;
MIDL_INTERFACE("e4819ddc-4cf0-4025-bd26-5de82a3e07b7")
ID3D11InputLayout : public ID3D11DeviceChild{
public:
};

EXTERN_C const IID IID_ID3D11RasterizerState;
MIDL_INTERFACE("9bb4ab81-ab1a-4d8f-b506-fc04200b6ee7")
ID3D11RasterizerState : public ID3D11DeviceChild{
public:
	virtual void STDMETHODCALLTYPE GetDesc( 
		_Out_  D3D11_RASTERIZER_DESC *pDesc) = 0;
};

EXTERN_C const IID IID_ID3D11HullShader;
MIDL_INTERFACE("8e5c6061-628a-4c8e-8264-bbe45cb3d5dd")
ID3D11HullShader : public ID3D11DeviceChild{
public:
};

EXTERN_C const IID IID_ID3D11PixelShader;
MIDL_INTERFACE("ea82e40d-51dc-4f33-93d4-db7c9125ae8c")
ID3D11PixelShader : public ID3D11DeviceChild{
public:
};

EXTERN_C const IID IID_ID3D11VertexShader;
MIDL_INTERFACE("3b301d64-d678-4289-8897-22f8928b72f3")
ID3D11VertexShader : public ID3D11DeviceChild{
public:
};

EXTERN_C const IID IID_ID3D11ComputeShader;
MIDL_INTERFACE("4f5b196e-c2bd-495e-bd01-1fded38e4969")
ID3D11ComputeShader : public ID3D11DeviceChild{
public:
};

EXTERN_C const IID IID_ID3D11DomainShader;
MIDL_INTERFACE("f582c508-0f36-490c-9977-31eece268cfa")
ID3D11DomainShader : public ID3D11DeviceChild{
public:
};

EXTERN_C const IID IID_ID3D11View;
MIDL_INTERFACE("839d1216-bb2e-412b-b7f4-a9dbebe08ed1")
ID3D11View : public ID3D11DeviceChild{
public:
	virtual void STDMETHODCALLTYPE GetResource( 
		_Outptr_  ID3D11Resource **ppResource) = 0;
};

EXTERN_C const IID IID_ID3D11UnorderedAccessView;
MIDL_INTERFACE("28acf509-7f5c-48f6-8611-f316010a6380")
ID3D11UnorderedAccessView : public ID3D11View{
public:
	virtual void STDMETHODCALLTYPE GetDesc( 
		_Out_  D3D11_UNORDERED_ACCESS_VIEW_DESC *pDesc) = 0;
};
	
EXTERN_C const IID IID_ID3D11ShaderResourceView;
MIDL_INTERFACE("b0e06fe0-8192-4e1a-b1ca-36d7414710b2")
ID3D11ShaderResourceView : public ID3D11View{
public:
	virtual void STDMETHODCALLTYPE GetDesc( 
		_Out_  D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc) = 0;
};
	
EXTERN_C const IID IID_ID3D11Resource;
MIDL_INTERFACE("dc8e63f3-d12b-4952-b47b-5e45026a862d")
ID3D11Resource : public ID3D11DeviceChild{
public:
	virtual void STDMETHODCALLTYPE GetType( 
		_Out_  D3D11_RESOURCE_DIMENSION *pResourceDimension) = 0;
	virtual void STDMETHODCALLTYPE SetEvictionPriority( 
		_In_  UINT EvictionPriority) = 0;
	virtual UINT STDMETHODCALLTYPE GetEvictionPriority( void) = 0;
};

EXTERN_C const IID IID_ID3D11Buffer;
MIDL_INTERFACE("48570b85-d1ee-4fcd-a250-eb350722b037")
ID3D11Buffer : public ID3D11Resource{
public:
	virtual void STDMETHODCALLTYPE GetDesc( 
		_Out_  D3D11_BUFFER_DESC *pDesc) = 0;
};
	
EXTERN_C const IID IID_ID3D11Texture1D;
MIDL_INTERFACE("f8fb5c27-c6b3-4f75-a4c8-439af2ef564c")
ID3D11Texture1D : public ID3D11Resource{
public:
	virtual void STDMETHODCALLTYPE GetDesc( 
		_Out_  D3D11_TEXTURE1D_DESC *pDesc) = 0;	
};
	
EXTERN_C const IID IID_ID3D11Texture2D;
MIDL_INTERFACE("6f15aaf2-d208-4e89-9ab4-489535d34f9c")
ID3D11Texture2D : public ID3D11Resource{
public:
	virtual void STDMETHODCALLTYPE GetDesc( 
		_Out_  D3D11_TEXTURE2D_DESC *pDesc) = 0;
};

EXTERN_C const IID IID_ID3D11Texture3D;
MIDL_INTERFACE("037e866e-f56d-4357-a8af-9dabbe6e250e")
ID3D11Texture3D : public ID3D11Resource{
public:
	virtual void STDMETHODCALLTYPE GetDesc( 
		_Out_  D3D11_TEXTURE3D_DESC *pDesc) = 0;
};

EXTERN_C const IID IID_ID3D11RenderTargetView;    
MIDL_INTERFACE("dfdba067-0b8d-4865-875b-d7b4516cc164")
ID3D11RenderTargetView : public ID3D11View{
public:
	virtual void STDMETHODCALLTYPE GetDesc( 
		_Out_  D3D11_RENDER_TARGET_VIEW_DESC *pDesc) = 0;
};

EXTERN_C const IID IID_ID3D11DepthStencilView;
MIDL_INTERFACE("9fdac92a-1876-48c3-afad-25b94f84a9b6")
ID3D11DepthStencilView : public ID3D11View{
public:
	virtual void STDMETHODCALLTYPE GetDesc( 
		_Out_  D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc) = 0;
};

EXTERN_C const IID IID_ID3D11ClassLinkage;
MIDL_INTERFACE("ddf57cba-9543-46e4-a12b-f207a0fe7fed")
ID3D11ClassLinkage : public ID3D11DeviceChild{
public:
	virtual HRESULT STDMETHODCALLTYPE GetClassInstance( 
		_In_  LPCSTR pClassInstanceName,
		_In_  UINT InstanceIndex,
		_COM_Outptr_  ID3D11ClassInstance **ppInstance) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CreateClassInstance(
		_In_  LPCSTR pClassTypeName,
		_In_  UINT ConstantBufferOffset,
		_In_  UINT ConstantVectorOffset,
		_In_  UINT TextureOffset,
		_In_  UINT SamplerOffset,
		_COM_Outptr_  ID3D11ClassInstance **ppInstance) = 0;
};
	
EXTERN_C const IID IID_ID3D11BlendState; 
MIDL_INTERFACE("75b68faa-347d-4159-8f45-a0640f01cd9a")
ID3D11BlendState : public ID3D11DeviceChild{
public:
	virtual void STDMETHODCALLTYPE GetDesc( 
		_Out_  D3D11_BLEND_DESC *pDesc) = 0;
};

EXTERN_C const IID IID_ID3D11DepthStencilState;
MIDL_INTERFACE("03823efb-8d8f-4e1c-9aa2-f64bb2cbfdf1")
ID3D11DepthStencilState : public ID3D11DeviceChild{
public:
	virtual void STDMETHODCALLTYPE GetDesc( 
		_Out_  D3D11_DEPTH_STENCIL_DESC *pDesc) = 0;
};

EXTERN_C const IID IID_ID3D11SamplerState;    
MIDL_INTERFACE("da6fea51-564c-4487-9810-f0d0f9b4e3a5")
ID3D11SamplerState : public ID3D11DeviceChild{
public:
	virtual void STDMETHODCALLTYPE GetDesc( 
		_Out_  D3D11_SAMPLER_DESC *pDesc) = 0;
};

EXTERN_C const IID IID_ID3D11Device;
MIDL_INTERFACE("db6f6ddb-ac77-4e88-8253-819df9bbf140")
ID3D11Device : public IUnknown{
public:
	virtual HRESULT STDMETHODCALLTYPE CreateBuffer(  
		_In_  const D3D11_BUFFER_DESC *pDesc,
		_In_opt_  const D3D11_SUBRESOURCE_DATA *pInitialData,
		_COM_Outptr_opt_  ID3D11Buffer **ppBuffer) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CreateTexture1D( 
		_In_  const D3D11_TEXTURE1D_DESC *pDesc,
		_In_reads_opt_(_Inexpressible_(pDesc->MipLevels * pDesc->ArraySize))  const D3D11_SUBRESOURCE_DATA *pInitialData,
		_COM_Outptr_opt_  ID3D11Texture1D **ppTexture1D) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CreateTexture2D( 
		_In_  const D3D11_TEXTURE2D_DESC *pDesc,
		_In_reads_opt_(_Inexpressible_(pDesc->MipLevels * pDesc->ArraySize))  const D3D11_SUBRESOURCE_DATA *pInitialData,
		_COM_Outptr_opt_  ID3D11Texture2D **ppTexture2D) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CreateTexture3D( 
		_In_  const D3D11_TEXTURE3D_DESC *pDesc,
		_In_reads_opt_(_Inexpressible_(pDesc->MipLevels))  const D3D11_SUBRESOURCE_DATA *pInitialData,
		_COM_Outptr_opt_  ID3D11Texture3D **ppTexture3D) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CreateShaderResourceView( 
		_In_  ID3D11Resource *pResource,
		_In_opt_  const D3D11_SHADER_RESOURCE_VIEW_DESC *pDesc,
		_COM_Outptr_opt_  ID3D11ShaderResourceView **ppSRView) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CreateUnorderedAccessView( 
		_In_  ID3D11Resource *pResource,
		_In_opt_  const D3D11_UNORDERED_ACCESS_VIEW_DESC *pDesc,
		_COM_Outptr_opt_  ID3D11UnorderedAccessView **ppUAView) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CreateRenderTargetView( 
		_In_  ID3D11Resource *pResource,
		_In_opt_  const D3D11_RENDER_TARGET_VIEW_DESC *pDesc,
		_COM_Outptr_opt_  ID3D11RenderTargetView **ppRTView) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CreateDepthStencilView( 
		_In_  ID3D11Resource *pResource,
		_In_opt_  const D3D11_DEPTH_STENCIL_VIEW_DESC *pDesc,
		_COM_Outptr_opt_  ID3D11DepthStencilView **ppDepthStencilView) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CreateInputLayout( 
		_In_reads_(NumElements)  const D3D11_INPUT_ELEMENT_DESC *pInputElementDescs,
		_In_range_( 0, D3D11_IA_VERTEX_INPUT_STRUCTURE_ELEMENT_COUNT )  UINT NumElements,
		_In_reads_(BytecodeLength)  const void *pShaderBytecodeWithInputSignature,
		_In_  SIZE_T BytecodeLength,
		_COM_Outptr_opt_  ID3D11InputLayout **ppInputLayout) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CreateVertexShader( 
		_In_reads_(BytecodeLength)  const void *pShaderBytecode,
		_In_  SIZE_T BytecodeLength,
		_In_opt_  ID3D11ClassLinkage *pClassLinkage,
		_COM_Outptr_opt_  ID3D11VertexShader **ppVertexShader) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CreateGeometryShader( 
		_In_reads_(BytecodeLength)  const void *pShaderBytecode,
		_In_  SIZE_T BytecodeLength,
		_In_opt_  ID3D11ClassLinkage *pClassLinkage,
		_COM_Outptr_opt_  ID3D11GeometryShader **ppGeometryShader) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CreateGeometryShaderWithStreamOutput( 
		_In_reads_(BytecodeLength)  const void *pShaderBytecode,
		_In_  SIZE_T BytecodeLength,
		_In_reads_opt_(NumEntries)  const D3D11_SO_DECLARATION_ENTRY *pSODeclaration,
		_In_range_( 0, D3D11_SO_STREAM_COUNT * D3D11_SO_OUTPUT_COMPONENT_COUNT )  UINT NumEntries,
		_In_reads_opt_(NumStrides)  const UINT *pBufferStrides,
		_In_range_( 0, D3D11_SO_BUFFER_SLOT_COUNT )  UINT NumStrides,
		_In_  UINT RasterizedStream,
		_In_opt_  ID3D11ClassLinkage *pClassLinkage,
		_COM_Outptr_opt_  ID3D11GeometryShader **ppGeometryShader) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CreatePixelShader( 
		_In_reads_(BytecodeLength)  const void *pShaderBytecode,
		_In_  SIZE_T BytecodeLength,
		_In_opt_  ID3D11ClassLinkage *pClassLinkage,
		_COM_Outptr_opt_  ID3D11PixelShader **ppPixelShader) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CreateHullShader( 
		_In_reads_(BytecodeLength)  const void *pShaderBytecode,
		_In_  SIZE_T BytecodeLength,
		_In_opt_  ID3D11ClassLinkage *pClassLinkage,
		_COM_Outptr_opt_  ID3D11HullShader **ppHullShader) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CreateDomainShader( 
		_In_reads_(BytecodeLength)  const void *pShaderBytecode,
		_In_  SIZE_T BytecodeLength,
		_In_opt_  ID3D11ClassLinkage *pClassLinkage,
		_COM_Outptr_opt_  ID3D11DomainShader **ppDomainShader) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CreateComputeShader( 
		_In_reads_(BytecodeLength)  const void *pShaderBytecode,
		_In_  SIZE_T BytecodeLength,
		_In_opt_  ID3D11ClassLinkage *pClassLinkage,
		_COM_Outptr_opt_  ID3D11ComputeShader **ppComputeShader) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CreateClassLinkage( 
		_COM_Outptr_  ID3D11ClassLinkage **ppLinkage) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CreateBlendState( 
		_In_  const D3D11_BLEND_DESC *pBlendStateDesc,
		_COM_Outptr_opt_  ID3D11BlendState **ppBlendState) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CreateDepthStencilState( 
		_In_  const D3D11_DEPTH_STENCIL_DESC *pDepthStencilDesc,
		_COM_Outptr_opt_  ID3D11DepthStencilState **ppDepthStencilState) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CreateRasterizerState( 
		_In_  const D3D11_RASTERIZER_DESC *pRasterizerDesc,
		_COM_Outptr_opt_  ID3D11RasterizerState **ppRasterizerState) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CreateSamplerState( 
		_In_  const D3D11_SAMPLER_DESC *pSamplerDesc,
		_COM_Outptr_opt_  ID3D11SamplerState **ppSamplerState) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CreateQuery( 
		_In_  const D3D11_QUERY_DESC *pQueryDesc,
		_COM_Outptr_opt_  ID3D11Query **ppQuery) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CreatePredicate( 
		_In_  const D3D11_QUERY_DESC *pPredicateDesc,
		_COM_Outptr_opt_  ID3D11Predicate **ppPredicate) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CreateCounter( 
		_In_  const D3D11_COUNTER_DESC *pCounterDesc,
		_COM_Outptr_opt_  ID3D11Counter **ppCounter) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CreateDeferredContext( 
		UINT ContextFlags,
		_COM_Outptr_opt_  ID3D11DeviceContext **ppDeferredContext) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE OpenSharedResource( 
		_In_  HANDLE hResource,
		_In_  REFIID ReturnedInterface,
		_COM_Outptr_opt_  void **ppResource) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CheckFormatSupport( 
		_In_  DXGI_FORMAT Format,
		_Out_  UINT *pFormatSupport) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CheckMultisampleQualityLevels( 
		_In_  DXGI_FORMAT Format,
		_In_  UINT SampleCount,
		_Out_  UINT *pNumQualityLevels) = 0;
	
	virtual void STDMETHODCALLTYPE CheckCounterInfo( 
		_Out_  D3D11_COUNTER_INFO *pCounterInfo) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CheckCounter( 
		_In_  const D3D11_COUNTER_DESC *pDesc,
		_Out_  D3D11_COUNTER_TYPE *pType,
		_Out_  UINT *pActiveCounters,
		_Out_writes_opt_(*pNameLength)  LPSTR szName,
		_Inout_opt_  UINT *pNameLength,
		_Out_writes_opt_(*pUnitsLength)  LPSTR szUnits,
		_Inout_opt_  UINT *pUnitsLength,
		_Out_writes_opt_(*pDescriptionLength)  LPSTR szDescription,
		_Inout_opt_  UINT *pDescriptionLength) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CheckFeatureSupport( 
		D3D11_FEATURE Feature,
		_Out_writes_bytes_(FeatureSupportDataSize)  void *pFeatureSupportData,
		UINT FeatureSupportDataSize) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetPrivateData( 
		_In_  REFGUID guid,
		_Inout_  UINT *pDataSize,
		_Out_writes_bytes_opt_(*pDataSize)  void *pData) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE SetPrivateData( 
		_In_  REFGUID guid,
		_In_  UINT DataSize,
		_In_reads_bytes_opt_(DataSize)  const void *pData) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface( 
		_In_  REFGUID guid,
		_In_opt_  const IUnknown *pData) = 0;
	
	virtual D3D_FEATURE_LEVEL STDMETHODCALLTYPE GetFeatureLevel( void) = 0;
	
	virtual UINT STDMETHODCALLTYPE GetCreationFlags( void) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetDeviceRemovedReason( void) = 0;
	
	virtual void STDMETHODCALLTYPE GetImmediateContext( 
		_Outptr_  ID3D11DeviceContext **ppImmediateContext) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE SetExceptionMode( 
		UINT RaiseFlags) = 0;
	
	virtual UINT STDMETHODCALLTYPE GetExceptionMode( void) = 0;
	
};

#endif