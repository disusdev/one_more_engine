#include "PCH.h"

static Context* l_context = nullptr;

vkStagingManager stagingManager;

static void
Wait(
	vkStagingManager& manager,
	StagingBuffer& stage )
{
	if ( stage.submitted == false )
	{
		return;
	}

	VK_CHECK( vkWaitForFences( l_context->device, 1, &stage.fence, VK_TRUE, UINT64_MAX ) );
	VK_CHECK( vkResetFences( l_context->device, 1, &stage.fence ) );

	stage.offset = 0;
	stage.submitted = false;

	VkCommandBufferBeginInfo commandBufferBeginInfo = {};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	VK_CHECK( vkBeginCommandBuffer( stage.commandBuffer, &commandBufferBeginInfo ) );
}

void
InitStagingManager(
  vkStagingManager& manager,
	Context& context)
{
	l_context = &context;

  manager.maxBufferSize = 
		(size_t)( /*r_vkUploadBufferSizeMB.GetInteger()*/64 * 1024 * 1024 );

	VkBufferCreateInfo bufferCreateInfo = {};
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.size = manager.maxBufferSize;
	bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	for ( int i = 0; i < NUM_FRAME_DATA; ++i )
	{
		manager.buffers[ i ].offset = 0;

		VK_CHECK( vkCreateBuffer( context.device, &bufferCreateInfo, NULL, &manager.buffers[ i ].buffer ) );
	}

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements( context.device, manager.buffers[ 0 ].buffer, &memoryRequirements );

	const VkDeviceSize alignMod 
		= memoryRequirements.size % memoryRequirements.alignment;
	const VkDeviceSize alignedSize 
		= ( alignMod == 0 ) 
		? memoryRequirements.size 
		: ( memoryRequirements.size + memoryRequirements.alignment - alignMod );

	VkMemoryAllocateInfo memoryAllocateInfo = {};
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.allocationSize = alignedSize * NUM_FRAME_DATA;
	memoryAllocateInfo.memoryTypeIndex 
		= FindMemoryTypeIndex(
			*l_context,
			memoryRequirements.memoryTypeBits, 
			VULKAN_MEMORY_USAGE_CPU_TO_GPU );

	VK_CHECK( vkAllocateMemory( 
		context.device, 
		&memoryAllocateInfo, 
		NULL, 
		&manager.memory ) );

	for ( int i = 0; i < NUM_FRAME_DATA; ++i )
	{
		VK_CHECK( vkBindBufferMemory( 
			context.device, 
			manager.buffers[ i ].buffer, 
			manager.memory, 
			i * alignedSize ) );
	}

	VK_CHECK( vkMapMemory( 
		context.device, 
		manager.memory, 
		0, 
		alignedSize * NUM_FRAME_DATA, 
		0, 
		reinterpret_cast< void ** >( &manager.mappedData ) ) );

	VkCommandPoolCreateInfo commandPoolCreateInfo = {};
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	commandPoolCreateInfo.queueFamilyIndex = context.graphicsFamilyIdx;
	VK_CHECK( vkCreateCommandPool( context.device, &commandPoolCreateInfo, NULL, &manager.commandPool ) );

	VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.commandPool = manager.commandPool;
	commandBufferAllocateInfo.commandBufferCount = 1;

	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

	VkCommandBufferBeginInfo commandBufferBeginInfo = {};
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	for ( int i = 0; i < NUM_FRAME_DATA; ++i )
	{
		VK_CHECK( vkAllocateCommandBuffers( context.device, &commandBufferAllocateInfo, &manager.buffers[ i ].commandBuffer ) );
		VK_CHECK( vkCreateFence( context.device, &fenceCreateInfo, NULL, &manager.buffers[ i ].fence ) );
		VK_CHECK( vkBeginCommandBuffer( manager.buffers[ i ].commandBuffer, &commandBufferBeginInfo ) );

		manager.buffers[ i ].data = (byte *)manager.mappedData + (i * alignedSize);
	}
}

byte*
Stage(
  vkStagingManager& manager,
  const int size,
  const int alignment,
  VkCommandBuffer& commandBuffer,
  VkBuffer& buffer,
  int& bufferOffset)
{
  if ( size > manager.maxBufferSize )
	{
		// idLib::FatalError( "Can't allocate %d MB in gpu transfer buffer", (int)( size / 1024 / 1024 ) );
		platform_assert(false);
	}

	StagingBuffer* stage = &manager.buffers[ manager.currentBuffer ];
	const int alignMod = stage->offset % alignment;
	stage->offset = ( ( stage->offset % alignment ) == 0 ) ? stage->offset : ( stage->offset + alignment - alignMod );

	if ( ( stage->offset + size ) >= ( manager.maxBufferSize ) && !stage->submitted )
	{
		Flush(manager);
	}

	stage = &manager.buffers[ manager.currentBuffer ];
	if ( stage->submitted )
	{
		Wait(manager, *stage );
	}

	commandBuffer = stage->commandBuffer;
	buffer = stage->buffer;
	bufferOffset = stage->offset;

	byte * data = stage->data + stage->offset;
	stage->offset += size;

	return data;
}

void
Flush(
  vkStagingManager& manager)
{
	StagingBuffer& stage = manager.buffers[ manager.currentBuffer ];
	if ( stage.submitted || stage.offset == 0 )
	{
		return;
	}

	VkMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
	barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	barrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_INDEX_READ_BIT;
	vkCmdPipelineBarrier( 
		stage.commandBuffer, 
		VK_PIPELINE_STAGE_TRANSFER_BIT, 
		VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 
		0, 1, &barrier, 0, NULL, 0, NULL );

	vkEndCommandBuffer( stage.commandBuffer );

	VkMappedMemoryRange memoryRange = {};
	memoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	memoryRange.memory = manager.memory;
	memoryRange.size = VK_WHOLE_SIZE;
	vkFlushMappedMemoryRanges( l_context->device, 1, &memoryRange );

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &stage.commandBuffer;

	vkQueueSubmit( l_context->graphicsQueue, 1, &submitInfo, stage.fence );

	stage.submitted = true;

	manager.currentBuffer = ( manager.currentBuffer + 1 ) % NUM_FRAME_DATA;
}

void
TermStagingManager(
  vkStagingManager& manager)
{
	vkUnmapMemory( l_context->device, manager.memory );
	manager.memory = VK_NULL_HANDLE;
	manager.mappedData = NULL;

	for ( int i = 0; i < NUM_FRAME_DATA; ++i )
	{
		vkDestroyFence( l_context->device, manager.buffers[ i ].fence, NULL );
		vkDestroyBuffer( l_context->device, manager.buffers[ i ].buffer, NULL );
		vkFreeCommandBuffers( l_context->device, manager.commandPool, 1, &manager.buffers[ i ].commandBuffer );
	}
	memset( manager.buffers, 0, sizeof( manager.buffers ) );

	manager.maxBufferSize = 0;
	manager.currentBuffer = 0;
}


