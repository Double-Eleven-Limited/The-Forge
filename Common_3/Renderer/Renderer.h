/*
 * Copyright (c) 2018-2020 The Forge Interactive Inc.
 *
 * This file is part of The-Forge
 * (see https://github.com/ConfettiFX/The-Forge).
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
*/

#pragma once

#include "IRenderer.h"

#if defined(DIRECT3D11)
#include <d3d11_1.h>
#include <dxgi1_2.h>
#endif
#if defined(XBOX)
#include "../../Xbox/Common_3/Renderer/Direct3D12/Direct3D12X.h"
#elif defined(DIRECT3D12)
#include "../OS/Interfaces/IOperatingSystem.h"
#if defined(__clang__)
#pragma clang diagnostic push
//#pragma clang diagnostic ignored "-Wignored-pragma-intrinsic"
//#pragma clang diagnostic ignored "-Wexpansion-to-defined"
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma clang diagnostic ignored "-Wunused-value"
#endif
#include <d3d12.h>
#if defined(__clang__)
#pragma clang diagnostic pop
#endif
#include "../ThirdParty/OpenSource/DirectXShaderCompiler/inc/dxcapi.h"
#include <dxgi1_6.h>
#include <dxgidebug.h>
#endif

#if defined(DIRECT3D11)
#include <d3d11_1.h>
#include <dxgi1_2.h>
#endif
#if defined(DIRECT3D12)
// Raytracing
#ifdef D3D12_RAYTRACING_AABB_BYTE_ALIGNMENT
#define ENABLE_RAYTRACING
#endif

// Forward declare memory allocator classes
namespace D3D12MA
{
	class Allocator;
	class Allocation;
};
#endif
#if defined(VULKAN)
#if defined(_WIN32)
#define VK_USE_PLATFORM_WIN32_KHR
#elif defined(__ANDROID__)
#ifndef VK_USE_PLATFORM_ANDROID_KHR
#define VK_USE_PLATFORM_ANDROID_KHR
#endif
#elif defined(__linux__) && !defined(VK_USE_PLATFORM_GGP)
#define VK_USE_PLATFORM_XLIB_KHR    //Use Xlib or Xcb as display server, defaults to Xlib
#endif
#if defined(NX64)
#define VK_USE_PLATFORM_VI_NN
#include <vulkan/vulkan.h>
#include "../../Switch/Common_3/Renderer/Vulkan/NX/NXVulkanExt.h"
#else
#include "../ThirdParty/OpenSource/volk/volk.h"
#endif

// Set this define to enable renderdoc layer
// NOTE: Setting this define will disable use of the khr dedicated allocation extension since it conflicts with the renderdoc capture layer
//#define USE_RENDER_DOC

// Raytracing
#ifdef VK_NV_RAY_TRACING_SPEC_VERSION
#define ENABLE_RAYTRACING
#endif

