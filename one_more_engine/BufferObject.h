#pragma once

enum bufferObjectType
{
	BO_NONE,
	BO_VERTEX  = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
	BO_INDEX   = VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
	BO_UNIFORM = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
};

enum bufferMapType
{
	BM_READ,			// map for reading
	BM_WRITE			// map for writing
};

enum bufferUsageType
{
	BU_STATIC,			// GPU R
	BU_DYNAMIC,			// GPU R, CPU R/W
};

struct BufferObject
{
  i32 size = 0;
  i32 offsetInOtherBuffer = 0;
	bufferUsageType usage = BU_STATIC;
	bufferObjectType type = BO_NONE;

	VkBuffer apiObject = VK_NULL_HANDLE;
	VmaAllocation vmaAllocation = nullptr;
	VmaAllocationInfo allocation;

	bool isMapped = false;
	bool isOwned  = true;
};


bool
AllocBufferObject(
	BufferObject& buffer,
	const void* data,
	i32 allocSize,
	bufferUsageType usage);

void
UpdateBufferObject(
	BufferObject& buffer,
	const void* data,
	i32 allocSize,
	i32 offset);

void*
MapBuffer(
	BufferObject& buffer,
	bufferMapType mapType);

void
UnmapBuffer(
	BufferObject& buffer);

void
Reference(
	BufferObject& buffer,
	const BufferObject& ref_buffer);

void
Reference(
	BufferObject& buffer,
	const BufferObject& ref_buffer,
	i32 refOffset,
	i32 refSize);

void
FreeBufferObject(BufferObject& buffer);