#pragma once

#include "VulkanSurface.h"
#include "VulkanPhysicalDevice.h"

namespace Crystal {
	class VulkanSwapChain {
	public:
		VulkanSwapChain(VulkanPhysicalDevice* physicalDevice, VulkanSurface* surface);
		~VulkanSwapChain();

	private:
		VulkanSurface* m_Surface;
	};
}
