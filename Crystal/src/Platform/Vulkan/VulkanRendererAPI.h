#pragma once

#include "Crystal/Renderer/RendererAPI.h"
#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanLogicalDevice.h"
#include "VulkanSurface.h"
#include "VulkanSwapChain.h"

namespace Crystal {
	class CRYSTAL_API VulkanRendererAPI : RendererAPI {
	public:
		VulkanRendererAPI();
		~VulkanRendererAPI();

		virtual void Init() override;
		virtual void SetViewPort(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;
	private:
		std::unique_ptr<VulkanInstance> m_Instance;
		std::unique_ptr<VulkanPhysicalDevice> m_PhysicalDevice;
		std::unique_ptr<VulkanSurface> m_Surface;

		std::unique_ptr<VulkanLogicalDevice> m_LogicalDevice;
		std::unique_ptr<VulkanSwapChain> m_SwapChain;
	};
}
