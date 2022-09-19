#pragma once

#include "VulkanSurface.h"
#include "VulkanLogicalDevice.h"

namespace Crystal {
	class VulkanSwapChain {
	public:
		VulkanSwapChain(VulkanLogicalDevice* device, VulkanSurface* surface);
		~VulkanSwapChain();

		VkSwapchainKHR GetVkSwapChain() const { return m_SwapChain; }
		VkExtent2D GetVkExtent2D() const { return m_Extent; }
		VkFormat GetVkImageFormat() const { return m_ImageFormat; }
		const std::vector<VkImageView>& GetImageViews() const { return m_SwapChainImageViews; }

		//Retrieve next swap chain image info
		uint32_t GetNextImageIndex(uint64_t timeout = UINT64_MAX) const;
		VkImage GetNextImage(uint64_t timeout = UINT64_MAX) const;
		VkImageView GetNextImageView(uint64_t timeout = UINT64_MAX) const;
		VkSemaphore GetImageAvailableSemaphore() const { return m_ImageAvailableSemaphore; }

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

		VkSemaphore m_ImageAvailableSemaphore;
	};
}
