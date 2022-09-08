#include "clpch.h"
#include "VulkanRendererAPI.h"
#include "Crystal/Core/Application.h"

namespace Crystal {
	VulkanRendererAPI::VulkanRendererAPI() {}

	VulkanRendererAPI::~VulkanRendererAPI() {}

	void Crystal::VulkanRendererAPI::Init() {
		m_Instance = std::make_unique<VulkanInstance>();
		m_PhysicalDevice = std::make_unique<VulkanPhysicalDevice>(m_Instance.get()->GetInstance());
		m_Surface = std::make_unique<VulkanSurface>(&Application::Get().GetWindow(), m_Instance.get(), m_PhysicalDevice.get());

		m_LogicalDevice = std::make_unique<VulkanLogicalDevice>(m_PhysicalDevice, m_Instance);
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
