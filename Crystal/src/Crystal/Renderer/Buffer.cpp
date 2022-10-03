#include "clpch.h"

#include "Buffer.h"
#include "Renderer.h"
#include "Platform/Vulkan/VulkanBuffer.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Crystal
{
	std::shared_ptr<Buffer> Buffer::Create(BufferType type, size_t size)
	{
		CL_CORE_ASSERT(type != BufferType::Staging, "Cannot create abstract staging buffer.")
		switch (Renderer::GetAPI())
		{
		case(RendererAPI::API::Vulkan):
		{
			VulkanRendererAPI* rendererAPI = (VulkanRendererAPI*)Renderer::GetRendererAPI();
			return std::make_shared<VulkanBuffer>(rendererAPI->GetLogicalDevice(), rendererAPI->GetMemoryAllocator(), type, size);
		}
		break;
		case(RendererAPI::API::None):
			CL_CORE_ASSERT(false, "None api not currently supported!");
			break;
		default:
			break;
		}
		return nullptr;
	}

	std::shared_ptr<Buffer> Buffer::Create(std::set<BufferType>& types, size_t size)
	{
		//CL_CORE_ASSERT(type != BufferType::Staging, "Cannot create abstract staging buffer.")
		switch (Renderer::GetAPI())
		{
		case(RendererAPI::API::Vulkan):
		{
			VulkanRendererAPI* rendererAPI = (VulkanRendererAPI*)Renderer::GetRendererAPI();
			return std::make_shared<VulkanBuffer>(rendererAPI->GetLogicalDevice(), rendererAPI->GetMemoryAllocator(), types, size);
		}
		break;
		case(RendererAPI::API::None):
			CL_CORE_ASSERT(false, "None api not currently supported!");
			break;
		default:
			break;
		}
		return nullptr;
	}
}