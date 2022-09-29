#pragma once

#include "VulkanLogicalDevice.h"
#include "Crystal/Renderer/Vertex.h"
#include "Crystal/Renderer/Buffer.h"

#include <vulkan/vulkan.h>

namespace Crystal
{
	class VulkanBuffer : public Buffer
	{
	public:

		VulkanBuffer(VulkanLogicalDevice* device, BufferType bufferType,
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

		/// <summary>
		/// Bind CPU data to GPU memory.
		/// </summary>
		/// <param name="data">A pointer to the data.</param>
		void BindData(void* data);

		virtual BufferType GetType() override
		{
			return m_Type;
		}

		VkBuffer GetVkBuffer() const { return m_Buffer; }
		size_t GetSize() const { return m_Size; }
	
		void VulkanBuffer::CopyBuffer(VulkanBuffer& buffer);

	private:
		VulkanLogicalDevice* m_Device;
		VkBuffer m_Buffer;
		BufferType m_Type;

		//Buffer Memory
		size_t m_Size;
		VkMemoryRequirements m_MemRequirements;
		VkDeviceMemory m_BufferMemory;
		void* m_Data;
	};
}
