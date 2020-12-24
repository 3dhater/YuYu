#ifndef __dxgi_h__
#define __dxgi_h__

#ifndef _In_
#define _In_
#endif

#ifndef _in_
#define _in_
#endif


#ifndef _Inout_
#define _Inout_
#endif

#ifndef _Inout_opt_
#define _Inout_opt_
#endif

#ifndef _In_opt_
#define _In_opt_
#endif

#ifndef __in_opt
#define __in_opt
#endif

#ifndef __out_opt
#define __out_opt
#endif

#ifndef __in_bcount
#define __in_bcount(x)
#endif

#ifndef __in_ecount_opt
#define __in_ecount_opt(x)
#endif

#ifndef _In_reads_
#define _In_reads_(x)
#endif

#ifndef _In_reads_opt_
#define _In_reads_opt_(x)
#endif

#ifndef _In_reads_bytes_
#define _In_reads_bytes_(x)
#endif

#ifndef _In_reads_bytes_opt_
#define _In_reads_bytes_opt_(x)
#endif

#ifndef _In_range_
#define _In_range_(x,y)
#endif

#ifndef _Out_
#define _Out_
#endif

#ifndef _Out_opt_
#define _Out_opt_
#endif

#ifndef _Outptr_
#define _Outptr_
#endif

#ifndef _Out_writes_
#define _Out_writes_(x)
#endif

#ifndef _Out_writes_opt_
#define _Out_writes_opt_(x)
#endif

#ifndef _Out_writes_to_opt_
#define _Out_writes_to_opt_(x,y)
#endif

#ifndef _Out_writes_bytes_opt_
#define _Out_writes_bytes_opt_(x)
#endif

#ifndef _Out_writes_bytes_
#define _Out_writes_bytes_(x)
#endif

#ifndef _Outptr_result_maybenull_
#define _Outptr_result_maybenull_
#endif

#ifndef _Outptr_opt_result_maybenull_
#define _Outptr_opt_result_maybenull_
#endif

#ifndef _COM_Outptr_
#define _COM_Outptr_
#endif

#ifndef _COM_Outptr_opt_
#define _COM_Outptr_opt_
#endif

#ifndef _COM_Outptr_opt_result_maybenull_
#define _COM_Outptr_opt_result_maybenull_
#endif

//typedef interface IDXGIObject IDXGIObject;
//typedef interface IDXGIDevice IDXGIDevice;
//typedef interface IDXGIAdapter IDXGIAdapter;
//typedef interface IDXGIAdapter1 IDXGIAdapter1;
//typedef interface IDXGIOutput IDXGIOutput;
//typedef interface IDXGISurface IDXGISurface;
//typedef interface IDXGIDeviceSubObject IDXGIDeviceSubObject;
//typedef interface IDXGISwapChain IDXGISwapChain;
//typedef interface IDXGIFactory IDXGIFactory;
//typedef interface IDXGIFactory1 IDXGIFactory1;

#define DXGI_CPU_ACCESS_NONE    ( 0 )
#define DXGI_CPU_ACCESS_DYNAMIC    ( 1 )
#define DXGI_CPU_ACCESS_READ_WRITE    ( 2 )
#define DXGI_CPU_ACCESS_SCRATCH    ( 3 )
#define DXGI_CPU_ACCESS_FIELD        15
#define DXGI_USAGE_SHADER_INPUT             0x00000010UL
#define DXGI_USAGE_RENDER_TARGET_OUTPUT     0x00000020UL
#define DXGI_USAGE_BACK_BUFFER              0x00000040UL
#define DXGI_USAGE_SHARED                   0x00000080UL
#define DXGI_USAGE_READ_ONLY                0x00000100UL
#define DXGI_USAGE_DISCARD_ON_PRESENT       0x00000200UL
#define DXGI_USAGE_UNORDERED_ACCESS         0x00000400UL
using DXGI_USAGE = UINT;

#define DXGI_MWA_NO_WINDOW_CHANGES      ( 1 << 0 )
#define DXGI_MWA_NO_ALT_ENTER           ( 1 << 1 )
#define DXGI_MWA_NO_PRINT_SCREEN        ( 1 << 2 )
#define DXGI_MWA_VALID                  ( 0x7 )

