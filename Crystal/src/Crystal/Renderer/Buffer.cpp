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
			return std::make_shared<VulkanBuffer>(((VulkanRendererAPI*)Renderer::GetRendererAPI())->GetLogicalDevice(), type, size);
		case(RendererAPI::API::None):
			CL_CORE_ASSERT(false, "None api not currently supported!");
			break;
		default:
			break;
		}
		return nullptr;
	}
}