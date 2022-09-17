#include "clpch.h"

#include "VulkanSwapChain.h"

namespace Crystal {

	static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	//Framebuffer width and height requried to create swap extent
	static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, int width, int height) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}
		else {
			//int width, height;
			//glfwGetFramebufferSize(window, &width, &height);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}

	VulkanSwapChain::VulkanSwapChain(VulkanLogicalDevice* device, VulkanSurface* surface)
		: m_Device(device), m_Surface(surface) {
		
		m_SupportDetails = QuerySwapChainSupport();
		m_SurfaceFormat = chooseSwapSurfaceFormat(m_SupportDetails.formats);
		m_PresentMode = chooseSwapPresentMode(m_SupportDetails.presentModes);
		auto [width, height] = m_Surface->GetFrameBufferSize();
		m_Extent = chooseSwapExtent(m_SupportDetails.capabilities, width, height);

		uint32_t imageCount = m_SupportDetails.capabilities.minImageCount + 1;
		if (m_SupportDetails.capabilities.maxImageCount > 0 && imageCount > m_SupportDetails.capabilities.maxImageCount) {
			imageCount = m_SupportDetails.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_Surface->GetVkSurface();

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = m_SurfaceFormat.format;
		createInfo.imageColorSpace = m_SurfaceFormat.colorSpace;
		createInfo.imageExtent = m_Extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		
		const auto& queueFamilyIndices = m_Device->GetQueueFamilyIndices();
		if (queueFamilyIndices[QueueFlags::Graphics] != queueFamilyIndices[QueueFlags::Present]) {
			// Images can be used across multiple queue families without explicit ownership transfers
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			unsigned int indices[] = {
				static_cast<unsigned int>(queueFamilyIndices[QueueFlags::Graphics]),
				static_cast<unsigned int>(queueFamilyIndices[QueueFlags::Present])
			};
			createInfo.pQueueFamilyIndices = indices;
		}
		else {
			// An image is owned by one queue family at a time and ownership must be explicitly transfered between uses
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		createInfo.preTransform = m_SupportDetails.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = m_PresentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		//Create swap chain
		VkResult res;
		res = vkCreateSwapchainKHR(m_Device->GetVkDevice(), &createInfo, nullptr, &m_SwapChain);
		CL_CORE_ASSERT(res == VK_SUCCESS, "Swap chain creation failed!");

		//Sreate swap chain images
		res = vkGetSwapchainImagesKHR(m_Device->GetVkDevice(), m_SwapChain, &m_ImageCount, nullptr);
		CL_CORE_ASSERT(res == VK_SUCCESS, "Failed to get swapchain images!");
		m_SwapChainImages.resize(m_ImageCount);
		res = vkGetSwapchainImagesKHR(m_Device->GetVkDevice(), m_SwapChain, &m_ImageCount, m_SwapChainImages.data());
		CL_CORE_ASSERT(res == VK_SUCCESS, "Failed to get swapchain images!");

		m_ImageFormat = m_SurfaceFormat.format;

		CL_CORE_INFO("Created swap chain with {0} images.", m_ImageCount);

		//Create swap chain image views
		m_SwapChainImageViews.resize(m_SwapChainImages.size());
		for (size_t i = 0; i < m_SwapChainImages.size(); i++) {
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = m_SwapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = m_ImageFormat;

			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			res = vkCreateImageView(m_Device->GetVkDevice(), &createInfo, nullptr, &m_SwapChainImageViews[i]);
			CL_CORE_ASSERT(res == VK_SUCCESS, "Failed to create swap chain image view.");
		}

		(void)res;
	}

	VulkanSwapChain::~VulkanSwapChain() {
		for (auto imageView : m_SwapChainImageViews) {
			vkDestroyImageView(m_Device->GetVkDevice(), imageView, nullptr);
		}

		vkDestroySwapchainKHR(m_Device->GetVkDevice(), m_SwapChain, nullptr);
	}

	VulkanSwapChain::SwapChainSupportDetails VulkanSwapChain::QuerySwapChainSupport() {
		SwapChainSupportDetails details;
		VkPhysicalDevice device = m_Device->GetPhysicalDevice()->GetVkPhysicalDevice();
		VkSurfaceKHR surface = m_Surface->GetVkSurface();
		
		VkResult err;

		err = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
		CL_CORE_ASSERT(err == VK_SUCCESS, "Could not get surface capabilities.");

		uint32_t formatCount;
		err = vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
		CL_CORE_ASSERT(err == VK_SUCCESS, "Could not get surface formats.");

		if (formatCount != 0) {
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		err = vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
		CL_CORE_ASSERT(err == VK_SUCCESS, "Could not get surface present modes.");

		if (presentModeCount != 0) {
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		}

		(void)err;

		return details;
	}
}