#pragma once

#include "VulkanLogicalDevice.h"
#include "Crystal/Renderer/Vertex.h"
#include "Crystal/Renderer/Buffer.h"

#include "vma/vk_mem_alloc.h"
#include <vulkan/vulkan.h>

namespace Crystal
{
	class VulkanBuffer : public Buffer
	{
	public:

		VulkanBuffer(VulkanLogicalDevice* device, 
			VmaAllocator* allocator,
			BufferType bufferType,
			size_t size);

		VulkanBuffer(VulkanLogicalDevice* device, 
			VmaAllocator* allocator,
			std::set<BufferType>& bufferTypes,
			size_t size);
		virtual ~VulkanBuffer() override;

		/// <summary>
		/// Bind CPU data to GPU memory. This method wraps the generic binding.
		/// To be used specifically for vertex buffers and therefore takes a 
		/// vecotr of vertices as the parameter. Asserts that the current buffer
		/// type is a vertex buffer.
		/// </summary>
		/// <param name="vertices">
		/// The reference to vertices to store in the buffer.
		/// </param>
		virtual void BindData(std::vector<Vertex> &vertices) override;
		virtual void BindData(std::vector<Index> &indices) override;
		virtual void BindData(std::vector<void*>& data, std::vector<BufferRegion>& typeOffsets) override;

		/// <summary>
		/// Bind CPU data to GPU memory.
		/// </summary>
		/// <param name="data">A pointer to the data.</param>
		void BindData(void* data);
		void BindData(void* data, VkDeviceSize size, VkDeviceSize offset);

		virtual bool HasType(BufferType type) override;

		VkBuffer GetVkBuffer() const { return m_Buffer; }
		size_t GetSize() const { return m_Size; }
		bool GetOffset(BufferType type, VkDeviceSize& offset);
	
		void VulkanBuffer::CopyBuffer(VulkanBuffer& buffer, VkDeviceSize offset = 0);

	private:
		bool BindIfHostVisible(void* data, VkDeviceSize size, VkDeviceSize offset);
		void PersistentBindData(void* data);
		void PersistentBindData(void* data, VkDeviceSize size, VkDeviceSize offset);

		VulkanLogicalDevice* m_Device;
		VmaAllocator* m_Allocator;
		VkBuffer m_Buffer;
		VkDeviceSize m_Size;
		//BufferType m_Type;
		std::vector<BufferType> m_Types;
		std::vector<VkDeviceSize> m_Offsets;

		VmaAllocationInfo m_AllocInfo;
		VmaAllocation m_Allocation;
		VkMemoryRequirements m_MemRequirements;
		
		void* m_Data;
	};
}
