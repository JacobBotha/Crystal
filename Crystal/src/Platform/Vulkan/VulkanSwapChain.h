#pragma once

#include "VulkanSurface.h"
#include "VulkanLogicalDevice.h"

namespace Crystal {
	class VulkanSwapChain {
	public:
		VulkanSwapChain(VulkanLogicalDevice* device, VulkanSurface* surface);
		~VulkanSwapChain();

	private:
		struct SwapChainSupportDetails {
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};

		SwapChainSupportDetails QuerySwapChainSupport();

		VulkanLogicalDevice* m_Device;
		VulkanSurface* m_Surface;

		SwapChainSupportDetails m_SupportDetails;
		VkSurfaceFormatKHR m_SurfaceFormat;
		VkPresentModeKHR m_PresentMode;
		VkExtent2D m_Extent;
		VkFormat m_ImageFormat;

		VkSwapchainKHR m_SwapChain;

		uint32_t m_ImageCount;
		std::vector<VkImage> m_SwapChainImages;
		std::vector<VkImageView> m_SwapChainImageViews;
	};
}
