#include "PCH.h"

static void
CopyBuffer(
  byte* dst,
  const byte* src,
  int numBytes )
{
	assert_16_byte_aligned( dst );
	assert_16_byte_aligned( src );
	memcpy( dst, src, numBytes );
}

bool
AllocBufferObject(
  BufferObject& buffer,
  bufferObjectType type,
  const void* data,
  i32 allocSize,
  bufferUsageType usage)
{
  platform_assert(buffer.apiObject == VK_NULL_HANDLE);
  platform_assert(type == BO_NONE);
  assert_16_byte_aligned(data);

  if (allocSize <= 0)
  {
    VK_VALIDATE(false, "Bad allocation size!")
  }

  buffer.type = type;
  buffer.size = allocSize;
  buffer.usage = usage;

  bool allocationFailed = false;

  int numBytes = (buffer.size + 15) & ~15;

  VkBufferCreateInfo bufferCreateInfo = {};
  bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.pNext = nullptr;
	bufferCreateInfo.size = numBytes;
	bufferCreateInfo.usage = type;
  if (type == BO_VERTEX)
	  bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	if ( buffer.usage == BU_STATIC ) 
  {
		bufferCreateInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	}

  VmaMemoryRequirements vmaReq = {};
	if ( buffer.usage == BU_STATIC )
  {
		vmaReq.usage = VMA_MEMORY_USAGE_GPU_ONLY;
	}
  else if ( buffer.usage == BU_DYNAMIC )
  {
		vmaReq.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		vmaReq.flags = VMA_MEMORY_REQUIREMENT_PERSISTENT_MAP_BIT;
	}

  VK_CHECK(vmaCreateBuffer(vmaAllocator, &bufferCreateInfo, &vmaReq, &buffer.apiObject, &buffer.vmaAllocation, &buffer.allocation));

  if (data != nullptr)
  {
    // update(data, allocSize)
  }

  return !allocationFailed;
}

void
UpdateBufferObject(
  BufferObject& buffer,
  const void* data,
  i32 size,
  i32 offset)
{
  platform_assert(buffer.apiObject == VK_NULL_HANDLE);
  assert_16_byte_aligned(data);

  if (size <= 0)
  {
    VK_VALIDATE(false, "Bad allocation size!")
  }

  if ( buffer.usage == BU_DYNAMIC ) 
  {
		CopyBuffer(
			(byte *)buffer.allocation.pMappedData + buffer.offsetInOtherBuffer + offset,
			(const byte *)data, size );
	} else {
		VkBuffer stageBuffer;
		VkCommandBuffer commandBuffer;
		int stageOffset = 0;
		byte * stageData = Stage(stagingManager, size, 1, commandBuffer, stageBuffer, stageOffset);

		memcpy( stageData, data, size );

		VkBufferCopy bufferCopy = {};
		bufferCopy.srcOffset = stageOffset;
		bufferCopy.dstOffset = buffer.offsetInOtherBuffer + offset;
		bufferCopy.size = size;

		vkCmdCopyBuffer( commandBuffer, stageBuffer, buffer.apiObject, 1, &bufferCopy );
	}
}

void*
MapBuffer(
  BufferObject& bufferObject,
  bufferMapType mapType)
{
  assert( bufferObject.apiObject != VK_NULL_HANDLE );

	if ( bufferObject.usage == BU_STATIC )
  {
		//idLib::FatalError( "idVertexBuffer::MapBuffer: Cannot map a buffer marked as BU_STATIC." );
    platform_assert(false);
	}

	void* buffer = (byte *)bufferObject.allocation.pMappedData + bufferObject.offsetInOtherBuffer;

  bufferObject.isMapped = true;

	if ( buffer == NULL )
  {
		//idLib::FatalError( "idVertexBuffer::MapBuffer: failed" );
    platform_assert(false);
	}

	return buffer;
}

void
UnmapBuffer(
  BufferObject& buffer)
{
  platform_assert(buffer.apiObject != VK_NULL_HANDLE);

  if (buffer.usage == BU_STATIC)
  {
    platform_assert(false);
  }

  buffer.isMapped = false;
}

void
Reference(
  BufferObject& buffer,
  const BufferObject& ref_buffer)
{
  platform_assert( buffer.isMapped == false );
	//platform_assert( other.IsMapped() == false );	// this happens when building idTriangles while at the same time setting up triIndex_t
	platform_assert( ref_buffer.size > 0 );

	FreeBufferObject(buffer);
	buffer.size = ref_buffer.size;					// this strips the MAPPED_FLAG
	buffer.offsetInOtherBuffer = ref_buffer.offsetInOtherBuffer;	// this strips the OWNS_BUFFER_FLAG
	buffer.usage = ref_buffer.usage;
	buffer.apiObject = ref_buffer.apiObject;
	buffer.allocation = ref_buffer.allocation;

  buffer.isMapped = false;
  buffer.isOwned = false;
}

void
Reference(
  BufferObject& buffer,
  const BufferObject& ref_buffer,
  i32 refOffset,
  i32 refSize)
{
  platform_assert( buffer.isMapped == false );
	//assert( other.IsMapped() == false );	// this happens when building idTriangles while at the same time setting up triIndex_t
	platform_assert( refOffset >= 0 );
	platform_assert( refSize >= 0 );
	platform_assert( refOffset + refSize <= ref_buffer.size );

	FreeBufferObject(buffer);
	buffer.size = refSize;
	buffer.offsetInOtherBuffer = ref_buffer.offsetInOtherBuffer + refOffset;
	buffer.usage = ref_buffer.usage;
	buffer.apiObject = ref_buffer.apiObject;
	buffer.allocation = ref_buffer.allocation;

	buffer.isMapped = false;
  buffer.isOwned = false;
}

void
FreeBufferObject(BufferObject& buffer)
{
  buffer.isMapped = false;
  
  if (!buffer.isOwned)
  {
    buffer = BufferObject();
    return;
  }

  if (buffer.apiObject == VK_NULL_HANDLE)
  {
    return;
  }
  else
  {
    vmaDestroyBuffer(vmaAllocator, buffer.apiObject, buffer.vmaAllocation);
  }

  buffer = BufferObject();
}