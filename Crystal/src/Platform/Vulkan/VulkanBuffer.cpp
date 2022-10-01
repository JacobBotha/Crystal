#include "clpch.h"

#include "VulkanBuffer.h"
#include "Crystal/Renderer/Renderer.h"
#include "VulkanRendererAPI.h"

namespace Crystal
{
	VulkanBuffer::VulkanBuffer(
		VulkanLogicalDevice* device, 
		VmaAllocator* allocator,
		BufferType bufferType, 
		size_t size
	)
		: m_Buffer(VK_NULL_HANDLE),
		m_Type(bufferType),
		m_Device(device),
		m_Allocator(allocator),
		m_Size(size),
		m_Data(nullptr)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		//Can add support for concurrent sharing, but currently only support 
		//for exclusive buffer use - exclusive is more efficient as well.
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

		VkMemoryPropertyFlags propertyFlags = VK_MEMORY_PROPERTY_PROTECTED_BIT;
		switch (bufferType)
		{
		case BufferType::Vertex:
			bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			propertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			allocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
			break;
		case BufferType::Index:
			bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
			allocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
			break;
		case BufferType::Uniform:
			bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
			allocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
			break;
		case BufferType::Staging:
			bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			propertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
			allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
				VMA_ALLOCATION_CREATE_MAPPED_BIT;
			break;
		default:
			break;
		}


		//VMA doesn't requrie the memory type to be explicitly set
		//vkGetBufferMemoryRequirements(m_Device->GetVkDevice(), m_Buffer, &m_MemRequirements);
		//allocInfo.memoryTypeBits = m_Device->GetPhysicalDevice()
		//	->FindMemoryType(m_MemRequirements.memoryTypeBits, propertyFlags);

		VkResult err = vmaCreateBuffer(*allocator, &bufferInfo, &allocInfo, &m_Buffer, &m_Allocation, &m_AllocInfo);

		CL_CORE_ASSERT(err == VK_SUCCESS, "Could not create buffer!");
	}

	VulkanBuffer::~VulkanBuffer()
	{
		m_Device->WaitGPU();
		vmaDestroyBuffer(*m_Allocator, m_Buffer, m_Allocation);
	}

	void VulkanBuffer::BindData(std::vector<Vertex>& vertices)
	{
		CL_CORE_ASSERT(m_Type == BufferType::Vertex, 
			"Cannot bind vertices to a buffer that is not of type vertex!");
		
		void* data = vertices.data();

		if (BindIfHostVisible(data));

		//Create a staging buffer to bind the data then copy into the vertex buffer
		VulkanBuffer stagingBuffer(m_Device, m_Allocator, BufferType::Staging, m_Size);
		stagingBuffer.PersistentBindData(data);

		CopyBuffer(stagingBuffer);
	}

	void VulkanBuffer::BindData(std::vector<Index>& indices)
	{
		CL_CORE_ASSERT(m_Type == BufferType::Index, 
			"Cannot bind indices to a buffer that is not of type Index!")

		void* data = indices.data();

		if (BindIfHostVisible(data)) return;

		//Create a staging buffer to bind the data then copy into the vertex buffer
		VulkanBuffer stagingBuffer(m_Device, m_Allocator, BufferType::Staging, m_Size);
		stagingBuffer.PersistentBindData(data);

		CopyBuffer(stagingBuffer);
	}

	void VulkanBuffer::PersistentBindData(void* data)
	{
		memcpy(m_AllocInfo.pMappedData, data, m_Size);
	}

	void VulkanBuffer::BindData(void* data)
	{
		vmaMapMemory(*m_Allocator, m_Allocation, &m_Data);
		memcpy(m_Data, data, m_Size);
		vmaUnmapMemory(*m_Allocator, m_Allocation);
	}

	bool VulkanBuffer::BindIfHostVisible(void* data)
	{
		VkMemoryPropertyFlags memPropFlags;
		vmaGetAllocationMemoryProperties(*m_Allocator, m_Allocation, &memPropFlags);

		if (memPropFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
		{
			CL_CORE_INFO("Buffer is stored in mappable memory");
			PersistentBindData(data);
			return true;
		}

		return false;
	}

	void VulkanBuffer::CopyBuffer(VulkanBuffer& buffer) {
		VkBuffer srcBuffer = buffer.GetVkBuffer();
		VulkanRendererAPI* rendererAPI = (VulkanRendererAPI*) Renderer::GetRendererAPI();

		VulkanCommandPool* commandPool = rendererAPI->GetTransferCommandPool();
		VulkanCommandBuffer commandBuffer(commandPool, true);

		commandBuffer.Begin(true);

		//Assign area to copy
		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		m_Size = buffer.GetSize();
		copyRegion.size = m_Size;
		vkCmdCopyBuffer(commandBuffer.GetVkCommandBuffer(), srcBuffer, m_Buffer, 1, &copyRegion);

		commandBuffer.End();

		VkSubmitInfo submitInfo{};
		VkCommandBuffer commandBuffers[] = { commandBuffer.GetVkCommandBuffer() };
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = commandBuffers;

		vkQueueSubmit(m_Device->GetQueue(QueueFlags::Transfer), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_Device->GetQueue(QueueFlags::Transfer));
	}
}