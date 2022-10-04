#include "clpch.h"

#include "VulkanBuffer.h"
#include "Crystal/Renderer/Renderer.h"
#include "VulkanRendererAPI.h"

namespace Crystal
{
	inline namespace Utils
	{
		VkBufferCreateInfo InitCreateInfo(size_t size, bool exclusiveSharing = true) 
		{
			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = size;
			if (exclusiveSharing) //exclusive may be more efficient
				bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			else
				bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;

			return bufferInfo;
		}

		VmaAllocationCreateInfo InitAllocationCreateInfo()
		{
			VmaAllocationCreateInfo allocInfo = {};
			allocInfo.usage = VMA_MEMORY_USAGE_AUTO;

			return allocInfo;
		}

		void InitVertexBufferInfo(
			VkBufferCreateInfo& bufferInfo, 
			VkMemoryPropertyFlags& propertyFlags, 
			VmaAllocationCreateInfo& allocInfo)
		{
			bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
			propertyFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
			allocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
		}

		void InitIndexBufferInfo(
			VkBufferCreateInfo& bufferInfo,
			VkMemoryPropertyFlags& propertyFlags,
			VmaAllocationCreateInfo& allocInfo)
		{
			bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
			allocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
		}

		void InitStagingBufferInfo(
			VkBufferCreateInfo& bufferInfo,
			VkMemoryPropertyFlags& propertyFlags,
			VmaAllocationCreateInfo& allocInfo)
		{
			bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			propertyFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
			allocInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT |
				VMA_ALLOCATION_CREATE_MAPPED_BIT;
		}

		void InitUniformBufferInfo(
			VkBufferCreateInfo& bufferInfo,
			VkMemoryPropertyFlags& propertyFlags,
			VmaAllocationCreateInfo& allocInfo)
		{
			bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
			allocInfo.flags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;
		}

		void InitBufferInfo(
			Buffer::BufferType bufferType,
			VkBufferCreateInfo& bufferInfo,
			VkMemoryPropertyFlags& propertyFlags,
			VmaAllocationCreateInfo& allocInfo)
		{
			switch (bufferType)
			{
			case Buffer::BufferType::Vertex:
				InitVertexBufferInfo(bufferInfo, propertyFlags, allocInfo);
				break;
			case Buffer::BufferType::Index:
				InitIndexBufferInfo(bufferInfo, propertyFlags, allocInfo);
				break;
			case Buffer::BufferType::Uniform:
				InitUniformBufferInfo(bufferInfo, propertyFlags, allocInfo);
				break;
			case Buffer::BufferType::Staging:
				InitStagingBufferInfo(bufferInfo, propertyFlags, allocInfo);
				break;
			default:
				break;
			}
		}

		bool Contains(std::vector<Buffer::BufferType>& types, Buffer::BufferType type)
		{
			return std::find(types.begin(), types.end(), type) != types.end();
		}

		int32_t IndexOf(std::vector<Buffer::BufferType>& types, Buffer::BufferType type)
		{
			auto it = std::find(types.begin(), types.end(), type);
			if (it == types.end()) return -1;
			return it - types.begin();
		}
	}

	VulkanBuffer::VulkanBuffer(
		VulkanLogicalDevice* device, 
		VmaAllocator* allocator,
		BufferType bufferType, 
		size_t size
	)
		: m_Buffer(VK_NULL_HANDLE),
		//m_Type(bufferType),
		m_Device(device),
		m_Allocator(allocator),
		m_Size(size),
		m_Data(nullptr),
		m_MemRequirements({})
	{
		VkBufferCreateInfo bufferInfo = InitCreateInfo(size);
		VmaAllocationCreateInfo allocInfo = InitAllocationCreateInfo();
		VkMemoryPropertyFlags propertyFlags = VK_MEMORY_PROPERTY_PROTECTED_BIT;

		m_Types.push_back(bufferType);

		InitBufferInfo(bufferType, bufferInfo, propertyFlags, allocInfo);

		VkResult err = vmaCreateBuffer(*allocator, &bufferInfo, &allocInfo, &m_Buffer, &m_Allocation, &m_AllocInfo);

		CL_CORE_ASSERT(err == VK_SUCCESS, "Could not create buffer!");
	}

	VulkanBuffer::VulkanBuffer(VulkanLogicalDevice* device,
		VmaAllocator* allocator,
		std::set<BufferType>& bufferTypes,
		size_t size
	)
		: m_Buffer(VK_NULL_HANDLE),
		//m_Type(BufferType::Multi),
		m_Device(device),
		m_Allocator(allocator),
		m_Size(size),
		m_Data(nullptr),
		m_MemRequirements({})
	{
		VkBufferCreateInfo bufferInfo = InitCreateInfo(size);
		VmaAllocationCreateInfo allocInfo = InitAllocationCreateInfo();
		VkMemoryPropertyFlags propertyFlags = VK_MEMORY_PROPERTY_PROTECTED_BIT;

		for (auto bufferType : bufferTypes)
		{
			VkBufferCreateInfo bInfo{};
			VmaAllocationCreateInfo aInfo{};
			VkMemoryPropertyFlags pFlags;

			
			InitBufferInfo(bufferType, bInfo, pFlags, aInfo);

			bufferInfo.usage |= bInfo.usage;
			allocInfo.flags |= aInfo.flags;
			propertyFlags |= pFlags;

			//m_Types.push_back(bufferType);
		}

		m_Types.resize(bufferTypes.size());
		m_Offsets.resize(bufferTypes.size());

		int typeIndex = 0;
		if (bufferTypes.count(BufferType::Vertex)) //Make vertex first type stored
		{
			m_Types[typeIndex] = BufferType::Vertex;
			typeIndex++;
		}
		

		//Make Index first type saved after Vertex.
		if (bufferTypes.count(BufferType::Index))
		{
			m_Types[typeIndex] = BufferType::Index;
			typeIndex++;
		}

		for (auto bufferType : bufferTypes)
		{
			if (bufferType == BufferType::Vertex || bufferType == BufferType::Index)
				continue;

			m_Types[typeIndex] = bufferType;
			typeIndex++;
		}

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
		CL_CORE_ASSERT(Contains(m_Types, BufferType::Vertex), 
			"Cannot bind vertices to a buffer that is not of type vertex!");
		
		void* data = vertices.data();

		BindData(data);
	}

