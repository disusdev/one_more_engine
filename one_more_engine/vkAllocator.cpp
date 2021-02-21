//
#include "PCH.h"
//
///*
//===============================================================================
//static vars
//===============================================================================
//*/
//
//static const u32
//hostVisibleMemoryMB = 24 * 1024 * 1024;
//
//static const u32
//deviceLocalMemoryMB = 128 * 1024 * 1024;
//
//static Context&
//context;
//
//static std::vector<vkMemoryPool*>
//pools;
//
//static u32 
//newxtPoolId = 0;

VmaAllocator vmaAllocator;


/*
===============================================================================
FindMemoryTypeIndex
===============================================================================
*/

u32 
FindMemoryTypeIndex(
	Context& context,
  const u32 memoryTypeBits, 
  const vkMemoryUsage usage)
{
  VkPhysicalDeviceMemoryProperties & physicalMemoryProperties = context.gpu->memProps;

	VkMemoryPropertyFlags required = 0;
	VkMemoryPropertyFlags preferred = 0;

	switch ( usage ) {
	case VULKAN_MEMORY_USAGE_GPU_ONLY:
		preferred |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		break;
	case VULKAN_MEMORY_USAGE_CPU_ONLY:
		required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT 
      | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		break;
	case VULKAN_MEMORY_USAGE_CPU_TO_GPU:
		required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		preferred |= VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		break;
	case VULKAN_MEMORY_USAGE_GPU_TO_CPU:
		required |= VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		preferred |= VK_MEMORY_PROPERTY_HOST_COHERENT_BIT 
      | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
		break;
	default:
    VK_VALIDATE(false, "Unknown memory usage.");
	}

	for ( u32 i = 0; i < physicalMemoryProperties.memoryTypeCount; ++i ) {
		if ( ( ( memoryTypeBits >> i ) & 1 ) == 0 ) {
			continue;
		}
		
		const VkMemoryPropertyFlags properties 
      = physicalMemoryProperties.memoryTypes[ i ].propertyFlags;
		if ( ( properties & required ) != required ) {
			continue;
		}

		if ( ( properties & preferred ) != preferred ) {
			continue;
		}

		return i;
	}

	for ( u32 i = 0; i < physicalMemoryProperties.memoryTypeCount; ++i ) {
		if ( ( ( memoryTypeBits >> i ) & 1 ) == 0 ) {
			continue;
		}
			
		const VkMemoryPropertyFlags properties 
      = physicalMemoryProperties.memoryTypes[ i ].propertyFlags;
		if ( ( properties & required ) != required ) {
			continue;
		}

		return i;
	}

	return UINT32_MAX;
}

///*
//===============================================================================
//CreateMemoryPool
//===============================================================================
//*/
//
//vkMemoryPool*
//CreateMemoryPool(
//  const u32 poolId,
//  const u32 typeBits,
//  const u32 size,
//  const bool hostVisible)
//{
//  return nullptr;
//}
//
///*
//===============================================================================
//InitVulkanAllocator
//===============================================================================
//*/
//
//void
//InitVulkanAllocator(
//  Context& context)
//{
//  context = context;
//}
//
///*
//===============================================================================
//AllocateFromPools
//===============================================================================
//*/
//
//bool
//AllocateFromPools(
//  Context& context,
//  const u32 size,
//  const u32 align,
//  const u32 memoryTypeBits,
//  const bool needHostVisible,
//  vkAllocation& allocation)
//{
//  const VkPhysicalDeviceMemoryProperties& 
//    physicalMemoryProperties = context.gpu->memProps;
//  const int 
//    num = pools.size();
//
//  const VkMemoryPropertyFlags required = needHostVisible ? 
//    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT 
//    : 0;
//
//  const VkMemoryPropertyFlags preferred = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
//
//  for (i32 i = 0; i < num; i++)
//  {
//    vkMemoryPool* pool = pools[i];
//    const u32 memoryTypeIndex = pool->memoryTypeIndex;
//
//    if (needHostVisible && pool->hostVisible == false)
//    {
//      continue;
//    }
//
//    if (((memoryTypeBits >> memoryTypeIndex) & 1) == 0)
//    {
//      continue;
//    }
//
//    const VkMemoryPropertyFlags properties 
//      = physicalMemoryProperties.memoryTypes[memoryTypeIndex].propertyFlags;
//
//    if (BIT_N_COMPARE(properties,required))
//    {
//      continue;
//    }
//
//    if (BIT_N_COMPARE(properties, preferred))
//    {
//      continue;
//    }
//
//    if (PoolAllocate(size, align, allocation))
//    {
//      return true;
//    }
//  }
//
//  for (i32 i = 0; i < num; i++)
//  {
//    vkMemoryPool* pool = pools[i];
//    const u32 memoryTypeIndex = pool->memoryTypeIndex;
//
//    if (needHostVisible && pool->hostVisible == false)
//    {
//      continue;
//    }
//
//    if (((memoryTypeBits >> memoryTypeIndex) & 1) == 0)
//    {
//      continue;
//    }
//
//    const VkMemoryPropertyFlags properties 
//      = physicalMemoryProperties.memoryTypes[memoryTypeIndex].propertyFlags;
//    if (BIT_N_COMPARE(properties, required))
//    {
//      continue;
//    }
//
//    if (PoolAllocate(size, align, allocation))
//    {
//      return true;
//    }
//  }
//
//  return false;
//}
//
///*
//===============================================================================
//Allocate
//===============================================================================
//*/
//
//vkAllocation
//Allocate(
//  const u32 size, 
//  const u32 align, 
//  const u32 memoryTypeBits, 
//  const bool hostVisible)
//{
//  vkAllocation allocation;
//
//  if (AllocateFromPools(
//    context, 
//    size, 
//    align, 
//    memoryTypeBits, 
//    hostVisible, 
//    allocation))
//  {
//    return allocation;
//  }
//
//  VkDeviceSize poolSize = hostVisible ? hostVisibleMemoryMB 
//    : deviceLocalMemoryMB;
//
//  vkMemoryPool* pool = 
//    CreateMemoryPool(
//      newxtPoolId++, 
//      memoryTypeBits, 
//      poolSize, 
//      hostVisible);
//
//  if (PoolInit())
//  {
//    pools.push_back(pool);
//  }
//  else
//  {
//    // Error
//  }
//
//  PoolAllocate(size, align, allocation);
//
//  return allocation;
//}
//
///*
//===============================================================================
//PoolInit
//===============================================================================
//*/
//
//bool
//PoolInit()
//{
//  return false;
//}