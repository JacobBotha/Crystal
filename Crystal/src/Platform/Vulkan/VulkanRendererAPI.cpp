#include "clpch.h"
#include "VulkanRendererAPI.h"

namespace Crystal {
	VulkanRendererAPI::VulkanRendererAPI() {}

	VulkanRendererAPI::~VulkanRendererAPI() {}

	void Crystal::VulkanRendererAPI::Init() {
		m_VulkanInstance = std::make_unique<VulkanInstance>();
		m_VulkanPhysicalDevice = std::make_unique<VulkanPhysicalDevice>(m_VulkanInstance.get()->GetInstance());
	}

	void Crystal::VulkanRendererAPI::SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
	}

	void Crystal::VulkanRendererAPI::SetClearColor(const glm::vec4& color)
	{
	}

	void VulkanRendererAPI::Clear()
	{
	}
}