/*
EXTERN_C const GUID IID_IDXGIObject = {0xaec22fb8,0x76f3,0x4639,0x9b,0xe0,0x28,0xeb,0x43,0xa6,0x7a,0x2e};
EXTERN_C const GUID IID_IDXGIResource = {0x035f3ab4,0x482e,0x4e50,0xb4,0x1f,0x8a,0x7f,0x8b,0xd8,0x96,0x0b};
EXTERN_C const GUID IID_IDXGIKeyedMutex = {0x9d8e1289,0xd7b3,0x465f,0x81,0x26,0x25,0x0e,0x34,0x9a,0xf8,0x5d};
EXTERN_C const GUID IID_IDXGISurface = {0xcafcb56c,0x6ac3,0x4889,0xbf,0x47,0x9e,0x23,0xbb,0xd2,0x60,0xec};
EXTERN_C const GUID IID_IDXGISurface1 = {0x4AE63092,0x6327,0x4c1b,0x80,0xAE,0xBF,0xE1,0x2E,0xA3,0x2B,0x86};
EXTERN_C const GUID IID_IDXGIAdapter = {0x2411e7e1,0x12ac,0x4ccf,0xbd,0x14,0x97,0x98,0xe8,0x53,0x4d,0xc0};
EXTERN_C const GUID IID_IDXGIOutput = {0xae02eedb,0xc735,0x4690,0x8d,0x52,0x5a,0x8d,0xc2,0x02,0x13,0xaa};
EXTERN_C const GUID IID_IDXGISwapChain = {0x310d36a0,0xd2e7,0x4c0a,0xaa,0x04,0x6a,0x9d,0x23,0xb8,0x88,0x6a};
EXTERN_C const GUID IID_IDXGIFactory = {0x7b7166ec,0x21c7,0x44ae,0xb2,0x1a,0xc9,0xae,0x32,0x1a,0xe3,0x69};
EXTERN_C const GUID IID_IDXGIDevice = {0x54ec77fa,0x1377,0x44e6,0x8c,0x32,0x88,0xfd,0x5f,0x44,0xc8,0x4c};
EXTERN_C const GUID IID_IDXGIFactory1 = {0x770aae78,0xf26f,0x4dba,0xa8,0x29,0x25,0x3c,0x83,0xd1,0xb3,0x87};
EXTERN_C const GUID IID_IDXGIAdapter1 = {0x29038f61,0x3839,0x4626,0x91,0xfd,0x08,0x68,0x79,0x01,0x1a,0x05};
EXTERN_C const GUID IID_IDXGIDevice1 = {0x77db970f,0x6276,0x48ba,0xba,0x28,0x07,0x01,0x43,0xb4,0x39,0x2c};
*/
/*
DEFINE_GUID(IID_IDXGIObject,0xaec22fb8,0x76f3,0x4639,0x9b,0xe0,0x28,0xeb,0x43,0xa6,0x7a,0x2e);
DEFINE_GUID(IID_IDXGIDeviceSubObject,0x3d3e0379,0xf9de,0x4d58,0xbb,0x6c,0x18,0xd6,0x29,0x92,0xf1,0xa6);
DEFINE_GUID(IID_IDXGIResource,0x035f3ab4,0x482e,0x4e50,0xb4,0x1f,0x8a,0x7f,0x8b,0xd8,0x96,0x0b);
DEFINE_GUID(IID_IDXGIKeyedMutex,0x9d8e1289,0xd7b3,0x465f,0x81,0x26,0x25,0x0e,0x34,0x9a,0xf8,0x5d);
DEFINE_GUID(IID_IDXGISurface,0xcafcb56c,0x6ac3,0x4889,0xbf,0x47,0x9e,0x23,0xbb,0xd2,0x60,0xec);
DEFINE_GUID(IID_IDXGISurface1,0x4AE63092,0x6327,0x4c1b,0x80,0xAE,0xBF,0xE1,0x2E,0xA3,0x2B,0x86);
DEFINE_GUID(IID_IDXGIAdapter,0x2411e7e1,0x12ac,0x4ccf,0xbd,0x14,0x97,0x98,0xe8,0x53,0x4d,0xc0);
DEFINE_GUID(IID_IDXGIOutput,0xae02eedb,0xc735,0x4690,0x8d,0x52,0x5a,0x8d,0xc2,0x02,0x13,0xaa);
DEFINE_GUID(IID_IDXGISwapChain,0x310d36a0,0xd2e7,0x4c0a,0xaa,0x04,0x6a,0x9d,0x23,0xb8,0x88,0x6a);
DEFINE_GUID(IID_IDXGIFactory,0x7b7166ec,0x21c7,0x44ae,0xb2,0x1a,0xc9,0xae,0x32,0x1a,0xe3,0x69);
DEFINE_GUID(IID_IDXGIDevice,0x54ec77fa,0x1377,0x44e6,0x8c,0x32,0x88,0xfd,0x5f,0x44,0xc8,0x4c);
DEFINE_GUID(IID_IDXGIFactory1,0x770aae78,0xf26f,0x4dba,0xa8,0x29,0x25,0x3c,0x83,0xd1,0xb3,0x87);
DEFINE_GUID(IID_IDXGIAdapter1,0x29038f61,0x3839,0x4626,0x91,0xfd,0x08,0x68,0x79,0x01,0x1a,0x05);
DEFINE_GUID(IID_IDXGIDevice1,0x77db970f,0x6276,0x48ba,0xba,0x28,0x07,0x01,0x43,0xb4,0x39,0x2c);
*/

