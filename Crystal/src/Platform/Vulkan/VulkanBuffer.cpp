#include "clpch.h"

#include "VulkanBuffer.h"
#include "Crystal/Renderer/Renderer.h"
#include "VulkanRendererAPI.h"

namespace Crystal
{
	VulkanBuffer::VulkanBuffer(
		VulkanLogicalDevice* device, 
		BufferType bufferType, 
		size_t size
	)
		: m_Buffer(VK_NULL_HANDLE),
		m_Type(bufferType),
		m_Device(device),
		m_Size(size),
		m_Data(nullptr)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		//Can add support for concurrent sharing, but currently only support 
		//for exclusive buffer use.
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VkMemoryPropertyFlags propertyFlags = VK_MEMORY_PROPERTY_PROTECTED_BIT;
		switch (bufferType)
		{
		case BufferType::Vertex:
			bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			propertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

			//Create a staging buffer for transfering CPU data.

			break;
		case BufferType::Uniform:
			bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
			break;
		case BufferType::Staging:
			bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			propertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
			break;
		default:
			break;
		}


		VkResult err = vkCreateBuffer(m_Device->GetVkDevice(), &bufferInfo, nullptr, &m_Buffer);
		CL_CORE_ASSERT(err == VK_SUCCESS, "Could not create buffer!");

		vkGetBufferMemoryRequirements(m_Device->GetVkDevice(), m_Buffer, &m_MemRequirements);
		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = m_MemRequirements.size;
		allocInfo.memoryTypeIndex = m_Device->GetPhysicalDevice()
			->FindMemoryType(m_MemRequirements.memoryTypeBits, propertyFlags);

		err = vkAllocateMemory(m_Device->GetVkDevice(), &allocInfo, nullptr, &m_BufferMemory);
		CL_CORE_ASSERT(err == VK_SUCCESS, "Cannot create buffer memory!");

		vkBindBufferMemory(m_Device->GetVkDevice(), m_Buffer, m_BufferMemory, 0);
	}

	VulkanBuffer::~VulkanBuffer()
	{
		m_Device->WaitGPU();
		vkDestroyBuffer(m_Device->GetVkDevice(), m_Buffer, nullptr);
		vkFreeMemory(m_Device->GetVkDevice(), m_BufferMemory, nullptr);
	}

	void VulkanBuffer::BindData(std::vector<Vertex>& vertices)
	{
		CL_CORE_ASSERT(m_Type == BufferType::Vertex, 
			"Cannot bind vertices to a buffer that is not of type vertex");
		//Create a staging buffer to bind the data then copy into the vertex buffer
		VulkanBuffer stagingBuffer(m_Device, BufferType::Staging, m_Size);
		stagingBuffer.BindData((void*)vertices.data());

		CopyBuffer(stagingBuffer);
	}

	void VulkanBuffer::BindData(void* data)
	{
		vkMapMemory(m_Device->GetVkDevice(), m_BufferMemory, 0, m_Size, 0, &m_Data);
		memcpy(m_Data, data, m_Size);
		vkUnmapMemory(m_Device->GetVkDevice(), m_BufferMemory);
	}

	void VulkanBuffer::CopyBuffer(VulkanBuffer& buffer) {
		VkBuffer srcBuffer = buffer.GetVkBuffer();
		VulkanRendererAPI* rendererAPI = (VulkanRendererAPI*) Renderer::GetRendererAPI();
		VkCommandPool commandPool = rendererAPI->GetCommandPool()->GetVkCommandPool();
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(m_Device->GetVkDevice(), &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0; // Optional
		copyRegion.dstOffset = 0; // Optional
		m_Size = buffer.GetSize();
		copyRegion.size = m_Size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, m_Buffer, 1, &copyRegion);

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(m_Device->GetQueue(QueueFlags::Transfer), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_Device->GetQueue(QueueFlags::Transfer));

		vkFreeCommandBuffers(m_Device->GetVkDevice(), commandPool, 1, &commandBuffer);
	}
}