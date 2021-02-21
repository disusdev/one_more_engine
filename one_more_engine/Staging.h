#pragma once

struct StagingBuffer
{
	bool				submitted = false;
	VkCommandBuffer		commandBuffer = VK_NULL_HANDLE;
	VkBuffer			buffer = VK_NULL_HANDLE;
	VkFence				fence = VK_NULL_HANDLE;
	VkDeviceSize		offset = 0;
	byte *				data = nullptr;
};

struct vkStagingManager
{
	i32 maxBufferSize;
	i32 currentBuffer;
	byte* mappedData;
	VkDeviceMemory memory;
	VkCommandPool commandPool;

	StagingBuffer buffers[2];
};

void
InitStagingManager(
	vkStagingManager& manager);

byte*
Stage(
	vkStagingManager& manager,
	const int size,
	const int alignment,
	VkCommandBuffer& commandBuffer,
	VkBuffer& buffer,
	int& bufferOffset);

void
Flush(
	vkStagingManager& manager);

void
TermStagingManager(
	vkStagingManager& manager);


extern vkStagingManager stagingManager;