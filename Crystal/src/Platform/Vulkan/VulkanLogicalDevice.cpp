#include "clpch.h"

#include "Crystal/Renderer/QueueFlags.h"
#include "VulkanLogicalDevice.h"

namespace Crystal {
	VulkanLogicalDevice::VulkanLogicalDevice(VulkanPhysicalDevice* physicalDevice, VulkanSurface* surface) 
	: m_PhysicalDevice(physicalDevice) {
		const QueueFlagBits requiredQueues = QueueFlagBit::ComputeBit | QueueFlagBit::GraphicsBit | QueueFlagBit::TransferBit | QueueFlagBit::PresentBit;
		m_QueueFamilyIndices = m_PhysicalDevice->GetQueueFamilyIndices(surface->GetVkSurface(), requiredQueues);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		
		std::set<int> uniqueQueueFamilies;
		bool queueSupport = true;
		for (unsigned int i = 0; i < requiredQueues.size(); ++i) {
			if (requiredQueues[i]) {
				queueSupport &= (m_QueueFamilyIndices[i] >= 0);
				uniqueQueueFamilies.insert(m_QueueFamilyIndices[i]);
			}
		}


		float queuePriority = 1.0f;
		for (int queueFamily : uniqueQueueFamilies) {
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		//No Device Features enabled for now
		VkPhysicalDeviceFeatures deviceFeatures{};
		createInfo.pEnabledFeatures = &deviceFeatures;

		//Only required extensions for now are device extensions
		std::vector extensions = m_PhysicalDevice->GetDeviceExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		//Current version of vulkan does not require enabled layers for logical 
		//device - here for legacy support
		std::vector<const char*> enabledLayers = m_PhysicalDevice->GetInstance()->GetEnabledLayers();
		createInfo.enabledLayerCount = static_cast<uint32_t>(enabledLayers.size());
		createInfo.ppEnabledLayerNames = enabledLayers.data();
		
		VkResult result = vkCreateDevice(m_PhysicalDevice->GetVkPhysicalDevice(), &createInfo, nullptr, &m_Device);
		CL_CORE_ASSERT(result == VK_SUCCESS, "Could not create logical device!");

		for (unsigned int i = 0; i < requiredQueues.size(); ++i) {
			if (requiredQueues[i]) {
				vkGetDeviceQueue(m_Device, m_QueueFamilyIndices[i], 0, &m_Queues[i]);
			}
		}
	}

	VulkanLogicalDevice::~VulkanLogicalDevice() {
		CL_CORE_INFO("Destroying device.");
		vkDestroyDevice(m_Device, nullptr);
	}

	VkQueue VulkanLogicalDevice::GetQueue(QueueFlags flag) const {
		return m_Queues[flag];
	}


	QueueFamilyIndex VulkanLogicalDevice::GetQueueIndex(QueueFlags flag) const {
		return m_QueueFamilyIndices[flag];
	}
}