#include "dxgicommon.h"
#include "dxgitype.h"

enum DXGI_ADAPTER_FLAG{
	DXGI_ADAPTER_FLAG_NONE	= 0,
	DXGI_ADAPTER_FLAG_REMOTE	= 1,
	DXGI_ADAPTER_FLAG_SOFTWARE	= 2,
	DXGI_ADAPTER_FLAG_FORCE_DWORD	= 0xffffffff
};

struct DXGI_ADAPTER_DESC1{
    WCHAR Description[ 128 ];
    UINT VendorId;
    UINT DeviceId;
    UINT SubSysId;
    UINT Revision;
    SIZE_T DedicatedVideoMemory;
    SIZE_T DedicatedSystemMemory;
    SIZE_T SharedSystemMemory;
    LUID AdapterLuid;
    UINT Flags;
};

struct DXGI_SHARED_RESOURCE{
    HANDLE Handle;
};

enum DXGI_RESIDENCY{
    DXGI_RESIDENCY_FULLY_RESIDENT	= 1,
    DXGI_RESIDENCY_RESIDENT_IN_SHARED_MEMORY	= 2,
    DXGI_RESIDENCY_EVICTED_TO_DISK	= 3
};
	
	
enum DXGI_SWAP_EFFECT{
	DXGI_SWAP_EFFECT_DISCARD	= 0,
	DXGI_SWAP_EFFECT_SEQUENTIAL	= 1,
	DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL	= 3,
	DXGI_SWAP_EFFECT_FLIP_DISCARD	= 4
};

struct DXGI_ADAPTER_DESC{
    WCHAR Description[ 128 ];
    UINT VendorId;
    UINT DeviceId;
    UINT SubSysId;
    UINT Revision;
    SIZE_T DedicatedVideoMemory;
    SIZE_T DedicatedSystemMemory;
    SIZE_T SharedSystemMemory;
    LUID AdapterLuid;
};
	
struct DXGI_FRAME_STATISTICS{
    UINT PresentCount;
    UINT PresentRefreshCount;
    UINT SyncRefreshCount;
    LARGE_INTEGER SyncQPCTime;
    LARGE_INTEGER SyncGPUTime;
};

struct DXGI_MAPPED_RECT{
    INT Pitch;
    BYTE *pBits;
};
	
struct DXGI_OUTPUT_DESC{
    WCHAR DeviceName[ 32 ];
    RECT DesktopCoordinates;
    BOOL AttachedToDesktop;
    DXGI_MODE_ROTATION Rotation;
    HMONITOR Monitor;
};

struct DXGI_SURFACE_DESC{
    UINT Width;
    UINT Height;
    DXGI_FORMAT Format;
    DXGI_SAMPLE_DESC SampleDesc;
};

struct DXGI_SWAP_CHAIN_DESC{
    DXGI_MODE_DESC BufferDesc;
    DXGI_SAMPLE_DESC SampleDesc;
    DXGI_USAGE BufferUsage;
    UINT BufferCount;
    HWND OutputWindow;
    BOOL Windowed;
    DXGI_SWAP_EFFECT SwapEffect;
    UINT Flags;
};
	