	void VulkanBuffer::BindData(std::vector<Index>& indices)
	{
		CL_CORE_ASSERT(HasType(BufferType::Index),
			"Cannot bind indices to a buffer that is not of type Index!")

		void* data = indices.data();

		BindData(data);
	}

	void VulkanBuffer::BindData(std::vector<void*>& data, std::vector<BufferRegion>& typeOffsets)
	{
		CL_CORE_ASSERT(data.size() == typeOffsets.size(),
			"The number of data pointers must equal the number of Types");

		VkDeviceSize cumSize = 0;
		VulkanBuffer stagingBuffer(m_Device, m_Allocator, BufferType::Staging, m_Size);
		for (uint16_t i = 0; i < typeOffsets.size(); i++)
		{
			BufferType type = typeOffsets[i].type;
			CL_CORE_ASSERT(Contains(m_Types, type),
				"Buffer does not contain provided type.");

			VkDeviceSize offset = typeOffsets[i].offset;
			uint32_t index = IndexOf(m_Types, type);
			CL_CORE_ASSERT(index >= 0, "Buffer offset index out of range!");
			m_Offsets[index] = offset;
			
			VkDeviceSize size = typeOffsets[i].size;
			CL_CORE_ASSERT(cumSize <= offset, "Overflow of buffer data!")
			BindData(data[i], size, offset, &stagingBuffer);

			cumSize += size;
		}

		CopyBuffer(stagingBuffer, 0);
	}
	
	void VulkanBuffer::BindData(void* data)
	{
		BindData(data, m_Size, 0, nullptr);
	}

	void VulkanBuffer::BindData(void* data, VkDeviceSize size, VkDeviceSize offset, VulkanBuffer* stagingBuffer)
	{
		CL_CORE_ASSERT(size + offset <= m_Size, 
			"Buffer overflow when trying to copy data!");

		if (BindIfHostVisible(data, size, offset)) return;

		//Create a staging buffer to bind the data then copy into the vertex buffer
		if (stagingBuffer)
		{
			stagingBuffer->PersistentBindData(data, size, offset);
			return;
		} 
		
		VulkanBuffer stagingBuff(m_Device, m_Allocator, BufferType::Staging, size);
		stagingBuff.PersistentBindData(data);

		CopyBuffer(stagingBuff, offset);
	}

	bool VulkanBuffer::HasType(BufferType type)
	{
		if (Contains(m_Types, type))
			return true;

		return false;
	}

	void VulkanBuffer::PersistentBindData(void* data)
	{
		PersistentBindData(data, m_Size, 0);
	}

	void VulkanBuffer::PersistentBindData(void* data, VkDeviceSize size, VkDeviceSize offset)
	{
		CL_CORE_ASSERT(m_AllocInfo.pMappedData, "Cannot bind data to unmapped memory!");
		//Convert offset from bytes to chars
		uint64_t charOffset = offset / sizeof(char);
		memcpy(&((char*)m_AllocInfo.pMappedData)[charOffset], data, size);
	}

	bool VulkanBuffer::BindIfHostVisible(void* data, VkDeviceSize size, VkDeviceSize offset)
	{
		VkMemoryPropertyFlags memPropFlags;
		vmaGetAllocationMemoryProperties(*m_Allocator, m_Allocation, &memPropFlags);

		if (memPropFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
		{
			CL_CORE_INFO("Buffer is stored in mappable memory, no staging buffer required");
			PersistentBindData(data, size, offset);
			return true;
		}

		return false;
	}

	bool VulkanBuffer::GetOffset(BufferType type, VkDeviceSize& offset)
	{
		if (Contains(m_Types, type))
		{
			offset = m_Offsets[IndexOf(m_Types, type)];
			return true;
		}
		return false;
	}

	void VulkanBuffer::CopyBuffer(VulkanBuffer& buffer, VkDeviceSize offset) {
		CL_CORE_ASSERT(buffer.GetSize() + offset <= m_Size, 
			"Buffer overflow when trying to copy data!");

		VkBuffer srcBuffer = buffer.GetVkBuffer();
		VulkanRendererAPI* rendererAPI = (VulkanRendererAPI*) Renderer::GetRendererAPI();

		VulkanCommandPool* commandPool = rendererAPI->GetTransferCommandPool();
		VulkanCommandBuffer commandBuffer(commandPool, true);

		commandBuffer.Begin(true);

		//Assign area to copy
		VkBufferCopy copyRegion{};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = offset;
		copyRegion.size = buffer.GetSize();
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