#define CHECK_VKRESULT(exp)                                                    \
{                                                                              \
	VkResult vkres = (exp);                                                    \
	if (VK_SUCCESS != vkres)                                                   \
	{                                                                          \
		LOGF(eERROR, "%s: FAILED with VkResult: %u", #exp, (uint32_t)vkres);   \
		ASSERT(false);                                                         \
	}                                                                          \
}

#endif
#if defined(METAL)
#import <MetalKit/MetalKit.h>
#include "Metal/MetalAvailabilityMacros.h"
#endif
#if defined(ORBIS)
#include "../../PS4/Common_3/Renderer/Orbis/OrbisStructs.h"
#endif
#if defined(PROSPERO)
#include "../../Prospero/Common_3/Renderer/ProsperoStructs.h"
#endif

#include "../OS/Interfaces/IOperatingSystem.h"
#include "../OS/Interfaces/IThread.h"

// Enable graphics validation in debug builds by default.
#if defined(FORGE_DEBUG) && !defined(DISABLE_GRAPHICS_DEBUG)
#define ENABLE_GRAPHICS_DEBUG
#endif

typedef struct QueryPool
{
#if defined(DIRECT3D12)
	ID3D12QueryHeap* pDxQueryHeap;
	D3D12_QUERY_TYPE  mType;
	uint32_t          mCount;
#endif
#if defined(VULKAN)
	VkQueryPool       pVkQueryPool;
	VkQueryType       mType;
	uint32_t          mCount;
#endif
#if defined(DIRECT3D11)
	ID3D11Query** ppDxQueries;
	D3D11_QUERY       mType;
	uint32_t          mCount;
#endif
#if defined(METAL)
	double            mGpuTimestampStart;
	double            mGpuTimestampEnd;
	uint32_t          mCount;
#endif
#if defined(ORBIS)
	OrbisQueryPool    mStruct;
	uint32_t          mType;
	uint32_t          mCount;
#endif
#if defined(PROSPERO)
	ProsperoQueryPool mStruct;
	uint32_t          mType;
	uint32_t          mCount;
#endif
} QueryPool;

typedef struct DEFINE_ALIGNED(Buffer, 64)
{
	/// CPU address of the mapped buffer (appliacable to buffers created in CPU accessible heaps (CPU, CPU_TO_GPU, GPU_TO_CPU)
	void* pCpuMappedAddress;
#if defined(DIRECT3D12)
	/// GPU Address - Cache to avoid calls to ID3D12Resource::GetGpuVirtualAddress
	D3D12_GPU_VIRTUAL_ADDRESS        mDxGpuAddress;
	/// Descriptor handle of the CBV in a CPU visible descriptor heap (applicable to BUFFER_USAGE_UNIFORM)
	D3D12_CPU_DESCRIPTOR_HANDLE      mDxDescriptorHandles;
	/// Offset from mDxDescriptors for srv descriptor handle
	uint64_t                         mDxSrvOffset : 8;
	/// Offset from mDxDescriptors for uav descriptor handle
	uint64_t                         mDxUavOffset : 8;
	/// Native handle of the underlying resource
	ID3D12Resource* pDxResource;
	/// Contains resource allocation info such as parent heap, offset in heap
	D3D12MA::Allocation* pDxAllocation;
#endif
#if defined(DIRECT3D11)
	ID3D11Buffer* pDxResource;
	ID3D11ShaderResourceView* pDxSrvHandle;
	ID3D11UnorderedAccessView* pDxUavHandle;
	uint64_t                         mPadA;
	uint64_t                         mPadB;
#endif
#if defined(VULKAN)
	/// Native handle of the underlying resource
	VkBuffer                         pVkBuffer;
	/// Buffer view
	VkBufferView                     pVkStorageTexelView;
	VkBufferView                     pVkUniformTexelView;
	/// Contains resource allocation info such as parent heap, offset in heap
	struct VmaAllocation_T* pVkAllocation;
	uint64_t                         mOffset;
#endif
#if defined(METAL)
	struct VmaAllocation_T* pAllocation;
	id<MTLBuffer>                    mtlBuffer;
	id<MTLIndirectCommandBuffer>     mtlIndirectCommandBuffer API_AVAILABLE(macos(10.14), ios(12.0));
	uint64_t                         mOffset;
	uint64_t                         mPadB;
#endif
#if defined(ORBIS)
	OrbisBuffer                      mStruct;
#endif
#if defined(PROSPERO)
	ProsperoBuffer                   mStruct;
#endif
	uint64_t                         mSize : 32;
	uint64_t                         mDescriptors : 20;
	uint64_t                         mMemoryUsage : 3;
	uint64_t                         mNodeIndex : 4;
} Buffer;
// One cache line
COMPILE_ASSERT(sizeof(Buffer) == 8 * sizeof(uint64_t));

// Virtual texture page as a part of the partially resident texture
// Contains memory bindings, offsets and status information
struct VirtualTexturePage
{
	/// Buffer which contains the image data and be used for copying it to Virtual texture
	Buffer* pIntermediateBuffer;
	/// Miplevel for this page
	uint32_t mipLevel;
	/// Array layer for this page
	uint32_t layer;
	/// Index for this page
	uint32_t index;
#if defined(DIRECT3D12)
	/// Offset for this page
	D3D12_TILED_RESOURCE_COORDINATE offset;
	/// Size for this page
	D3D12_TILED_RESOURCE_COORDINATE extent;
	/// Byte size for this page
	uint32_t size;
#endif

#if defined(VULKAN)
	/// Offset for this page
	VkOffset3D offset;
	/// Size for this page
	VkExtent3D extent;
	/// Sparse image memory bind for this page
	VkSparseImageMemoryBind imageMemoryBind;
	/// Byte size for this page
	VkDeviceSize size;
#endif
};

typedef struct VirtualTexture
{
#if defined(DIRECT3D12)
	ID3D12Heap* pSparseImageMemory;
	/// Array for Sparse texture's pages
	void* pSparseCoordinates;
	/// Array for heap memory offsets
	void* pHeapRangeStartOffsets;
#endif
#if defined(VULKAN)
	/// Sparse queue binding information
	VkBindSparseInfo mBindSparseInfo;
	/// Sparse image memory bindings of all memory-backed virtual tables
	void* pSparseImageMemoryBinds;
	/// Sparse ?aque memory bindings for the mip tail (if present)	
	void* pOpaqueMemoryBinds;
	/// First mip level in mip tail
	uint32_t mMipTailStart;
	/// Lstly filled mip level in mip tail
	uint32_t mLastFilledMip;
	/// Memory type for Sparse texture's memory
	uint32_t mSparseMemoryTypeIndex;
	/// Sparse image memory bind info 
	VkSparseImageMemoryBindInfo mImageMemoryBindInfo;
	/// Sparse image opaque memory bind info (mip tail)
	VkSparseImageOpaqueMemoryBindInfo mOpaqueMemoryBindInfo;
	/// First mip level in mip tail
	uint32_t mipTailStart;
#endif
	/// Virtual Texture members
	/// Contains all virtual pages of the texture
	void* pPages;
	/// Visibility data
	Buffer* mVisibility;
	/// PrevVisibility data
	Buffer* mPrevVisibility;
	/// Alive Page's Index
	Buffer* mAlivePage;
	/// Page's Index which should be removed
	Buffer* mRemovePage;
	/// a { uint alive; uint remove; } count of pages which are alive or should be removed
	Buffer* mPageCounts;
	/// Original Pixel image data
	void* mVirtualImageData;
	///  Total pages count
	uint32_t mVirtualPageTotalCount;
	/// Sparse Virtual Texture Width
	uint64_t mSparseVirtualTexturePageWidth;
	/// Sparse Virtual Texture Height
	uint64_t mSparseVirtualTexturePageHeight;
} VirtualTexture;

typedef struct DEFINE_ALIGNED(Texture, 64)
{
#if defined(DIRECT3D12)
	/// Descriptor handle of the SRV in a CPU visible descriptor heap (applicable to TEXTURE_USAGE_SAMPLED_IMAGE)
	D3D12_CPU_DESCRIPTOR_HANDLE  mDxDescriptorHandles;
	/// Native handle of the underlying resource
	ID3D12Resource* pDxResource;
	/// Contains resource allocation info such as parent heap, offset in heap
	D3D12MA::Allocation* pDxAllocation;
	uint64_t                     mHandleCount : 24;
	uint64_t                     mUavStartIndex : 1;
	uint32_t                     mDescriptorSize;
#endif
#if defined(VULKAN)
	/// Opaque handle used by shaders for doing read/write operations on the texture
	VkImageView                  pVkSRVDescriptor;
	/// Opaque handle used by shaders for doing read/write operations on the texture
	VkImageView* pVkUAVDescriptors;
	/// Opaque handle used by shaders for doing read/write operations on the texture
	VkImageView                  pVkSRVStencilDescriptor;
	/// Native handle of the underlying resource
	VkImage                      pVkImage;
	/// Contains resource allocation info such as parent heap, offset in heap
	struct VmaAllocation_T* pVkAllocation;
#endif
#if defined(METAL)
	struct VmaAllocation_T* pAllocation;
	/// Native handle of the underlying resource
	id<MTLTexture>               mtlTexture;
	id<MTLTexture> __strong* pMtlUAVDescriptors;
	id                           mpsTextureAllocator;
	uint32_t                     mtlPixelFormat;
	uint32_t                     mFlags : 31;
	uint32_t                     mIsColorAttachment : 1;
#endif
#if defined(DIRECT3D11)
	ID3D11Resource* pDxResource;
	ID3D11ShaderResourceView* pDxSRVDescriptor;
	ID3D11UnorderedAccessView** pDxUAVDescriptors;
	uint64_t                     mPadA;
	uint64_t                     mPadB;
#endif
#if defined(ORBIS)
	OrbisTexture                 mStruct;
	/// Contains resource allocation info such as parent heap, offset in heap
#endif
#if defined(PROSPERO)
	ProsperoTexture              mStruct;
#endif
	VirtualTexture* pSvt;
	/// Current state of the buffer
	uint32_t                     mWidth : 16;
	uint32_t                     mHeight : 16;
	uint32_t                     mDepth : 16;
	uint32_t                     mMipLevels : 5;
	uint32_t                     mArraySizeMinusOne : 11;
	uint32_t                     mFormat : 8;
	/// Flags specifying which aspects (COLOR,DEPTH,STENCIL) are included in the pVkImageView
	uint32_t                     mAspectMask : 4;
	uint32_t                     mNodeIndex : 4;
	uint32_t                     mUav : 1;
	/// This value will be false if the underlying resource is not owned by the texture (swapchain textures,...)
	uint32_t                     mOwnsImage : 1;
} Texture;
// One cache line
COMPILE_ASSERT(sizeof(Texture) == 8 * sizeof(uint64_t));

typedef struct DEFINE_ALIGNED(RenderTarget, 64)
{
	Texture* pTexture;

#if defined(DIRECT3D12)
	D3D12_CPU_DESCRIPTOR_HANDLE   mDxDescriptors;
	uint32_t                      mDxDescriptorSize;
	uint32_t                      mPadA;
	uint64_t                      mPadB;
	uint64_t                      mPadC;
#endif
#if defined(VULKAN)
	VkImageView                   pVkDescriptor;
	VkImageView* pVkSliceDescriptors;
	uint32_t                      mId;
	volatile uint32_t             mUsed;
#endif
#if defined(METAL)
	uint64_t                      mPadA[3];
#endif
#if defined(DIRECT3D11)
	union
	{
		/// Resources
		ID3D11RenderTargetView* pDxRtvDescriptor;
		ID3D11DepthStencilView* pDxDsvDescriptor;
	};
	union
	{
		/// Resources
		ID3D11RenderTargetView** pDxRtvSliceDescriptors;
		ID3D11DepthStencilView** pDxDsvSliceDescriptors;
	};
	uint64_t                      mPadA;
#endif
#if defined(ORBIS)
	OrbisRenderTarget             mStruct;
#endif
#if defined(PROSPERO)
	ProsperoRenderTarget          mStruct;
#endif
	ClearValue                    mClearValue;
	uint32_t                      mArraySize : 16;
	uint32_t                      mDepth : 16;
	uint32_t                      mWidth : 16;
	uint32_t                      mHeight : 16;
	uint32_t                      mDescriptors : 20;
	uint32_t                      mMipLevels : 10;
	uint32_t                      mSampleQuality : 5;
	TinyImageFormat               mFormat;
	SampleCount                   mSampleCount;
} RenderTarget;
COMPILE_ASSERT(sizeof(RenderTarget) <= 32 * sizeof(uint64_t));

typedef struct DEFINE_ALIGNED(Sampler, 16)
{
#if defined(DIRECT3D12)
	/// Description for creating the Sampler descriptor for ths sampler
	D3D12_SAMPLER_DESC          mDxDesc;
	/// Descriptor handle of the Sampler in a CPU visible descriptor heap
	D3D12_CPU_DESCRIPTOR_HANDLE mDxHandle;
#endif
#if defined(VULKAN)
	/// Native handle of the underlying resource
	VkSampler                   pVkSampler;
#endif
#if defined(METAL)
	/// Native handle of the underlying resource
	id<MTLSamplerState>         mtlSamplerState;
#endif
#if defined(DIRECT3D11)
	/// Native handle of the underlying resource
	ID3D11SamplerState* pSamplerState;
#endif
#if defined(ORBIS)
	OrbisSampler                mStruct;
#endif
#if defined(PROSPERO)
	ProsperoSampler             mStruct;
#endif
} Sampler;
#if defined(DIRECT3D12)
COMPILE_ASSERT(sizeof(Sampler) == 8 * sizeof(uint64_t));
#else
COMPILE_ASSERT(sizeof(Sampler) == 2 * sizeof(uint64_t));
#endif

typedef struct DEFINE_ALIGNED(RootSignature, 64)
{
	/// Number of descriptors declared in the root signature layout
	uint32_t                   mDescriptorCount;
	/// Graphics or Compute
	PipelineType               mPipelineType;
	/// Array of all descriptors declared in the root signature layout
	DescriptorInfo* pDescriptors;
	/// Translates hash of descriptor name to descriptor index in pDescriptors array
	DescriptorIndexMap* pDescriptorNameToIndexMap;
#if defined(DIRECT3D12)
	ID3D12RootSignature* pDxRootSignature;
	uint8_t                    mDxRootConstantRootIndices[MAX_ROOT_CONSTANTS_PER_ROOTSIGNATURE];
	uint8_t                    mDxViewDescriptorTableRootIndices[DESCRIPTOR_UPDATE_FREQ_COUNT];
	uint8_t                    mDxSamplerDescriptorTableRootIndices[DESCRIPTOR_UPDATE_FREQ_COUNT];
	uint8_t                    mDxRootDescriptorRootIndices[DESCRIPTOR_UPDATE_FREQ_COUNT];
	uint32_t                   mDxCumulativeViewDescriptorCounts[DESCRIPTOR_UPDATE_FREQ_COUNT];
	uint32_t                   mDxCumulativeSamplerDescriptorCounts[DESCRIPTOR_UPDATE_FREQ_COUNT];
	uint16_t                   mDxViewDescriptorCounts[DESCRIPTOR_UPDATE_FREQ_COUNT];
	uint16_t                   mDxSamplerDescriptorCounts[DESCRIPTOR_UPDATE_FREQ_COUNT];
	uint8_t                    mDxRootDescriptorCounts[DESCRIPTOR_UPDATE_FREQ_COUNT];
	uint32_t                   mDxRootConstantCount;
	uint64_t                   mPadA;
	uint64_t                   mPadB;
#endif
#if defined(VULKAN)
	VkDescriptorSetLayout      mVkDescriptorSetLayouts[DESCRIPTOR_UPDATE_FREQ_COUNT];
	uint32_t                   mVkCumulativeDescriptorCounts[DESCRIPTOR_UPDATE_FREQ_COUNT];
	uint16_t                   mVkDescriptorCounts[DESCRIPTOR_UPDATE_FREQ_COUNT];
	uint8_t                    mVkDynamicDescriptorCounts[DESCRIPTOR_UPDATE_FREQ_COUNT];
	uint8_t                    mVkRaytracingDescriptorCounts[DESCRIPTOR_UPDATE_FREQ_COUNT];
	VkPipelineLayout           pPipelineLayout;
	VkDescriptorUpdateTemplate mUpdateTemplates[DESCRIPTOR_UPDATE_FREQ_COUNT];
	VkDescriptorSet            mVkEmptyDescriptorSets[DESCRIPTOR_UPDATE_FREQ_COUNT];
	void** pUpdateTemplateData[DESCRIPTOR_UPDATE_FREQ_COUNT];
	uint32_t                   mVkPushConstantCount;
	uint32_t                   mPadA;
	uint64_t                   mPadB[7];
#endif
#if defined(METAL)
	NSMutableArray<MTLArgumentDescriptor*>* mArgumentDescriptors[DESCRIPTOR_UPDATE_FREQ_COUNT] API_AVAILABLE(macos(10.13), ios(11.0));
	uint32_t                   mRootTextureCount : 10;
	uint32_t                   mRootBufferCount : 10;
	uint32_t                   mRootSamplerCount : 10;
#endif
#if defined(DIRECT3D11)
	ID3D11SamplerState** ppStaticSamplers;
	uint32_t* pStaticSamplerSlots;
	ShaderStage* pStaticSamplerStages;
	uint32_t                   mStaticSamplerCount;
	uint32_t                   mSrvCount : 10;
	uint32_t                   mUavCount : 10;
	uint32_t                   mCbvCount : 10;
	uint32_t                   mSamplerCount : 10;
	uint32_t                   mDynamicCbvCount : 10;
	uint32_t                   mPadA;
#endif
#if defined(ORBIS)
	OrbisRootSignature         mStruct;
#endif
#if defined(PROSPERO)
	ProsperoRootSignature      mStruct;
#endif
} RootSignature;
#if defined(VULKAN)
// 4 cache lines
COMPILE_ASSERT(sizeof(RootSignature) == 32 * sizeof(uint64_t));
#elif defined(DIRECT3D11) || defined(METAL)
// 1 cache line
COMPILE_ASSERT(sizeof(RootSignature) == 8 * sizeof(uint64_t));
#else
// 2 cache lines
COMPILE_ASSERT(sizeof(RootSignature) <= 16 * sizeof(uint64_t));
#endif

typedef struct DEFINE_ALIGNED(DescriptorSet, 64)
{
#if defined(DIRECT3D12)
	/// Start handle to cbv srv uav descriptor table
	uint64_t                      mCbvSrvUavHandle;
	/// Start handle to sampler descriptor table
	uint64_t                      mSamplerHandle;
	/// Stride of the cbv srv uav descriptor table (number of descriptors * descriptor size)
	uint32_t                      mCbvSrvUavStride;
	/// Stride of the sampler descriptor table (number of descriptors * descriptor size)
	uint32_t                      mSamplerStride;
	const RootSignature* pRootSignature;
	D3D12_GPU_VIRTUAL_ADDRESS* pRootAddresses;
	ID3D12RootSignature* pRootSignatureHandle;
	uint64_t                      mMaxSets : 16;
	uint64_t                      mUpdateFrequency : 3;
	uint64_t                      mNodeIndex : 4;
	uint64_t                      mRootAddressCount : 1;
	uint64_t                      mCbvSrvUavRootIndex : 4;
	uint64_t                      mSamplerRootIndex : 4;
	uint64_t                      mRootDescriptorRootIndex : 4;
	uint64_t                      mPipelineType : 3;
#elif defined(VULKAN)
	VkDescriptorSet* pHandles;
	const RootSignature* pRootSignature;
	/// Values passed to vkUpdateDescriptorSetWithTemplate. Initialized to default descriptor values.
	union DescriptorUpdateData** ppUpdateData;
	struct SizeOffset* pDynamicSizeOffsets;
	uint32_t                      mMaxSets;
	uint8_t                       mDynamicOffsetCount;
	uint8_t                       mUpdateFrequency;
	uint8_t                       mNodeIndex;
	uint8_t                       mPadA;
#elif defined(METAL)
	id<MTLArgumentEncoder>        mArgumentEncoder API_AVAILABLE(macos(10.13), ios(11.0));
	Buffer* mArgumentBuffer API_AVAILABLE(macos(10.13), ios(11.0));
	const RootSignature* pRootSignature;
	/// Descriptors that are bound without argument buffers
	/// This is necessary since there are argument buffers bugs in some iOS Metal drivers which causes shader compiler crashes or incorrect shader generation. This makes it necessary to keep fallback descriptor binding path alive
	struct RootDescriptorData* pRootDescriptorData;
	uint32_t                      mChunkSize;
	uint32_t                      mMaxSets;
	uint8_t                       mUpdateFrequency;
	uint8_t                       mNodeIndex;
	uint8_t                       mStages;
#elif defined(DIRECT3D11)
	struct DescriptorDataArray* pHandles;
	struct CBV** pDynamicCBVs;
	uint32_t* pDynamicCBVsCapacity;
	uint32_t* pDynamicCBVsCount;
	uint32_t* pDynamicCBVsPrevCount;
	const RootSignature* pRootSignature;
	uint16_t                      mMaxSets;
#elif defined(ORBIS)
	OrbisDescriptorSet            mStruct;
#elif defined(PROSPERO)
	ProsperoDescriptorSet         mStruct;
#endif
} DescriptorSet;

typedef struct CmdPool
{
#if defined(DIRECT3D12)
	ID3D12CommandAllocator* pDxCmdAlloc;
#endif
#if defined(VULKAN)
	VkCommandPool           pVkCmdPool;
#endif
	Queue* pQueue;
} CmdPool;

typedef struct DEFINE_ALIGNED(Cmd, 64)
{
#if defined(DIRECT3D12)
#if defined(XBOX)
	DmaCmd                       mDma;
#endif
	ID3D12GraphicsCommandList* pDxCmdList;

	// Cached in beginCmd to avoid fetching them during rendering
	struct DescriptorHeap* pBoundHeaps[2];
	D3D12_GPU_DESCRIPTOR_HANDLE  mBoundHeapStartHandles[2];

	// Command buffer state
	const ID3D12RootSignature* pBoundRootSignature;
	DescriptorSet* pBoundDescriptorSets[DESCRIPTOR_UPDATE_FREQ_COUNT];
	uint16_t                     mBoundDescriptorSetIndices[DESCRIPTOR_UPDATE_FREQ_COUNT];
	uint32_t                     mNodeIndex : 4;
	uint32_t                     mType : 3;
	CmdPool* pCmdPool;
	uint32_t                     mPadA;
#if !defined(XBOX)
	uint64_t                     mPadB;
#endif
#endif
#if defined(VULKAN)
	VkCommandBuffer              pVkCmdBuf;
	VkRenderPass                 pVkActiveRenderPass;
	VkPipelineLayout             pBoundPipelineLayout;
	uint32_t                     mNodeIndex : 4;
	uint32_t                     mType : 3;
	uint32_t                     mPadA;
	CmdPool* pCmdPool;
	uint64_t                     mPadB[9];
#endif
#if defined(METAL)
	id<MTLCommandBuffer>         mtlCommandBuffer;
	id<MTLRenderCommandEncoder>  mtlRenderEncoder;
	id<MTLComputeCommandEncoder> mtlComputeEncoder;
	id<MTLBlitCommandEncoder>    mtlBlitEncoder;
	MTLRenderPassDescriptor* pRenderPassDesc;
	Shader* pShader;
	NOREFS id<MTLBuffer>         mSelectedIndexBuffer;
	uint64_t                     mSelectedIndexBufferOffset;
	// We have to track color attachments which will be read by shader
	// Metal documentation says to call useResource on these as late as possible
	// This will avoid possible decompression of all color attachments inside the heap
	NOREFS id<MTLResource>* pColorAttachments;
	QueryPool* pLastFrameQuery;
	uint32_t                     mIndexType : 1;
	uint32_t                     mIndexStride : 3;
	uint32_t                     mSelectedPrimitiveType : 4;
	uint32_t                     mPipelineType : 3;
	uint32_t                     mColorAttachmentCount : 10;
	uint32_t                     mColorAttachmentCapacity : 10;
	uint64_t                     mPadA[4];
#endif
#if defined(DIRECT3D11)
	ID3D11Buffer* pRootConstantBuffer;
	ID3D11Buffer* pTransientConstantBuffer;
	uint8_t* pDescriptorCache;
	uint32_t                     mDescriptorCacheOffset;
	uint32_t                     mPadA;
	uint64_t                     mPadB[10];
#endif
#if defined(ORBIS)
	OrbisCmd                     mStruct;
#endif
#if defined(PROSPERO)
	ProsperoCmd                  mStruct;
#endif
	Renderer* pRenderer;
	Queue* pQueue;
} Cmd;
COMPILE_ASSERT(sizeof(Cmd) <= 64 * sizeof(uint64_t));

typedef struct Fence
{
#if defined(DIRECT3D12)
	ID3D12Fence* pDxFence;
	HANDLE               pDxWaitIdleFenceEvent;
	uint64_t             mFenceValue;
	uint64_t             mPadA;
#endif
#if defined(VULKAN)
	VkFence              pVkFence;
	uint32_t             mSubmitted : 1;
	uint32_t             mPadA;
	uint64_t             mPadB;
	uint64_t             mPadC;
#endif
#if defined(METAL)
	dispatch_semaphore_t pMtlSemaphore;
	uint32_t             mSubmitted : 1;
	uint32_t             mPadA;
	uint64_t             mPadB;
	uint64_t             mPadC;
#endif
#if defined(DIRECT3D11)
	ID3D11Query* pDX11Query;
	uint32_t             mSubmitted : 1;
	uint32_t             mPadA;
	uint64_t             mPadB;
	uint64_t             mPadC;
#endif
#if defined(ORBIS)
	OrbisFence           mStruct;
#endif
#if defined(PROSPERO)
	ProsperoFence        mStruct;
#endif
} Fence;

typedef struct Semaphore
{
#if defined(DIRECT3D12)
	// DirectX12 does not have a concept of semaphores
	// All synchronization is done using fences
	// Simlate semaphore signal and wait using DirectX12 fences

	// Semaphores used in DirectX12 only in queueSubmit
	// queueSubmit -> How the semaphores work in DirectX12

	// pp_wait_semaphores -> queue->Wait is manually called on each fence in this
	// array before calling ExecuteCommandLists to make the fence work like a wait semaphore

	// pp_signal_semaphores -> Manually call queue->Signal on each fence in this array after
	// calling ExecuteCommandLists and increment the underlying fence value

	// queuePresent does not use the wait semaphore since the swapchain Present function
	// already does the synchronization in this case
	ID3D12Fence* pDxFence;
	HANDLE               pDxWaitIdleFenceEvent;
	uint64_t             mFenceValue;
	uint64_t             mPadA;
#endif
#if defined(VULKAN)
	VkSemaphore          pVkSemaphore;
	uint32_t             mCurrentNodeIndex : 5;
	uint32_t             mSignaled : 1;
	uint32_t             mPadA;
	uint64_t             mPadB;
	uint64_t             mPadC;
#endif
#if defined(METAL)
	id<MTLEvent>         pMtlSemaphore API_AVAILABLE(macos(10.14), ios(12.0));
	uint32_t             mSignaled;
	uint32_t             mPadA;
	uint64_t             mPadB;
#endif
#if defined(ORBIS)
	OrbisFence           mStruct;
#endif
#if defined(PROSPERO)
	ProsperoSemaphore    mStruct;
#endif
} Semaphore;

typedef struct Queue
{
#if defined(DIRECT3D12)
	ID3D12CommandQueue* pDxQueue;
	uint32_t             mType : 3;
	uint32_t             mNodeIndex : 4;
	Fence* pFence;
#endif
#if defined(VULKAN)
	VkQueue              pVkQueue;
	Mutex* pSubmitMutex;
	uint32_t             mVkQueueFamilyIndex : 5;
	uint32_t             mVkQueueIndex : 5;
	uint32_t             mType : 3;
	uint32_t             mNodeIndex : 4;
	uint32_t             mGpuMode : 3;
	uint32_t             mFlags;
	float                mTimestampPeriod;
#endif
#if defined(METAL)
	id<MTLCommandQueue>  mtlCommandQueue;
	id<MTLFence>         mtlQueueFence API_AVAILABLE(macos(10.13), ios(10.0));
	uint32_t             mBarrierFlags;
	uint32_t             mType : 3;
	uint32_t             mNodeIndex : 4;
	uint32_t             mPadB;
#endif
#if defined(DIRECT3D11)
	ID3D11Device* pDxDevice;
	ID3D11DeviceContext* pDxContext;
	uint32_t             mType : 3;
	uint32_t             mNodeIndex : 4;
	Fence* pFence;
#endif
#if defined(ORBIS)
	OrbisQueue           mStruct;
	uint32_t             mType : 3;
	uint32_t             mNodeIndex : 4;
#endif
#if defined(PROSPERO)
	ProsperoQueue        mStruct;
	uint32_t             mType : 3;
	uint32_t             mNodeIndex : 4;
#endif
} Queue;

typedef struct Shader
{
	ShaderStage                   mStages;
	uint32_t                      mNumThreadsPerGroup[3];
#if defined(DIRECT3D12)
	IDxcBlobEncoding** pShaderBlobs;
	LPCWSTR* pEntryNames;
#endif
#if defined(VULKAN)
	VkShaderModule* pShaderModules;
	char** pEntryNames;
#endif
#if defined(METAL)
	id<MTLFunction>               mtlVertexShader;
	id<MTLFunction>               mtlFragmentShader;
	id<MTLFunction>               mtlComputeShader;
	id<MTLLibrary>		          mtlLibrary;
	char** pEntryNames;
	uint32_t                      mTessellation : 1;
#endif
#if defined(DIRECT3D11)
	union
	{
		struct
		{
			ID3D11VertexShader* pDxVertexShader;
			ID3D11PixelShader* pDxPixelShader;
			ID3D11GeometryShader* pDxGeometryShader;
			ID3D11DomainShader* pDxDomainShader;
			ID3D11HullShader* pDxHullShader;
		};
		ID3D11ComputeShader* pDxComputeShader;
	};
	ID3DBlob* pDxInputSignature;
#endif
#if defined(ORBIS)
	OrbisShader                   mStruct;
#endif
#if defined(PROSPERO)
	ProsperoShader                mStruct;
#endif
	PipelineReflection* pReflection;
} Shader;

typedef struct DEFINE_ALIGNED(Pipeline, 64)
{
#if defined(DIRECT3D12)
	ID3D12PipelineState* pDxPipelineState;
#ifdef ENABLE_RAYTRACING
	ID3D12StateObject* pDxrPipeline;
#endif
	ID3D12RootSignature* pRootSignature;
	PipelineType                mType;
	D3D_PRIMITIVE_TOPOLOGY      mDxPrimitiveTopology;
	uint64_t                    mPadB[3];
#endif
#if defined(VULKAN)
	VkPipeline                  pVkPipeline;
	PipelineType                mType;
	uint32_t                    mShaderStageCount;
	//In DX12 this information is stored in ID3D12StateObject.
	//But for Vulkan we need to store it manually
	const char** ppShaderStageNames;
	uint64_t                    mPadB[4];
#endif
#if defined(METAL)
	Shader* pShader;
	id<MTLRenderPipelineState>  mtlRenderPipelineState;
	id<MTLComputePipelineState> mtlComputePipelineState;
	id<MTLDepthStencilState>    mtlDepthStencilState;
	RaytracingPipeline* pRaytracingPipeline;
	uint32_t                    mCullMode : 3;
	uint32_t                    mFillMode : 3;
	uint32_t                    mWinding : 3;
	uint32_t                    mDepthClipMode : 1;
	uint32_t                    mMtlPrimitiveType : 4;
	float                       mDepthBias;
	float                       mSlopeScale;
	PipelineType                mType;
	uint64_t                    mPadA;
#endif
#if defined(DIRECT3D11)
	ID3D11VertexShader* pDxVertexShader;
	ID3D11PixelShader* pDxPixelShader;
	ID3D11GeometryShader* pDxGeometryShader;
	ID3D11DomainShader* pDxDomainShader;
	ID3D11HullShader* pDxHullShader;
	ID3D11ComputeShader* pDxComputeShader;
	ID3D11InputLayout* pDxInputLayout;
	ID3D11BlendState* pBlendState;
	ID3D11DepthStencilState* pDepthState;
	ID3D11RasterizerState* pRasterizerState;
	PipelineType                mType;
	D3D_PRIMITIVE_TOPOLOGY      mDxPrimitiveTopology;
	uint32_t                    mPadA;
	uint64_t                    mPadB[4];
#endif
#if defined(ORBIS)
	OrbisPipeline               mStruct;
#endif
#if defined(PROSPERO)
	ProsperoPipeline            mStruct;
#endif
} Pipeline;
#if defined(DIRECT3D11) || defined(ORBIS)
// Requires more cache lines due to no concept of an encapsulated pipeline state object
COMPILE_ASSERT(sizeof(Pipeline) <= 64 * sizeof(uint64_t));
#elif defined(PROSPERO)
COMPILE_ASSERT(sizeof(Pipeline) == 16 * sizeof(uint64_t));
#else
// One cache line
COMPILE_ASSERT(sizeof(Pipeline) == 8 * sizeof(uint64_t));
#endif

typedef struct PipelineCache
{
#if defined(DIRECT3D12)
	ID3D12PipelineLibrary* pLibrary;
	void* pData;
#endif
#if defined(VULKAN)
	VkPipelineCache        pCache;
#endif
} PipelineCache;

typedef struct SwapChain
{
	/// Render targets created from the swapchain back buffers
	RenderTarget** ppRenderTargets;
#if defined(XBOX)
	uint64_t                 mFramePipelineToken;
	/// Sync interval to specify how interval for vsync
	uint32_t                 mDxSyncInterval : 3;
	uint32_t                 mFlags : 10;
	uint32_t                 mImageCount : 3;
	uint32_t                 mEnableVsync : 1;
	uint32_t                 mIndex;
	void* pWindow;
	Queue* pPresentQueue;
	uint64_t                 mPadB[5];
#elif defined(DIRECT3D12)
	/// Use IDXGISwapChain3 for now since IDXGISwapChain4
	/// isn't supported by older devices.
	IDXGISwapChain3* pDxSwapChain;
	/// Sync interval to specify how interval for vsync
	uint32_t                 mDxSyncInterval : 3;
	uint32_t                 mFlags : 10;
	uint32_t                 mImageCount : 3;
	uint32_t                 mEnableVsync : 1;
	uint32_t                 mPadA;
	uint64_t                 mPadB[5];
#endif
#if defined(DIRECT3D11)
	/// Use IDXGISwapChain3 for now since IDXGISwapChain4
	/// isn't supported by older devices.
	IDXGISwapChain* pDxSwapChain;
	/// Sync interval to specify how interval for vsync
	uint32_t                 mDxSyncInterval : 3;
	uint32_t                 mFlags : 10;
	uint32_t                 mImageCount : 3;
	uint32_t                 mImageIndex : 3;
	uint32_t                 mEnableVsync : 1;
	DXGI_SWAP_EFFECT         mSwapEffect;
	uint32_t                 mPadA;
	uint64_t                 mPadB[5];
#endif
#if defined(VULKAN)
	/// Present queue if one exists (queuePresent will use this queue if the hardware has a dedicated present queue)
	VkQueue                  pPresentQueue;
	VkSwapchainKHR           pSwapChain;
	VkSurfaceKHR             pVkSurface;
	SwapChainDesc* pDesc;
	uint32_t                 mPresentQueueFamilyIndex : 5;
	uint32_t                 mImageCount : 3;
	uint32_t                 mEnableVsync : 1;
	uint32_t                 mPadA;
#endif
#if defined(METAL)
#if defined(TARGET_IOS)
	UIView* pForgeView;
#else
	NSView* pForgeView;
#endif
	id<CAMetalDrawable>      mMTKDrawable;
	id<MTLCommandBuffer>     presentCommandBuffer;
	uint32_t                 mImageCount : 3;
	uint32_t                 mEnableVsync : 1;
	uint32_t                 mIndex;
	uint64_t                 mPadB[4];
#endif
#if defined(ORBIS)
	OrbisSwapChain           mStruct;
	uint32_t                 mImageCount : 3;
	uint32_t                 mEnableVsync : 1;
#endif
#if defined(PROSPERO)
	ProsperoSwapChain        mStruct;
	uint32_t                 mImageCount : 3;
	uint32_t                 mEnableVsync : 1;
#endif
} SwapChain;

typedef struct DEFINE_ALIGNED(Renderer, 64)
{
#if defined(DIRECT3D12)
	// API specific descriptor heap and memory allocator
	struct DescriptorHeap** pCPUDescriptorHeaps;
	struct DescriptorHeap** pCbvSrvUavHeaps;
	struct DescriptorHeap** pSamplerHeaps;
	class  D3D12MA::Allocator* pResourceAllocator;
#if defined(XBOX)
	IDXGIFactory2* pDXGIFactory;
	IDXGIAdapter* pDxActiveGPU;
	ID3D12Device* pDxDevice;
	EsramManager* pESRAMManager;
#elif defined(DIRECT3D12)
	IDXGIFactory6* pDXGIFactory;
	IDXGIAdapter4* pDxActiveGPU;
	ID3D12Device* pDxDevice;
	uint64_t                        mPadA;
#endif
	ID3D12Debug* pDXDebug;
#if defined(_WINDOWS)
	ID3D12InfoQueue* pDxDebugValidation;
#endif
#endif
#if defined(DIRECT3D11)
	IDXGIFactory1* pDXGIFactory;
	IDXGIAdapter1* pDxActiveGPU;
	ID3D11Device* pDxDevice;
	ID3D11DeviceContext* pDxContext;
	ID3D11BlendState* pDefaultBlendState;
	ID3D11DepthStencilState* pDefaultDepthState;
	ID3D11RasterizerState* pDefaultRasterizerState;
	uint32_t                        mPartialUpdateConstantBufferSupported : 1;
	D3D_FEATURE_LEVEL               mFeatureLevel;
	uint64_t                        mPadB;
	uint32_t                        mPadA;
#endif
#if defined(VULKAN)
	VkInstance                      pVkInstance;
	VkPhysicalDevice                pVkActiveGPU;
	VkPhysicalDeviceProperties2* pVkActiveGPUProperties;
	VkDevice                        pVkDevice;
#ifdef USE_DEBUG_UTILS_EXTENSION
	VkDebugUtilsMessengerEXT        pVkDebugUtilsMessenger;
#else
	VkDebugReportCallbackEXT        pVkDebugReport;
#endif
	uint32_t** pAvailableQueueCount;
	uint32_t** pUsedQueueCount;
	struct DescriptorPool* pDescriptorPool;
	struct VmaAllocator_T* pVmaAllocator;
	uint32_t                        mRaytracingExtension : 1;
	union
	{
		struct
		{
			uint8_t                 mGraphicsQueueFamilyIndex;
			uint8_t                 mTransferQueueFamilyIndex;
			uint8_t                 mComputeQueueFamilyIndex;
		};
		uint8_t                     mQueueFamilyIndices[3];
	};
#endif
#if defined(METAL)
	id<MTLDevice>                   pDevice;
	struct VmaAllocator_T* pVmaAllocator;
	NOREFS id<MTLHeap>* pHeaps API_AVAILABLE(macos(10.13), ios(10.0));
	uint32_t                        mHeapCount;
	uint32_t                        mHeapCapacity;
	// #TODO: Store this in GpuSettings struct
	uint64_t                        mVRAM;
	// To synchronize resource allocation done through automatic heaps
	Mutex* pHeapMutex;
	uint64_t                        mPadA[3];
#endif
#if defined(ORBIS)
	uint64_t                        mPadA;
	uint64_t                        mPadB;
#endif
	struct NullDescriptors* pNullDescriptors;
	char* pName;
	GPUSettings* pActiveGpuSettings;
	ShaderMacro* pBuiltinShaderDefines;
	GPUCapBits* pCapBits;
	uint32_t                        mLinkedNodeCount : 4;
	uint32_t                        mGpuMode : 3;
	uint32_t                        mShaderTarget : 4;
	uint32_t                        mApi : 5;
	uint32_t                        mEnableGpuBasedValidation : 1;
	uint32_t                        mBuiltinShaderDefinesCount;
} Renderer;
// 2 cache lines
COMPILE_ASSERT(sizeof(Renderer) <= 16 * sizeof(uint64_t));

typedef struct CommandSignature
{
#if defined(DIRECT3D12)
	ID3D12CommandSignature* pDxHandle;
#endif
#if defined(VULKAN)
	IndirectArgumentType    mDrawType;
	uint32_t                mStride;
#endif
#if defined(METAL)
	IndirectArgumentType    mDrawType;
	uint32_t                mStride;
#endif
#if defined(ORBIS)
	IndirectArgumentType    mDrawType;
	uint32_t                mStride;
#endif
#if defined(PROSPERO)
	IndirectArgumentType    mDrawType;
	uint32_t                mStride;
#endif
} CommandSignature;