EXTERN_C const IID IID_IDXGIObject;    
//MIDL_INTERFACE("aec22fb8-76f3-4639-9be0-28eb43a67a2e")
IDXGIObject : public IUnknown{
public:
	virtual HRESULT STDMETHODCALLTYPE SetPrivateData( 
		_In_  REFGUID Name,
		/* [in] */ UINT DataSize,
		_In_reads_bytes_(DataSize)  const void *pData) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE SetPrivateDataInterface( 
		_In_  REFGUID Name,
		_In_opt_  const IUnknown *pUnknown) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetPrivateData( 
		_In_  REFGUID Name,
		_Inout_  UINT *pDataSize,
		_Out_writes_bytes_(*pDataSize)  void *pData) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetParent( 
		_In_  REFIID riid,
		_COM_Outptr_  void **ppParent) = 0;
};

EXTERN_C const IID IID_IDXGIDevice;
MIDL_INTERFACE("54ec77fa-1377-44e6-8c32-88fd5f44c84c")
IDXGIDevice : public IDXGIObject{
public:
	virtual HRESULT STDMETHODCALLTYPE GetAdapter( 
		_COM_Outptr_  IDXGIAdapter **pAdapter) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CreateSurface( 
		_In_  const DXGI_SURFACE_DESC *pDesc,
		/* [in] */ UINT NumSurfaces,
		/* [in] */ DXGI_USAGE Usage,
		_In_opt_  const DXGI_SHARED_RESOURCE *pSharedResource,
		_COM_Outptr_  IDXGISurface **ppSurface) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE QueryResourceResidency( 
		_In_reads_(NumResources)  IUnknown *const *ppResources,
		_Out_writes_(NumResources)  DXGI_RESIDENCY *pResidencyStatus,
		/* [in] */ UINT NumResources) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE SetGPUThreadPriority( 
		/* [in] */ INT Priority) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetGPUThreadPriority( 
		_Out_  INT *pPriority) = 0;
};

EXTERN_C const IID IID_IDXGIFactory;    
MIDL_INTERFACE("7b7166ec-21c7-44ae-b21a-c9ae321ae369")
IDXGIFactory : public IDXGIObject{
public:
	virtual HRESULT STDMETHODCALLTYPE EnumAdapters( 
		/* [in] */ UINT Adapter,
		_COM_Outptr_  IDXGIAdapter **ppAdapter) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE MakeWindowAssociation( 
		HWND WindowHandle,
		UINT Flags) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetWindowAssociation( 
		_Out_  HWND *pWindowHandle) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CreateSwapChain( 
		_In_  IUnknown *pDevice,
		_In_  DXGI_SWAP_CHAIN_DESC *pDesc,
		_COM_Outptr_  IDXGISwapChain **ppSwapChain) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CreateSoftwareAdapter( 
		/* [in] */ HMODULE Module,
		_COM_Outptr_  IDXGIAdapter **ppAdapter) = 0;
};

EXTERN_C const IID IID_IDXGIOutput;    
MIDL_INTERFACE("ae02eedb-c735-4690-8d52-5a8dc20213aa")
IDXGIOutput : public IDXGIObject{
public:
	virtual HRESULT STDMETHODCALLTYPE GetDesc( 
		_Out_  DXGI_OUTPUT_DESC *pDesc) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetDisplayModeList( 
		/* [in] */ DXGI_FORMAT EnumFormat,
		/* [in] */ UINT Flags,
		_Inout_  UINT *pNumModes,
		_Out_writes_to_opt_(*pNumModes,*pNumModes)  DXGI_MODE_DESC *pDesc) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE FindClosestMatchingMode( 
		_In_  const DXGI_MODE_DESC *pModeToMatch,
		_Out_  DXGI_MODE_DESC *pClosestMatch,
		_In_opt_  IUnknown *pConcernedDevice) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE WaitForVBlank( void) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE TakeOwnership( 
		_In_  IUnknown *pDevice,
		BOOL Exclusive) = 0;
	
	virtual void STDMETHODCALLTYPE ReleaseOwnership( void) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetGammaControlCapabilities( 
		_Out_  DXGI_GAMMA_CONTROL_CAPABILITIES *pGammaCaps) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE SetGammaControl( 
		_In_  const DXGI_GAMMA_CONTROL *pArray) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetGammaControl( 
		_Out_  DXGI_GAMMA_CONTROL *pArray) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE SetDisplaySurface( 
		_In_  IDXGISurface *pScanoutSurface) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetDisplaySurfaceData( 
		_In_  IDXGISurface *pDestination) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetFrameStatistics( 
		_Out_  DXGI_FRAME_STATISTICS *pStats) = 0;
};

