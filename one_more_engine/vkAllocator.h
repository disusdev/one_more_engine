#pragma once

/*
===============================================================================
enums
===============================================================================
*/

struct Context;

enum vkMemoryUsage
{
	VULKAN_MEMORY_USAGE_UNKNOWN,
	VULKAN_MEMORY_USAGE_GPU_ONLY,
	VULKAN_MEMORY_USAGE_CPU_ONLY,
	VULKAN_MEMORY_USAGE_CPU_TO_GPU,
	VULKAN_MEMORY_USAGE_GPU_TO_CPU,
	VULKAN_MEMORY_USAGES,
};

enum vkAllocationType
{
	VULKAN_ALLOCATION_TYPE_FREE,
	VULKAN_ALLOCATION_TYPE_BUFFER,
	VULKAN_ALLOCATION_TYPE_IMAGE,
	VULKAN_ALLOCATION_TYPE_IMAGE_LINEAR,
	VULKAN_ALLOCATION_TYPE_IMAGE_OPTIMAL,
	VULKAN_ALLOCATION_TYPES,
};

u32
FindMemoryTypeIndex(
	Context& context,
	const u32 memoryTypeBits,
	const vkMemoryUsage usage);

/*
===============================================================================
structs
===============================================================================
*/

//struct vkBlock;
//
//struct vkAllocation
//{
//  vkBlock*       block = nullptr;
//  u32            id = 0;
//  VkDeviceMemory deviceMemory = VK_NULL_HANDLE;
//  VkDeviceSize   offset = 0;
//  VkDeviceSize   size = 0;
//  byte*          data = nullptr;
//};
//
//struct vkBlock
//{
//  struct chunk_t
//  {
//    u32 id;
//    VkDeviceSize size;
//    VkDeviceSize offset;
//    chunk_t* prev;
//    chunk_t* next;
//    vkAllocationType type;
//  };
//  chunk_t head;
//
//  u32 nextBlockId;
//  u32 memoryTypeIndex;
//  vkMemoryUsage usage;
//  VkDeviceMemory deviceMemory;
//  VkDeviceSize size;
//  VkDeviceSize allocated;
//  byte* data;
//};// Init() Term() Allocate() Free()
//
//typedef std::vector<vkBlock*> vkBlocks[VK_MAX_MEMORY_TYPES];
//
//struct vkAllocator
//{
//  i32                       garbageIndex;
//  i32                       deviceLocalMemoryMB;
//  i32                       hostVisibleMemoryMB;
//  VkDeviceSize              bufferImageGranularity;
//  vkBlocks                  blocks;
//  std::vector<vkAllocation> garbage[NUM_FRAME_DATA];
//};

extern VmaAllocator vmaAllocator;

//#if defined( USE_AMD_ALLOCATOR )
//#else
//extern vkAllocator vulkanAllocator;
//#endif