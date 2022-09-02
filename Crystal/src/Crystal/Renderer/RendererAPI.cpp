#include "clpch.h"

#include "RendererAPI.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Crystal {

	RendererAPI::API RendererAPI::s_API = RendererAPI::API::Vulkan;

	RendererAPI* RendererAPI::Create()
	{
		switch (s_API)
		{
		case RendererAPI::API::None:    CL_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		//Case for vulkan rendererapi instance
		case RendererAPI::API::Vulkan:  return (RendererAPI*) new VulkanRendererAPI();
		}

		CL_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}