EXTERN_C const IID IID_IDXGIAdapter;    
MIDL_INTERFACE("2411e7e1-12ac-4ccf-bd14-9798e8534dc0")
IDXGIAdapter : public IDXGIObject{
public:
	virtual HRESULT STDMETHODCALLTYPE EnumOutputs( 
		/* [in] */ UINT Output,
		_COM_Outptr_  IDXGIOutput **ppOutput) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetDesc( 
		_Out_  DXGI_ADAPTER_DESC *pDesc) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE CheckInterfaceSupport( 
		_In_  REFGUID InterfaceName,
		_Out_  LARGE_INTEGER *pUMDVersion) = 0;
};

EXTERN_C const IID IID_IDXGIAdapter1;    
MIDL_INTERFACE("29038f61-3839-4626-91fd-086879011a05")
IDXGIAdapter1 : public IDXGIAdapter{
public:
	virtual HRESULT STDMETHODCALLTYPE GetDesc1( 
		_Out_  DXGI_ADAPTER_DESC1 *pDesc) = 0;
};
	
EXTERN_C const IID IID_IDXGIDeviceSubObject;    
MIDL_INTERFACE("3d3e0379-f9de-4d58-bb6c-18d62992f1a6")
IDXGIDeviceSubObject : public IDXGIObject{
public:
	virtual HRESULT STDMETHODCALLTYPE GetDevice( 
		_In_  REFIID riid,
		_COM_Outptr_  void **ppDevice) = 0;
};

EXTERN_C const IID IID_IDXGISwapChain;    
MIDL_INTERFACE("310d36a0-d2e7-4c0a-aa04-6a9d23b8886a")
IDXGISwapChain : public IDXGIDeviceSubObject{
public:
	virtual HRESULT STDMETHODCALLTYPE Present( 
		/* [in] */ UINT SyncInterval,
		/* [in] */ UINT Flags) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetBuffer( 
		/* [in] */ UINT Buffer,
		_In_  REFIID riid,
		_COM_Outptr_  void **ppSurface) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE SetFullscreenState( 
		/* [in] */ BOOL Fullscreen,
		_In_opt_  IDXGIOutput *pTarget) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetFullscreenState( 
		_Out_opt_  BOOL *pFullscreen,
		_COM_Outptr_opt_result_maybenull_  IDXGIOutput **ppTarget) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetDesc( 
		_Out_  DXGI_SWAP_CHAIN_DESC *pDesc) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE ResizeBuffers( 
		/* [in] */ UINT BufferCount,
		/* [in] */ UINT Width,
		/* [in] */ UINT Height,
		/* [in] */ DXGI_FORMAT NewFormat,
		/* [in] */ UINT SwapChainFlags) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE ResizeTarget( 
		_In_  const DXGI_MODE_DESC *pNewTargetParameters) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetContainingOutput( 
		_COM_Outptr_  IDXGIOutput **ppOutput) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetFrameStatistics( 
		_Out_  DXGI_FRAME_STATISTICS *pStats) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE GetLastPresentCount( 
		_Out_  UINT *pLastPresentCount) = 0;
};
	
EXTERN_C const IID IID_IDXGISurface;    
MIDL_INTERFACE("cafcb56c-6ac3-4889-bf47-9e23bbd260ec")
IDXGISurface : public IDXGIDeviceSubObject{
public:
	virtual HRESULT STDMETHODCALLTYPE GetDesc( 
		_Out_  DXGI_SURFACE_DESC *pDesc) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE Map( 
		_Out_  DXGI_MAPPED_RECT *pLockedRect,
		/* [in] */ UINT MapFlags) = 0;
	virtual HRESULT STDMETHODCALLTYPE Unmap( void) = 0;
};

EXTERN_C const IID IID_IDXGIFactory1;    
MIDL_INTERFACE("770aae78-f26f-4dba-a829-253c83d1b387")
IDXGIFactory1 : public IDXGIFactory{
public:
	virtual HRESULT STDMETHODCALLTYPE EnumAdapters1( 
		/* [in] */ UINT Adapter,
		_COM_Outptr_  IDXGIAdapter1 **ppAdapter) = 0;
	virtual BOOL STDMETHODCALLTYPE IsCurrent( void) = 0;
};

#endif
